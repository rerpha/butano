#ifndef BF_GAME_ENEMIES_GRID_H
#define BF_GAME_ENEMIES_GRID_H

#include "btn_pool.h"
#include "btn_intrusive_forward_list.h"
#include "bf_constants.h"

namespace btn
{
    class fixed_rect;
    class fixed_point;
}

namespace bf::game
{

class enemy;
class check_hero_bullet_data;

class enemies_grid
{

public:
    void add_enemy(enemy& enemy);

    void remove_enemy(enemy& enemy);

    [[nodiscard]] bool update_enemy(enemy& enemy);

    [[nodiscard]] bool check_hero(const btn::fixed_rect& hero_rect) const;

    [[nodiscard]] bool check_hero_bullet(const check_hero_bullet_data& data);

    #if BF_CFG_ENEMIES_GRID_LOG_ENABLED
        void log() const;
    #endif

private:
    static constexpr int cell_increment = constants::max_enemy_size / constants::enemies_grid_size;
    static constexpr int columns = ((constants::view_width * 2) / constants::enemies_grid_size) + (cell_increment * 2);
    static constexpr int rows = ((constants::view_height * 2) / constants::enemies_grid_size) + (cell_increment * 2);

    class enemies_list_node_type : public btn::intrusive_forward_list_node_type
    {

    public:
        enemy* enemy_ptr;

        enemies_list_node_type(enemy& enemy_ref) :
            enemy_ptr(&enemy_ref)
        {
        }
    };

    using enemies_list = btn::intrusive_forward_list<enemies_list_node_type>;
    using enemies_pool = btn::pool<enemies_list_node_type, constants::max_enemies_in_grid>;

    class cell
    {

    public:
        [[nodiscard]] const enemies_list& enemies() const
        {
            return _enemies;
        }

        [[nodiscard]] enemies_list& enemies()
        {
            return _enemies;
        }

        void add_enemy(enemy& enemy, enemies_pool& enemies_pool);

        void remove_enemy(enemy& enemy, enemies_pool& enemies_pool);

    private:
        enemies_list _enemies;
    };

    cell _cells[columns * rows];
    enemies_pool _pool;

    [[nodiscard]] static int _column(const btn::fixed_point& position);

    [[nodiscard]] static int _row(const btn::fixed_point& position);

    [[nodiscard]] const cell* _cells_row(int row) const
    {
        return &(_cells[0]) + (columns * row);
    }

    [[nodiscard]] cell* _cells_row(int row)
    {
        return &(_cells[0]) + (columns * row);
    }

    void _add_enemy_row(int row, int column, enemy& enemy);

    void _remove_enemy_row(int row, int column, enemy& enemy);

    void _add_enemy_column(int row, int column, enemy& enemy);

    void _remove_enemy_column(int row, int column, enemy& enemy);
};

}

#endif