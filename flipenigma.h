#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_random.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>
#include <notification/notification_messages.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/scene_manager.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/text_input.h>
#include <gui/modules/text_box.h>
#include "scenes/flipenigma_scene.h"
#include "views/flipenigma_startscreen.h"

#define FLIPENIGMA_VERSION "v1.0"

// "To make cryptanalysis harder, messages were limited to 250 characters."
#define TEXT_BUFFER_SIZE 251
#define TEXT_SIZE (TEXT_BUFFER_SIZE - 1)

typedef struct {
    Gui* gui;
    NotificationApp* notification;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    SceneManager* scene_manager;
    VariableItemList* variable_item_list;
    TextInput* text_input;
    TextBox* text_box;
    FlipEnigmaStartscreen* flipenigma_startscreen;
    // Text input
    uint8_t input_state;
    char cipher_text[TEXT_BUFFER_SIZE];
    char input_text[TEXT_BUFFER_SIZE];
    // Settings options
    int haptic;
    const char* rotors_model[3];
    uint8_t rotor_positions[3];
    uint8_t rotor_ring_settings[3];
    const char* reflector_model;
    uint8_t plugboard_switches[10][2];
    size_t plugboard_size;
} FlipEnigma;

typedef enum {
    FlipEnigmaViewIdStartscreen,
    FlipEnigmaViewIdMenu,
    FlipEnigmaViewIdSettings,
    FlipEnigmaViewIdTextInput,
    FlipEnigmaViewIdTextBox,
} FlipEnigmaViewId;

typedef enum {
    FlipEnigmaHapticOff,
    FlipEnigmaHapticOn,
} FlipEnigmaHapticState;

typedef enum {
    FlipEnigmaRotorM3I,
    FlipEnigmaRotorM3II,
    FlipEnigmaRotorM3III,
    FlipEnigmaRotorM3IV,
    FlipEnigmaRotorM3V,
} FlipEnigmaRotor;

typedef enum {
    FlipEnigmaReflectorM3A,
    FlipEnigmaReflectorM3B,
    FlipEnigmaReflectorM3C,
} FlipEnigmaReflector;

typedef enum { FlipEnigmaTextInputDefault, FlipEnigmaTextInputActive } FlipEnigmaTextInputState;

typedef enum {
    FlipEnigmaStatusNone = 0,
    FlipEnigmaStatusMovePlayer = 1,
    FlipEnigmaStatusMoveAI = 2,
    FlipEnigmaStatusMoveUndo = 3,
    FlipEnigmaStatusReturn = 10,
    FlipEnigmaStatusLoadError = 11,
    FlipEnigmaStatusSaveError = 12,
} FlipEnigmaStatus;
