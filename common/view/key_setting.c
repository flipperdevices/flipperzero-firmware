#include "key_setting.h"

#define DEFAULT_KEY_CODES_COUNT 4

typedef struct {
    uint16_t key_code;
    uint8_t count;
} KeyPress;

typedef struct {
    // Hex color (RRGGBB) for the key when it is not pressed
    uint32_t color_up;

    // Hex color (RRGGBB) for the key when it is pressed
    uint32_t color_down;

    // Frequency (in Hz) of the tone to play when key is pressed
    float frequency;

    // Key codes to send when this key is pressed
    KeyPress* key_codes;

    // Number of key codes saved in key_codes
    uint8_t key_codes_count;

    // Message to send when this key is pressed
    FuriString* message;
} KeySettingModel;

typedef struct {
    KeySettingModel* model;
    VariableItemList* variable_item_list_colors;
} KeySetting;

KeySetting* key_setting_alloc() {
    KeySetting* key_setting = malloc(sizeof(KeySetting));
    KeySettingModel* model = malloc(sizeof(KeySettingModel));
    key_setting->model = model;
    model->key_codes_count = DEFAULT_KEY_CODES_COUNT;
    model->key_codes = malloc(sizeof(KeyPress) * model->key_codes_count);
    model->message = NULL;
    return key_setting;
}

void key_setting_free(KeySetting* key_setting) {
    KeySettingModel* model = key_setting->model;
    if(model->key_codes) {
        free(model->key_codes);
    }

    if(model->message) {
        furi_string_free(model->message);
    }

    free(model);
    free(key_setting);
}