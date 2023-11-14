#pragma once

/**
 * @file button_config.h
 * @brief This file contains the ButtonConfig type and related functions.
 * @details This file contains the ButtonConfig type and related functions.
 * The button_config module is responsible for managing the configuration of a
 * button on the Flipboard.  When you press multiple buttons at the same time,
 * it is treated as a virtual button (so there are really 15 buttons that can
 * be configured).
*/

#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include "keystroke_selector.h"

typedef struct FlipboardModel FlipboardModel;
typedef struct ButtonConfig ButtonConfig;

/**
 * @brief Allocate and initialize ButtonConfig structure.
 * @details Allocate and initialize ButtonConfig structure.  Applications can
 * pass in a list of keys to be used for the keystroke selector.
 * @param model The FlipboardModel.
 * @param config_view_id The view id for the configure view.
 * @param keys The list of keys to be used for the keystroke selector.
 * @param shift_keys The list of shift keys to be used for the keystroke selector.
 * @param rows The number of rows in the keystroke selector.  KEYSTROKE_SELECTOR_COLS is
 * used for the number of columns.
 */
ButtonConfig* button_config_alloc(
    FlipboardModel* model,
    uint32_t config_view_id,
    KeystrokeSelectorKey* keys,
    KeystrokeSelectorKey* shift_keys,
    uint8_t keyboard_rows);

/**
 * @brief button_config_free releases allocated resources.
 * @param button_config The ButtonConfig to free.
 */
void button_config_free(ButtonConfig* button_config);

/**
 * @brief Get view of ButtonConfig structure.
 * @details This function return view of ButtonConfig structure.  It is used to add ButtonConfig
 * view to ViewDispatcher.
 * @param button_config Pointer to ButtonConfig structure.
 * @return Pointer to view of ButtonConfig structure.
 */
View* button_config_get_view(ButtonConfig* button_config);

/**
 * @brief Get view id of ButtonConfig structure.
 * @details This function return view id of ButtonConfig structure.  It is used to add ButtonConfig
 * view to the application menu.
 * @param button_config Pointer to ButtonConfig structure.
 * @return View id of ButtonConfig structure.
 */
uint32_t button_config_get_view_id(ButtonConfig* button_config);

/**
 * @brief button_config_register_dispatcher registers the ViewDispatcher.
 * @param button_config The ButtonConfig.
 * @param view_dispatcher The ViewDispatcher.
 */
void button_config_register_dispatcher(
    ButtonConfig* button_config,
    ViewDispatcher* view_dispatcher);

/**
 * @brief button_config_register_variable_item_list registers the VariableItemList.
 * @details button_config_register_variable_item_list registers the VariableItemList. The
 * VariableItemList is used to show the configuration of a button.
 * @param button_config The ButtonConfig.
 * @param variable_item_list_view_id The view id for the VariableItemList.
 */
void button_config_register_variable_item_list(
    ButtonConfig* button_config,
    uint32_t variable_item_list_view_id);

/**
 * @brief button_config_register_text_input registers the TextInput.
 * @details button_config_register_text_input registers the TextInput. The
 * TextInput is used to enter a message.
 * @param button_config The ButtonConfig.
 * @param text_input_id The view id for the TextInput.
*/
void button_config_register_text_input(ButtonConfig* button_config, uint32_t text_input_id);

/**
 * @brief button_config_register_keystroke_selector registers the KeystrokeSelector.
 * @details button_config_register_keystroke_selector registers the KeystrokeSelector. The
 * KeystrokeSelector is used to select a keystroke.
 * @param button_config The ButtonConfig.
 * @param keystroke_selector_id The view id for the KeystrokeSelector.
*/
void button_config_register_keystroke_selector(
    ButtonConfig* button_config,
    uint32_t keystroke_selector_id);
