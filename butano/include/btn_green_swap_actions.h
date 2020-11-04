/*
 * Copyright (c) 2020 Gustavo Valiente gustavo.valiente@protonmail.com
 * zlib License, see LICENSE file.
 */

#ifndef BTN_GREEN_SWAP_ACTIONS_H
#define BTN_GREEN_SWAP_ACTIONS_H

#include "btn_green_swap.h"
#include "btn_template_actions.h"

/**
 * @brief Manages if green swap must be enabled or not.
 *
 * @ingroup green_swap
 * @ingroup action
 */
namespace btn
{

class green_swap_manager
{

public:
    /**
     * @brief Indicates if green swap is enabled or not.
     */
    [[nodiscard]] static bool get()
    {
        return green_swap::enabled();
    }

    /**
     * @brief Sets if green swap must be enabled or not.
     */
    static void set(bool enabled)
    {
        green_swap::set_enabled(enabled);
    }
};


/**
 * @brief Toggles if green swap must be enabled or not when the action is updated a given number of times.
 *
 * @ingroup green_swap
 * @ingroup action
 */
class green_swap_toggle_action : public bool_toggle_template_action<green_swap_manager>
{

public:
    /**
     * @brief Constructor.
     * @param duration_updates How much times the action has to be updated to toggle
     * if green swap must be inverted or not.
     */
    explicit green_swap_toggle_action(int duration_updates) :
        bool_toggle_template_action(duration_updates)
    {
    }
};

}

#endif
