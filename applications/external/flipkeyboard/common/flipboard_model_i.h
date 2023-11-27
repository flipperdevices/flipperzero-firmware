#pragma once

#include "flipboard_file.h"
#include "flipboard_model.h"
#include "backlight.h"
#include "button_monitor.h"
#include "keyboard.h"
#include "leds.h"
#include "speaker.h"

#include <gui/gui.h>

/**
 * @brief    The flipboard model struct.
 * @details  This struct contains all the data needed for the flipboard model.
 */
struct FlipboardModel {
    // ButtonModel for each of the button combinations
    ButtonModel* button_model[16];

    // The fields of the ButtonModel that are currently active
    ButtonModelFields button_model_fields;

    // The name of the model (used for saving and loading)
    char* name;

    // HID keyboard driver
    FlipboardKeyboard* keyboard;

    // LED driver
    FlipboardLeds* leds;

    // Button monitor that detects button presses
    ButtonMonitor* button_monitor;

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