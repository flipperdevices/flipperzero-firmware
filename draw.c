#include "draw.h"

#include <gui/gui.h>
#include <gui/icon.h>
#include <gui/elements.h>
#include <gui/icon_i.h>
#include "fonts.h"
#include "game_vexed_icons.h"
#include "ui.h"
#include "move.h"
#include "utils.h"

//-----------------------------------------------------------------------------

u_int32_t frameNo = 0;

//-----------------------------------------------------------------------------

void draw_app(Canvas* canvas, Game* game) {
    canvas_clear(canvas);

    if(game->state == MAIN_MENU) {
        draw_main_menu(canvas, game);
    }

    if(game->state == INTRO) {
        draw_intro(canvas, game, frameNo);
    }

    if(game->state >= SELECT_BRICK) {
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
    }

    frameNo++;
}

void draw_intro(Canvas* canvas, Game* game, uint32_t frameNo) {
    if(frameNo % 2 == 1) {
        if(game->move.frameNo < 100) game->move.frameNo++;
    }

    if((game->move.frameNo < 12)) {
        uint8_t x, y;
        for(y = 0; y < SIZE_Y_BG; y++) {
            for(x = 0; x < SIZE_X_BG; x++) {
                canvas_draw_icon(
                    canvas, x * TILE_SIZE, y * TILE_SIZE, tile_to_icon(game->bg[y][x], false));
            }
        }
    }

    if((game->move.frameNo < 4)) {
        gray_canvas(canvas);
    }

    if(game->move.frameNo > 7) {
        canvas_set_color(canvas, ColorXOR);
        canvas_draw_box(canvas, 0, 0, GUI_DISPLAY_WIDTH, GUI_DISPLAY_HEIGHT);
        canvas_draw_icon(canvas, 0, 0, &I_logo_vexed_big);
    }

    if(game->move.frameNo > 11) {
        canvas_set_color(canvas, ColorBlack);
        canvas_draw_icon(canvas, 0, 0, &I_logo_vexed_big);
    }

    if(game->move.frameNo == 24) {
        game->state = MAIN_MENU;
    }
}

void draw_set_info(Canvas* canvas, Game* game) {
    const uint8_t y = 2;
    const uint8_t w = 118;
    const uint8_t h = 46;
    const uint8_t x = (GUI_DISPLAY_WIDTH - w) / 2;

    canvas_set_color(canvas, ColorBlack);
    canvas_draw_rbox(canvas, x + 1, y + 1, w, h, 3);
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_rbox(canvas, x, y, w, h, 3);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_rframe(canvas, x, y, w, h, 3);

    canvas_set_color(canvas, ColorBlack);
    canvas_draw_rbox(canvas, x, y, w, 11, 3);
    canvas_draw_box(canvas, x, y + 3, w, 11 - 3);

    canvas_set_color(canvas, ColorWhite);
    canvas_set_custom_u8g2_font(canvas, app_u8g2_font_squeezed_r7_tr);
    canvas_draw_str_aligned(
        canvas,
        GUI_DISPLAY_WIDTH / 2,
        y + 2,
        AlignCenter,
        AlignTop,
        furi_string_get_cstr(game->levelSet->title));

    canvas_set_custom_u8g2_font(canvas, app_u8g2_font_squeezed_r6_tr);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_str_aligned(
        canvas,
        GUI_DISPLAY_WIDTH / 2,
        y + 12,
        AlignCenter,
        AlignTop,
        furi_string_get_cstr(game->levelSet->author));

    canvas_draw_str_aligned(
        canvas,
        GUI_DISPLAY_WIDTH / 2,
        y + 20,
        AlignCenter,
        AlignTop,
        furi_string_get_cstr(game->levelSet->url));

    canvas_draw_hline_dotted(canvas, x, y + 28, w);

    my_canvas_frame_set(canvas, x + 3, y + 28, w - 6, h - 29 - 1);
    elements_multiline_text_aligned_limited(
        canvas,
        canvas->width / 2,
        canvas->height / 2,
        2,
        AlignCenter,
        AlignCenter,
        furi_string_get_cstr(game->levelSet->description));
    my_canvas_frame_set(canvas, 0, 0, GUI_DISPLAY_WIDTH, GUI_DISPLAY_HEIGHT);
}

