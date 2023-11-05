#include "app_gameplay.h"
#include "game.h"
#include <furi.h>
#include <math.h>

#define TAG "UltimateTicTacToeGameplay"

struct AppGameplayState {
    int selectionX;
    int selectionY;
    PlayerType playerType[2];
    int lastActionAt;

    GameState* game;
};

int modulo(int x, int N) {
    return (x % N + N) % N;
}

int wrap(int x, int min, int max) {
    return modulo(x - min, max - min + 1) + min;
}

void convert_cell_to_xy(int boardIndex, int cellIndex, int* outX, int* outY) {
    *outX = boardIndex % 3 * 3 + cellIndex % 3;
    *outY = boardIndex / 3 * 3 + cellIndex / 3;
}

void convert_xy_to_cell(int x, int y, int* outBoardIndex, int* outCellIndex) {
    *outBoardIndex = y / 3 * 3 + x / 3;
    *outCellIndex = y % 3 * 3 + x % 3;
}

GameState* gameplay_get_game(AppGameplayState* gameplay) {
    return gameplay->game;
}

int gameplay_selection_get_x(AppGameplayState* gameplay) {
    return gameplay->selectionX;
}

int gameplay_selection_get_y(AppGameplayState* gameplay) {
    return gameplay->selectionY;
}

void gameplay_selection_get(AppGameplayState* gameplay, int* boardIndex, int* cellIndex) {
    convert_xy_to_cell(gameplay->selectionX, gameplay->selectionY, boardIndex, cellIndex);
}

int gameplay_get_last_action_at(AppGameplayState* gameplay) {
    return gameplay->lastActionAt;
}

PlayerType gameplay_get_player_type(AppGameplayState* gameplay, PlayerTurn playerIndex) {
    return gameplay->playerType[playerIndex];
}

PlayerType gameplay_get_next_player_type(AppGameplayState* gameplay) {
    return gameplay_get_player_type(gameplay, game_get_player_turn(gameplay->game));
}

void gameplay_selection_set(AppGameplayState* gameplay, int boardIndex, int cellIndex) {
    convert_cell_to_xy(boardIndex, cellIndex, &gameplay->selectionX, &gameplay->selectionY);
}

void gameplay_set_last_action_at(AppGameplayState* gameplay, int lastActionAt) {
    gameplay->lastActionAt = lastActionAt;
}

void gameplay_set_player_type(
    AppGameplayState* gameplay,
    PlayerTurn playerIndex,
    PlayerType playerType) {
    gameplay->playerType[playerIndex] = playerType;
}

bool gameplay_selection_try_set_to_cell(AppGameplayState* gameplay, int boardIndex, int cellIndex) {
    if(game_get_cell(gameplay->game, boardIndex, cellIndex) == CellState_Empty) {
        gameplay_selection_set(gameplay, boardIndex, cellIndex);
        return true;
    }
    return false;
}

void gameplay_selection_set_to_valid_cell(AppGameplayState* gameplay) {
    int nextBoard = game_get_next_board(gameplay->game);

    if(nextBoard != -1) {
        // First try the center cell, then the cells around it
        for(int i = 0; i < 9; i++) {
            int cellIndex = (4 + i) % 9;
            if(gameplay_selection_try_set_to_cell(gameplay, nextBoard, cellIndex)) return;
        }

        FURI_LOG_E(TAG, "No valid cell found in next board");
    }

    // Any board.
    // First, try the center cell of the center board, then the cells around it
    if(gameplay_selection_try_set_to_cell(gameplay, 4, 4)) return;

    for(int i = 0; i < 9; i++) {
        if(gameplay_selection_try_set_to_cell(gameplay, 4, i)) return;
    }

    // Any center cell.
    for(int i = 0; i < 9; i++) {
        if(gameplay_selection_try_set_to_cell(gameplay, i, 4)) return;
    }

    // Any cell.
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            if(gameplay_selection_try_set_to_cell(gameplay, i, j)) return;
        }
    }

    FURI_LOG_E(TAG, "No valid cell found anywhere");
}

