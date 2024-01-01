#pragma once

#include <furi.h>
#include <gui/gui.h>

#include "common.h"
#include "load.h"
#include "stats.h"

//-----------------------------------------------------------------------------

typedef struct {
    uint8_t u;
    uint8_t l;
    uint8_t d;
    uint8_t r;
    uint8_t dl;
    uint8_t ur;
    uint8_t ul;
    uint8_t dr;
} Neighbors;

typedef enum {
    MAIN_MENU,
    INTRO,
    SELECT_BRICK,
    SELECT_DIRECTION,
    MOVE_SIDES,
    MOVE_GRAVITY,
    EXPLODE,
    PAUSED,
    HISTOGRAM,
    GAME_OVER,
    LEVEL_FINISHED,
} State;

typedef enum {
    NEW_GAME,
    CONTINUE,
    CUSTOM,
} GameMode;

typedef enum {
    NOT_GAME_OVER = 0,
    CANNOT_MOVE = 1,
    BRICKS_LEFT = 2,
} GameOver;

typedef struct {
    u_int32_t frameNo;
    u_int32_t dir;
    u_int32_t x;
    u_int32_t y;
    u_int32_t delay;
} MoveInfo;

typedef struct {
    ViewPort* view_port;
    FuriMutex* mutex;
    State state;

    LevelSet* levelSet;
    LevelData* levelData;

    // score
    uint8_t current_level;
    unsigned int gameMoves;

    // board
    PlayGround board_curr;
    PlayGround board_undo;
    PlayGround board_ani;
    PlayGround movables;

    // board stats
    Stats* stats;

    // selections
    uint8_t undo_movable;
    uint8_t current_movable;
    uint8_t next_movable;
    uint8_t menu_paused_pos;
    uint8_t main_menu_pos;
    GameMode main_menu_mode;
    bool hasContinue;

    // game state
    GameOver gameOverReason;
    MoveInfo move;

    BackGround bg;

} Game;

//-----------------------------------------------------------------------------

Game* alloc_game_state(int* error);
void free_game_state(Game* game);

//-----------------------------------------------------------------------------

GameOver is_game_over(PlayGround* mv, Stats* stats);
bool is_level_finished(Stats* stats);
Neighbors find_neighbors(PlayGround* pg, uint8_t x, uint8_t y);

//-----------------------------------------------------------------------------

void initial_load_game(Game* game);
void start_game_at_level(Game* game, uint8_t levelNo);
void refresh_level(Game* g);

//-----------------------------------------------------------------------------

void click_selected(Game* game);

void start_gravity(Game* g);
void stop_gravity(Game* g);
void start_explosion(Game* g);
void stop_explosion(Game* g);
void start_move(Game* g, uint8_t direction);
void stop_move(Game* g);

void movement_stoped(Game* g);
bool undo(Game* g);