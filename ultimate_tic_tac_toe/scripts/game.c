#include "game.h"
#include <stdlib.h>

#define TAG "UltimateTicTacToeGame"

struct GameState {
    CellState board[9][9];
    BoardWinner boardWinner[9];
    BoardWinner winner;
    PlayerTurn playerTurn;
    int nextBoard;
};

void game_try_set_board_winner(GameState* game, int boardIndex, int a, int b, int c) {
    if(game->boardWinner[boardIndex] != BoardWinner_TBD) return;

    if(game->board[boardIndex][a] != CellState_Empty &&
       game->board[boardIndex][a] == game->board[boardIndex][b] &&
       game->board[boardIndex][a] == game->board[boardIndex][c])
        game->boardWinner[boardIndex] = (BoardWinner)game->board[boardIndex][a];
}

void game_try_set_winner(GameState* game, int a, int b, int c) {
    if(game->winner != BoardWinner_TBD) return;

    if(game->boardWinner[a] == BoardWinner_TBD || game->boardWinner[a] == BoardWinner_Draw) return;

    if(game->boardWinner[a] == game->boardWinner[b] &&
       game->boardWinner[a] == game->boardWinner[c])
        game->winner = game->boardWinner[a];
}

void game_update_winner(GameState* game) {
    game_try_set_winner(game, 0, 1, 2);
    game_try_set_winner(game, 3, 4, 5);
    game_try_set_winner(game, 6, 7, 8);
    game_try_set_winner(game, 0, 3, 6);
    game_try_set_winner(game, 1, 4, 7);
    game_try_set_winner(game, 2, 5, 8);
    game_try_set_winner(game, 0, 4, 8);
    game_try_set_winner(game, 2, 4, 6);

    // Calculate tie: no winner, all boards finished
    if(game->winner == BoardWinner_TBD) {
        int finishedBoards = 0;
        for(int boardIndex = 0; boardIndex < 9; boardIndex++)
            finishedBoards += game->boardWinner[boardIndex] != BoardWinner_TBD;

        if(finishedBoards == 9) game->winner = BoardWinner_Draw;
    }
}

void game_update_board_winner(GameState* game, int boardIndex) {
    game_try_set_board_winner(game, boardIndex, 0, 1, 2);
    game_try_set_board_winner(game, boardIndex, 3, 4, 5);
    game_try_set_board_winner(game, boardIndex, 6, 7, 8);
    game_try_set_board_winner(game, boardIndex, 0, 3, 6);
    game_try_set_board_winner(game, boardIndex, 1, 4, 7);
    game_try_set_board_winner(game, boardIndex, 2, 5, 8);
    game_try_set_board_winner(game, boardIndex, 0, 4, 8);
    game_try_set_board_winner(game, boardIndex, 2, 4, 6);

    if(game->boardWinner[boardIndex] == BoardWinner_X ||
       game->boardWinner[boardIndex] == BoardWinner_O) {
        // Fill the board with the winner to avoid further changes
        for(int cellIndex = 0; cellIndex < 9; cellIndex++)
            game->board[boardIndex][cellIndex] = (CellState)game->boardWinner[boardIndex];
    }

    // Calculate tie: no winner, all cells filled
    if(game->boardWinner[boardIndex] == BoardWinner_TBD) {
        int filledCells = 0;
        for(int cellIndex = 0; cellIndex < 9; cellIndex++)
            filledCells += game->board[boardIndex][cellIndex] != CellState_Empty;

        if(filledCells == 9) game->boardWinner[boardIndex] = BoardWinner_Draw;
    }

    // If the board was finished, check if the game is over
    if(game->boardWinner[boardIndex] != BoardWinner_TBD) game_update_winner(game);
}

void game_set_cell(GameState* game, int boardIndex, int cellIndex, CellState newState) {
    game->board[boardIndex][cellIndex] = newState;
    game_update_board_winner(game, boardIndex);
}

void game_perform_player_movement(GameState* game, int boardIndex, int cellIndex) {
    CellState newState = game->playerTurn == PlayerTurn_X ? CellState_X : CellState_O;
    game_set_cell(game, boardIndex, cellIndex, newState);
    game->playerTurn = game->playerTurn == PlayerTurn_X ? PlayerTurn_O : PlayerTurn_X;
    game->nextBoard = game->boardWinner[cellIndex] == BoardWinner_TBD ? cellIndex : -1;
}

void game_reset(GameState* game) {
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            game->board[i][j] = CellState_Empty;
        }
    }

    for(int i = 0; i < 9; i++) game->boardWinner[i] = BoardWinner_TBD;

    game->winner = BoardWinner_TBD;
    game->playerTurn = PlayerTurn_X;
    game->nextBoard = -1;
}

GameState* game_alloc() {
    GameState* game = malloc(sizeof(GameState));
    game_reset(game);
    return game;
}

void game_free(GameState* game) {
    free(game);
}

// Read-only

CellState game_get_cell(GameState* game, int boardIndex, int cellIndex) {
    return game->board[boardIndex][cellIndex];
}

BoardWinner game_get_board_winner(GameState* game, int boardIndex) {
    return game->boardWinner[boardIndex];
}

PlayerTurn game_get_player_turn(GameState* game) {
    return game->playerTurn;
}

int game_get_next_board(GameState* game) {
    return game->nextBoard;
}

BoardWinner game_get_winner(GameState* game) {
    return game->winner;
}

void game_clone(GameState* game, GameState* gameCopy) {
    memcpy(gameCopy, game, sizeof(GameState));
}