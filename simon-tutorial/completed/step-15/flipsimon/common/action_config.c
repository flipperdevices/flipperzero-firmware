#include "action_config_i.h"

/**
 * @brief color_up_changed is called when the color up setting is changed.
 * @param item The VariableItem that was changed.
 */
static void color_up_changed(VariableItem* item) {
    ActionModel* action_model = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    action_model_set_color_up(action_model, color_values[index]);
    variable_item_set_current_value_text(item, color_names[index]);
}

/**
 * @brief color_down_changed is called when the color down setting is changed.
 * @param item The VariableItem that was changed.
 */
static void color_down_changed(VariableItem* item) {
    ActionModel* action_model = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    action_model_set_color_down(action_model, color_values[index]);
    variable_item_set_current_value_text(item, color_names[index]);
}

/**
 * @brief tone_changed is called when the tone setting is changed.
 * @param item The VariableItem that was changed.
 */
static void tone_changed(VariableItem* item) {
    ActionModel* action_model = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    action_model_set_frequency(action_model, tone_values[index]);
    variable_item_set_current_value_text(item, tone_names[index]);
}

/**
 * @brief Returns keystroke index for the selected keystroke.
 * @details maps a menu item index into to keystroke index.  Determining the
 * index relies on the fact that the menu items are added in the order of
 * Keystroke, Count, Keystroke, Count, etc.  and then finally Add Keystroke.
 * @param action_model The ActionModel.
 * @return The keystroke index
*/
static uint8_t keystroke_item_index(ActionModel* action_model) {
    ActionConfig* action_config = (ActionConfig*)action_model_get_action_config(action_model);
    uint8_t add_item_index = action_model_get_keystroke_index(action_model);
    uint8_t count = action_model_get_keystrokes_count(action_model);
    uint8_t offset = add_item_index - (count * 2);
    uint8_t selected_item_index =
        variable_item_list_get_selected_item_index(action_config->item_list);
    uint8_t item_index = (selected_item_index - offset) / 2;
    FURI_LOG_D(TAG, "item_index=%d", item_index);
    return item_index;
}

/**
 * @brief keystroke_changed is called when the keystroke setting is changed.
 * @details keystroke_changed is called when the keystroke setting is changed using
 * the left/right buttons.  It updates the ActionModel with the new keystroke.
 * @param item The VariableItem that was changed.
 */
static void keystroke_changed(VariableItem* item) {
    ActionModel* action_model = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    uint8_t item_index = keystroke_item_index(action_model);
    Keystroke ks = action_model_get_keystroke(action_model, item_index);
    FURI_LOG_D(TAG, "ks.button_code=%d .count=%d", ks.button_code, ks.count);
    action_model_set_keystroke(action_model, item_index, keystroke_values[index], ks.count);
    variable_item_set_current_value_text(item, keystroke_names[index]);
}

/**
 * @brief keystroke_count_changed is called when the keystroke count setting is changed.
 * @param item The VariableItem that was changed.
*/
static void keystroke_count_changed(VariableItem* item) {
    ActionModel* action_model = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    uint8_t item_index = keystroke_item_index(action_model);
    Keystroke ks = action_model_get_keystroke(action_model, item_index);
    FURI_LOG_D(TAG, "ks.button_code=%d .count=%d", ks.button_code, ks.count);
    action_model_set_keystroke(action_model, item_index, ks.button_code, index);
    variable_item_set_current_value_text(item, keystroke_count_names[index]);
}

