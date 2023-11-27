#include "button_config_i.h"

static void populate_variable_item_list(ButtonConfig* button_config, ButtonModel* bm);

/**
 * @brief color_up_changed is called when the color up setting is changed.
 * @param item The VariableItem that was changed.
 */
static void color_up_changed(VariableItem* item) {
    ButtonModel* bm = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    button_model_set_color_up(bm, color_values[index]);
    variable_item_set_current_value_text(item, color_names[index]);
}

/**
 * @brief color_down_changed is called when the color down setting is changed.
 * @param item The VariableItem that was changed.
 */
static void color_down_changed(VariableItem* item) {
    ButtonModel* bm = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    button_model_set_color_down(bm, color_values[index]);
    variable_item_set_current_value_text(item, color_names[index]);
}

/**
 * @brief tone_changed is called when the tone setting is changed.
 * @param item The VariableItem that was changed.
 */
static void tone_changed(VariableItem* item) {
    ButtonModel* bm = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    button_model_set_frequency(bm, tone_values[index]);
    variable_item_set_current_value_text(item, tone_names[index]);
}

/**
 * @brief maps a menu item index into to keystroke index.
 * @details maps a menu item index into to keystroke index.  Determining the
 * index relies on the fact that the menu items are added in the order of
 * Keystroke, Count, Keystroke, Count, etc.  and then finally Add Keystroke.
 * @param bm The ButtonModel.
 * @return The keystroke index
*/
static uint8_t keystroke_item_index(ButtonModel* bm) {
    ButtonConfig* button_config = (ButtonConfig*)button_model_get_button_config(bm);
    uint8_t add_item_index = button_model_get_keystroke_index(bm);
    uint8_t count = button_model_get_keystrokes_count(bm);
    uint8_t offset = add_item_index - (count * 2);
    uint8_t selected_item_index =
        variable_item_list_get_selected_item_index(button_config->item_list);
    uint8_t item_index = (selected_item_index - offset) / 2;
    FURI_LOG_D("Flipboard", "item_index=%d", item_index);
    return item_index;
}

/**
 * @brief keystroke_changed is called when the keystroke setting is changed.
 * @param item The VariableItem that was changed.
 */
static void keystroke_changed(VariableItem* item) {
    ButtonModel* bm = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    uint8_t item_index = keystroke_item_index(bm);
    Keystroke ks = button_model_get_keystroke(bm, item_index);
    FURI_LOG_D("Flipboard", "ks.button_code=%d .count=%d", ks.button_code, ks.count);
    button_model_set_keystroke(bm, item_index, keystroke_values[index], ks.count);
    variable_item_set_current_value_text(item, keystroke_names[index]);
}

/**
 * @brief keystroke_count_changed is called when the keystroke count setting is changed.
 * @param item The VariableItem that was changed.
*/
static void keystroke_count_changed(VariableItem* item) {
    ButtonModel* bm = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    uint8_t item_index = keystroke_item_index(bm);
    Keystroke ks = button_model_get_keystroke(bm, item_index);
    FURI_LOG_D("Flipboard", "ks.button_code=%d .count=%d", ks.button_code, ks.count);
    button_model_set_keystroke(bm, item_index, ks.button_code, index);
    variable_item_set_current_value_text(item, keystroke_count_names[index]);
}

/**
 * @brief populate_variable_item_list_color adds a color configuration.
 * @details populate_variable_item_list_color adds a color configuration.  It 
 * adds a VariableItem (config) to the VariableItemList.  It sets the current
 * value index to the index of the color passed in, if it finds a match.
 * @param button_config The ButtonConfig.
 * @param bm The ButtonModel.
 * @param label The label for the setting.
 * @param callback The callback for when the setting is changed.
 * @param initial_color The initial color in HEX to default to (RRGGBB).
*/
static void populate_variable_item_list_color(
    ButtonConfig* button_config,
    ButtonModel* bm,
    char* label,
    VariableItemChangeCallback callback,
    uint32_t initial_color) {
    VariableItem* item = variable_item_list_add(
        button_config->item_list, label, COUNT_OF(color_names), callback, bm);
    uint8_t index = 0;
    for(size_t i = 0; i < COUNT_OF(color_values); i++) {
        if(initial_color == color_values[i]) {
            index = i;
            break;
        }
    }
    variable_item_set_current_value_index(item, index);
    variable_item_set_current_value_text(item, color_names[index]);
}

