#pragma once

#include <furi.h>

#include "game.h"

bool is_block(uint8_t tile);
uint8_t cap_x(uint8_t coord);
uint8_t cap_y(uint8_t coord);
bool is_state_pause(State gameState);
void copy_level(PlayGround target, PlayGround source);
void clear_board(PlayGround* ani);
void randomize_bg(BackGround* bg);
const char* game_mode_label(GameMode mode);
