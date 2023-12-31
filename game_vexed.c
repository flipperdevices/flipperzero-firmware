#include <furi.h>
#include <furi_hal.h>
#include <furi/core/string.h>

#include <gui/gui.h>
#include <gui/icon.h>
#include <gui/elements.h>

#include <input/input.h>
#include <stdlib.h>

#include "game_vexed_icons.h"
#include "common.h"
#include "game.h"
#include "utils.h"
#include "load.h"
#include "move.h"
#include "fonts.h"
#include "ui.h"

//-----------------------------------------------------------------------------

u_int32_t frameNo = 0;

//-----------------------------------------------------------------------------

inline bool is_state_pause(State gameState) {
    return ((gameState < SELECT_BRICK) || (gameState >= PAUSED));
}

//-----------------------------------------------------------------------------

inline void copy_level(PlayGround target, PlayGround source) {
    memcpy(target, source, sizeof(uint8_t) * SIZE_X * SIZE_Y);
}

//-----------------------------------------------------------------------------

void movement_stoped(Game* game);
void start_explosion(Game* game);

GameOver is_game_over(PlayGround* mv, Stats* stats) {
    uint8_t sumMov = 0;
    uint8_t sum = 0;
    uint8_t x, y;
    for(uint8_t i = 0; i < WALL_TILE; i++) {
        sum += stats->ofBrick[i];
    }
    for(y = 0; y < SIZE_Y; y++) {
        for(x = 0; x < SIZE_X; x++) {
            sumMov += (*mv)[y][x];
        }
    }
    if((sum > 0) && (sumMov == 0)) {
        return CANNOT_MOVE;
    }
    for(uint8_t i = 0; i < WALL_TILE; i++) {
        if(stats->ofBrick[i] == 1) return BRICKS_LEFT;
    }
    return NOT_GAME_OVER;
}

bool is_level_finished(Stats* stats) {
    uint8_t sum = 0;
    for(uint8_t i = 0; i < WALL_TILE; i++) {
        sum += stats->ofBrick[i];
    }
    return (sum == 0);
}

void start_gravity(Game* g) {
    uint8_t x, y;
    bool change = false;

    clear_board(&g->board_ani);

    // go through it bottom to top so as all the blocks tumble down on top of each other
    for(y = (SIZE_Y - 2); y > 0; y--) {
        for(x = (SIZE_X - 1); x > 0; x--) {
            if((is_block(g->board_curr[y][x])) && (g->board_curr[y + 1][x] == EMPTY_TILE)) {
                change = true;
                g->board_ani[y][x] = 1;
            }
        }
    }

    if(change) {
        g->move.frameNo = 0;
        g->move.delay = 5;
        g->state = MOVE_GRAVITY;
    } else {
        g->state = SELECT_BRICK;
        start_explosion(g);
    }
}

void stop_gravity(Game* g) {
    uint8_t x, y;
    for(y = 0; y < SIZE_Y - 1; y++) {
        for(x = 0; x < SIZE_X; x++) {
            if(g->board_ani[y][x] == 1) {
                g->board_curr[y + 1][x] = g->board_curr[y][x];
                g->board_curr[y][x] = EMPTY_TILE;
            }
        }
    }

    start_gravity(g);
}

void start_explosion(Game* g) {
    uint8_t x, y;
    bool change = false;

    clear_board(&g->board_ani);

    // go through it bottom to top so as all the blocks tumble down on top of each other
    for(y = 0; y < SIZE_Y; y++) {
        for(x = 0; x < SIZE_X; x++) {
            if(is_block(g->board_curr[y][x])) {
                if(((y > 0) && (g->board_curr[y][x] == g->board_curr[y - 1][x])) ||
                   ((x > 0) && (g->board_curr[y][x] == g->board_curr[y][x - 1])) ||
                   ((y < SIZE_Y - 1) && (g->board_curr[y][x] == g->board_curr[y + 1][x])) ||
                   ((x < SIZE_X - 1) && (g->board_curr[y][x] == g->board_curr[y][x + 1]))) {
                    change = true;
                    g->board_ani[y][x] = 1;
                }
            }
        }
    }

    if(change) {
        g->move.frameNo = 0;
        g->move.delay = 12;
        g->state = EXPLODE;
    } else {
        g->state = SELECT_BRICK;
        movement_stoped(g);
    }
}

void stop_explosion(Game* g) {
    uint8_t x, y;
    for(y = 0; y < SIZE_Y - 1; y++) {
        for(x = 0; x < SIZE_X; x++) {
            if(g->board_ani[y][x] == 1) {
                g->board_curr[y][x] = EMPTY_TILE;
            }
        }
    }

    start_gravity(g);
}