/**
 * @brief populate_variable_item_list_frequency adds a frequency configuration.
 * @details populate_variable_item_list_frequency adds a frequency configuration.
 * It sets the current value index to the index of the frequency passed in, if
 * it finds a match.
 * @param button_config The ButtonConfig.
 * @param bm The ButtonModel.
 * @param label The label for the setting.
 * @param callback The callback for when the setting is changed.
 * @param frequency The initial frequency to default to.
*/
static void populate_variable_item_list_frequency(
    ButtonConfig* button_config,
    ButtonModel* bm,
    char* label,
    VariableItemChangeCallback callback,
    float frequency) {
    VariableItem* item = variable_item_list_add(
        button_config->item_list, label, COUNT_OF(tone_names), callback, bm);
    uint8_t index = 0;
    for(size_t i = 0; i < COUNT_OF(tone_values); i++) {
        float diff = frequency - tone_values[i];
        if(diff < 0.0f) diff = -diff;
        if(diff < 1.0f) {
            index = i;
            break;
        }
    }
    variable_item_set_current_value_index(item, index);
    variable_item_set_current_value_text(item, tone_names[index]);
}

/**
 * @brief populate_variable_item_list_keystrokes adds keystroke and count configurations.
 * @param button_config The ButtonConfig.
 * @param bm The ButtonModel.
 * @return The number of lines added.
*/
static uint8_t
    populate_variable_item_list_keystrokes(ButtonConfig* button_config, ButtonModel* bm) {
    uint8_t lines_added = 0;

    uint8_t count = button_model_get_keystrokes_count(bm);

    for(int j = 0; j < count; j++) {
        Keystroke ks = button_model_get_keystroke(bm, j);
        FURI_LOG_D("Flipboard", "POPULATE ks.button_code=%d .count=%d", ks.button_code, ks.count);

        VariableItem* item = variable_item_list_add(
            button_config->item_list,
            "Keystroke",
            COUNT_OF(keystroke_names),
            keystroke_changed,
            bm);
        lines_added++;
        uint8_t index = 0;
        for(size_t i = 0; i < COUNT_OF(keystroke_names); i++) {
            if(keystroke_values[i] == ks.button_code) {
                index = i;
                break;
            }
        }
        variable_item_set_current_value_index(item, index);
        variable_item_set_current_value_text(item, keystroke_names[index]);

        item = variable_item_list_add(
            button_config->item_list,
            "Count",
            COUNT_OF(keystroke_count_names),
            keystroke_count_changed,
            bm);
        lines_added++;
        index = COUNT_OF(keystroke_count_names) - 1;
        for(size_t i = 0; i < COUNT_OF(keystroke_count_names); i++) {
            if(i == ks.count) {
                index = i;
                break;
            }
        }
        variable_item_set_current_value_index(item, index);
        variable_item_set_current_value_text(item, keystroke_count_names[index]);
    }

    return lines_added;
}

/**
 * @brief message_updated is called when the text message is updated.
 * @param context The ButtonModel.
 */
static void message_updated(void* context) {
    ButtonModel* bm = (ButtonModel*)context;
    ButtonConfig* button_config = (ButtonConfig*)button_model_get_button_config(bm);
    furi_assert(button_config);
    button_model_set_message(bm, button_model_get_temp_buffer(bm));
    view_dispatcher_switch_to_view(
        button_config->view_dispatcher, button_config->view_item_list_id);
}

/**
 * @brief keystroke_selector_callback is called when a keystroke is selected.
 * @param button_code The button code that was selected.
 * @param context The ButtonModel.
*/
static void keystroke_selector_callback(uint16_t button_code, void* context) {
    ButtonModel* bm = (ButtonModel*)context;
    ButtonConfig* button_config = (ButtonConfig*)button_model_get_button_config(bm);
    uint8_t item = button_model_get_temp_index(bm);
    Keystroke ks = button_model_get_keystroke(bm, item);
    if(ks.button_code != button_code) {
        button_model_set_keystroke(bm, (uint8_t)item, button_code, ks.count);
        populate_variable_item_list(button_config, bm);
    }
    view_dispatcher_switch_to_view(
        button_config->view_dispatcher, button_config->view_item_list_id);
}

