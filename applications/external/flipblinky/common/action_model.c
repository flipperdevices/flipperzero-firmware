#include "action_model_i.h"

/**
 * @brief Allocates a new action model.
 * @param action_id The action id for the model.
 * @return The new action model.
 */
ActionModel* action_model_alloc(uint8_t action_id) {
    ActionModel* model = malloc(sizeof(ActionModel));
    model->action_id = action_id;
    model->color_up = 0x000000;
    model->color_down = 0x000000;
    model->frequency = 0.0f;
    model->keystrokes_count = 0;
    model->keystrokes = NULL;
    model->message[0] = NULL;
    model->message[1] = NULL;
    model->message[2] = NULL;
    model->message[3] = NULL;
    model->message_index = 0;
    model->temp_buffer_size = 30;
    model->temp_buffer = (char*)malloc(sizeof(char) * model->temp_buffer_size);
    return model;
}

/**
 * @brief Allocates a new action model from a FlipperFormat.
 * @param action_id The action id for the model.
 * @param flipper_format The FlipperFormat to load from.
 * @return The new action model.
*/
ActionModel* action_model_alloc_from_ff(uint8_t action_id, FlipperFormat* flipper_format) {
    ActionModelFields fields = action_model_load_has_id(action_id, flipper_format);
    if(fields == ActionModelFieldNone) {
        return NULL;
    }

    ActionModel* model = action_model_alloc(action_id);
    action_model_load_fields(model, flipper_format);
    return model;
}

/**
 * @brief Frees a action model.
 * @param model The model to free.
 */
void action_model_free(ActionModel* model) {
    if(model->keystrokes) {
        free(model->keystrokes);
    }
    for(int i = 0; i < 4; i++) {
        if(model->message[i]) {
            furi_string_free(model->message[i]);
            model->message[i] = NULL;
        }
    }
    if(model->temp_buffer) {
        free(model->temp_buffer);
    }
    free(model);
}

/**
 * @brief Gets the action id for the model.
 * @param model The model to get the action id for.
 * @return The action id for the model.
 */
uint8_t action_model_get_action_id(ActionModel* model) {
    return model->action_id;
}

/**
 * @brief Gets the HEX color when the action is not pressed.
 * @param model The model to get the color for.
 * @return The hex color for when action is up.
 */
uint32_t action_model_get_color_up(ActionModel* model) {
    return model->color_up;
}

/**
 * @brief Gets the HEX color when the action is pressed.
 * @param model The model to get the color for.
 * @return The hex color for when action is pressed down.
 */
uint32_t action_model_get_color_down(ActionModel* model) {
    return model->color_down;
}

/**
 * @brief Gets the index of the menu item for editing the message.
 * @param model The model to get the message index for.
 * @return The index of the menu item for editing the message.
 */
uint8_t action_model_get_message_index(ActionModel* model) {
    return model->message_index;
}

/**
 * @brief Gets the index of the menu item for adding a keystroke.
 * @param model The model to get the keystroke index for.
 * @return The index of the menu item for adding a keystroke.
 */
uint8_t action_model_get_keystroke_index(ActionModel* model) {
    return model->keystroke_index;
}

/**
 * @brief Gets the temp buffer for editing the message.
 * @param model The model to get the temp buffer for.
 * @return The temp buffer for editing the message.
 */
char* action_model_get_temp_buffer(ActionModel* model) {
    return model->temp_buffer;
}

/**
 * @brief Gets the size of the temp buffer for editing the message.
 * @param model The model to get the temp buffer size for.
 * @return The size of the temp buffer for editing the message.
 */
size_t action_model_get_temp_buffer_size(ActionModel* model) {
    return model->temp_buffer_size;
}

/**
 * @brief Gets the index of the item being edited.
 * @param model The model to get the temp index for.
 * @return The index of the item being edited.
 */
uint8_t action_model_get_temp_index(ActionModel* model) {
    return model->temp_index;
}

/**
 * @brief Gets the action config associated with this model.
 * @param model The model to get the action config for.
 * @return The action config associated with this model.
 */
void* action_model_get_action_config(ActionModel* model) {
    return model->action_config;
}

/**
 * @brief Gets the frequency for the action, in Hz.
 * @param model The model to get the frequency for.
 * @return The frequency for the action.
 */
float action_model_get_frequency(ActionModel* model) {
    if(model == NULL) {
        return 0.0f;
    }

    return model->frequency;
}