bool undo(Game* g) {
    if(g->undo_movable != MOVABLE_NOT_FOUND) {
        g->current_movable = g->undo_movable;
        g->undo_movable = MOVABLE_NOT_FOUND;
        copy_level(g->board_curr, g->board_undo);
        map_movability(&g->board_curr, &g->movables);
        update_board_stats(&g->board_curr, g->stats);
        g->gameMoves--;
        g->state = SELECT_BRICK;
        return true;
    } else {
        g->state = SELECT_BRICK;
        return false;
    }
}

void start_move(Game* g, uint8_t direction) {
    g->undo_movable = g->current_movable;
    copy_level(g->board_undo, g->board_curr);
    g->gameMoves++;
    g->move.dir = direction;
    g->move.x = coord_x(g->current_movable);
    g->move.y = coord_y(g->current_movable);
    g->move.frameNo = 0;
    g->next_movable = coord_from((g->move.x + ((direction == MOVABLE_LEFT) ? -1 : 1)), g->move.y);
    g->state = MOVE_SIDES;
}

void movement_stoped(Game* g) {
    map_movability(&g->board_curr, &g->movables);
    update_board_stats(&g->board_curr, g->stats);
    g->current_movable = g->next_movable;
    g->next_movable = MOVABLE_NOT_FOUND;
    if(!is_block(g->board_curr[coord_y(g->current_movable)][coord_x(g->current_movable)])) {
        find_movable_down(&g->movables, &g->current_movable);
    }
    if(!is_block(g->board_curr[coord_y(g->current_movable)][coord_x(g->current_movable)])) {
        find_movable_right(&g->movables, &g->current_movable);
    }
    if(!is_block(g->board_curr[coord_y(g->current_movable)][coord_x(g->current_movable)])) {
        g->current_movable = MOVABLE_NOT_FOUND;
    }

    g->gameOverReason = is_game_over(&g->movables, g->stats);

    if(g->gameOverReason > NOT_GAME_OVER) {
        g->state = GAME_OVER;
    } else if(is_level_finished(g->stats)) {
        g->state = LEVEL_FINISHED;
    } else {
        g->state = SELECT_BRICK;
    }
}

void stop_move(Game* g) {
    uint8_t deltaX = ((g->move.dir & MOVABLE_LEFT) != 0) ? -1 : 1;
    uint8_t tile = g->board_curr[g->move.y][g->move.x];

    g->board_curr[g->move.y][g->move.x] = EMPTY_TILE;
    g->board_curr[g->move.y][cap_x(g->move.x + deltaX)] = tile;

    start_gravity(g);
}

Neighbors find_neighbors(PlayGround* pg, uint8_t x, uint8_t y) {
    Neighbors ne;

    ne.u = (y > 0) ? (*pg)[y - 1][x] : EMPTY_TILE;
    ne.l = (x > 0) ? (*pg)[y][x - 1] : EMPTY_TILE;

    ne.d = (y < SIZE_Y - 1) ? (*pg)[y + 1][x] : EMPTY_TILE;
    ne.r = (x < SIZE_X - 1) ? (*pg)[y][x + 1] : EMPTY_TILE;

    ne.dl = ((y < SIZE_Y - 1) && (x > 0)) ? (*pg)[y + 1][x - 1] : EMPTY_TILE;
    ne.ur = ((y > 0) && (x < SIZE_X - 1)) ? (*pg)[y - 1][x + 1] : EMPTY_TILE;

    ne.ul = ((y > 0) && (x > 0)) ? (*pg)[y - 1][x - 1] : EMPTY_TILE;
    ne.dr = ((x < SIZE_X - 1) && (y < SIZE_Y - 1)) ? (*pg)[y + 1][x + 1] : EMPTY_TILE;

    return ne;
}

