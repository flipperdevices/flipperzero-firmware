#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/widget.h>
#include <gui/modules/popup.h>
#include <gui/modules/loading.h>
#include <gui/modules/empty_screen.h>

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
    MineSweeperPopupCount,  // Leave at end
} MineSweeperView;

// Scenes
typedef enum {
    MineSweeperStartupScene,
    MineSweeperGameScene,
    MineSweeperSceneCount,  // Leave at end
} MineSweeperScene;

typedef enum {
    Tile0,
    Tile1,
    Tile2,
    Tile3,
    Tile4,
    Tile5,
    Tile6,
    Tile7,
    Tile8,
    TileClear,
    TileFlag,
    TileMine,
} Tile;

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
} MineSweeperGameKeyEvent;

typedef enum {
    TileStatusEmpty,
    TileStatusMine,
} TileStatus;

typedef struct CurPos {
    uint8_t x;
    uint8_t y;
} CurPos;

// App state
typedef struct AppState {
    TileStatus minefield[PLAY_HEIGHT][PLAY_WIDTH];
    Tile playfield[PLAY_HEIGHT][PLAY_WIDTH];
    //FuriTimer* timer;
    CurPos curPos;
    int fields_cleared;
    int flags_used;
} AppState;

// App
typedef struct App {
    AppState* app_state;
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    View* popup_view;
    Popup* popup;
    View* empty_screen_view;
    EmptyScreen* empty_screen;
    Loading* loading;
} App;


#endif
