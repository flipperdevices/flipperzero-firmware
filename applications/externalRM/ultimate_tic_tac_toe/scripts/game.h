//#pragma once
#include <stdbool.h>
#include <string.h>

typedef struct GameState GameState;

typedef enum PlayerTurn { PlayerTurn_X, PlayerTurn_O } PlayerTurn;

typedef enum CellState { CellState_Empty, CellState_X, CellState_O } CellState;

typedef enum BoardWinner {
    BoardWinner_TBD,
    BoardWinner_X,
    BoardWinner_O,
    BoardWinner_Draw
} BoardWinner;

GameState* game_alloc();
void game_free(GameState* game);
void game_reset(GameState* game);

// Actions
void game_perform_player_movement(GameState* game, int boardIndex, int cellIndex);

// Read-only
CellState game_get_cell(GameState* game, int boardIndex, int cellIndex);
BoardWinner game_get_board_winner(GameState* game, int boardIndex);
PlayerTurn game_get_player_turn(GameState* game);
int game_get_next_board(GameState* game);
BoardWinner game_get_winner(GameState* game);
void game_clone(GameState* game, GameState* gameCopy);