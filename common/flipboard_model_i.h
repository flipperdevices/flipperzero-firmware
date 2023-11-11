#pragma once

#include "flipboard_model.h"

#include "flipboard_file.h"
#include "backlight.h"
#include "leds.h"
#include "keyboard.h"
#include "key_monitor.h"
#include "speaker.h"

#include <gui/gui.h>

/**
 * @brief    The flipboard model struct.
 * @details  This struct contains all the data needed for the flipboard model.
 */
struct FlipboardModel {
    // KeySettingModel for each of the button combinations
    KeySettingModel* key_setting_model[16];

    // The fields of the KeySettingModel that are currently active
    KeySettingModelFields key_setting_model_fields;

    // The name of the model (used for saving and loading)
    char* name;

    // HID keyboard driver
    FlipboardKeyboard* keyboard;

    // LED driver
    FlipboardLeds* leds;

    // Key monitor that detects button presses
    KeyMonitor* key_monitor;

    // Speaker driver
    Speaker* speaker;

    // Timer for refreshing screen
    FuriTimer* gui_refresh_timer;

    // True if only one button is used when multiple buttons are pressed
    bool single_button_mode;

    // True if the speaker is enabled
    bool has_speaker;

    // True if the backlight is always on
    bool backlight_always_on;

    // Custom data that can be used when extending the application.
    void* custom_data;
};