#pragma once

#include <furi.h>
#include "key_monitor.h"
#include "key_setting_model.h"
#include "speaker.h"

typedef struct FlipboardModel FlipboardModel;
typedef struct FlipboardKeyboard FlipboardKeyboard;
typedef struct FlipboardLeds FlipboardLeds;
typedef struct KeySettingModel KeySettingModel;

FlipboardModel*
    flipboard_model_alloc(char* app_name, bool single_button_mode, KeySettingModelFields fields);
char* flipboard_model_get_name(FlipboardModel* model);
bool flipboard_model_get_single_button_mode(FlipboardModel* model);
KeySettingModelFields flipboard_model_get_key_setting_model_fields(FlipboardModel* model);
KeySettingModel* flipboard_model_get_key_setting_model(FlipboardModel* model, uint8_t key);
void flipboard_model_set_key_setting_model(
    FlipboardModel* model,
    uint8_t key,
    KeySettingModel* key_setting_model);
void flipboard_model_set_gui_refresh_speed_ms(FlipboardModel* model, uint32_t update_rate_ms);
KeyMonitor* flipboard_model_get_key_monitor(FlipboardModel* model);
FlipboardKeyboard* flipboard_model_get_keyboard(FlipboardModel* model);
Speaker* flipboard_model_get_speaker(FlipboardModel* model);
FlipboardLeds* flipboard_model_get_leds(FlipboardModel* model);
void flipboard_model_set_key_monitor(
    FlipboardModel* model,
    KeyMonitorEventCallback callback,
    void* context);
void flipboard_model_free(FlipboardModel* model);

void flipboard_model_play_tone(FlipboardModel* model, KeySettingModel* ksm);
void flipboard_model_set_colors(FlipboardModel* model, KeySettingModel* ksm, uint8_t new_key);
void flipboard_model_send_keystrokes(FlipboardModel* model, KeySettingModel* ksm);
void flipboard_model_send_text(FlipboardModel* model, KeySettingModel* ksm);
uint8_t flipboard_model_reduce(FlipboardModel* model, uint8_t new_key, bool left_wins);