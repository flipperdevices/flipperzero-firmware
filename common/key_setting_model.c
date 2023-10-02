#include "key_setting_model_i.h"

KeySettingModel* key_setting_model_alloc(uint8_t key_id) {
    KeySettingModel* model = malloc(sizeof(KeySettingModel));
    model->key_id = key_id;
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

uint8_t key_setting_model_get_key_id(KeySettingModel* model) {
    return model->key_id;
}

uint32_t key_setting_model_get_color_up(KeySettingModel* model) {
    return model->color_up;
}

void key_setting_model_set_color_up(KeySettingModel* model, uint32_t color_up) {
    model->color_up = color_up;
}

uint32_t key_setting_model_get_color_down(KeySettingModel* model) {
    return model->color_down;
}

void key_setting_model_set_color_down(KeySettingModel* model, uint32_t color_down) {
    model->color_down = color_down;
}

void key_setting_model_set_message_index(KeySettingModel* model, uint8_t index) {
    model->message_index = index;
}

uint8_t key_setting_model_get_message_index(KeySettingModel* model) {
    return model->message_index;
}

void key_setting_model_set_keystroke_index(KeySettingModel* model, uint8_t index) {
    model->keystroke_index = index;
}

uint8_t key_setting_model_get_keystroke_index(KeySettingModel* model) {
    return model->keystroke_index;
}

char* key_setting_model_get_temp_buffer(KeySettingModel* model) {
    return model->temp_buffer;
}

size_t key_setting_model_get_temp_buffer_size(KeySettingModel* model) {
    return model->temp_buffer_size;
}

void key_setting_model_set_key_config(KeySettingModel* model, void* key_config) {
    model->key_config = key_config;
}

void* key_setting_model_get_key_config(KeySettingModel* model) {
    return model->key_config;
}

float key_setting_model_get_frequency(KeySettingModel* model) {
    if(model == NULL) {
        return 0.0f;
    }

    return model->frequency;
}

void key_setting_model_set_frequency(KeySettingModel* model, float frequency) {
    model->frequency = frequency;
}

uint8_t key_setting_model_get_keystrokes_count(KeySettingModel* model) {
    if(model == NULL) {
        return 0;
    }

    return model->keystrokes_count;
}

Keystroke key_setting_model_get_keystroke(KeySettingModel* model, uint8_t index) {
    if(index < model->keystrokes_count) {
        return model->keystrokes[index];
    }

    Keystroke no_keystroke;
    no_keystroke.count = 0;
    no_keystroke.key_code = 0;
    return no_keystroke;
}

bool key_setting_model_set_keystroke(
    KeySettingModel* model,
    uint8_t index,
    uint16_t key_code,
    uint8_t count) {
    if(index < model->keystrokes_count) {
        model->keystrokes[index].count = count;
        model->keystrokes[index].key_code = key_code;
        return true;
    }

    return false;
}

void key_setting_model_append_keystroke(KeySettingModel* model, uint16_t key_code, uint8_t count) {
    model->keystrokes_count++;
    if(model->keystrokes == NULL) {
        model->keystrokes = malloc(sizeof(Keystroke));
    } else {
        model->keystrokes =
            realloc(model->keystrokes, sizeof(Keystroke) * model->keystrokes_count);
    }
    model->keystrokes[model->keystrokes_count - 1].count = count;
    model->keystrokes[model->keystrokes_count - 1].key_code = key_code;
}

bool key_setting_model_remove_last_keystroke(KeySettingModel* model) {
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

FuriString* key_setting_model_get_message(KeySettingModel* model) {
    if(model == NULL) {
        return NULL;
    }

    return model->message;
}

void key_setting_model_set_message(KeySettingModel* model, const char* message) {
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

bool key_setting_model_save(
    KeySettingModel* model,
    FlipperFormat* flipper_format,
    KeySettingModelFields fields) {
    if(!flipper_format_write_comment_cstr(flipper_format, "")) {
        return false;
    }

    uint32_t data32 = model->key_id;
    if(!flipper_format_write_uint32(flipper_format, "KeyId", &data32, 1)) {
        return false;
    }

    data32 = fields;
    if(!flipper_format_write_uint32(flipper_format, "Fields", &data32, 1)) {
        return false;
    }

    data32 = key_setting_model_get_color_up(model);
    if((fields & KeySettingModelFieldColorUp) &&
       !flipper_format_write_uint32(flipper_format, "ColorUp", &data32, 1)) {
        return false;
    }

    data32 = key_setting_model_get_color_down(model);
    if((fields & KeySettingModelFieldColorDown) &&
       !flipper_format_write_uint32(flipper_format, "ColorDown", &data32, 1)) {
        return false;
    }

    float dataf = key_setting_model_get_frequency(model);
    if((fields & KeySettingModelFieldFrequency) &&
       !flipper_format_write_float(flipper_format, "Frequency", &dataf, 1)) {
        return false;
    }

    FuriString* str = key_setting_model_get_message(model);
    if((fields & KeySettingModelFieldMessage)) {
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

    uint16_t size = key_setting_model_get_keystrokes_count(model);
    data32 = size;
    if((fields & KeySettingModelFieldKeystrokes) &&
       !flipper_format_write_uint32(flipper_format, "KeystrokeCount", &data32, 1)) {
        return false;
    }

    if((fields & KeySettingModelFieldKeystrokes) && size != 0) {
        uint32_t* info = malloc(sizeof(uint32_t) * 2 * size);
        for(uint8_t i = 0; i < size; i++) {
            Keystroke keystroke = key_setting_model_get_keystroke(model, i);
            info[i * 2] = (uint32_t)keystroke.key_code;
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

static KeySettingModelFields
    key_setting_model_load_has_id(uint16_t key_id, FlipperFormat* flipper_format) {
    uint32_t data32;
    flipper_format_rewind(flipper_format);
    while(flipper_format_read_uint32(flipper_format, "KeyId", &data32, 1)) {
        if(data32 == key_id) {
            if(flipper_format_read_uint32(flipper_format, "Fields", &data32, 1)) {
                return (KeySettingModelFields)data32;
            }
            return (KeySettingModelFields)KeySettingModelFieldNone;
        }
    }

    return (KeySettingModelFields)KeySettingModelFieldNone;
}

static void key_setting_model_load_fields(KeySettingModel* model, FlipperFormat* flipper_format) {
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
            key_setting_model_set_message(model, furi_string_get_cstr(message));
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
                        key_setting_model_append_keystroke(model, info[i * 2], info[i * 2 + 1]);
                    }
                }
            }
            free(info);
        }
    }

    furi_string_free(message);
}

KeySettingModelFields
    key_setting_model_load(KeySettingModel* model, FlipperFormat* flipper_format) {
    KeySettingModelFields fields = key_setting_model_load_has_id(model->key_id, flipper_format);
    if(fields) {
        key_setting_model_load_fields(model, flipper_format);
    }

    return fields;
}

KeySettingModel* key_setting_model_alloc_from_ff(uint8_t key_id, FlipperFormat* flipper_format) {
    KeySettingModelFields fields = key_setting_model_load_has_id(key_id, flipper_format);
    if(fields == KeySettingModelFieldNone) {
        return NULL;
    }

    KeySettingModel* model = key_setting_model_alloc(key_id);
    key_setting_model_load_fields(model, flipper_format);
    return model;
}

void key_setting_model_free(KeySettingModel* model) {
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