/**
 * @brief Gets the number of keystrokes for the action.
 * @param model The model to get the keystrokes count for.
 * @return The number of keystrokes for the action.
 */
uint8_t action_model_get_keystrokes_count(ActionModel* model) {
    if(model == NULL) {
        return 0;
    }

    return model->keystrokes_count;
}

/**
 * @brief Gets the keystroke at the given index.
 * @param model The model to get the keystroke for.
 * @param index The index of the keystroke to get.
 * @return The keystroke at the given index.
 */
Keystroke action_model_get_keystroke(ActionModel* model, uint8_t index) {
    if(index < model->keystrokes_count) {
        return model->keystrokes[index];
    }

    Keystroke no_keystroke;
    no_keystroke.count = 0;
    no_keystroke.button_code = 0;
    return no_keystroke;
}

/**
 * @brief Gets the message for the action.
 * @param model The model to get the message for.
 * @param message_number The message number to get.
 * @return The message for the action.
 */
FuriString* action_model_get_message(ActionModel* model, uint8_t message_number) {
    if(model == NULL || message_number >= 4) {
        return NULL;
    }

    return model->message[message_number];
}

/**
 * @brief Sets the HEX color when the action is not pressed.
 * @param model The model to set the color for.
 * @param color_up The hex color for when action is up.
 */
void action_model_set_color_up(ActionModel* model, uint32_t color_up) {
    model->color_up = color_up;
}

/**
 * @brief Sets the HEX color when the action is pressed.
 * @param model The model to set the color for.
 * @param color_down The hex color for when action is pressed down.
 */
void action_model_set_color_down(ActionModel* model, uint32_t color_down) {
    model->color_down = color_down;
}

/**
 * @brief Sets the index of the menu item for editing the message.
 * @param model The model to set the message index for.
 * @param index The index of the menu item for editing the message.
 */
void action_model_set_message_index(ActionModel* model, uint8_t index) {
    model->message_index = index;
}

/**
 * @brief Sets the index of the menu item for adding a keystroke.
 * @param model The model to set the keystroke index for.
 * @param index The index of the menu item for adding a keystroke.
 */
void action_model_set_keystroke_index(ActionModel* model, uint8_t index) {
    model->keystroke_index = index;
}

/**
 * @brief Sets the index of the item being edited.
 * @param model The model to set the temp index for.
 */
void action_model_set_temp_index(ActionModel* model, uint8_t index) {
    model->temp_index = index;
}

/**
 * @brief Sets the action config associated with this model.
 * @param model The model to set the action config for.
 * @param action_config The action config associated with this model.
 */
void action_model_set_action_config(ActionModel* model, void* action_config) {
    model->action_config = action_config;
}

/**
 * @brief Sets the frequency for the action, in Hz.
 * @param model The model to set the frequency for.
 * @param frequency The frequency for the action.
 */
void action_model_set_frequency(ActionModel* model, float frequency) {
    model->frequency = frequency;
}

/**
 * @brief Sets the keystrokes and count for the action.
 * @param model The model to set the keystrokes count for.
 * @param index The index of the keystroke to set.
 * @param button_code The key code to send when this key is pressed.
 * @param count The number of keystrokes for the button.
 * @return True if the keystroke was set, false otherwise.
 */
bool action_model_set_keystroke(
    ActionModel* model,
    uint8_t index,
    uint16_t button_code,
    uint8_t count) {
    if(index < model->keystrokes_count) {
        model->keystrokes[index].count = count;
        model->keystrokes[index].button_code = button_code;
        return true;
    }

    return false;
}

/**
 * @brief Appends a keystroke to the action.
 * @param model The model to append the keystroke to.
 * @param button_code The key code to send when this key is pressed.
 * @param count The number of keystrokes for the button.
 */
void action_model_append_keystroke(ActionModel* model, uint16_t button_code, uint8_t count) {
    model->keystrokes_count++;
    if(model->keystrokes == NULL) {
        model->keystrokes = malloc(sizeof(Keystroke));
    } else {
        model->keystrokes =
            realloc(model->keystrokes, sizeof(Keystroke) * model->keystrokes_count);
    }
    model->keystrokes[model->keystrokes_count - 1].count = count;
    model->keystrokes[model->keystrokes_count - 1].button_code = button_code;
}

/**
 * @brief Removes the last keystroke from the action.
 * @param model The model to remove the keystroke from.
 * @return True if the keystroke was removed, false otherwise.
 */
