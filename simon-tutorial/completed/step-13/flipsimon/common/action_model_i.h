#pragma once

#include "action_model.h"

struct ActionModel {
    // The action this setting is for (0-15)
    uint8_t action_id;

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

    // Index of the menu item for adding another keystroke
    uint8_t keystroke_index;

    // Message to send when this key is pressed
    FuriString* message[4];

    // Temp buffer for editing message
    char* temp_buffer;
    size_t temp_buffer_size;

    // Index of the menu item for editing message
    uint8_t message_index;

    // ActionConfig associated with this key
    void* action_config;

    // Temp index. Used for storing the index of the key being edited
    uint8_t temp_index;
};

static ActionModelFields
    action_model_load_has_id(uint16_t action_id, FlipperFormat* flipper_format);

static void action_model_load_fields(ActionModel* model, FlipperFormat* flipper_format);