void draw_playground(Canvas* canvas, Game* game) {
    Neighbors tiles;
    uint8_t tile, x, y, sx, sy, ex, ey;

    bool whiteB = (game->state == LEVEL_FINISHED);

    //canvas_set_color(canvas, ColorWhite);
    ///canvas_draw_box(
    // canvas, start_x * TILE_SIZE, start_y * TILE_SIZE, draw_w * TILE_SIZE, draw_h * TILE_SIZE);

    for(y = 0; y < SIZE_Y; y++) {
        for(x = 0; x < SIZE_X; x++) {
            tile = game->board_curr[y][x];

            sx = x * TILE_SIZE;
            sy = y * TILE_SIZE;
            ex = ((x + 1) * TILE_SIZE) - 1;
            ey = ((y + 1) * TILE_SIZE) - 1;

            if(tile > 0) {
                if((game->state == MOVE_SIDES) && (x == game->move.x) && (y == game->move.y))
                    continue;
                if(((game->state == MOVE_GRAVITY) || (game->state == EXPLODE)) &&
                   (game->board_ani[y][x] == 1))
                    continue;

                canvas_set_color(canvas, ColorBlack);
                canvas_draw_icon(canvas, sx, sy, tile_to_icon(tile, game->state == GAME_OVER));
            }
            if(tile == WALL_TILE) {
                tiles = find_neighbors(&game->board_curr, x, y);

                /*
                whiteB =

                    !(((tiles.u != WALL_TILE) && (tiles.d != WALL_TILE) &&
                       (tiles.l != WALL_TILE) && (tiles.r != WALL_TILE)));*/

                // UP
                if(tiles.u != WALL_TILE) {
                    canvas_set_color(canvas, ColorBlack);
                    canvas_draw_line(canvas, sx, sy + 1, ex, sy + 1);

                    canvas_set_color(canvas, ColorWhite);
                    canvas_draw_line(canvas, sx, sy, ex, sy);
                    if(whiteB) canvas_draw_line(canvas, sx, sy + 2, ex, sy + 2);
                }

                // DOWN
                if(tiles.d != WALL_TILE) {
                    canvas_set_color(canvas, ColorBlack);
                    canvas_draw_line(canvas, sx, ey, ex, ey);
                    canvas_set_color(canvas, ColorWhite);
                    if(whiteB) canvas_draw_line(canvas, sx, ey - 1, ex, ey - 1);
                }

                // LEFT
                if(tiles.l != WALL_TILE) {
                    canvas_set_color(canvas, ColorBlack);
                    canvas_draw_line(
                        canvas, sx + 1, sy + ((tiles.u != WALL_TILE) ? 1 : 0), sx + 1, ey);

                    canvas_set_color(canvas, ColorWhite);
                    canvas_draw_line(canvas, sx, sy, sx, ey);
                    if(whiteB)
                        canvas_draw_line(
                            canvas,
                            sx + 2,
                            sy + ((tiles.u != WALL_TILE) ? 2 : 0),
                            sx + 2,
                            ey - ((tiles.d != WALL_TILE) ? 2 : 0));
                }

                // RIGHT
                if(tiles.r != WALL_TILE) {
                    canvas_set_color(canvas, ColorBlack);
                    canvas_draw_line(canvas, ex, (sy) + ((tiles.u != WALL_TILE) ? 1 : 0), ex, ey);
                    canvas_set_color(canvas, ColorWhite);
                    if(whiteB)
                        canvas_draw_line(
                            canvas,
                            ex - 1,
                            sy + ((tiles.u != WALL_TILE) ? 2 : 0),
                            ex - 1,
                            ey - ((tiles.d != WALL_TILE) ? 2 : 0));
                }

                if((tiles.dl != WALL_TILE) && (tiles.l == WALL_TILE)) {
                    canvas_set_color(canvas, ColorBlack);
                    canvas_draw_line(canvas, sx, ey, sx + 1, ey);
                    canvas_set_color(canvas, ColorWhite);
                    if(whiteB) canvas_draw_line(canvas, sx, ey - 1, sx + 2, ey - 1);
                }

                if((tiles.ur != WALL_TILE) && (tiles.u == WALL_TILE)) {
                    canvas_set_color(canvas, ColorBlack);
                    canvas_draw_line(canvas, ex, sy, ex, sy + 1);
                    canvas_set_color(canvas, ColorWhite);
                    if(whiteB) canvas_draw_line(canvas, ex - 1, sy, ex - 1, sy + 2);
                }

                if(tiles.ul != WALL_TILE) {
                    canvas_set_color(canvas, ColorWhite);
                    canvas_draw_dot(canvas, sx, sy);
                    if(whiteB) canvas_draw_dot(canvas, sx + 2, sy + 2);
                    if(tiles.l == WALL_TILE) {
                        canvas_set_color(canvas, ColorBlack);
                        canvas_draw_line(canvas, sx, sy + 1, sx + 1, sy + 1);
                        canvas_set_color(canvas, ColorWhite);
                        if(whiteB) canvas_draw_line(canvas, sx, sy + 2, sx + 1, sy + 2);
                    }
                    if(tiles.u == WALL_TILE) {
                        canvas_set_color(canvas, ColorBlack);
                        canvas_draw_line(canvas, sx + 1, sy, sx + 1, sy + 1);
                        canvas_set_color(canvas, ColorWhite);
                        if(whiteB) canvas_draw_line(canvas, sx + 2, sy, sx + 2, sy + 1);
                    }
                }

                if((tiles.dr != WALL_TILE) && (tiles.r == WALL_TILE) && (tiles.d == WALL_TILE)) {
                    canvas_set_color(canvas, ColorWhite);
                    if(whiteB) canvas_draw_line(canvas, ex - 1, ey - 1, ex - 1, ey);
                }
            }
        }
    }
}

