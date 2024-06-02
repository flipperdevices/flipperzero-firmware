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
#include "scenes/flipenigma_scene.h"
#include "views/flipenigma_startscreen.h"
#include "views/flipenigma_scene_1.h"

#define FLIPENIGMA_VERSION "v1.10"

#define TEXT_BUFFER_SIZE 96
#define TEXT_SIZE (TEXT_BUFFER_SIZE - 1)

typedef struct {
    Gui* gui;
    NotificationApp* notification;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    SceneManager* scene_manager;
    VariableItemList* variable_item_list;
    TextInput* text_input;
    FlipEnigmaStartscreen* flipenigma_startscreen;
    FlipEnigmaScene1* flipenigma_scene_1;
    // Settings options
    int haptic;
    int white_mode;
    int black_mode;
    // Startscreen options
    uint8_t sound;
    // Main menu options
    uint8_t import_game;
    // Text input
    uint8_t input_state;
    char import_game_text[TEXT_BUFFER_SIZE];
    char input_text[TEXT_BUFFER_SIZE];
} FlipEnigma;

typedef enum {
    FlipEnigmaViewIdStartscreen,
    FlipEnigmaViewIdMenu,
    FlipEnigmaViewIdScene1,
    FlipEnigmaViewIdSettings,
    FlipEnigmaViewIdTextInput,
} FlipEnigmaViewId;

typedef enum {
    FlipEnigmaHapticOff,
    FlipEnigmaHapticOn,
} FlipEnigmaHapticState;

typedef enum {
    FlipEnigmaPlayerHuman = 0,
    FlipEnigmaPlayerAI1 = 1,
    FlipEnigmaPlayerAI2 = 2,
    FlipEnigmaPlayerAI3 = 3,
} FlipEnigmaPlayerMode;

typedef enum { FlipEnigmaTextInputDefault, FlipEnigmaTextInputGame } FlipEnigmaTextInputState;

typedef enum {
    FlipEnigmaStatusNone = 0,
    FlipEnigmaStatusMovePlayer = 1,
    FlipEnigmaStatusMoveAI = 2,
    FlipEnigmaStatusMoveUndo = 3,
    FlipEnigmaStatusReturn = 10,
    FlipEnigmaStatusLoadError = 11,
    FlipEnigmaStatusSaveError = 12,
} FlipEnigmaStatus;