/**
 * @brief item_clicked is called when an item is clicked in the config menu.
 * @details item_clicked is called when an item is clicked in the config menu.
 * It determines which item was clicked and switches to the appropriate view,
 * if the item has an editor.
 * @param context The ButtonModel.
 * @param index The index of the item that was clicked.
*/
static void item_clicked(void* context, uint32_t index) {
    ButtonModel* bm = (ButtonModel*)context;
    uint8_t message_index = button_model_get_message_index(bm);
    if(index == message_index) {
        FURI_LOG_D("Flipboard", "Message index clicked");
        ButtonConfig* button_config = (ButtonConfig*)button_model_get_button_config(bm);
        furi_assert(button_config);

        text_input_set_header_text(button_config->text_input, "Enter message");
        if(button_model_get_message(bm)) {
            strncpy(
                button_model_get_temp_buffer(bm),
                furi_string_get_cstr(button_model_get_message(bm)),
                button_model_get_temp_buffer_size(bm) - 1);
        } else {
            button_model_get_temp_buffer(bm)[0] = 0;
        }

        view_set_previous_callback(
            text_input_get_view(button_config->text_input),
            get_menu_callback(button_config->view_item_list_id));

        text_input_set_result_callback(
            button_config->text_input,
            message_updated,
            bm,
            button_model_get_temp_buffer(bm),
            button_model_get_temp_buffer_size(bm),
            false);

        view_dispatcher_switch_to_view(
            button_config->view_dispatcher, button_config->view_text_input_id);

        return;
    }

    uint8_t keystroke_index = button_model_get_keystroke_index(bm);
    if(index == keystroke_index) {
        FURI_LOG_D("Flipboard", "Keystroke index clicked");
        ButtonConfig* button_config = (ButtonConfig*)button_model_get_button_config(bm);

        uint16_t keycode = 0;
        button_model_append_keystroke(bm, keycode, 1);

        populate_variable_item_list(button_config, bm);
        return;
    }

    if(index > message_index && index < keystroke_index) {
        uint32_t item = (index - message_index);
        if(item % 2 == 0) {
            FURI_LOG_D("Flipboard", "Count clicked.  Ignorning");
            return;
        }

        item = item / 2;
        ButtonConfig* button_config = (ButtonConfig*)button_model_get_button_config(bm);
        if(button_config->keystroke_selector == NULL) {
            return;
        }

        view_set_previous_callback(
            keystroke_selector_get_view(button_config->keystroke_selector),
            get_menu_callback(button_config->view_item_list_id));

        Keystroke keystroke = button_model_get_keystroke(bm, (uint8_t)item);
        keystroke_selector_set_key(button_config->keystroke_selector, keystroke.button_code);
        button_model_set_temp_index(bm, (uint8_t)item);
        keystroke_selector_set_callback(
            button_config->keystroke_selector, keystroke_selector_callback, bm);

        view_dispatcher_switch_to_view(
            button_config->view_dispatcher, button_config->view_keystroke_selector_id);

        return;
    }

    FURI_LOG_D("Flipboard", "Unknown index clicked %ld", index);
}

/**
 * @brief populate_variable_item_list adds the variable items to the list.
 * @details populate_variable_item_list adds the variable items to the list.  It starts
 * by resetting the list.  Then it adds the items based on the fields in the
 * ButtonConfig and the ButtonModel.
 * @param button_config The ButtonConfig.
 * @param bm The ButtonModel.
*/
static void populate_variable_item_list(ButtonConfig* button_config, ButtonModel* bm) {
    variable_item_list_reset(button_config->item_list);
    uint8_t item_index = 0;

    if(flipboard_model_get_button_model_fields(button_config->model) & ButtonModelFieldColorDown) {
        populate_variable_item_list_color(
            button_config, bm, "Press color", color_down_changed, button_model_get_color_down(bm));
        item_index++;
    }

    if(flipboard_model_get_button_model_fields(button_config->model) & ButtonModelFieldColorUp) {
        populate_variable_item_list_color(
            button_config, bm, "Release color", color_up_changed, button_model_get_color_up(bm));
        item_index++;
    }

    if(flipboard_model_get_button_model_fields(button_config->model) & ButtonModelFieldFrequency) {
        populate_variable_item_list_frequency(
            button_config, bm, "Music note", tone_changed, button_model_get_frequency(bm));
        item_index++;
    }

    if(flipboard_model_get_button_model_fields(button_config->model) & ButtonModelFieldMessage) {
        variable_item_list_add(button_config->item_list, "Message", 0, NULL, NULL);
        variable_item_list_set_enter_callback(button_config->item_list, item_clicked, bm);
        button_model_set_message_index(bm, item_index);
        item_index++;
    }

    if(flipboard_model_get_button_model_fields(button_config->model) &
       ButtonModelFieldKeystrokes) {
        item_index += populate_variable_item_list_keystrokes(button_config, bm);
        variable_item_list_add(button_config->item_list, "Add Keystroke", 0, NULL, NULL);
        variable_item_list_set_enter_callback(button_config->item_list, item_clicked, bm);
        button_model_set_keystroke_index(bm, item_index);
        item_index++;
    }
}

