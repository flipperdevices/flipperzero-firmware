#include "game_ai.h"
#include "app_gameplay.h"
#include "game.h"
#include <furi.h>

const int TimeThinking = 500;
const int TimeMoving = 500;

void game_ai_get_movement_random(GameState* game, int* outBoardIndex, int* outCellIndex) {
    while(true) {
        int boardIndex = game_get_next_board(game) == -1 ? rand() % 9 : game_get_next_board(game);
        int cellIndex = rand() % 9;
        if(game_get_cell(game, boardIndex, cellIndex) == CellState_Empty) {
            *outBoardIndex = boardIndex;
            *outCellIndex = cellIndex;
            return;
        }
    }
}

void game_ai_get_movement_minimax(
    GameState* game,
    int* outBoardIndex,
    int* outCellIndex,
    int* outScore,
    int depth) {
    const int WinnerScore = 100000;

    BoardWinner myWinner = game_get_player_turn(game) == PlayerTurn_X ? BoardWinner_X :
                                                                        BoardWinner_O;
    int bestScore = -10000000;
    int bestBoardIndex = -1;
    int bestCellIndex = -1;

    bool winFound = false;

    GameState* gameCopy = game_alloc();

    for(int boardIndex = 0; boardIndex < 9 && !winFound; boardIndex++) {
        for(int cellIndex = 0; cellIndex < 9 && !winFound; cellIndex++) {
            if(game_get_cell(game, boardIndex, cellIndex) != CellState_Empty) continue;

            if(game_get_next_board(game) != -1 && game_get_next_board(game) != boardIndex)
                continue;

            game_clone(game, gameCopy);
            game_perform_player_movement(gameCopy, boardIndex, cellIndex);

            int score = 0;

            if(game_get_winner(gameCopy) == myWinner) {
                score += WinnerScore;
                winFound = true;
            }

            for(int k = 0; k < 9; k++) {
                BoardWinner winner = game_get_board_winner(gameCopy, k);
                if(winner == myWinner) score += 1000;
            }

            if(game_get_next_board(gameCopy) == -1) score -= 100;

            score += cellIndex % 2 == 0 ?
                         rand() % 95 :
                         rand() % 85; // Randomize ties. Slightly favor the center and the corners.

            // Minimax
            if(depth > 0 && game_get_winner(gameCopy) == BoardWinner_TBD) {
                int _, outScore;
                game_ai_get_movement_minimax(gameCopy, &_, &_, &outScore, depth - 1);
                score -= outScore;
            }

            if(score > bestScore) {
                bestScore = score;
                bestBoardIndex = boardIndex;
                bestCellIndex = cellIndex;
            }
        }
    }

    game_free(gameCopy);

    *outBoardIndex = bestBoardIndex;
    *outCellIndex = bestCellIndex;
    *outScore = bestScore;
}

int game_ai_get_depth(PlayerType ai) {
    switch(ai) {
    case PlayerType_AiMinMax1:
        return 1;
    case PlayerType_AiMinMax2:
        return 2;
    case PlayerType_AiMinMax3:
        return 3;
    default:
        return 0;
    }
}

void game_ai_run(AppGameplayState* gameplay) {
    if(game_get_winner(gameplay_get_game(gameplay)) != BoardWinner_TBD) return;

    PlayerType playerType = gameplay_get_next_player_type(gameplay);

    if(playerType == PlayerType_Human) return;

    int timeSinceLastMovement = furi_get_tick() - gameplay_get_last_action_at(gameplay);

    int selectionBoardIndex, selectionCellIndex;
    gameplay_selection_get(gameplay, &selectionBoardIndex, &selectionCellIndex);

    if(timeSinceLastMovement > TimeThinking &&
       (selectionBoardIndex == -1 || selectionCellIndex == -1)) {
        GameState* game = gameplay_get_game(gameplay);

        if(playerType == PlayerType_AiRandom) {
            game_ai_get_movement_random(game, &selectionBoardIndex, &selectionCellIndex);
        } else {
            int _;
            game_ai_get_movement_minimax(
                game, &selectionBoardIndex, &selectionCellIndex, &_, game_ai_get_depth(playerType));
        }

        gameplay_selection_set(gameplay, selectionBoardIndex, selectionCellIndex);
        gameplay_set_last_action_at(gameplay, furi_get_tick());
        return;
    }

    if(timeSinceLastMovement > TimeMoving &&
       (selectionBoardIndex != -1 && selectionCellIndex != -1)) {
        gameplay_selection_perform_current(gameplay);
        return;
    }
}