void draw_movable(Canvas* canvas, Game* game, uint32_t frameNo) {
    bool oddFrame = (frameNo % 20 < 10);
    if(game->current_movable != MOVABLE_NOT_FOUND) {
        canvas_set_color(canvas, ColorBlack);
        uint8_t x = coord_x(game->current_movable);
        uint8_t y = coord_y(game->current_movable);
        uint8_t how_movable = game->movables[y][x];

        if((how_movable & MOVABLE_LEFT) != 0) {
            canvas_draw_icon(
                canvas, (x - 1) * TILE_SIZE + (oddFrame ? 0 : 1), y * TILE_SIZE, &I_arr_l);
        }

        if((how_movable & MOVABLE_RIGHT) != 0) {
            canvas_draw_icon(
                canvas, (x + 1) * TILE_SIZE + (oddFrame ? 1 : 0), y * TILE_SIZE, &I_arr_r);
        }

        canvas_draw_frame(
            canvas, x * TILE_SIZE - 1, y * TILE_SIZE - 1, TILE_SIZE + 3, TILE_SIZE + 3);

        if(oddFrame) {
            canvas_draw_frame(
                canvas, x * TILE_SIZE - 2, y * TILE_SIZE - 2, TILE_SIZE + 5, TILE_SIZE + 5);
        } else {
            canvas_draw_frame(canvas, x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE + 1, TILE_SIZE + 1);
        }
    }
}

void draw_direction(Canvas* canvas, Game* game, uint32_t frameNo) {
    bool oddFrame = (frameNo % 20 < 10);
    if(game->current_movable != MOVABLE_NOT_FOUND) {
        canvas_set_color(canvas, ColorBlack);
        uint8_t x = coord_x(game->current_movable);
        uint8_t y = coord_y(game->current_movable);

        if(oddFrame) {
            canvas_draw_icon(canvas, (x - 1) * TILE_SIZE, y * TILE_SIZE, &I_mov_l);
            canvas_draw_icon(canvas, ((x + 1) * TILE_SIZE) + 1, y * TILE_SIZE, &I_mov_r);
        }
    }
}

void draw_ani_sides(Canvas* canvas, Game* game) {
    uint8_t tile, sx, sy, deltaX;

    if(game->state == MOVE_SIDES) {
        tile = game->board_curr[game->move.y][game->move.x];
        deltaX = ((game->move.dir & MOVABLE_LEFT) != 0) ? -1 : 1;

        sx = (game->move.x * TILE_SIZE) + (deltaX * game->move.frameNo);
        sy = game->move.y * TILE_SIZE;

        canvas_set_color(canvas, ColorBlack);
        canvas_draw_icon(canvas, sx, sy, tile_to_icon(tile, game->state == GAME_OVER));

        game->move.frameNo++;

        if(game->move.frameNo > TILE_SIZE) {
            stop_move(game);
        }
    }
}

void draw_ani_gravity(Canvas* canvas, Game* game) {
    uint8_t tile, x, y, sx, sy;

    if(game->state == MOVE_GRAVITY) {
        for(y = 0; y < SIZE_Y; y++) {
            for(x = 0; x < SIZE_X; x++) {
                tile = game->board_curr[y][x];

                sx = x * TILE_SIZE;
                sy = y * TILE_SIZE;

                if((tile > 0) && (game->board_ani[y][x] == 1)) {
                    canvas_set_color(canvas, ColorBlack);
                    canvas_draw_icon(
                        canvas,
                        sx,
                        sy + game->move.frameNo,
                        tile_to_icon(tile, game->state == GAME_OVER));
                }
            }
        }

        if(game->move.delay > 0) {
            game->move.delay--;
            return;
        }

        game->move.frameNo++;
        if(game->move.frameNo > TILE_SIZE) {
            stop_gravity(game);
        }
    }
}

