#pragma once

#include <furi.h>
#include <flipper_format/flipper_format.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct KeySettingModel KeySettingModel;

typedef struct {
    uint16_t key_code;
    uint8_t count;
} Keystroke;

typedef enum {
    KeySettingModelFieldNone = 0,
    KeySettingModelFieldColorUp = 1 << 0,
    KeySettingModelFieldColorDown = 1 << 1,
    KeySettingModelFieldFrequency = 1 << 2,
    KeySettingModelFieldMessage = 1 << 3,
    KeySettingModelFieldKeystrokes = 1 << 4,
    KeySettingModelFieldAll = (1 << 5) - 1,
} KeySettingModelFields;

KeySettingModel* key_setting_model_alloc(uint8_t key_id);
KeySettingModel* key_setting_model_alloc_from_ff(uint8_t key_id, FlipperFormat* flipper_format);
void key_setting_model_free(KeySettingModel* model);

bool key_setting_model_save(
    KeySettingModel* model,
    FlipperFormat* flipper_format,
    KeySettingModelFields fields);
KeySettingModelFields
    key_setting_model_load(KeySettingModel* model, FlipperFormat* flipper_format);

uint8_t key_setting_model_get_key_id(KeySettingModel* model);

uint32_t key_setting_model_get_color_up(KeySettingModel* model);
void key_setting_model_set_color_up(KeySettingModel* model, uint32_t color_up);

uint32_t key_setting_model_get_color_down(KeySettingModel* model);
void key_setting_model_set_color_down(KeySettingModel* model, uint32_t color_down);

void key_setting_model_set_message_index(KeySettingModel* model, uint8_t index);
uint8_t key_setting_model_get_message_index(KeySettingModel* model);
char* key_setting_model_get_temp_buffer(KeySettingModel* model);
size_t key_setting_model_get_temp_buffer_size(KeySettingModel* model);

void key_setting_model_set_key_config(KeySettingModel* model, void* key_config);
void* key_setting_model_get_key_config(KeySettingModel* model);

float key_setting_model_get_frequency(KeySettingModel* model);
void key_setting_model_set_frequency(KeySettingModel* model, float frequency);

uint8_t key_setting_model_get_keystrokes_count(KeySettingModel* model);
Keystroke key_setting_model_get_keystroke(KeySettingModel* model, uint8_t index);
bool key_setting_model_set_keystroke(
    KeySettingModel* model,
    uint8_t index,
    uint16_t key_code,
    uint8_t count);
void key_setting_model_append_keystroke(KeySettingModel* model, uint16_t key_code, uint8_t count);
bool key_setting_model_remove_last_keystroke(KeySettingModel* model);
void key_setting_model_set_keystroke_index(KeySettingModel* model, uint8_t index);
uint8_t key_setting_model_get_keystroke_index(KeySettingModel* model);

FuriString* key_setting_model_get_message(KeySettingModel* model);
void key_setting_model_set_message(KeySettingModel* model, const char* message);

#ifdef __cplusplus
}
#endif