void draw_level_info(Canvas* canvas, Game* game) {
    // TODO: instead, last record for level
    draw_set_info(canvas, game);
}

void draw_main_menu_new_game(Canvas* canvas, Game* game) {
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);
    elements_button_center(canvas, "Start");
    if(game->hasContinue) {
        canvas_set_custom_u8g2_font(canvas, app_u8g2_font_squeezed_r7_tr);
        elements_button_right(canvas, "Continue");
        canvas_draw_str_aligned(
            canvas,
            GUI_DISPLAY_CENTER_X,
            37,
            AlignCenter,
            AlignTop,
            "!!! Forgets all progress and scores !!!");
    }
}

void draw_main_menu_continue(Canvas* canvas, Game* game) {
    int bufSize = 80;
    char buf[bufSize];
    bool hasNext = (game->continueLevel + 1) < game->levelSet->maxLevel;

    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);
    elements_button_left(canvas, "New");
    elements_button_center(canvas, "Start");
    canvas_set_custom_u8g2_font(canvas, app_u8g2_font_squeezed_r7_tr);
    elements_button_right(canvas, "Custom");

    memset(buf, 0, bufSize);
    if(hasNext) {
        snprintf(
            buf,
            sizeof(buf),
            "%s #%d",
            furi_string_get_cstr(game->continueSet),
            game->continueLevel + 2);
    } else {
        snprintf(buf, sizeof(buf), "%s finished!", furi_string_get_cstr(game->continueSet));
    }

    canvas_draw_str_aligned(canvas, GUI_DISPLAY_CENTER_X, 37, AlignCenter, AlignTop, buf);
}

void draw_main_menu_custom(Canvas* canvas, Game* game) {
    int bufSize = 80;
    char buf[bufSize];

    canvas_set_color(canvas, ColorBlack);
    canvas_set_custom_u8g2_font(canvas, app_u8g2_font_squeezed_r7_tr);
    main_menu_pill(
        canvas,
        35,
        90,
        game->main_menu_btn == LEVELSET_BTN,
        game->setPos > 0,
        game->setPos < game->setCount - 1,
        furi_string_get_cstr(game->selectedSet));

    canvas_set_font(canvas, FontSecondary);
    memset(buf, 0, bufSize);
    snprintf(
        buf, sizeof(buf), "Level: %u of %u", game->selectedLevel + 1, game->levelSet->maxLevel);
    main_menu_pill(
        canvas,
        50,
        75,
        game->main_menu_btn == LEVELNO_BTN,
        game->selectedLevel > 0,
        game->selectedLevel < game->levelSet->maxLevel - 1,
        buf);
}