/**
 * @brief item_callback is called when a button is selected in the menu.
 * @details item_callback is called when a button is selected in the menu.  It
 * sets the ButtonConfig in the ButtonModel.  It then populates the
 * VariableItemList with the settings for the button.  Finally, it switches to
 * the VariableItemList view.
 * @param context The ButtonConfig.
 * @param index The index of the button that was selected.
*/
static void item_callback(void* context, uint32_t index) {
    ButtonConfig* button_config = (ButtonConfig*)context;
    FlipboardModel* model = button_config->model;
    ButtonModel* bm = flipboard_model_get_button_model(model, index);
    if(!bm) {
        FURI_LOG_E("TAG", "Index=%ld bm=NULL", index);
    } else {
        FURI_LOG_D("TAG", "Index=%ld bm.button_id=%d", index, button_model_get_button_id(bm));
    }

    furi_assert(bm && button_model_get_button_id(bm) == index);
    button_model_set_button_config(bm, button_config);
    populate_variable_item_list(button_config, bm);
    variable_item_list_set_selected_item(button_config->item_list, 0);

    if(button_config->view_dispatcher) {
        view_dispatcher_switch_to_view(
            button_config->view_dispatcher, button_config->view_item_list_id);
    }
}

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
    uint8_t rows) {
    ButtonConfig* button_config = (ButtonConfig*)malloc(sizeof(ButtonConfig));
    button_config->view_dispatcher = NULL;
    button_config->model = model;
    button_config->menu_buttons = submenu_alloc();
    button_config->view_menu_buttons_id = config_view_id;
    button_config->text_input = text_input_alloc();
    button_config->view_text_input_id = 0;
    button_config->keystroke_selector =
        (keys == NULL) ? NULL : keystroke_selector_alloc(keys, shift_keys, rows);
    button_config->view_keystroke_selector_id = 0;
    button_config->item_list = variable_item_list_alloc();
    button_config->view_item_list_id = 0;
    view_set_previous_callback(
        variable_item_list_get_view(button_config->item_list), get_menu_callback(config_view_id));

    FuriString* button_name = furi_string_alloc();

    bool single = flipboard_model_get_single_button_mode(model);

    int display_count = 0;
    for(int i = 1; i < 16;) {
        display_count++;
        furi_string_printf(button_name, "Action %d (", display_count);
        if(i == 15) {
            furi_string_cat_printf(button_name, "all buttons");
        } else {
            int btn = 0;
            if(i & 1) {
                furi_string_cat_printf(button_name, "btn1");
                btn |= 1;
                if(btn != i) {
                    furi_string_cat_printf(button_name, ", ");
                }
            }
            if(i & 2) {
                furi_string_cat_printf(button_name, "btn2");
                btn |= 2;
                if(btn != i) {
                    furi_string_cat_printf(button_name, ", ");
                }
            }
            if(i & 4) {
                furi_string_cat_printf(button_name, "btn3");
                btn |= 4;
                if(btn != i) {
                    furi_string_cat_printf(button_name, ", ");
                }
            }
            if(i & 8) {
                furi_string_cat_printf(button_name, "btn4");
                btn |= 8;
            }
        }
        furi_string_cat_printf(button_name, ")");
        submenu_add_item(
            button_config->menu_buttons,
            furi_string_get_cstr(button_name),
            i,
            item_callback,
            button_config);
        if(single) {
            i = i << 1;
        } else {
            i++;
        }
    }
    submenu_set_header(button_config->menu_buttons, "Configure Action");

    return button_config;
}