void draw_ani_explode(Canvas* canvas, Game* game) {
    uint8_t tile, x, y, sx, sy, cx, cy, s, o;

    if(game->state == EXPLODE) {
        for(y = 0; y < SIZE_Y; y++) {
            for(x = 0; x < SIZE_X; x++) {
                tile = game->board_curr[y][x];

                if((tile > 0) && (game->board_ani[y][x] == 1)) {
                    sx = x * TILE_SIZE;
                    sy = y * TILE_SIZE;
                    cx = sx + 4;
                    cy = sy + 4;

                    if((game->move.delay % 4 < 2) || (game->move.delay > 8)) {
                        canvas_set_color(canvas, ColorBlack);
                        canvas_draw_icon(
                            canvas, sx, sy, tile_to_icon(tile, game->state == GAME_OVER));
                    }

                    if(game->move.frameNo > 0) {
                        canvas_set_color(canvas, ColorXOR);
                        o = MIN(((game->move.frameNo - 1) / 2), (uint8_t)4);
                        s = (o * 2) + 1;
                        canvas_draw_box(canvas, cx - o, cy - o, s, s);
                    }
                }
            }
        }

        if(game->move.delay > 0) {
            game->move.delay--;
            return;
        }

        game->move.frameNo++;
        if(game->move.frameNo > 10) {
            stop_explosion(game);
        }
    }
}

void draw_scores(Canvas* canvas, Game* game) {
    int bufSize = 80;
    char buf[bufSize];

    canvas_set_color(canvas, ColorBlack);

    canvas_draw_rbox(canvas, 82, 1, 46, 17, 2);

    canvas_set_custom_u8g2_font(canvas, u8g2_font_squeezed_r6_tr);
    canvas_set_color(canvas, ColorWhite);
    elements_multiline_text_aligned_limited(
        canvas, 105, 9, 2, AlignCenter, AlignCenter, furi_string_get_cstr(game->levelData->title));

    canvas_set_color(canvas, ColorBlack);
    //canvas_set_font(canvas, FontPrimary);
    canvas_set_custom_u8g2_font(canvas, u8g2_font_wedge_tr);
    canvas_draw_str_aligned(canvas, 104, 20, AlignCenter, AlignTop, "Level");
    canvas_set_custom_u8g2_font(canvas, u8g2_font_tom_thumb_4x6_mn);
    memset(buf, 0, bufSize);
    snprintf(buf, sizeof(buf), "%u/%u", game->current_level + 1, game->levelSet->maxLevel);
    canvas_draw_str_aligned(canvas, 104, 27, AlignCenter, AlignTop, buf);

    canvas_set_custom_u8g2_font(canvas, u8g2_font_wedge_tr);
    canvas_draw_str_aligned(canvas, 104, 34, AlignCenter, AlignTop, "Moves");
    canvas_set_custom_u8g2_font(canvas, u8g2_font_tom_thumb_4x6_mn);
    memset(buf, 0, bufSize);
    snprintf(buf, sizeof(buf), "%u/%u", game->gameMoves, game->levelData->gamePar);
    canvas_draw_str_aligned(canvas, 104, 41, AlignCenter, AlignTop, buf);
}

void draw_paused(Canvas* canvas, Game* game) {
    gray_canvas(canvas);

    menu_pill(
        canvas,
        0,
        MENU_PAUSED_COUNT,
        ((game->menu_paused_pos == 0) && (game->undo_movable != MOVABLE_NOT_FOUND)),
        game->undo_movable == MOVABLE_NOT_FOUND,
        "Undo",
        &I_ico_undo);
    menu_pill(
        canvas, 1, MENU_PAUSED_COUNT, game->menu_paused_pos == 1, false, "Restart", &I_ico_restart);
    menu_pill(
        canvas, 2, MENU_PAUSED_COUNT, game->menu_paused_pos == 2, false, "Menu", &I_ico_home);
    menu_pill(
        canvas, 3, MENU_PAUSED_COUNT, game->menu_paused_pos == 3, false, "Skip", &I_ico_skip);
    menu_pill(
        canvas, 4, MENU_PAUSED_COUNT, game->menu_paused_pos == 4, false, "Count", &I_ico_hist);
    menu_pill(
        canvas, 5, MENU_PAUSED_COUNT, game->menu_paused_pos == 5, false, "Solve", &I_ico_check);

    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);
    elements_button_right_back(canvas, "Back to game");
}

void draw_histogram(Canvas* canvas, Stats* stats) {
    gray_canvas(canvas);
    panel_histogram(canvas, furi_string_get_cstr(stats->bricksNonZero), stats->statsNonZero);
}

