#pragma once

#include <furi.h>
#include <gui/gui.h>

#include "common.h"
#include "load.h"
#include "stats.h"

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

    // game state
    GameOver gameOverReason;
    MoveInfo move;

} Game;

Game* alloc_game_state(int* error);
void free_game_state(Game* game);