void draw_main_menu(Canvas* canvas, Game* game) {
    canvas_set_color(canvas, ColorBlack);

    canvas_draw_line(canvas, 0, 6, GUI_DISPLAY_WIDTH, 6);
    canvas_draw_line(canvas, 0, 9, GUI_DISPLAY_WIDTH, 9);

    canvas_set_color(canvas, ColorBlack);
    canvas_draw_rbox(canvas, GUI_DISPLAY_CENTER_X - 16 - 6, 1, 32 + 12, 14, 3);
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_icon(canvas, GUI_DISPLAY_CENTER_X - 16, 2, &I_logo_vexed_mini);

    canvas_set_font(canvas, FontSecondary);
    main_menu_pill(
        canvas,
        20,
        90,
        game->main_menu_btn == MODE_BTN,
        game->main_menu_mode != NEW_GAME,
        game->main_menu_mode != CUSTOM,
        game_mode_label(game->main_menu_mode));

    switch(game->main_menu_mode) {
    case CONTINUE:
        draw_main_menu_continue(canvas, game);
        break;
    case CUSTOM:
        draw_main_menu_custom(canvas, game);
        break;
    case NEW_GAME:
    default:
        draw_main_menu_new_game(canvas, game);
        break;
    }

    if(game->mainMenuInfo) {
        gray_canvas(canvas);

        if(game->main_menu_btn == LEVELSET_BTN) {
            draw_set_info(canvas, game);
        } else if(game->main_menu_btn == LEVELNO_BTN) {
            draw_level_info(canvas, game);
        }

        canvas_set_color(canvas, ColorBlack);
        canvas_set_font(canvas, FontSecondary);
        elements_button_center(canvas, "Start");
        elements_button_right_back(canvas, "Back");
    }
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

    canvas_set_custom_u8g2_font(canvas, app_u8g2_font_squeezed_r6_tr);
    canvas_set_color(canvas, ColorWhite);
    elements_multiline_text_aligned_limited(
        canvas, 105, 9, 2, AlignCenter, AlignCenter, furi_string_get_cstr(game->levelData->title));

    canvas_set_color(canvas, ColorBlack);
    //canvas_set_font(canvas, FontPrimary);
    canvas_set_custom_u8g2_font(canvas, app_u8g2_font_wedge_tr);
    canvas_draw_str_aligned(canvas, 104, 20, AlignCenter, AlignTop, "Level");
    canvas_set_custom_u8g2_font(canvas, app_u8g2_font_tom_thumb_4x6_mn);
    memset(buf, 0, bufSize);
    snprintf(buf, sizeof(buf), "%u/%u", game->currentLevel + 1, game->levelSet->maxLevel);
    canvas_draw_str_aligned(canvas, 104, 27, AlignCenter, AlignTop, buf);

    canvas_set_custom_u8g2_font(canvas, app_u8g2_font_wedge_tr);
    canvas_draw_str_aligned(canvas, 104, 34, AlignCenter, AlignTop, "Moves");
    canvas_set_custom_u8g2_font(canvas, app_u8g2_font_tom_thumb_4x6_mn);
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
    canvas_draw_str_aligned(canvas, GUI_DISPLAY_CENTER_X, 11, AlignCenter, AlignTop, "Game Over");

    canvas_set_font(canvas, FontSecondary);
    if(gameOverReason == CANNOT_MOVE) {
        canvas_draw_str_aligned(
            canvas, GUI_DISPLAY_CENTER_X, 30, AlignCenter, AlignTop, "Cannot move");
    } else if(gameOverReason == BRICKS_LEFT) {
        canvas_draw_str_aligned(
            canvas, GUI_DISPLAY_CENTER_X, 30, AlignCenter, AlignTop, "Unpaired bricks left");
    }

    elements_button_left(canvas, "Retry");
    elements_button_center(canvas, "Menu");
    elements_button_right_back(canvas, "Undo");
}

void draw_level_finished(Canvas* canvas, Game* game) {
    int bufSize = 80;
    char buf[bufSize];

    bool hasNext = game->currentLevel < game->levelSet->maxLevel - 1;

    gray_canvas(canvas);

    canvas_set_color(canvas, ColorWhite);
    canvas_draw_rbox(canvas, 14, 6, 100, 40, 4);

    canvas_set_color(canvas, ColorBlack);
    canvas_draw_line(canvas, 14, 20, 112, 20);
    canvas_draw_rframe(canvas, 14, 6, 100, 40, 4);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(
        canvas,
        GUI_DISPLAY_CENTER_X,
        9,
        AlignCenter,
        AlignTop,
        hasNext ? "Level finished!" : "Pack finished!");

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(
        canvas, GUI_DISPLAY_CENTER_X, 24, AlignCenter, AlignTop, "Moves / Par");
    memset(buf, 0, bufSize);
    snprintf(buf, sizeof(buf), "%u / %u", game->gameMoves, game->levelData->gamePar);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, GUI_DISPLAY_CENTER_X, 42, AlignCenter, AlignBottom, buf);

    canvas_set_font(canvas, FontSecondary);
    //elements_button_left(canvas, "Replay");
    if(hasNext) {
        elements_button_center(canvas, "Next");
        elements_button_right_back(canvas, "Menu");
    } else {
        elements_button_center(canvas, "Menu");
    }
}
