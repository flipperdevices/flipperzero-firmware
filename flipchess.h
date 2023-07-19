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
#include "scenes/flipchess_scene.h"
#include "views/flipchess_startscreen.h"
#include "views/flipchess_scene_1.h"

#define FLIPCHESS_VERSION "v1.7.0"

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
    FlipChessStartscreen* flipchess_startscreen;
    FlipChessScene1* flipchess_scene_1;
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
} FlipChess;

typedef enum {
    FlipChessViewIdStartscreen,
    FlipChessViewIdMenu,
    FlipChessViewIdScene1,
    FlipChessViewIdSettings,
    FlipChessViewIdTextInput,
} FlipChessViewId;

typedef enum {
    FlipChessHapticOff,
    FlipChessHapticOn,
} FlipChessHapticState;

typedef enum {
    FlipChessPlayerHuman = 0,
    FlipChessPlayerAI1 = 1,
    FlipChessPlayerAI2 = 2,
    FlipChessPlayerAI3 = 3,
} FlipChessPlayerMode;

typedef enum { FlipChessTextInputDefault, FlipChessTextInputGame } FlipChessTextInputState;

typedef enum {
    FlipChessStatusNone = 0,
    FlipChessStatusMovePlayer = 1,
    FlipChessStatusMoveAI = 2,
    FlipChessStatusMoveUndo = 3,
    FlipChessStatusReturn = 10,
    FlipChessStatusLoadError = 11,
    FlipChessStatusSaveError = 12,
} FlipChessStatus;
