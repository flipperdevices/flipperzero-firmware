// Game "Reversi" for Flipper Zero
// Copyright 2023 Dmitry Matyukhin

#include "reversi.h"

// Psst! Most of this file was written with Copilot

// Check if the move is legal by checking if it results in any opponent pieces being captured
bool is_legal_move(int8_t board[BOARD_SIZE][BOARD_SIZE], int row, int col, int player) {
    if(board[row][col] != 0) return false;
    int opponent = -player;
    for(int i = -1; i <= 1; i++) {
        for(int j = -1; j <= 1; j++) {
            if(i == 0 && j == 0) continue;
            int r = row + i, c = col + j;
            if(r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE && board[r][c] == opponent) {
                int k = 2;
                while(true) {
                    r += i;
                    c += j;
                    if(r < 0 || r >= BOARD_SIZE || c < 0 || c >= BOARD_SIZE) break;
                    if(board[r][c] == player) return true;
                    if(board[r][c] == 0) break;
                    k++;
                }
            }
        }
    }
    return false;
}

// Check if the game is over by checking if there are no more moves left for
// either player
bool is_game_over(int8_t board[BOARD_SIZE][BOARD_SIZE]) {
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            if(is_legal_move(board, i, j, BLACK) || is_legal_move(board, i, j, WHITE)) {
                return false;
            }
        }
    }
    return true;
}

bool has_legal_moves(int8_t board[BOARD_SIZE][BOARD_SIZE], int8_t player_color) {
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            if(is_legal_move(board, i, j, player_color)) {
                return true;
            }
        }
    }
    return false;
}

// Calculate the heuristic value of the current board. This function can
// be replaced with a more complex evaluation function that takes into
// account factors such as mobility, piece square tables, etc.
int heuristic(int8_t board[BOARD_SIZE][BOARD_SIZE]) {
    int white = 0, black = 0;
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            if(board[i][j] == 1) white++;
            if(board[i][j] == -1) black++;
        }
    }
    return white - black;
}

// Make a move on the board and capture any opponent pieces
void make_move(GameState* state, int x, int y, int player) {
    state->board[x][y] = player;
    int opponent = -player;
    for(int i = -1; i <= 1; i++) {
        for(int j = -1; j <= 1; j++) {
            if(i == 0 && j == 0) continue;
            int r = x + i, c = y + j;
            if(r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE &&
               state->board[r][c] == opponent) {
                int k = 2;
                while(true) {
                    r += i;
                    c += j;
                    if(r < 0 || r >= BOARD_SIZE || c < 0 || c >= BOARD_SIZE) break;
                    if(state->board[r][c] == player) {
                        r -= i;
                        c -= j;
                        while(r != x || c != y) {
                            state->board[r][c] = player;
                            r -= i;
                            c -= j;
                        }
                        break;
                    }
                    if(state->board[r][c] == 0) break;
                    k++;
                }
            }
        }
    }
    state->is_game_over = is_game_over(state->board);
}

void init_game(GameState* state) {
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            state->board[i][j] = 0;
        }
    }

    // Place the initial pieces
    int mid = BOARD_SIZE / 2;
    state->board[mid - 1][mid - 1] = WHITE;
    state->board[mid][mid] = WHITE;
    state->board[mid - 1][mid] = BLACK;
    state->board[mid][mid - 1] = BLACK;

    state->cursor_x = mid - 1;
    state->cursor_y = mid + 1;

    // Set up turn order
    state->human_color = WHITE;
    state->current_player = WHITE;

    state->is_game_over = false;
}

void human_move(GameState* game_state) {
    if(game_state->current_player != game_state->human_color) {
        return;
    }

    if(is_legal_move(
           game_state->board,
           game_state->cursor_x,
           game_state->cursor_y,
           game_state->current_player)) {
        make_move(
            game_state, game_state->cursor_x, game_state->cursor_y, game_state->current_player);
        game_state->current_player = -game_state->current_player;
    }
}

void computer_move(GameState* game_state) {
    if(game_state->current_player == game_state->human_color) {
        return;
    }
    int best_row = -1, best_col = -1, best_score = -1000000;
    for(int i = 0; i < BOARD_SIZE; i++) {
        for(int j = 0; j < BOARD_SIZE; j++) {
            if(!is_legal_move(game_state->board, i, j, game_state->current_player)) {
                continue;
            }
            int score = heuristic(game_state->board);
            if(score > best_score) {
                best_score = score;
                best_row = i;
                best_col = j;
            }
        }
    }
    if(best_row != -1) {
        make_move(game_state, best_row, best_col, game_state->current_player);
    }
    if(has_legal_moves(game_state->board, game_state->human_color)) {
        game_state->current_player = -game_state->current_player;
    }
}
