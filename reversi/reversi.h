#pragma once

#include <furi.h>
#include <stdbool.h>

#define BLACK 1
#define WHITE -1
#define BOARD_SIZE 8

typedef struct {
    int8_t board[BOARD_SIZE][BOARD_SIZE];
    int8_t current_player;
    int8_t human_color;
    uint8_t cursor_x;
    uint8_t cursor_y;
    uint8_t is_game_over;
} GameState;

void init_game(GameState* state);
void computer_move(GameState* game_state);
void human_move(GameState* game_state);
