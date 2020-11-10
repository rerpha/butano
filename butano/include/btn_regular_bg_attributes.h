/*
 * Copyright (c) 2020 Gustavo Valiente gustavo.valiente@protonmail.com
 * zlib License, see LICENSE file.
 */

#ifndef BTN_REGULAR_BG_ATTRIBUTES_H
#define BTN_REGULAR_BG_ATTRIBUTES_H

/**
 * @file
 * btn::regular_bg_attributes header file.
 *
 * @ingroup regular_bg
 */

#include "btn_regular_bg_map_ptr.h"

namespace btn
{

/**
 * @brief Manages the attributes to commit to the GBA register of a regular background.
 *
 * @ingroup regular_bg
 */
class regular_bg_attributes
{

public:
    /**
     * @brief Constructor.
     * @param map regular_bg_map_ptr of a regular background to copy.
     * @param priority Priority of a regular background relative to sprites and other backgrounds,
     * in the range [0..3].
     *
     * Backgrounds with higher priorities are drawn first
     * (and therefore can be covered by later sprites and backgrounds).
     * @param mosaic_enabled Indicates if the mosaic effect is applied to a regular background or not.
     */
    regular_bg_attributes(const regular_bg_map_ptr& map, int priority, bool mosaic_enabled);

    /**
     * @brief Constructor.
     * @param map regular_bg_map_ptr of a regular background to move.
     * @param priority Priority of a regular background relative to sprites and other backgrounds,
     * in the range [0..3].
     *
     * Backgrounds with higher priorities are drawn first
     * (and therefore can be covered by later sprites and backgrounds).
     * @param mosaic_enabled Indicates if the mosaic effect is applied to a regular background or not.
     */
    regular_bg_attributes(regular_bg_map_ptr&& map, int priority, bool mosaic_enabled);

    /**
     * @brief Returns the regular_bg_map_ptr of a regular background.
     */
    [[nodiscard]] const regular_bg_map_ptr& map() const
    {
        return _map;
    }

    /**
     * @brief Sets the regular_bg_map_ptr of a regular background.
     * @param map regular_bg_map_ptr of a regular background to copy.
     */
    void set_map(const regular_bg_map_ptr& map)
    {
        _map = map;
    }

    /**
     * @brief Sets the regular_bg_map_ptr of a regular background.
     * @param map regular_bg_map_ptr of a regular background to move.
     */
    void set_map(regular_bg_map_ptr&& map)
    {
        _map = move(map);
    }

    /**
     * @brief Returns the priority of a regular background relative to sprites and other backgrounds.
     *
     * Backgrounds with higher priorities are drawn first
     * (and therefore can be covered by later sprites and backgrounds).
     */
    [[nodiscard]] int priority() const
    {
        return _priority;
    }

    /**
     * @brief Sets the priority of a regular background relative to sprites and other backgrounds.
     *
     * Backgrounds with higher priorities are drawn first
     * (and therefore can be covered by later sprites and backgrounds).
     *
     * @param priority Priority in the range [0..3].
     */
    void set_priority(int priority);

    /**
     * @brief Indicates if the mosaic effect is applied to a regular background or not.
     */
    [[nodiscard]] bool mosaic_enabled() const
    {
        return _mosaic_enabled;
    }

    /**
     * @brief Sets if the mosaic effect must be applied to a regular background or not.
     */
    void set_mosaic_enabled(bool mosaic_enabled)
    {
        _mosaic_enabled = mosaic_enabled;
    }

    /**
     * @brief Default equal operator.
     */
    [[nodiscard]] friend bool operator==(const regular_bg_attributes& a, const regular_bg_attributes& b) = default;

private:
    regular_bg_map_ptr _map;
    int8_t _priority;
    bool _mosaic_enabled;
};

}

#endif