void draw_playfield_hint(Canvas* canvas, Game* game) {
    if(game->state == SELECT_BRICK) {
        if((game->current_movable != MOVABLE_NOT_FOUND) &&
           (movable_dir(&game->movables, game->current_movable) == MOVABLE_BOTH)) {
            hint_pill_double(canvas, "Select", "Choose", &I_hint_2);
        } else {
            hint_pill_double(canvas, "Select", "Move", &I_hint_1);
        }
    }

    if(game->state == SELECT_DIRECTION) {
        hint_pill_double(canvas, "Move", "Cancel", &I_hint_3);
    }

    if(game->state == MOVE_SIDES) {
        hint_pill_single(canvas, "moving..");
    }

    if(game->state == MOVE_GRAVITY) {
        hint_pill_single(canvas, "falling..");
    }

    if(game->state == EXPLODE) {
        hint_pill_single(canvas, "BOOM!");
    }
}

void draw_game_over(Canvas* canvas, GameOver gameOverReason) {
    gray_canvas(canvas);

    canvas_set_color(canvas, ColorWhite);
    canvas_draw_rbox(canvas, 14, 8, 100, 38, 4);

    canvas_set_color(canvas, ColorBlack);
    canvas_draw_line(canvas, 14, 22, 112, 22);
    canvas_draw_rframe(canvas, 14, 8, 100, 38, 4);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 11, AlignCenter, AlignTop, "Game Over");

    canvas_set_font(canvas, FontSecondary);
    if(gameOverReason == CANNOT_MOVE) {
        canvas_draw_str_aligned(canvas, 64, 30, AlignCenter, AlignTop, "Cannot move");
    } else if(gameOverReason == BRICKS_LEFT) {
        canvas_draw_str_aligned(canvas, 64, 30, AlignCenter, AlignTop, "Unpaired bricks left");
    }

    //elements_button_center(canvas, "Menu");
    //elements_button_right_back(canvas, "Undo");

    elements_button_left(canvas, "Retry");
    elements_button_center(canvas, "Menu");
    elements_button_right_back(canvas, "Undo");
}

void draw_level_finished(Canvas* canvas, Game* game) {
    int bufSize = 80;
    char buf[bufSize];

    gray_canvas(canvas);

    canvas_set_color(canvas, ColorWhite);
    canvas_draw_rbox(canvas, 14, 6, 100, 40, 4);

    canvas_set_color(canvas, ColorBlack);
    canvas_draw_line(canvas, 14, 20, 112, 20);
    canvas_draw_rframe(canvas, 14, 6, 100, 40, 4);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 9, AlignCenter, AlignTop, "Level finished !");

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 64, 24, AlignCenter, AlignTop, "Moves / Par");
    memset(buf, 0, bufSize);
    snprintf(buf, sizeof(buf), "%u / %u", game->gameMoves, game->levelData->gamePar);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 42, AlignCenter, AlignBottom, buf);

    canvas_set_font(canvas, FontSecondary);
    //elements_button_left(canvas, "Replay");
    elements_button_center(canvas, "Next");
    elements_button_right_back(canvas, "Menu");
}

// Screen is 128x64 px
static void app_draw_callback(Canvas* canvas, void* ctx) {
    furi_assert(ctx);
    Game* game = ctx;
    furi_mutex_acquire(game->mutex, FuriWaitForever);

    canvas_clear(canvas);

    //if(game->state > MAIN_MENU) {
    draw_playground(canvas, game);

    switch(game->state) {
    case SELECT_BRICK:
        draw_movable(canvas, game, frameNo);
        break;
    case SELECT_DIRECTION:
        draw_direction(canvas, game, frameNo);
        break;
    case MOVE_SIDES:
        draw_ani_sides(canvas, game);
        break;
    case MOVE_GRAVITY:
        draw_ani_gravity(canvas, game);
        break;
    case EXPLODE:
        draw_ani_explode(canvas, game);
        break;
    default:
        break;
    }

    draw_scores(canvas, game);
    draw_playfield_hint(canvas, game);

    switch(game->state) {
    case PAUSED:
        draw_paused(canvas, game);
        break;
    case HISTOGRAM:
        draw_histogram(canvas, game->stats);
        break;
    case GAME_OVER:
        draw_game_over(canvas, game->gameOverReason);
        break;
    case LEVEL_FINISHED:
        draw_level_finished(canvas, game);
        break;
    default:
        break;
    }
    //}

    frameNo++;

    furi_mutex_release(game->mutex);
}

void refresh_level(Game* g) {
    clear_board(&g->board_curr);
    clear_board(&g->board_undo);
    clear_board(&g->board_ani);

    // Open storage
    Storage* storage = furi_record_open(RECORD_STORAGE);
    if(load_level(
           storage,
           APP_ASSETS_PATH("levels/01 Classic Levels.vxl"),
           g->current_level,
           g->levelData)) {
        parse_level_notation(furi_string_get_cstr(g->levelData->board), &g->board_curr);
    }
    // Close storage
    furi_record_close(RECORD_STORAGE);

    map_movability(&g->board_curr, &g->movables);
    update_board_stats(&g->board_curr, g->stats);
    g->current_movable = find_movable(&g->movables);
    g->undo_movable = MOVABLE_NOT_FOUND;
    g->gameMoves = 0;
    g->state = SELECT_BRICK;
}