/**
 * @brief button_config_free releases allocated resources.
 * @param button_config The ButtonConfig to free.
*/
void button_config_free(ButtonConfig* button_config) {
    if(button_config->view_dispatcher != NULL) {
        if(button_config->view_item_list_id) {
            view_dispatcher_remove_view(
                button_config->view_dispatcher, button_config->view_item_list_id);
        }

        if(button_config->view_text_input_id) {
            view_dispatcher_remove_view(
                button_config->view_dispatcher, button_config->view_text_input_id);
        }

        if(button_config->view_keystroke_selector_id) {
            view_dispatcher_remove_view(
                button_config->view_dispatcher, button_config->view_keystroke_selector_id);
        }
    }
    variable_item_list_free(button_config->item_list);
    submenu_free(button_config->menu_buttons);
    text_input_free(button_config->text_input);
    if(button_config->keystroke_selector) {
        keystroke_selector_free(button_config->keystroke_selector);
    }
    free(button_config);
}

/**
 * @brief Get view of ButtonConfig structure.
 * @details This function return view of ButtonConfig structure.  It is used to add ButtonConfig
 * view to ViewDispatcher.
 * @param button_config Pointer to ButtonConfig structure.
 * @return Pointer to view of ButtonConfig structure.
*/
View* button_config_get_view(ButtonConfig* button_config) {
    return submenu_get_view(button_config->menu_buttons);
}

/**
 * @brief Get view id of ButtonConfig structure.
 * @details This function return view id of ButtonConfig structure.  It is used to add ButtonConfig
 * view to the application menu.
 * @param button_config Pointer to ButtonConfig structure.
 * @return View id of ButtonConfig structure.
*/
uint32_t button_config_get_view_id(ButtonConfig* button_config) {
    return button_config->view_menu_buttons_id;
}

/**
 * @brief button_config_register_dispatcher registers the ViewDispatcher.
 * @param button_config The ButtonConfig.
 * @param view_dispatcher The ViewDispatcher.
*/
void button_config_register_dispatcher(
    ButtonConfig* button_config,
    ViewDispatcher* view_dispatcher) {
    button_config->view_dispatcher = view_dispatcher;
}

/**
 * @brief button_config_register_variable_item_list registers the VariableItemList.
 * @details button_config_register_variable_item_list registers the VariableItemList. The
 * VariableItemList is used to show the configuration of a button.
 * @param button_config The ButtonConfig.
 * @param variable_item_list_view_id The view id for the VariableItemList.
*/
void button_config_register_variable_item_list(
    ButtonConfig* button_config,
    uint32_t variable_item_list_view_id) {
    furi_assert(button_config->view_dispatcher != NULL);
    button_config->view_item_list_id = variable_item_list_view_id;
    view_dispatcher_add_view(
        button_config->view_dispatcher,
        button_config->view_item_list_id,
        variable_item_list_get_view(button_config->item_list));
}

/**
 * @brief button_config_register_text_input registers the TextInput.
 * @details button_config_register_text_input registers the TextInput. The
 * TextInput is used to enter a message.
 * @param button_config The ButtonConfig.
 * @param text_input_id The view id for the TextInput.
*/
void button_config_register_text_input(ButtonConfig* button_config, uint32_t text_input_id) {
    furi_assert(button_config->view_dispatcher != NULL);
    button_config->view_text_input_id = text_input_id;
    view_dispatcher_add_view(
        button_config->view_dispatcher,
        button_config->view_text_input_id,
        text_input_get_view(button_config->text_input));
}

/**
 * @brief button_config_register_keystroke_selector registers the KeystrokeSelector.
 * @details button_config_register_keystroke_selector registers the KeystrokeSelector. The
 * KeystrokeSelector is used to select a keystroke.
 * @param button_config The ButtonConfig.
 * @param keystroke_selector_id The view id for the KeystrokeSelector.
*/
void button_config_register_keystroke_selector(
    ButtonConfig* button_config,
    uint32_t keystroke_selector_id) {
    furi_assert(button_config->view_dispatcher != NULL);
    if(button_config->keystroke_selector == NULL) {
        return;
    }
    button_config->view_keystroke_selector_id = keystroke_selector_id;
    view_dispatcher_add_view(
        button_config->view_dispatcher,
        button_config->view_keystroke_selector_id,
        keystroke_selector_get_view(button_config->keystroke_selector));
}
