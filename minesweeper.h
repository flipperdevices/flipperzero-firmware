#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/widget.h>
#include <gui/modules/popup.h>
#include <gui/modules/loading.h>
#include <gui/modules/empty_screen.h>
#include <gui/modules/dialog_ex.h>

#include <string.h> // memset
#include <unistd.h> // sleep

#include "minesweeper_icons.h"
//#include <gui/modules/text_input.h>
//#include <gui/submenu.h>

#define PLAY_WIDTH  16
#define PLAY_HEIGHT 7
#define TILE_WIDTH 8

#define MINE_COUNT 20

// Views
typedef enum {
    MineSweeperPopupView,
    MineSweeperLoadingView,
    MineSweeperEmptyScreenView,
    MineSweeperMenuView,
    MineSweeperPopupCount,  // Leave at end
} MineSweeperView;

// Scenes
typedef enum {
    MineSweeperStartupScene,
    MineSweeperGameScene,
    MineSweeperMenuScene,
    MineSweeperSceneCount,  // Leave at end
} MineSweeperScene;

typedef enum {
    MineSweeperGameKeyUpShortEvent,
    MineSweeperGameKeyDownShortEvent,
    MineSweeperGameKeyRightShortEvent,
    MineSweeperGameKeyLeftShortEvent,
    MineSweeperGameKeyOkShortEvent,
    MineSweeperGameKeyBackShortEvent,
    
    MineSweeperGameKeyUpLongEvent, 
    MineSweeperGameKeyDownLongEvent,
    MineSweeperGameKeyRightLongEvent,
    MineSweeperGameKeyLeftLongEvent,
    MineSweeperGameKeyOkLongEvent,
    MineSweeperGameKeyBackLongEvent,
    MineSweeperGameKeyEventSize
} MineSweeperGameKeyEvent;

typedef struct CurPos {
    uint8_t x;
    uint8_t y;
} CurPos;

// App state
typedef struct AppState {
    CurPos curPos;
    int fields_cleared;
    int flags_used;
} AppState;

// App
typedef struct App {
    AppState* app_state;
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Popup* popup;
    View* empty_screen_view;
    EmptyScreen* empty_screen;
    Loading* loading;
    DialogEx* menu;
} App;


#endif