void click_selected(Game* game) {
    const uint8_t dir = movable_dir(&game->movables, game->current_movable);
    switch(dir) {
    case MOVABLE_LEFT:
    case MOVABLE_RIGHT:
        start_move(game, dir);
        break;
    case MOVABLE_BOTH:
        game->state = SELECT_DIRECTION;
        break;
    default:
        break;
    }
}

void events_for_selection(InputEvent* event, Game* game) {
    if((event->type == InputTypePress) || (event->type == InputTypeRepeat)) {
        switch(event->key) {
        case InputKeyLeft:
            //current_level--;
            //refresh_level();
            find_movable_left(&game->movables, &game->current_movable);
            break;
        case InputKeyRight:
            //current_level++;
            //refresh_level();
            find_movable_right(&game->movables, &game->current_movable);
            break;
        case InputKeyUp:
            //image_position.y -= 2;
            //whiteB = !whiteB;
            find_movable_up(&game->movables, &game->current_movable);
            break;
        case InputKeyDown:
            //image_position.y += 2;
            //whiteB = !whiteB;
            find_movable_down(&game->movables, &game->current_movable);
            break;
        case InputKeyOk:
            click_selected(game);
            break;
        case InputKeyBack:
            game->menu_paused_pos = (game->undo_movable == MOVABLE_NOT_FOUND) ? 4 : 0;
            game->state = PAUSED;
            break;
        default:
            break;
        }
    }

    if(event->type == InputTypeLong) {
        switch(event->key) {
        case InputKeyOk:
            game->current_level++;
            refresh_level(game);
            break;
        default:
            break;
        }
    }
}

void events_for_direction(InputEvent* event, Game* game) {
    if((event->type == InputTypePress) || (event->type == InputTypeRepeat)) {
        switch(event->key) {
        case InputKeyLeft:
            start_move(game, MOVABLE_LEFT);
            break;
        case InputKeyRight:
            start_move(game, MOVABLE_RIGHT);
            break;
        case InputKeyUp:
        case InputKeyDown:
        case InputKeyBack:
        case InputKeyOk:
            game->state = SELECT_BRICK;
            break;
        default:
            break;
        }
    }
}

void events_for_paused(InputEvent* event, Game* game) {
    if((event->type == InputTypePress) || (event->type == InputTypeRepeat)) {
        switch(event->key) {
        case InputKeyLeft:
            game->menu_paused_pos =
                (game->menu_paused_pos + MENU_PAUSED_COUNT - 1) % MENU_PAUSED_COUNT;
            if((game->menu_paused_pos == 0) && (game->undo_movable == MOVABLE_NOT_FOUND)) {
                game->menu_paused_pos =
                    (game->menu_paused_pos + MENU_PAUSED_COUNT - 1) % MENU_PAUSED_COUNT;
            }
            break;
        case InputKeyRight:
            game->menu_paused_pos = (game->menu_paused_pos + 1) % MENU_PAUSED_COUNT;
            if((game->menu_paused_pos == 0) && (game->undo_movable == MOVABLE_NOT_FOUND)) {
                game->menu_paused_pos = (game->menu_paused_pos + 1) % MENU_PAUSED_COUNT;
            }
            break;

        case InputKeyUp:
            game->menu_paused_pos =
                (game->menu_paused_pos + MENU_PAUSED_COUNT - 2) % MENU_PAUSED_COUNT;
            if((game->menu_paused_pos == 0) && (game->undo_movable == MOVABLE_NOT_FOUND)) {
                game->menu_paused_pos =
                    (game->menu_paused_pos + MENU_PAUSED_COUNT - 2) % MENU_PAUSED_COUNT;
            }
            break;

        case InputKeyDown:
            game->menu_paused_pos = (game->menu_paused_pos + 2) % MENU_PAUSED_COUNT;
            if((game->menu_paused_pos == 0) && (game->undo_movable == MOVABLE_NOT_FOUND)) {
                game->menu_paused_pos = (game->menu_paused_pos + 2) % MENU_PAUSED_COUNT;
            }
            break;
        case InputKeyOk:
            switch(game->menu_paused_pos) {
            case 0:
                undo(game);
                break;
            case 1:
                refresh_level(game);
                break;
            case 2:
                game->current_level--;
                refresh_level(game);
                break;
            case 3:
                game->current_level++;
                refresh_level(game);
                break;
            case 4:
                game->state = HISTOGRAM;
                break;
            default:
                break;
            }
            break;
        case InputKeyBack:
            game->state = SELECT_BRICK;
            break;
        default:
            break;
        }
    }
}

