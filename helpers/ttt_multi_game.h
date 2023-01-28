#pragma once

#include <furi.h>
#include <stdint.h>

typedef enum {
    TttMultiGamePlayerNone = 0,
    TttMultiGamePlayerO = 1,
    TttMultiGamePlayerX = 2,
} TttMultiGamePlayer;

typedef enum {
    TttMultiGameStateLocalTurn,
    TttMultiGameStateRemoteTurn,
    TttMultiGameStateFinished
} TttMultiGameState;

typedef enum {
    TttMultiGameResultOWin,
    TttMultiGameResultXWin,
    TttMultiGameResultDraw,
    TttMultiGameResultNone
} TttMultiGameResult;

typedef struct {
    TttMultiGamePlayer local_player;
    TttMultiGameState state;
    TttMultiGameResult result;

    uint8_t board[3][3];
} TttMultiGame;

typedef struct {
    TttMultiGamePlayer player;
    uint8_t x;
    uint8_t y;
} TttMultiGameMove;

TttMultiGame* ttt_multi_game_alloc();

void ttt_multi_game_free(TttMultiGame* game);