void game_selection_reset_for_next_player(AppGameplayState* gameplay) {
    gameplay_selection_set(gameplay, -1, -1);
    if(gameplay_get_player_type(gameplay, game_get_player_turn(gameplay->game)) ==
       PlayerType_Human)
        gameplay_selection_set_to_valid_cell(gameplay);
}

void find_next_empty_cell(
    AppGameplayState* gameplay,
    int baseX,
    int baseY,
    int dx,
    int dy,
    int fromX,
    int toX,
    int fromY,
    int toY,
    int* outBoardIndex,
    int* outCellIndex) {
    int currentX = baseX + dx;
    int currentY = baseY + dy;

    while(currentX != baseX || currentY != baseY) {
        if(currentX > toX)
            currentY += 1;
        else if(currentX < fromX)
            currentY -= 1;
        else if(currentY > toY)
            currentX += 1;
        else if(currentY < fromY)
            currentX -= 1;

        currentX = wrap(currentX, fromX, toX);
        currentY = wrap(currentY, fromY, toY);

        int boardIndex, cellIndex;
        convert_xy_to_cell(currentX, currentY, &boardIndex, &cellIndex);

        if(game_get_cell(gameplay->game, boardIndex, cellIndex) == CellState_Empty) {
            *outBoardIndex = boardIndex;
            *outCellIndex = cellIndex;
            return;
        }

        currentX = currentX + dx;
        currentY = currentY + dy;
    }

    *outBoardIndex = *outCellIndex = -1;
}

void gameplay_selection_handle_delta(AppGameplayState* gameplay, int dx, int dy) {
    int nextBoard = game_get_next_board(gameplay->game);

    int nextBoardX, nextBoardY;
    convert_cell_to_xy(nextBoard, 0, &nextBoardX, &nextBoardY);

    int minCellX = nextBoard == -1 ? 0 : nextBoardX;
    int maxCellX = nextBoard == -1 ? 8 : minCellX + 2;
    int minCellY = nextBoard == -1 ? 0 : nextBoardY;
    int maxCellY = nextBoard == -1 ? 8 : minCellY + 2;

    int selectionX = gameplay_selection_get_x(gameplay);
    int selectionY = gameplay_selection_get_y(gameplay);

    int resultBoardIndex, resultCellIndex;
    find_next_empty_cell(
        gameplay,
        selectionX,
        selectionY,
        dx,
        dy,
        minCellX,
        maxCellX,
        minCellY,
        maxCellY,
        &resultBoardIndex,
        &resultCellIndex);

    if(resultBoardIndex != -1 && resultCellIndex != -1)
        gameplay_selection_set(gameplay, resultBoardIndex, resultCellIndex);
}

bool gameplay_selection_perform_current(AppGameplayState* gameplay) {
    int boardIndex, cellIndex;
    gameplay_selection_get(gameplay, &boardIndex, &cellIndex);

    if(game_get_cell(gameplay->game, boardIndex, cellIndex) == CellState_Empty) {
        game_perform_player_movement(gameplay->game, boardIndex, cellIndex);
        gameplay->lastActionAt = furi_get_tick();
        game_selection_reset_for_next_player(gameplay);
        return true;
    }

    return false;
}

void gameplay_reset(AppGameplayState* gameplay) {
    game_reset(gameplay->game);
    gameplay->lastActionAt = furi_get_tick();
    game_selection_reset_for_next_player(gameplay);
}

AppGameplayState* gameplay_alloc() {
    AppGameplayState* gameplay = malloc(sizeof(AppGameplayState));
    gameplay->game = game_alloc();
    gameplay_reset(gameplay);

    gameplay_set_player_type(gameplay, PlayerTurn_X, PlayerType_Human);
    gameplay_set_player_type(gameplay, PlayerTurn_O, PlayerType_AiRandom);
    return gameplay;
}

void gameplay_free(AppGameplayState* gameplay) {
    game_free(gameplay->game);
    free(gameplay);
}