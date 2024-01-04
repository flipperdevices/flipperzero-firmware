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
    MODE_BTN = 0,
    LEVELSET_BTN = 1,
    LEVELNO_BTN = 2,
} MenuButtons;

typedef enum {
    MAIN_MENU,
    INTRO,
    RESET_PROMPT,
    INVALID_PROMPT,
    ABOUT,
    SELECT_BRICK,
    SELECT_DIRECTION,
    SOLUTION_SELECT,
    MOVE_SIDES,
    MOVE_GRAVITY,
    EXPLODE,
    PAUSED,
    HISTOGRAM,
    SOLUTION_PROMPT,
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
    ViewPort* viewPort;
    FuriMutex* mutex;
    State state;

    LevelSet* levelSet;
    LevelData* levelData;

    // score
    uint8_t currentLevel;
    unsigned int gameMoves;
    int16_t score;
    char parLabel[PAR_LABEL_SIZE];

    // board
    PlayGround board;
    PlayGround boardUndo;
    PlayGround toAnimate;
    PlayGround movables;

    // solution
    PlayGround boardBackup;
    uint8_t currentMovableBackup;
    bool solutionMode;
    uint8_t solutionStep;
    uint8_t solutionTotal;

    // board stats
    Stats* stats;

    // selections
    uint8_t undoMovable;
    uint8_t currentMovable;
    uint8_t nextMovable;

    // menus
    uint8_t menuPausedPos;
    MenuButtons mainMenuBtn;
    GameMode mainMenuMode;
    bool mainMenuInfo;
    bool hasContinue;
    FuriString* selectedSet;
    uint8_t selectedLevel;
    FuriString* continueSet;
    uint8_t continueLevel;
    uint8_t setPos;
    uint8_t setCount;

    // game state
    GameOver gameOverReason;
    MoveInfo move;

    // extra levels
    LevelList levelList;

    FuriString* errorMsg;
    BackGround bg;

} Game;

//-----------------------------------------------------------------------------

Game* alloc_game_state(int* error);
void free_game_state(Game* game);

//-----------------------------------------------------------------------------

void new_game(Game* game);
GameOver is_game_over(PlayGround* mv, Stats* stats);
bool is_level_finished(Stats* stats);
Neighbors find_neighbors(PlayGround* pg, uint8_t x, uint8_t y);

//-----------------------------------------------------------------------------

const char* level_on_pos(Game* game, int pos);
int level_count(Game* game);
void handle_ivalid_set(Game* game, Storage* storage, FuriString* setId, FuriString* errorMsg);
void initial_load_game(Game* game);
void load_gameset_if_needed(Game* game, FuriString* expectedSet);
void start_game_at_level(Game* game, uint8_t levelNo);
void refresh_level(Game* g);
void level_finished(Game* g);
void forget_continue(Game* g);
void score_for_level(Game* g, uint8_t levelNo, char* buf, size_t max);

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

//-----------------------------------------------------------------------------

void start_solution(Game* g);
void end_solution(Game* g);
void solution_select(Game* g);
void solution_move(Game* g);
void solution_next(Game* g);
bool solution_will_have_penalty(Game* g);
