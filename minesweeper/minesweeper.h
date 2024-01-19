#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#include <string.h> // memset
#include <inttypes.h> // PRIu8 & SCNu8

#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>

#include <notification/notification_messages.h>

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/loading.h>
#include <gui/modules/dialog_ex.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/text_box.h>

#include "scenes/minesweeper_scene.h"
#include "views/start_screen.h"
#include "views/minesweeper_game_screen.h"
#include "helpers/mine_sweeper_storage.h"
#include "minesweeper_redux_icons.h"

#include <assets_icons.h>

#define TAG "Mine Sweeper Application"

// This is a helper struct for the settings view/scene
typedef struct {
    uint8_t board_width, board_height, difficulty;
    bool ensure_solvable_board;
    FuriString* width_str;
    FuriString* height_str;
    VariableItem* difficulty_item;
    VariableItem* width_item;
    VariableItem* height_item;
    VariableItem* solvable_item;
} MineSweeperAppSettings;

// Main MineSweeperApp
typedef struct MineSweeperApp {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;

    NotificationApp* notification;

    StartScreen* start_screen;
    Loading* loading;
    MineSweeperGameScreen* game_screen;
    DialogEx* menu_screen;
    VariableItemList* settings_screen;
    DialogEx* confirmation_screen;
    TextBox* info_screen;

    MineSweeperAppSettings settings_info;
    MineSweeperAppSettings t_settings_info;

    bool is_settings_changed;
    bool ensure_map_solvable;

    uint32_t haptic;
    uint32_t speaker;
    uint32_t led;
} MineSweeperApp;

// View Id Enumeration
typedef enum {
    MineSweeperStartScreenView,
    MineSweeperLoadingView,
    MineSweeperGameScreenView,
    MineSweeperMenuView,
    MineSweeperSettingsView,
    MineSweeperConfirmationView,
    MineSweeperInfoView,
    MineSweeperViewCount,
} MineSweeperView;

// Enumerations for hardware states
// Will be used in later implementation
typedef enum {
    MineSweeperHapticOff,
    MineSweeperHapticOn,
} MineSweeperHapticState;

typedef enum {
    MineSweeperSpeakerOff,
    MineSweeperSpeakerOn,
} MineSweeperSpeakerState;

typedef enum {
    MineSweeperLedOff,
    MineSweeperLedOn,
} MineSweeperLedState;

#endif
