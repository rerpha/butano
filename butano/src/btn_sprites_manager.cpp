#include "btn_sprites_manager.h"

#include "btn_pool.h"
#include "btn_size.h"
#include "btn_color.h"
#include "btn_vector.h"
#include "btn_camera.h"
#include "btn_display.h"
#include "btn_algorithm.h"
#include "btn_sprite_builder.h"
#include "btn_config_sprites.h"
#include "btn_sprites_manager_item.h"
#include "btn_sprite_affine_mats_manager.h"
#include "../hw/include/btn_hw_sprite_affine_mats.h"

namespace btn::sprites_manager
{

namespace
{
    static_assert(BTN_CFG_SPRITES_MAX_ITEMS > 0);

    using item_type = sprites_manager_item;
    using sorted_items_type = vector<item_type*, BTN_CFG_SPRITES_MAX_ITEMS>;

    class static_data
    {

    public:
        pool<item_type, BTN_CFG_SPRITES_MAX_ITEMS> items_pool;
        hw::sprites::handle handles[hw::sprites::count()];
        int first_index_to_commit = hw::sprites::count();
        int last_index_to_commit = 0;
        int last_visible_items_count = 0;
        bool check_items_on_screen = false;
        bool rebuild_handles = false;
    };

    BTN_DATA_EWRAM static_data data;

    void _update_handles(item_type& item)
    {
        int handles_index = item.handles_index;

        if(handles_index >= 0)
        {
            item.handle.copy_to(data.handles[handles_index]);
            data.first_index_to_commit = min(data.first_index_to_commit, handles_index);
            data.last_index_to_commit = max(data.last_index_to_commit, handles_index);
        }
    }

    void _assign_affine_mat(sprite_affine_mat_ptr affine_mat_ptr, item_type& item)
    {
        item.affine_mat_ptr = move(affine_mat_ptr);

        bool double_size = item.double_size();
        hw::sprites::set_affine_mat(item.affine_mat_ptr->id(), double_size, item.handle);

        if(double_size)
        {
            set_position(&item, item.position);
        }
        else
        {
            _update_handles(item);
        }
    }

    void _remove_affine_mat(item_type& item)
    {
        sprite_affine_mat_ptr& affine_mat_ptr = *item.affine_mat_ptr;
        bool double_size = hw::sprites::double_size(item.handle);
        hw::sprites::set_horizontal_flip(affine_mat_ptr.horizontal_flip(), item.handle);
        hw::sprites::set_vertical_flip(affine_mat_ptr.vertical_flip(), item.handle);
        hw::sprites::remove_affine_mat(item.handle);
        item.affine_mat_ptr.reset();

        if(double_size)
        {
            set_position(&item, item.position);
        }
        else
        {
            _update_handles(item);
        }
    }

    void _check_affine_mats()
    {
        if(sprite_affine_mats_manager::updated())
        {
            for(auto& layer : sorted_sprites::layers())
            {
                for(item_type& item : layer.second)
                {
                    if(item.affine_mat_ptr)
                    {
                        const sprite_affine_mat_ptr& affine_mat_ptr = *item.affine_mat_ptr;
                        int affine_mat_ptr_id = affine_mat_ptr.id();

                        if(sprite_affine_mats_manager::updated(affine_mat_ptr_id))
                        {
                            if(item.remove_affine_mat_when_not_needed && affine_mat_ptr.is_identity())
                            {
                                _remove_affine_mat(item);
                            }
                            else if(sprite_double_size_mode(item.double_size_mode) == sprite_double_size_mode::AUTO)
                            {
                                bool old_double_size = hw::sprites::double_size(item.handle);
                                bool new_double_size = sprite_affine_mats_manager::double_size(affine_mat_ptr_id);

                                if(old_double_size != new_double_size)
                                {
                                    hw::sprites::set_affine_mat(affine_mat_ptr_id, new_double_size, item.handle);
                                    set_position(&item, item.position);
                                }
                            }
                        }
                    }
                }
            }

            sprite_affine_mats_manager::update();
        }
    }