void events_for_game_over(InputEvent* event, Game* game) {
    if((event->type == InputTypePress) || (event->type == InputTypeRepeat)) {
        switch(event->key) {
        case InputKeyBack:
            game->state = PAUSED;
            break;
        case InputKeyOk:
            game->current_level++;
            refresh_level(game);
            break;
        case InputKeyLeft:
            refresh_level(game);
            break;
        case InputKeyRight:
        case InputKeyUp:
        case InputKeyDown:
        default:
            break;
        }
    }
}

void events_for_level_finished(InputEvent* event, Game* game) {
    if((event->type == InputTypePress) || (event->type == InputTypeRepeat)) {
        switch(event->key) {
        case InputKeyLeft:
        case InputKeyRight:
        case InputKeyUp:
        case InputKeyDown:
        case InputKeyBack:
        case InputKeyOk:
            game->current_level++;
            refresh_level(game);
            break;
        default:
            break;
        }
    }
}

void events_for_histogram(InputEvent* event, Game* game) {
    if((event->type == InputTypePress) || (event->type == InputTypeRepeat)) {
        switch(event->key) {
        case InputKeyLeft:
        case InputKeyRight:
        case InputKeyUp:
        case InputKeyDown:
        case InputKeyBack:
        case InputKeyOk:
            game->state = SELECT_BRICK;
            break;
        default:
            break;
        }
    }
}

void game_tick(void* ctx) {
    furi_assert(ctx);
    const Game* game = ctx;
    view_port_update(game->view_port);
}

static void app_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);

    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

int32_t game_vexed_app(void* p) {
    UNUSED(p);
    int error;
    Game* game = alloc_game_state(&error);
    if(error > 0) {
        return error;
    }

    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    // Configure view port
    game->view_port = view_port_alloc();
    view_port_draw_callback_set(game->view_port, app_draw_callback, game);
    view_port_input_callback_set(game->view_port, app_input_callback, event_queue);

    // Register view port in GUI
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, game->view_port, GuiLayerFullscreen);

    /* Create a timer. We do data analysis in the callback. */
    FuriTimer* timer = furi_timer_alloc(game_tick, FuriTimerTypePeriodic, game);
    furi_timer_start(timer, furi_kernel_get_tick_frequency() / 20);

    Storage* storage = furi_record_open(RECORD_STORAGE);
    load_level_set(storage, APP_ASSETS_PATH("levels/01 Classic Levels.vxl"), game->levelSet);
    furi_record_close(RECORD_STORAGE);

    refresh_level(game);

    InputEvent event;

    bool running = true;
    bool paused = false;

    while(running) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, FuriWaitForever);

        if(event_status == FuriStatusOk) {
            furi_mutex_acquire(game->mutex, FuriWaitForever);

            if((event.type == InputTypeLong) && (event.key == InputKeyBack)) {
                running = false;

            } else {
                switch(game->state) {
                case SELECT_BRICK:
                    events_for_selection(&event, game);
                    break;
                case SELECT_DIRECTION:
                    events_for_direction(&event, game);
                    break;
                case PAUSED:
                    events_for_paused(&event, game);
                    break;
                case HISTOGRAM:
                    events_for_histogram(&event, game);
                    break;
                case GAME_OVER:
                    events_for_game_over(&event, game);
                    break;
                case LEVEL_FINISHED:
                    events_for_level_finished(&event, game);
                    break;
                default:
                    break;
                }
            }

            bool shouldBePaused = is_state_pause(game->state);

            if(paused != shouldBePaused) {
                paused = shouldBePaused;
                if(paused) {
                    furi_timer_stop(timer);
                    FURI_LOG_D(TAG, "PAUSE - timer stoped");
                } else {
                    furi_timer_start(timer, furi_kernel_get_tick_frequency() / 20);
                    FURI_LOG_D(TAG, "UNPAUSE - timer started");
                }
            }

            if(shouldBePaused) {
                view_port_update(game->view_port);
            }
            furi_mutex_release(game->mutex);
        }
    }

    furi_timer_free(timer);
    view_port_enabled_set(game->view_port, false);
    gui_remove_view_port(gui, game->view_port);
    furi_message_queue_free(event_queue);
    free_game_state(game);
    furi_record_close(RECORD_GUI);

    return 0;
}
