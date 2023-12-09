/**
 * @file action_config.h
 * @brief This file contains the ActionConfig type and related functions.
 * @details This file contains the ActionConfig type and related functions.
 * The action_config module is responsible for managing the configuration of a
 * action on the Flipboard.  An action may consist of multiple buttons at the
 * same time (so there are 15 actions that can be configured).
*/

#pragma once

#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include "keystroke_selector.h"

typedef struct FlipboardModel FlipboardModel;
typedef struct ActionConfig ActionConfig;

/**
 * @brief Allocate and initialize ActionConfig structure.
 * @details Allocate and initialize ActionConfig structure.  Applications can
 * pass in a list of keys to be used for the keystroke selector.
 * @param model The FlipboardModel.
 * @param config_view_id The view id for the configure view.
 * @param keys The list of keys to be used for the keystroke selector.
 * @param shift_keys The list of shift keys to be used for the keystroke selector.
 * @param rows The number of rows in the keystroke selector.  KEYSTROKE_SELECTOR_COLS is
 * used for the number of columns.
 */
ActionConfig* action_config_alloc(
    FlipboardModel* model,
    uint32_t config_view_id,
    KeystrokeSelectorKey* keys,
    KeystrokeSelectorKey* shift_keys,
    uint8_t keyboard_rows);

/**
 * @brief action_config_free releases allocated resources.
 * @param action_config The ActionConfig to free.
 */
void action_config_free(ActionConfig* action_config);

/**
 * @brief Get view of ActionConfig structure.
 * @details This function return view of ActionConfig structure.  It is used to add ActionConfig
 * view to ViewDispatcher.
 * @param action_config Pointer to ActionConfig structure.
 * @return Pointer to view of ActionConfig structure.
 */
View* action_config_get_view(ActionConfig* action_config);

/**
 * @brief Get view id of ActionConfig structure.
 * @details This function return view id of ActionConfig structure.  It is used to add ActionConfig
 * view to the application menu.
 * @param action_config Pointer to ActionConfig structure.
 * @return View id of ActionConfig structure.
 */
uint32_t action_config_get_view_id(ActionConfig* action_config);

/**
 * @brief action_config_register_dispatcher registers the ViewDispatcher.
 * @param action_config The ActionConfig.
 * @param view_dispatcher The ViewDispatcher.
 */
void action_config_register_dispatcher(
    ActionConfig* action_config,
    ViewDispatcher* view_dispatcher);

/**
 * @brief action_config_register_variable_item_list registers the VariableItemList.
 * @details action_config_register_variable_item_list registers the VariableItemList. The
 * VariableItemList is used to show the configuration of a action.
 * @param action_config The ActionConfig.
 * @param variable_item_list_view_id The view id for the VariableItemList.
 */
void action_config_register_variable_item_list(
    ActionConfig* action_config,
    uint32_t variable_item_list_view_id);

/**
 * @brief action_config_register_text_input registers the TextInput.
 * @details action_config_register_text_input registers the TextInput. The
 * TextInput is used to enter a message.
 * @param action_config The ActionConfig.
 * @param text_input_id The view id for the TextInput.
*/
void action_config_register_text_input(ActionConfig* action_config, uint32_t text_input_id);

/**
 * @brief action_config_register_keystroke_selector registers the KeystrokeSelector.
 * @details action_config_register_keystroke_selector registers the KeystrokeSelector. The
 * KeystrokeSelector is used to select a keystroke.
 * @param action_config The ActionConfig.
 * @param keystroke_selector_id The view id for the KeystrokeSelector.
*/
void action_config_register_keystroke_selector(
    ActionConfig* action_config,
    uint32_t keystroke_selector_id);