    void _check_items_on_screen()
    {
        if(data.check_items_on_screen)
        {
            fixed_point camera_position = camera::position();
            int display_width = display::width();
            int display_height = display::height();
            bool rebuild_handles = data.rebuild_handles;
            data.check_items_on_screen = false;

            for(auto& layer : sorted_sprites::layers())
            {
                for(item_type& item : layer.second)
                {
                    if(item.check_on_screen)
                    {
                        fixed_point position = item.position;

                        if(! item.ignore_camera)
                        {
                            position -= camera_position;
                        }

                        size dimensions = hw::sprites::dimensions(item.handle);
                        int x = position.x().integer() - (dimensions.width() / 2);
                        bool on_screen = false;
                        item.check_on_screen = false;

                        if(x + dimensions.width() > 0 && x < display_width)
                        {
                            int y = position.y().integer() - (dimensions.height() / 2);

                            if(y + dimensions.height() > 0 && y < display_height)
                            {
                                on_screen = true;
                            }
                        }

                        if(on_screen != item.on_screen)
                        {
                            item.on_screen = on_screen;
                            rebuild_handles = true;
                        }
                    }
                }
            }

            data.rebuild_handles = rebuild_handles;
        }
    }

    void _rebuild_handles()
    {
        if(data.rebuild_handles)
        {
            int visible_items_count = 0;
            data.rebuild_handles = false;

            for(auto& layer : sorted_sprites::layers())
            {
                for(item_type& item : layer.second)
                {
                    if(item.on_screen)
                    {
                        BTN_ASSERT(visible_items_count < hw::sprites::count(), "Too much sprites on screen");

                        item.handle.copy_to(data.handles[visible_items_count]);
                        item.handles_index = int8_t(visible_items_count);
                        ++visible_items_count;
                    }
                    else
                    {
                        item.handles_index = -1;
                    }
                }
            }

            int last_visible_items_count = data.last_visible_items_count;
            int items_to_hide = last_visible_items_count - visible_items_count;
            data.last_visible_items_count = visible_items_count;

            if(items_to_hide > 0)
            {
                hw::sprites::hide(items_to_hide, data.handles[visible_items_count]);
                visible_items_count = last_visible_items_count;
            }

            if(visible_items_count)
            {
                data.first_index_to_commit = 0;
                data.last_index_to_commit = visible_items_count - 1;
            }
        }
    }
}

int max_bg_priority()
{
    return hw::sprites::max_bg_priority();
}

int z_orders()
{
    return int(item_type::z_orders());
}

void init()
{
    hw::sprites::init();
    sprite_affine_mats_manager::init(sizeof(data.handles), data.handles);
}

optional<id_type> create(sprite_builder&& builder)
{
    if(data.items_pool.full())
    {
        return nullopt;
    }

    optional<sprite_tiles_ptr> tiles = builder.release_tiles();

    if(! tiles)
    {
        return nullopt;
    }

    optional<sprite_palette_ptr> palette = builder.release_palette();

    if(! palette)
    {
        return nullopt;
    }

    item_type* new_item = data.items_pool.create<item_type>(move(builder), move(*tiles), move(*palette));
    sorted_sprites::insert(*new_item);

    if(builder.visible())
    {
        data.check_items_on_screen = true;
    }

    return new_item;
}

void increase_usages(id_type id)
{
    auto item = static_cast<item_type*>(id);
    ++item->usages;
}

void decrease_usages(id_type id)
{
    auto item = static_cast<item_type*>(id);
    --item->usages;

    if(! item->usages)
    {
        if(item->on_screen)
        {
            data.rebuild_handles = true;
        }

        sorted_sprites::erase(*item);
    }
}

size dimensions(id_type id)
{
    auto item = static_cast<item_type*>(id);
    return hw::sprites::dimensions(item->handle);
}

const sprite_tiles_ptr& tiles_ptr(id_type id)
{
    auto item = static_cast<item_type*>(id);
    return item->tiles_ptr;
}

void set_tiles_ptr(id_type id, sprite_tiles_ptr tiles_ptr)
{
    auto item = static_cast<item_type*>(id);

    if(tiles_ptr != item->tiles_ptr)
    {
        BTN_ASSERT(item->tiles_ptr.tiles_count() == tiles_ptr.tiles_count(), "Invalid tiles count: ",
                   item->tiles_ptr.tiles_count(), " - ", tiles_ptr.tiles_count());

        hw::sprites::set_tiles(tiles_ptr.id(), item->handle);
        item->tiles_ptr = move(tiles_ptr);
        _update_handles(*item);
    }
}

const sprite_palette_ptr& palette_ptr(id_type id)
{
    auto item = static_cast<item_type*>(id);
    return item->palette_ptr;
}

void set_palette_ptr(id_type id, sprite_palette_ptr palette_ptr)
{
    auto item = static_cast<item_type*>(id);

    if(palette_ptr != item->palette_ptr)
    {
        BTN_ASSERT(item->palette_ptr.eight_bits_per_pixel() == palette_ptr.eight_bits_per_pixel(),
                   "Palette colors bpp mode mismatch: ",
                   item->palette_ptr.eight_bits_per_pixel(), " - ", palette_ptr.eight_bits_per_pixel());

        hw::sprites::set_palette(palette_ptr.id(), item->handle);
        item->palette_ptr = move(palette_ptr);
        _update_handles(*item);
    }
}

const fixed_point& position(id_type id)
{
    auto item = static_cast<item_type*>(id);
    return item->position;
}

void set_position(id_type id, const fixed_point& position)
{
    auto item = static_cast<item_type*>(id);
    fixed_point real_position = position;

    if(! item->ignore_camera)
    {
        real_position -= camera::position();
    }

    hw::sprites::set_position(real_position.x().integer(), real_position.y().integer(), item->handle);
    item->position = position;
    _update_handles(*item);

    if(item->visible)
    {
        item->check_on_screen = true;
        data.check_items_on_screen = true;
    }
}

int bg_priority(id_type id)
{
    auto item = static_cast<item_type*>(id);
    return item->bg_priority();
}

void set_bg_priority(id_type id, int bg_priority)
{
    BTN_ASSERT(bg_priority >= 0 && bg_priority <= hw::sprites::max_bg_priority(), "Invalid bg priority: ", bg_priority);

    auto item = static_cast<item_type*>(id);

    if(bg_priority != item->bg_priority())
    {
        hw::sprites::set_bg_priority(bg_priority, item->handle);
        sorted_sprites::erase(*item);
        item->update_sort_key(bg_priority, item->z_order());
        sorted_sprites::insert(*item);

        if(item->on_screen)
        {
            data.rebuild_handles = true;
        }
    }
}

int z_order(id_type id)
{
    auto item = static_cast<item_type*>(id);
    return item->z_order();
}

void set_z_order(id_type id, int z_order)
{
    BTN_ASSERT(z_order >= 0 && z_order < sprites_manager::z_orders(), "Invalid z order: ", z_order);

    auto item = static_cast<item_type*>(id);

    if(z_order != item->z_order())
    {
        sorted_sprites::erase(*item);
        item->update_sort_key(item->bg_priority(), z_order);
        sorted_sprites::insert(*item);

        if(item->on_screen)
        {
            data.rebuild_handles = true;
        }
    }
}

bool horizontal_flip(id_type id)
{
    auto item = static_cast<item_type*>(id);

    if(item->affine_mat_ptr)
    {
        return item->affine_mat_ptr->horizontal_flip();
    }

    return hw::sprites::horizontal_flip(item->handle);
}

void set_horizontal_flip(id_type id, bool horizontal_flip)
{
    auto item = static_cast<item_type*>(id);

    if(item->affine_mat_ptr)
    {
        item->affine_mat_ptr->set_horizontal_flip(horizontal_flip);
    }
    else
    {
        hw::sprites::set_horizontal_flip(horizontal_flip, item->handle);
        _update_handles(*item);
    }
}

bool vertical_flip(id_type id)
{
    auto item = static_cast<item_type*>(id);

    if(item->affine_mat_ptr)
    {
        return item->affine_mat_ptr->vertical_flip();
    }

    return hw::sprites::vertical_flip(item->handle);
}

void set_vertical_flip(id_type id, bool vertical_flip)
{
    auto item = static_cast<item_type*>(id);

    if(item->affine_mat_ptr)
    {
        item->affine_mat_ptr->set_vertical_flip(vertical_flip);
    }
    else
    {
        hw::sprites::set_vertical_flip(vertical_flip, item->handle);
        _update_handles(*item);
    }
}

bool mosaic_enabled(id_type id)
{
    auto item = static_cast<item_type*>(id);
    return hw::sprites::mosaic_enabled(item->handle);
}

void set_mosaic_enabled(id_type id, bool mosaic_enabled)
{
    auto item = static_cast<item_type*>(id);
    hw::sprites::set_mosaic_enabled(mosaic_enabled, item->handle);
    _update_handles(*item);
}

bool double_size(id_type id)
{
    auto item = static_cast<item_type*>(id);
    return hw::sprites::double_size(item->handle);
}

sprite_double_size_mode double_size_mode(id_type id)
{
    auto item = static_cast<item_type*>(id);
    return sprite_double_size_mode(item->double_size_mode);
}

void set_double_size_mode(id_type id, sprite_double_size_mode double_size_mode)
{
    auto item = static_cast<item_type*>(id);

    item->double_size_mode = unsigned(double_size_mode);

    if(item->affine_mat_ptr)
    {
        bool old_double_size = double_size(id);
        bool new_double_size = item->double_size();

        if(old_double_size != new_double_size)
        {
            hw::sprites::set_affine_mat(item->affine_mat_ptr->id(), new_double_size, item->handle);
            set_position(id, item->position);
        }
    }
}

bool visible(id_type id)
{
    auto item = static_cast<item_type*>(id);
    return item->visible;
}

void set_visible(id_type id, bool visible)
{
    auto item = static_cast<item_type*>(id);

    item->visible = visible;

    if(visible)
    {
        item->check_on_screen = true;
        data.check_items_on_screen = true;
    }
    else
    {
        if(item->on_screen)
        {
            data.rebuild_handles = true;
        }

        item->on_screen = false;
        item->check_on_screen = false;
    }
}

bool ignore_camera(id_type id)
{
    auto item = static_cast<item_type*>(id);
    return item->ignore_camera;
}

void set_ignore_camera(id_type id, bool ignore_camera)
{
    auto item = static_cast<item_type*>(id);
    item->ignore_camera = ignore_camera;
    set_position(id, item->position);
}

optional<sprite_affine_mat_ptr>& affine_mat_ptr(id_type id)
{
    auto item = static_cast<item_type*>(id);
    return item->affine_mat_ptr;
}

void set_affine_mat_ptr(id_type id, optional<sprite_affine_mat_ptr> affine_mat_ptr)
{
    auto item = static_cast<item_type*>(id);

    if(affine_mat_ptr)
    {
        sprite_affine_mat_ptr& affine_mat = *affine_mat_ptr;

        if(item->affine_mat_ptr)
        {
            if(item->affine_mat_ptr == affine_mat)
            {
                return;
            }
        }

        if(item->remove_affine_mat_when_not_needed && affine_mat.is_identity())
        {
            if(item->affine_mat_ptr)
            {
                _remove_affine_mat(*item);
            }
        }
        else
        {
            _assign_affine_mat(move(affine_mat), *item);
        }
    }
    else
    {
        if(item->affine_mat_ptr)
        {
            _remove_affine_mat(*item);
        }
    }
}

bool remove_affine_mat_when_not_needed(id_type id)
{
    auto item = static_cast<item_type*>(id);
    return item->remove_affine_mat_when_not_needed;
}

void set_remove_affine_mat_when_not_needed(id_type id, bool remove_when_not_needed)
{
    auto item = static_cast<item_type*>(id);
    item->remove_affine_mat_when_not_needed = remove_when_not_needed;

    if(remove_when_not_needed && item->affine_mat_ptr && item->affine_mat_ptr->is_identity())
    {
        _remove_affine_mat(*item);
    }
}

void update_camera()
{
    for(auto& layer : sorted_sprites::layers())
    {
        for(item_type& item : layer.second)
        {
            if(! item.ignore_camera)
            {
                set_position(&item, item.position);
            }
        }
    }
}

void update()
{
    _check_affine_mats();
    _check_items_on_screen();
    _rebuild_handles();
}

void commit()
{
    int first_index_to_commit = data.first_index_to_commit;
    int last_index_to_commit = data.last_index_to_commit;

    if(auto commit_data = sprite_affine_mats_manager::retrieve_commit_data())
    {
        int multiplier = hw::sprites::count() / hw::sprite_affine_mats::count();
        int first_mat_index_to_commit = commit_data->offset * multiplier;
        int last_mat_index_to_commit = first_mat_index_to_commit + (commit_data->count * multiplier) - 1;
        first_index_to_commit = min(first_index_to_commit, first_mat_index_to_commit);
        last_index_to_commit = max(last_index_to_commit, last_mat_index_to_commit);
    }

    if(first_index_to_commit < hw::sprites::count())
    {
        int commit_items_count = last_index_to_commit - first_index_to_commit + 1;
        hw::sprites::commit(data.handles[0], first_index_to_commit, commit_items_count);
        data.first_index_to_commit = hw::sprites::count();
        data.last_index_to_commit = 0;
    }
}

}
