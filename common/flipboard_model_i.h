#pragma once

#include "flipboard_model.h"

#include "flipboard_file.h"
#include "backlight.h"
#include "leds.h"
#include "keyboard.h"
#include "key_monitor.h"
#include "speaker.h"

#include <gui/gui.h>

typedef struct KeyMonitor KeyMonitor;

struct FlipboardModel {
    KeySettingModel* key_setting_model[16];
    KeySettingModelFields key_setting_model_fields;

    char* name;
    FlipboardKeyboard* keyboard;
    FlipboardLeds* leds;
    KeyMonitor* key_monitor;
    Speaker* speaker;
    FuriTimer* gui_refresh_timer;

    bool single_button_mode;
    bool has_speaker;
    bool backlight_always_on;
};