bool action_model_remove_last_keystroke(ActionModel* model) {
    if(model->keystrokes == NULL || model->keystrokes_count == 0) {
        return false;
    }

    if(model->keystrokes_count == 1) {
        if(model->keystrokes) {
            free(model->keystrokes);
        }
        model->keystrokes = NULL;
        model->keystrokes_count = 0;
        return true;
    }

    model->keystrokes_count--;
    model->keystrokes = realloc(model->keystrokes, sizeof(Keystroke) * model->keystrokes_count);
    return true;
}

/**
 * @brief Sets the message for the action.
 * @details Sets the message for the action. If the message is a space character, it will be
 * be considered as empty string.
 * @param model The model to set the message for.
 * @param message The message for the action.
 * @param message_number The message number to set.
 */
void action_model_set_message(ActionModel* model, const char* message, uint8_t message_number) {
    if(message != NULL && message[0] == ' ') {
        // Hack since we can't clear the message.
        message++;
    }

    if(message == NULL || message[0] == '\0') {
        if(model->message[message_number]) {
            furi_string_free(model->message[message_number]);
        }
        model->message[message_number] = NULL;
        return;
    }

    if(model->message[message_number] == NULL) {
        model->message[message_number] = furi_string_alloc();
    }
    furi_string_set(model->message[message_number], message);
}

/**
 * @brief Saves the action model to a FlipperFormat.
 * @param model The model to save.
 * @param flipper_format The FlipperFormat to save to.
 * @param fields The fields to save.
 * @return True if the model was saved, false otherwise.
 */
bool action_model_save(ActionModel* model, FlipperFormat* flipper_format, ActionModelFields fields) {
    if(!flipper_format_write_comment_cstr(flipper_format, "")) {
        return false;
    }

    uint32_t data32 = model->action_id;
    if(!flipper_format_write_uint32(flipper_format, "ActionId", &data32, 1)) {
        return false;
    }

    data32 = fields;
    if(!flipper_format_write_uint32(flipper_format, "Fields", &data32, 1)) {
        return false;
    }

    data32 = action_model_get_color_up(model);
    if((fields & ActionModelFieldColorUp) &&
       !flipper_format_write_uint32(flipper_format, "ColorUp", &data32, 1)) {
        return false;
    }

    data32 = action_model_get_color_down(model);
    if((fields & ActionModelFieldColorDown) &&
       !flipper_format_write_uint32(flipper_format, "ColorDown", &data32, 1)) {
        return false;
    }

    float dataf = action_model_get_frequency(model);
    if((fields & ActionModelFieldFrequency) &&
       !flipper_format_write_float(flipper_format, "Frequency", &dataf, 1)) {
        return false;
    }

    if((fields & ActionModelFieldMessage)) {
        FuriString* empty_str = furi_string_alloc();

        FuriString* str = action_model_get_message(model, 0);
        if(!flipper_format_write_string(flipper_format, "Message", str ? str : empty_str)) {
            if(empty_str) {
                furi_string_free(empty_str);
            }
            return false;
        }

        str = action_model_get_message(model, 1);
        if(!flipper_format_write_string(flipper_format, "Message2", str ? str : empty_str)) {
            if(empty_str) {
                furi_string_free(empty_str);
            }
            return false;
        }

        str = action_model_get_message(model, 2);
        if(!flipper_format_write_string(flipper_format, "Message3", str ? str : empty_str)) {
            if(empty_str) {
                furi_string_free(empty_str);
            }
            return false;
        }

        str = action_model_get_message(model, 3);
        if(!flipper_format_write_string(flipper_format, "Message4", str ? str : empty_str)) {
            if(empty_str) {
                furi_string_free(empty_str);
            }
            return false;
        }

        if(empty_str) {
            furi_string_free(empty_str);
        }
    }

    uint16_t size = action_model_get_keystrokes_count(model);
    data32 = size;
    if((fields & ActionModelFieldKeystrokes) &&
       !flipper_format_write_uint32(flipper_format, "KeystrokeCount", &data32, 1)) {
        return false;
    }

    if((fields & ActionModelFieldKeystrokes) && size != 0) {
        uint32_t* info = malloc(sizeof(uint32_t) * 2 * size);
        for(uint8_t i = 0; i < size; i++) {
            Keystroke keystroke = action_model_get_keystroke(model, i);
            info[i * 2] = (uint32_t)keystroke.button_code;
            info[i * 2 + 1] = (uint32_t)keystroke.count;
        }
        if(!flipper_format_write_uint32(flipper_format, "Keystrokes", info, size * 2)) {
            free(info);
            return false;
        }
        free(info);
    }

    return true;
}

