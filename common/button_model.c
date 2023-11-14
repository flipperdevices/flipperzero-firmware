#include "button_model_i.h"

static ButtonModelFields
    button_model_load_has_id(uint16_t button_id, FlipperFormat* flipper_format);
static void button_model_load_fields(ButtonModel* model, FlipperFormat* flipper_format);

/**
 * @brief Allocates a new button model.
 * @param button_id The button id for the model.
 * @return The new button model.
 */
ButtonModel* button_model_alloc(uint8_t button_id) {
    ButtonModel* model = malloc(sizeof(ButtonModel));
    model->button_id = button_id;
    model->color_up = 0x000000;
    model->color_down = 0x000000;
    model->frequency = 0.0f;
    model->keystrokes_count = 0;
    model->keystrokes = NULL;
    model->message = NULL;
    model->message_index = 0;
    model->temp_buffer_size = 30;
    model->temp_buffer = (char*)malloc(sizeof(char) * model->temp_buffer_size);
    return model;
}

/**
 * @brief Allocates a new button model from a FlipperFormat.
 * @param button_id The button id for the model.
 * @param flipper_format The FlipperFormat to load from.
 * @return The new button model.
*/
ButtonModel* button_model_alloc_from_ff(uint8_t button_id, FlipperFormat* flipper_format) {
    ButtonModelFields fields = button_model_load_has_id(button_id, flipper_format);
    if(fields == ButtonModelFieldNone) {
        return NULL;
    }

    ButtonModel* model = button_model_alloc(button_id);
    button_model_load_fields(model, flipper_format);
    return model;
}

/**
 * @brief Frees a button model.
 * @param model The model to free.
 */
void button_model_free(ButtonModel* model) {
    if(model->keystrokes) {
        free(model->keystrokes);
    }
    if(model->message) {
        furi_string_free(model->message);
    }
    if(model->temp_buffer) {
        free(model->temp_buffer);
    }
    free(model);
}

/**
 * @brief Gets the button id for the model.
 * @param model The model to get the button id for.
 * @return The button id for the model.
 */
uint8_t button_model_get_button_id(ButtonModel* model) {
    return model->button_id;
}

/**
 * @brief Gets the HEX color when the button is not pressed.
 * @param model The model to get the color for.
 * @return The hex color for when button is up.
 */
uint32_t button_model_get_color_up(ButtonModel* model) {
    return model->color_up;
}

/**
 * @brief Gets the HEX color when the button is pressed.
 * @param model The model to get the color for.
 * @return The hex color for when button is pressed down.
 */
uint32_t button_model_get_color_down(ButtonModel* model) {
    return model->color_down;
}

/**
 * @brief Gets the index of the menu item for editing the message.
 * @param model The model to get the message index for.
 * @return The index of the menu item for editing the message.
 */
uint8_t button_model_get_message_index(ButtonModel* model) {
    return model->message_index;
}

/**
 * @brief Gets the index of the menu item for adding a keystroke.
 * @param model The model to get the keystroke index for.
 * @return The index of the menu item for adding a keystroke.
 */
uint8_t button_model_get_keystroke_index(ButtonModel* model) {
    return model->keystroke_index;
}

/**
 * @brief Gets the temp buffer for editing the message.
 * @param model The model to get the temp buffer for.
 * @return The temp buffer for editing the message.
 */
char* button_model_get_temp_buffer(ButtonModel* model) {
    return model->temp_buffer;
}

/**
 * @brief Gets the size of the temp buffer for editing the message.
 * @param model The model to get the temp buffer size for.
 * @return The size of the temp buffer for editing the message.
 */
size_t button_model_get_temp_buffer_size(ButtonModel* model) {
    return model->temp_buffer_size;
}

/**
 * @brief Gets the index of the item being edited.
 * @param model The model to get the temp index for.
 * @return The index of the item being edited.
 */
uint8_t button_model_get_temp_index(ButtonModel* model) {
    return model->temp_index;
}

/**
 * @brief Gets the button config associated with this model.
 * @param model The model to get the button config for.
 * @return The button config associated with this model.
 */
void* button_model_get_button_config(ButtonModel* model) {
    return model->button_config;
}

/**
 * @brief Gets the frequency for the button, in Hz.
 * @param model The model to get the frequency for.
 * @return The frequency for the button.
 */
float button_model_get_frequency(ButtonModel* model) {
    if(model == NULL) {
        return 0.0f;
    }

    return model->frequency;
}

/**
 * @brief Gets the number of keystrokes for the button.
 * @param model The model to get the keystrokes count for.
 * @return The number of keystrokes for the button.
 */
