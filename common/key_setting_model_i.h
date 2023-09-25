#pragma once

#include <furi.h>
#include "key_setting_model.h"

struct KeySettingModel {
    // The key this setting is for (0-15)
    uint8_t key_id;

    // Hex color (RRGGBB) for the key when it is not pressed
    uint32_t color_up;

    // Hex color (RRGGBB) for the key when it is pressed
    uint32_t color_down;

    // Frequency (in Hz) of the tone to play when key is pressed
    float frequency;

    // Number of key codes saved in keystrokes
    uint8_t keystrokes_count;

    // Key codes to send when this key is pressed
    Keystroke* keystrokes;

    // Message to send when this key is pressed
    FuriString* message;

    // Temp buffer for editing message
    char* temp_buffer;
    size_t temp_buffer_size;

    // Index of the menu item for editing message
    uint8_t message_index;

    // KeyConfig associated with this key
    void* key_config;
};