/**
 * @brief Searches a FlipperFormat for a ActionId/KeyId/ButtonId and returns the Fields.
 * @param action_id The action id to search for.
 * @param flipper_format The FlipperFormat to search in.
 * @return The fields that were loaded (ActionModelFieldNone if not found)
*/
static ActionModelFields
    action_model_load_has_id(uint16_t action_id, FlipperFormat* flipper_format) {
    uint32_t data32;
    flipper_format_rewind(flipper_format);
    while(flipper_format_read_uint32(flipper_format, "ActionId", &data32, 1)) {
        if(data32 == action_id) {
            if(flipper_format_read_uint32(flipper_format, "Fields", &data32, 1)) {
                return (ActionModelFields)data32;
            }
            return (ActionModelFields)ActionModelFieldNone;
        }
    }
    flipper_format_rewind(flipper_format);
    while(flipper_format_read_uint32(flipper_format, "ButtonId", &data32, 1)) {
        if(data32 == action_id) {
            if(flipper_format_read_uint32(flipper_format, "Fields", &data32, 1)) {
                return (ActionModelFields)data32;
            }
            return (ActionModelFields)ActionModelFieldNone;
        }
    }
    flipper_format_rewind(flipper_format);
    while(flipper_format_read_uint32(flipper_format, "KeyId", &data32, 1)) {
        if(data32 == action_id) {
            if(flipper_format_read_uint32(flipper_format, "Fields", &data32, 1)) {
                return (ActionModelFields)data32;
            }
            return (ActionModelFields)ActionModelFieldNone;
        }
    }

    return (ActionModelFields)ActionModelFieldNone;
}

/**
 * @brief Loads the action model from a FlipperFormat.
 * @param model The model to load.
 * @param flipper_format The FlipperFormat to load from.
*/
static void action_model_load_fields(ActionModel* model, FlipperFormat* flipper_format) {
    uint32_t data32;
    float dataf;
    FuriString* message = furi_string_alloc();

    if(flipper_format_read_uint32(flipper_format, "ColorUp", &data32, 1)) {
        model->color_up = data32;
    }

    if(flipper_format_read_uint32(flipper_format, "ColorDown", &data32, 1)) {
        model->color_down = data32;
    }

    if(flipper_format_read_float(flipper_format, "Frequency", &dataf, 1)) {
        model->frequency = dataf;
    }

    if(flipper_format_read_string(flipper_format, "Message", message)) {
        if(furi_string_size(message)) {
            action_model_set_message(model, furi_string_get_cstr(message), 0);
        }
    }

    if(flipper_format_read_string(flipper_format, "Message2", message)) {
        if(furi_string_size(message)) {
            action_model_set_message(model, furi_string_get_cstr(message), 1);
        }

        if(flipper_format_read_string(flipper_format, "Message3", message)) {
            if(furi_string_size(message)) {
                action_model_set_message(model, furi_string_get_cstr(message), 2);
            }
        }

        if(flipper_format_read_string(flipper_format, "Message4", message)) {
            if(furi_string_size(message)) {
                action_model_set_message(model, furi_string_get_cstr(message), 3);
            }
        }
    } else {
        // Message 2 not found, so legacy format.  Rewind to being of Id.
        action_model_load_has_id(model->action_id, flipper_format);
    }

    if(flipper_format_read_uint32(flipper_format, "KeystrokeCount", &data32, 1)) {
        uint16_t num_entries = (uint16_t)data32;
        if(num_entries != 0) {
            uint16_t num_ints = num_entries * 2;
            uint32_t* info = malloc(sizeof(uint32_t) * num_ints);
            if(flipper_format_read_uint32(flipper_format, "Keystrokes", info, num_ints)) {
                for(uint8_t i = 0; i < num_entries; i++) {
                    if(info[i * 2]) {
                        action_model_append_keystroke(model, info[i * 2], info[i * 2 + 1]);
                    }
                }
            }
            free(info);
        }
    }

    furi_string_free(message);
}

/**
 * @brief Loads the action model from a FlipperFormat.
 * @param model The model to load.
 * @param flipper_format The FlipperFormat to load from.
 * @return The fields that were loaded (ActionModelFieldNone if not found)
*/
ActionModelFields action_model_load(ActionModel* model, FlipperFormat* flipper_format) {
    ActionModelFields fields = action_model_load_has_id(model->action_id, flipper_format);
    if(fields) {
        action_model_load_fields(model, flipper_format);
    }

    return fields;
}