uint8_t button_model_get_keystrokes_count(ButtonModel* model) {
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
Keystroke button_model_get_keystroke(ButtonModel* model, uint8_t index) {
    if(index < model->keystrokes_count) {
        return model->keystrokes[index];
    }

    Keystroke no_keystroke;
    no_keystroke.count = 0;
    no_keystroke.button_code = 0;
    return no_keystroke;
}

/**
 * @brief Gets the message for the button.
 * @param model The model to get the message for.
 * @return The message for the button.
 */
FuriString* button_model_get_message(ButtonModel* model) {
    if(model == NULL) {
        return NULL;
    }

    return model->message;
}

/**
 * @brief Sets the HEX color when the button is not pressed.
 * @param model The model to set the color for.
 * @param color_up The hex color for when button is up.
 */
void button_model_set_color_up(ButtonModel* model, uint32_t color_up) {
    model->color_up = color_up;
}

/**
 * @brief Sets the HEX color when the button is pressed.
 * @param model The model to set the color for.
 * @param color_down The hex color for when button is pressed down.
 */
void button_model_set_color_down(ButtonModel* model, uint32_t color_down) {
    model->color_down = color_down;
}

/**
 * @brief Sets the index of the menu item for editing the message.
 * @param model The model to set the message index for.
 * @param index The index of the menu item for editing the message.
 */
void button_model_set_message_index(ButtonModel* model, uint8_t index) {
    model->message_index = index;
}

/**
 * @brief Sets the index of the menu item for adding a keystroke.
 * @param model The model to set the keystroke index for.
 * @param index The index of the menu item for adding a keystroke.
 */
void button_model_set_keystroke_index(ButtonModel* model, uint8_t index) {
    model->keystroke_index = index;
}

/**
 * @brief Sets the index of the item being edited.
 * @param model The model to set the temp index for.
 */
void button_model_set_temp_index(ButtonModel* model, uint8_t index) {
    model->temp_index = index;
}

/**
 * @brief Sets the button config associated with this model.
 * @param model The model to set the button config for.
 * @param button_config The button config associated with this model.
 */
void button_model_set_button_config(ButtonModel* model, void* button_config) {
    model->button_config = button_config;
}

/**
 * @brief Sets the frequency for the button, in Hz.
 * @param model The model to set the frequency for.
 * @param frequency The frequency for the button.
 */
void button_model_set_frequency(ButtonModel* model, float frequency) {
    model->frequency = frequency;
}

/**
 * @brief Sets the keystrokes and count for the button.
 * @param model The model to set the keystrokes count for.
 * @param index The index of the keystroke to set.
 * @param button_code The key code to send when this key is pressed.
 * @param count The number of keystrokes for the button.
 * @return True if the keystroke was set, false otherwise.
 */
bool button_model_set_keystroke(
    ButtonModel* model,
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
 * @brief Appends a keystroke to the button.
 * @param model The model to append the keystroke to.
 * @param button_code The key code to send when this key is pressed.
 * @param count The number of keystrokes for the button.
 */
void button_model_append_keystroke(ButtonModel* model, uint16_t button_code, uint8_t count) {
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
 * @brief Removes the last keystroke from the button.
 * @param model The model to remove the keystroke from.
 * @return True if the keystroke was removed, false otherwise.
 */
bool button_model_remove_last_keystroke(ButtonModel* model) {
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
 * @brief Sets the message for the button.
 * @details Sets the message for the button. If the message is a space character, it will be
 * be considered as empty string.
 * @param model The model to set the message for.
 * @param message The message for the button.
 */
void button_model_set_message(ButtonModel* model, const char* message) {
    if(message != NULL && message[0] == ' ') {
        // Hack since we can't clear the message.
        message++;
    }

    if(message == NULL || message[0] == '\0') {
        if(model->message) {
            furi_string_free(model->message);
        }
        model->message = NULL;
        return;
    }

    if(model->message == NULL) {
        model->message = furi_string_alloc();
    }
    furi_string_set(model->message, message);
}

/**
 * @brief Saves the button model to a FlipperFormat.
 * @param model The model to save.
 * @param flipper_format The FlipperFormat to save to.
 * @param fields The fields to save.
 * @return True if the model was saved, false otherwise.
 */
bool button_model_save(ButtonModel* model, FlipperFormat* flipper_format, ButtonModelFields fields) {
    if(!flipper_format_write_comment_cstr(flipper_format, "")) {
        return false;
    }

    uint32_t data32 = model->button_id;
    if(!flipper_format_write_uint32(flipper_format, "ButtonId", &data32, 1)) {
        return false;
    }

    data32 = fields;
    if(!flipper_format_write_uint32(flipper_format, "Fields", &data32, 1)) {
        return false;
    }

    data32 = button_model_get_color_up(model);
    if((fields & ButtonModelFieldColorUp) &&
       !flipper_format_write_uint32(flipper_format, "ColorUp", &data32, 1)) {
        return false;
    }

    data32 = button_model_get_color_down(model);
    if((fields & ButtonModelFieldColorDown) &&
       !flipper_format_write_uint32(flipper_format, "ColorDown", &data32, 1)) {
        return false;
    }

    float dataf = button_model_get_frequency(model);
    if((fields & ButtonModelFieldFrequency) &&
       !flipper_format_write_float(flipper_format, "Frequency", &dataf, 1)) {
        return false;
    }

    FuriString* str = button_model_get_message(model);
    if((fields & ButtonModelFieldMessage)) {
        FuriString* temp_str = NULL;
        if(str == NULL) {
            temp_str = furi_string_alloc();
        }
        if(!flipper_format_write_string(flipper_format, "Message", str ? str : temp_str)) {
            if(temp_str) {
                furi_string_free(temp_str);
            }
            return false;
        }
        if(temp_str) {
            furi_string_free(temp_str);
        }
    }

    uint16_t size = button_model_get_keystrokes_count(model);
    data32 = size;
    if((fields & ButtonModelFieldKeystrokes) &&
       !flipper_format_write_uint32(flipper_format, "KeystrokeCount", &data32, 1)) {
        return false;
    }

    if((fields & ButtonModelFieldKeystrokes) && size != 0) {
        uint32_t* info = malloc(sizeof(uint32_t) * 2 * size);
        for(uint8_t i = 0; i < size; i++) {
            Keystroke keystroke = button_model_get_keystroke(model, i);
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
 * @brief Searches a FlipperFormat for a KeyId/ButtonId and returns the Fields.
 * @param button_id The button id to search for.
 * @param flipper_format The FlipperFormat to search in.
 * @return The fields that were loaded (ButtonModelFieldNone if not found)
*/
static ButtonModelFields
    button_model_load_has_id(uint16_t button_id, FlipperFormat* flipper_format) {
    uint32_t data32;
    flipper_format_rewind(flipper_format);
    while(flipper_format_read_uint32(flipper_format, "ButtonId", &data32, 1)) {
        if(data32 == button_id) {
            if(flipper_format_read_uint32(flipper_format, "Fields", &data32, 1)) {
                return (ButtonModelFields)data32;
            }
            return (ButtonModelFields)ButtonModelFieldNone;
        }
    }
    flipper_format_rewind(flipper_format);
    while(flipper_format_read_uint32(flipper_format, "KeyId", &data32, 1)) {
        if(data32 == button_id) {
            if(flipper_format_read_uint32(flipper_format, "Fields", &data32, 1)) {
                return (ButtonModelFields)data32;
            }
            return (ButtonModelFields)ButtonModelFieldNone;
        }
    }

    return (ButtonModelFields)ButtonModelFieldNone;
}

/**
 * @brief Loads the button model from a FlipperFormat.
 * @param model The model to load.
 * @param flipper_format The FlipperFormat to load from.
*/
static void button_model_load_fields(ButtonModel* model, FlipperFormat* flipper_format) {
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
            button_model_set_message(model, furi_string_get_cstr(message));
        }
    }

    if(flipper_format_read_uint32(flipper_format, "KeystrokeCount", &data32, 1)) {
        uint16_t num_entries = (uint16_t)data32;
        if(num_entries != 0) {
            uint16_t num_ints = num_entries * 2;
            uint32_t* info = malloc(sizeof(uint32_t) * num_ints);
            if(flipper_format_read_uint32(flipper_format, "Keystrokes", info, num_ints)) {
                for(uint8_t i = 0; i < num_entries; i++) {
                    if(info[i * 2]) {
                        button_model_append_keystroke(model, info[i * 2], info[i * 2 + 1]);
                    }
                }
            }
            free(info);
        }
    }

    furi_string_free(message);
}

/**
 * @brief Loads the button model from a FlipperFormat.
 * @param model The model to load.
 * @param flipper_format The FlipperFormat to load from.
 * @return The fields that were loaded (ButtonModelFieldNone if not found)
*/
ButtonModelFields button_model_load(ButtonModel* model, FlipperFormat* flipper_format) {
    ButtonModelFields fields = button_model_load_has_id(model->button_id, flipper_format);
    if(fields) {
        button_model_load_fields(model, flipper_format);
    }

    return fields;
}
