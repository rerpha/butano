#ifndef BTN_REGULAR_BG_MAP_PTR_H
#define BTN_REGULAR_BG_MAP_PTR_H

#include "btn_utility.h"
#include "btn_span_fwd.h"
#include "btn_functional.h"
#include "btn_optional_fwd.h"
#include "btn_regular_bg_map_cell.h"

namespace btn
{

class size;
class bg_tiles_ptr;
class bg_palette_ptr;
class bg_tiles_item;
class bg_palette_item;
enum class palette_bpp_mode;

class regular_bg_map_ptr
{

public:
    [[nodiscard]] static optional<regular_bg_map_ptr> find(
            const regular_bg_map_cell& cells_ref, const size& dimensions, const bg_tiles_ptr& tiles,
            const bg_palette_ptr& palette);

    [[nodiscard]] static regular_bg_map_ptr create(
            const regular_bg_map_cell& cells_ref, const size& dimensions, bg_tiles_ptr tiles, bg_palette_ptr palette);

    [[nodiscard]] static regular_bg_map_ptr find_or_create(
            const regular_bg_map_cell& cells_ref, const size& dimensions, bg_tiles_ptr tiles, bg_palette_ptr palette);

    [[nodiscard]] static regular_bg_map_ptr allocate(
            const size& dimensions, bg_tiles_ptr tiles, bg_palette_ptr palette);

    [[nodiscard]] static optional<regular_bg_map_ptr> create_optional(
            const regular_bg_map_cell& cells_ref, const size& dimensions, bg_tiles_ptr tiles, bg_palette_ptr palette);

    [[nodiscard]] static optional<regular_bg_map_ptr> find_or_create_optional(
            const regular_bg_map_cell& cells_ref, const size& dimensions, bg_tiles_ptr tiles, bg_palette_ptr palette);

    [[nodiscard]] static optional<regular_bg_map_ptr> allocate_optional(
            const size& dimensions, bg_tiles_ptr tiles, bg_palette_ptr palette);

    regular_bg_map_ptr(const regular_bg_map_ptr& other);

    regular_bg_map_ptr& operator=(const regular_bg_map_ptr& other);

    regular_bg_map_ptr(regular_bg_map_ptr&& other) noexcept :
        regular_bg_map_ptr(other._handle)
    {
        other._handle = -1;
    }

    regular_bg_map_ptr& operator=(regular_bg_map_ptr&& other) noexcept
    {
        btn::swap(_handle, other._handle);
        return *this;
    }

    ~regular_bg_map_ptr()
    {
        if(_handle >= 0)
        {
            _destroy();
        }
    }

    [[nodiscard]] int id() const;

    [[nodiscard]] size dimensions() const;

    [[nodiscard]] palette_bpp_mode bpp_mode() const;

    [[nodiscard]] const regular_bg_map_cell* cells_ref() const;

    void set_cells_ref(const regular_bg_map_cell& cells_ref, const size& dimensions);

    void reload_cells_ref();

    [[nodiscard]] const bg_tiles_ptr& tiles() const;

    void set_tiles(const bg_tiles_ptr& tiles);

    void set_tiles(bg_tiles_ptr&& tiles);

    void set_tiles(const bg_tiles_item& tiles_item);

    [[nodiscard]] const bg_palette_ptr& palette() const;

    void set_palette(const bg_palette_ptr& palette);

    void set_palette(bg_palette_ptr&& palette);

    void set_palette(const bg_palette_item& palette_item);

    void set_tiles_and_palette(bg_tiles_ptr tiles, bg_palette_ptr palette);

    void set_tiles_and_palette(const bg_tiles_item& tiles_item, const bg_palette_item& palette_item);

    [[nodiscard]] optional<span<regular_bg_map_cell>> vram();

    [[nodiscard]] unsigned hash() const
    {
        return make_hash(_handle);
    }

    void swap(regular_bg_map_ptr& other)
    {
        btn::swap(_handle, other._handle);
    }

    friend void swap(regular_bg_map_ptr& a, regular_bg_map_ptr& b)
    {
        btn::swap(a._handle, b._handle);
    }

    [[nodiscard]] friend bool operator==(const regular_bg_map_ptr& a, const regular_bg_map_ptr& b) = default;

private:
    int8_t _handle;

    explicit regular_bg_map_ptr(int handle) :
        _handle(int8_t(handle))
    {
    }

    void _destroy();
};


template<>
struct hash<regular_bg_map_ptr>
{
    [[nodiscard]] unsigned operator()(const regular_bg_map_ptr& value) const
    {
        return value.hash();
    }
};

}

#endif