/**
 * @brief populate_variable_item_list_color adds a color configuration.
 * @details populate_variable_item_list_color adds a color configuration.  It 
 * adds a VariableItem (config) to the VariableItemList.  It sets the current
 * value index to the index of the color passed in, if it finds a match.
 * @param action_config The ActionConfig.
 * @param action_model The ActionModel.
 * @param label The label for the setting.
 * @param callback The callback for when the setting is changed.
 * @param initial_color The initial color in HEX to default to (RRGGBB).
*/
static void populate_variable_item_list_color(
    ActionConfig* action_config,
    ActionModel* action_model,
    char* label,
    VariableItemChangeCallback callback,
    uint32_t initial_color) {
    VariableItem* item = variable_item_list_add(
        action_config->item_list, label, COUNT_OF(color_names), callback, action_model);
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
 * @param action_config The ActionConfig.
 * @param action_model The ActionModel.
 * @param label The label for the setting.
 * @param callback The callback for when the setting is changed.
 * @param frequency The initial frequency to default to.
*/
static void populate_variable_item_list_frequency(
    ActionConfig* action_config,
    ActionModel* action_model,
    char* label,
    VariableItemChangeCallback callback,
    float frequency) {
    VariableItem* item = variable_item_list_add(
        action_config->item_list, label, COUNT_OF(tone_names), callback, action_model);
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
 * @param action_config The ActionConfig.
 * @param action_model The ActionModel.
 * @return The number of lines added.
*/
static uint8_t
    populate_variable_item_list_keystrokes(ActionConfig* action_config, ActionModel* action_model) {
    uint8_t lines_added = 0;

    uint8_t count = action_model_get_keystrokes_count(action_model);

    for(int j = 0; j < count; j++) {
        Keystroke ks = action_model_get_keystroke(action_model, j);
        FURI_LOG_D("Flipboard", "POPULATE ks.button_code=%d .count=%d", ks.button_code, ks.count);

        VariableItem* item = variable_item_list_add(
            action_config->item_list,
            "Keystroke",
            COUNT_OF(keystroke_names),
            keystroke_changed,
            action_model);
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
            action_config->item_list,
            "Count",
            COUNT_OF(keystroke_count_names),
            keystroke_count_changed,
            action_model);
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
 * @param context The ActionModel.
 * @param index The index of the message that was updated.
 */
static void message_updated(void* context, uint8_t index) {
    ActionModel* action_model = (ActionModel*)context;
    ActionConfig* action_config = (ActionConfig*)action_model_get_action_config(action_model);
    furi_assert(action_config);
    action_model_set_message(action_model, action_model_get_temp_buffer(action_model), index);
    view_dispatcher_switch_to_view(
        action_config->view_dispatcher, action_config->view_item_list_id);
}

/**
 * @brief message_updated is called when the text message 1 is updated.
 * @param context The ActionModel.
 */
static void message1_updated(void* context) {
    message_updated(context, 0);
}

/**
 * @brief message_updated is called when the text message 2 is updated.
 * @param context The ActionModel.
 */
static void message2_updated(void* context) {
    message_updated(context, 1);
}

/**
 * @brief message_updated is called when the text message 3 is updated.
 * @param context The ActionModel.
 */
static void message3_updated(void* context) {
    message_updated(context, 2);
}

/**
 * @brief message_updated is called when the text message 4 is updated.
 * @param context The ActionModel.
 */
static void message4_updated(void* context) {
    message_updated(context, 3);
}

/**
 * @brief keystroke_selector_callback is called when a keystroke is selected.
 * @param button_code The button code that was selected.
 * @param context The ActionModel.
*/
static void keystroke_selector_callback(uint16_t button_code, void* context) {
    ActionModel* action_model = (ActionModel*)context;
    ActionConfig* action_config = (ActionConfig*)action_model_get_action_config(action_model);
    uint8_t item = action_model_get_temp_index(action_model);
    Keystroke ks = action_model_get_keystroke(action_model, item);
    if(ks.button_code != button_code) {
        action_model_set_keystroke(action_model, (uint8_t)item, button_code, ks.count);
        populate_variable_item_list(action_config, action_model);
    }
    view_dispatcher_switch_to_view(
        action_config->view_dispatcher, action_config->view_item_list_id);
}

/**
 * @brief item_message_clicked is called when a message is clicked in the config menu.
 * @details item_message_clicked is called when an item is clicked in the config menu.
 * It displays an enter message dialog and will store the message in the ActionModel.
 * @param action_model The ActionModel.
 * @param message_number The message number to edit (0-3).
*/
static void item_message_clicked(ActionModel* action_model, uint8_t message_number) {
    FURI_LOG_D(TAG, "Message index clicked");
    ActionConfig* action_config = (ActionConfig*)action_model_get_action_config(action_model);
    furi_assert(action_config);

    text_input_set_header_text(
        action_config->text_input,
        (message_number == 0) ? "Enter message 1" :
        (message_number == 1) ? "Enter message 2" :
        (message_number == 2) ? "Enter message 3" :
                                "Enter message 4");
    if(action_model_get_message(action_model, message_number)) {
        strncpy(
            action_model_get_temp_buffer(action_model),
            furi_string_get_cstr(action_model_get_message(action_model, message_number)),
            action_model_get_temp_buffer_size(action_model) - 1);
    } else {
        action_model_get_temp_buffer(action_model)[0] = 0;
    }

    view_set_previous_callback(
        text_input_get_view(action_config->text_input),
        get_menu_callback(action_config->view_item_list_id));

    text_input_set_result_callback(
        action_config->text_input,
        (message_number == 0) ? message1_updated :
        (message_number == 1) ? message2_updated :
        (message_number == 2) ? message3_updated :
                                message4_updated,
        action_model,
        action_model_get_temp_buffer(action_model),
        action_model_get_temp_buffer_size(action_model),
        false);

    view_dispatcher_switch_to_view(
        action_config->view_dispatcher, action_config->view_text_input_id);

    return;
}

/**
 * @brief item_clicked is called when an item is clicked in the config menu.
 * @details item_clicked is called when an item is clicked in the config menu.
 * It determines which item was clicked and switches to the appropriate view,
 * if the item has an editor.
 * @param context The ActionModel.
 * @param index The index of the item that was clicked.
*/
static void item_clicked(void* context, uint32_t index) {
    ActionModel* action_model = (ActionModel*)context;
    uint8_t message_index = action_model_get_message_index(action_model);
    if(index >= message_index && index < message_index + 4u) {
        item_message_clicked(action_model, index - message_index);
        return;
    }

    uint8_t keystroke_index = action_model_get_keystroke_index(action_model);
    if(index == keystroke_index) {
        FURI_LOG_D("Flipboard", "Keystroke index clicked");
        ActionConfig* action_config = (ActionConfig*)action_model_get_action_config(action_model);

        uint16_t keycode = 0;
        action_model_append_keystroke(action_model, keycode, 1);

        populate_variable_item_list(action_config, action_model);
        return;
    }

    if(index < keystroke_index) {
        uint32_t count = action_model_get_keystrokes_count(action_model);

        int32_t item = count - ((keystroke_index - index) / 2);

        FURI_LOG_D(
            "Flipboard",
            "Keystroke clicked?  item=%ld   count=%ld   keystroke_index=%d   index=%ld",
            item,
            count,
            keystroke_index,
            index);

        if(item < 0) {
            FURI_LOG_D("Flipboard", "Not keystroke clicked. Ignorning");
            return;
        }

        if(index % 2 == 0) {
            FURI_LOG_D("Flipboard", "Count clicked.  Ignorning");
            return;
        }

        ActionConfig* action_config = (ActionConfig*)action_model_get_action_config(action_model);
        if(action_config->keystroke_selector == NULL) {
            return;
        }

        view_set_previous_callback(
            keystroke_selector_get_view(action_config->keystroke_selector),
            get_menu_callback(action_config->view_item_list_id));

        Keystroke keystroke = action_model_get_keystroke(action_model, (uint8_t)item);
        keystroke_selector_set_key(action_config->keystroke_selector, keystroke.button_code);
        action_model_set_temp_index(action_model, (uint8_t)item);
        keystroke_selector_set_callback(
            action_config->keystroke_selector, keystroke_selector_callback, action_model);

        view_dispatcher_switch_to_view(
            action_config->view_dispatcher, action_config->view_keystroke_selector_id);

        return;
    }

    FURI_LOG_D("Flipboard", "Unknown index clicked %ld", index);
}

/**
 * @brief populate_variable_item_list adds the variable items to the list.
 * @details populate_variable_item_list adds the variable items to the list.  It starts
 * by resetting the list.  Then it adds the items based on the fields in the
 * ActionConfig and the ActionModel.
 * @param action_config The ActionConfig.
 * @param action_model The ActionModel.
*/
static void populate_variable_item_list(ActionConfig* action_config, ActionModel* action_model) {
    variable_item_list_reset(action_config->item_list);
    uint8_t item_index = 0;

    if(flipboard_model_get_action_model_fields(action_config->model) & ActionModelFieldColorDown) {
        populate_variable_item_list_color(
            action_config,
            action_model,
            "Press color",
            color_down_changed,
            action_model_get_color_down(action_model));
        item_index++;
    }

    if(flipboard_model_get_action_model_fields(action_config->model) & ActionModelFieldColorUp) {
        populate_variable_item_list_color(
            action_config,
            action_model,
            "Release color",
            color_up_changed,
            action_model_get_color_up(action_model));
        item_index++;
    }

    if(flipboard_model_get_action_model_fields(action_config->model) & ActionModelFieldFrequency) {
        populate_variable_item_list_frequency(
            action_config,
            action_model,
            "Music note",
            tone_changed,
            action_model_get_frequency(action_model));
        item_index++;
    }

    if(flipboard_model_get_action_model_fields(action_config->model) &
       ActionModelFieldKeystrokes) {
        item_index += populate_variable_item_list_keystrokes(action_config, action_model);
        variable_item_list_add(action_config->item_list, "Add Keystroke", 0, NULL, NULL);
        variable_item_list_set_enter_callback(
            action_config->item_list, item_clicked, action_model);
        action_model_set_keystroke_index(action_model, item_index);
        item_index++;
    }

    if(flipboard_model_get_action_model_fields(action_config->model) & ActionModelFieldMessage) {
        variable_item_list_add(action_config->item_list, "Message 1", 0, NULL, NULL);
        variable_item_list_set_enter_callback(
            action_config->item_list, item_clicked, action_model);
        action_model_set_message_index(action_model, item_index);
        item_index++;
    }

    if(flipboard_model_get_action_model_fields(action_config->model) & ActionModelFieldMessage) {
        variable_item_list_add(action_config->item_list, "Message 2", 0, NULL, NULL);
        variable_item_list_set_enter_callback(
            action_config->item_list, item_clicked, action_model);
        item_index++;
    }

    if(flipboard_model_get_action_model_fields(action_config->model) & ActionModelFieldMessage) {
        variable_item_list_add(action_config->item_list, "Message 3", 0, NULL, NULL);
        variable_item_list_set_enter_callback(
            action_config->item_list, item_clicked, action_model);
        item_index++;
    }

    if(flipboard_model_get_action_model_fields(action_config->model) & ActionModelFieldMessage) {
        variable_item_list_add(action_config->item_list, "Message 4", 0, NULL, NULL);
        variable_item_list_set_enter_callback(
            action_config->item_list, item_clicked, action_model);
        item_index++;
    }
}

/**
 * @brief item_callback is called when a action is selected in the menu.
 * @details item_callback is called when a action is selected in the menu.  It
 * sets the ActionConfig in the ActionModel.  It then populates the
 * VariableItemList with the settings for the action.  Finally, it switches to
 * the VariableItemList view.
 * @param context The ActionConfig.
 * @param index The index of the action that was selected.
*/
static void item_callback(void* context, uint32_t index) {
    ActionConfig* action_config = (ActionConfig*)context;
    FlipboardModel* model = action_config->model;
    ActionModel* action_model = flipboard_model_get_action_model(model, index);
    if(!action_model) {
        FURI_LOG_E("TAG", "Index=%ld action_model=NULL", index);
    } else {
        FURI_LOG_D(
            "TAG",
            "Index=%ld action_model.action_id=%d",
            index,
            action_model_get_action_id(action_model));
    }

    furi_assert(action_model && action_model_get_action_id(action_model) == index);
    action_model_set_action_config(action_model, action_config);
    populate_variable_item_list(action_config, action_model);
    variable_item_list_set_selected_item(action_config->item_list, 0);

    if(action_config->view_dispatcher) {
        view_dispatcher_switch_to_view(
            action_config->view_dispatcher, action_config->view_item_list_id);
    }
}

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
    uint8_t rows) {
    ActionConfig* action_config = (ActionConfig*)malloc(sizeof(ActionConfig));
    action_config->view_dispatcher = NULL;
    action_config->model = model;
    action_config->menu_actions = submenu_alloc();
    action_config->view_menu_actions_id = config_view_id;
    action_config->text_input = text_input_alloc();
    action_config->view_text_input_id = 0;
    action_config->keystroke_selector =
        (keys == NULL) ? NULL : keystroke_selector_alloc(keys, shift_keys, rows);
    action_config->view_keystroke_selector_id = 0;
    action_config->item_list = variable_item_list_alloc();
    action_config->view_item_list_id = 0;
    view_set_previous_callback(
        variable_item_list_get_view(action_config->item_list), get_menu_callback(config_view_id));

    FuriString* action_name = furi_string_alloc();

    bool single = flipboard_model_get_single_button_mode(model);

    for(int i = 1; i < 16;) {
        furi_string_printf(action_name, "Action %d (", i);
        if(i == 15) {
            furi_string_cat_printf(action_name, "all buttons");
        } else {
            furi_string_cat_printf(action_name, "button");
            if(i != 1 && i != 2 && i != 4 && i != 8) {
                furi_string_cat_printf(action_name, "s");
            }
            furi_string_cat_printf(action_name, " ");
            int btn = 0;
            if(i & 1) {
                furi_string_cat_printf(action_name, "1");
                btn |= 1;
                if(btn != i) {
                    furi_string_cat_printf(action_name, ", ");
                }
            }
            if(i & 2) {
                furi_string_cat_printf(action_name, "2");
                btn |= 2;
                if(btn != i) {
                    furi_string_cat_printf(action_name, ", ");
                }
            }
            if(i & 4) {
                furi_string_cat_printf(action_name, "3");
                btn |= 4;
                if(btn != i) {
                    furi_string_cat_printf(action_name, ", ");
                }
            }
            if(i & 8) {
                furi_string_cat_printf(action_name, "4");
                btn |= 8;
            }
        }
        furi_string_cat_printf(action_name, ")");
        submenu_add_item(
            action_config->menu_actions,
            furi_string_get_cstr(action_name),
            i,
            item_callback,
            action_config);
        if(single) {
            i = i << 1;
        } else {
            i++;
        }
    }
    submenu_set_header(action_config->menu_actions, "Configure Action");

    return action_config;
}

/**
 * @brief action_config_free releases allocated resources.
 * @param action_config The ActionConfig to free.
*/
void action_config_free(ActionConfig* action_config) {
    if(action_config->view_dispatcher != NULL) {
        if(action_config->view_item_list_id) {
            view_dispatcher_remove_view(
                action_config->view_dispatcher, action_config->view_item_list_id);
        }

        if(action_config->view_text_input_id) {
            view_dispatcher_remove_view(
                action_config->view_dispatcher, action_config->view_text_input_id);
        }

        if(action_config->view_keystroke_selector_id) {
            view_dispatcher_remove_view(
                action_config->view_dispatcher, action_config->view_keystroke_selector_id);
        }
    }
    variable_item_list_free(action_config->item_list);
    submenu_free(action_config->menu_actions);
    text_input_free(action_config->text_input);
    if(action_config->keystroke_selector) {
        keystroke_selector_free(action_config->keystroke_selector);
    }
    free(action_config);
}

/**
 * @brief Get view of ActionConfig structure.
 * @details This function return view of ActionConfig structure.  It is used to add ActionConfig
 * view to ViewDispatcher.
 * @param action_config Pointer to ActionConfig structure.
 * @return Pointer to view of ActionConfig structure.
*/
View* action_config_get_view(ActionConfig* action_config) {
    return submenu_get_view(action_config->menu_actions);
}

/**
 * @brief Get view id of ActionConfig structure.
 * @details This function return view id of ActionConfig structure.  It is used to add ActionConfig
 * view to the application menu.
 * @param action_config Pointer to ActionConfig structure.
 * @return View id of ActionConfig structure.
*/
uint32_t action_config_get_view_id(ActionConfig* action_config) {
    return action_config->view_menu_actions_id;
}

/**
 * @brief action_config_register_dispatcher registers the ViewDispatcher.
 * @param action_config The ActionConfig.
 * @param view_dispatcher The ViewDispatcher.
*/
void action_config_register_dispatcher(
    ActionConfig* action_config,
    ViewDispatcher* view_dispatcher) {
    action_config->view_dispatcher = view_dispatcher;
}

/**
 * @brief action_config_register_variable_item_list registers the VariableItemList.
 * @details action_config_register_variable_item_list registers the VariableItemList. The
 * VariableItemList is used to show the configuration of a action.
 * @param action_config The ActionConfig.
 * @param variable_item_list_view_id The view id for the VariableItemList.
*/
void action_config_register_variable_item_list(
    ActionConfig* action_config,
    uint32_t variable_item_list_view_id) {
    furi_assert(action_config->view_dispatcher != NULL);
    action_config->view_item_list_id = variable_item_list_view_id;
    view_dispatcher_add_view(
        action_config->view_dispatcher,
        action_config->view_item_list_id,
        variable_item_list_get_view(action_config->item_list));
}

/**
 * @brief action_config_register_text_input registers the TextInput.
 * @details action_config_register_text_input registers the TextInput. The
 * TextInput is used to enter a message.
 * @param action_config The ActionConfig.
 * @param text_input_id The view id for the TextInput.
*/
void action_config_register_text_input(ActionConfig* action_config, uint32_t text_input_id) {
    furi_assert(action_config->view_dispatcher != NULL);
    action_config->view_text_input_id = text_input_id;
    view_dispatcher_add_view(
        action_config->view_dispatcher,
        action_config->view_text_input_id,
        text_input_get_view(action_config->text_input));
}

/**
 * @brief action_config_register_keystroke_selector registers the KeystrokeSelector.
 * @details action_config_register_keystroke_selector registers the KeystrokeSelector. The
 * KeystrokeSelector is used to select a keystroke.
 * @param action_config The ActionConfig.
 * @param keystroke_selector_id The view id for the KeystrokeSelector.
*/
void action_config_register_keystroke_selector(
    ActionConfig* action_config,
    uint32_t keystroke_selector_id) {
    furi_assert(action_config->view_dispatcher != NULL);
    if(action_config->keystroke_selector == NULL) {
        return;
    }
    action_config->view_keystroke_selector_id = keystroke_selector_id;
    view_dispatcher_add_view(
        action_config->view_dispatcher,
        action_config->view_keystroke_selector_id,
        keystroke_selector_get_view(action_config->keystroke_selector));
}
