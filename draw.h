#pragma once

#include "game.h"

void draw_app(Canvas* canvas, Game* game);
void draw_intro(Canvas* canvas, Game* game, uint32_t frameNo);
void draw_reset_prompt(Canvas* canvas, Game* game);
void draw_about(Canvas* canvas, Game* game, uint32_t frameNo);
void draw_set_info(Canvas* canvas, Game* game);
void draw_level_info(Canvas* canvas, Game* game);
void draw_main_menu(Canvas* canvas, Game* game);
void draw_playground(Canvas* canvas, Game* game);
void draw_movable(Canvas* canvas, Game* game, uint32_t frameNo);
void draw_direction(Canvas* canvas, Game* game, uint32_t frameNo);
void draw_direction_solution(Canvas* canvas, Game* game, uint32_t frameNo);
void draw_ani_sides(Canvas* canvas, Game* game);
void draw_ani_gravity(Canvas* canvas, Game* game);
void draw_ani_explode(Canvas* canvas, Game* game);
void draw_scores(Canvas* canvas, Game* game, uint32_t frameNo);
void draw_paused(Canvas* canvas, Game* game);
void draw_histogram(Canvas* canvas, Stats* stats);
void draw_playfield_hint(Canvas* canvas, Game* game);
void draw_game_over(Canvas* canvas, GameOver gameOverReason);
void draw_level_finished(Canvas* canvas, Game* game);
void draw_solution_prompt(Canvas* canvas, Game* game);
void draw_invalid_prompt(Canvas* canvas, Game* game);