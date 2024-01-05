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
int r1 = 0, r2 = 0;

//-----------------------------------------------------------------------------

void draw_app(Canvas* canvas, Game* game) {
    canvas_clear(canvas);

    if((game->state == MAIN_MENU) || (game->state == RESET_PROMPT)) {
        draw_main_menu(canvas, game);
    }

    if(game->state == ABOUT) {
        draw_about(canvas, game, frameNo);
    }

    if(game->state == INTRO) {
        draw_intro(canvas, game, frameNo);
    }

    if(game->state == RESET_PROMPT) {
        draw_reset_prompt(canvas, game);
    }

    if(game->state == INVALID_PROMPT) {
        draw_invalid_prompt(canvas, game);
    }

    if(game->state >= SELECT_BRICK) {
        draw_playground(canvas, game);

        switch(game->state) {
        case SELECT_BRICK:
            draw_movable(canvas, game, frameNo);
            break;
        case SOLUTION_SELECT:
            draw_direction_solution(canvas, game, frameNo);
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

        draw_scores(canvas, game, frameNo);
        draw_playfield_hint(canvas, game);

        switch(game->state) {
        case PAUSED:
            draw_paused(canvas, game);
            break;
        case HISTOGRAM:
            draw_histogram(canvas, game->stats);
            break;
        case SOLUTION_PROMPT:
            draw_solution_prompt(canvas, game);
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

//-----------------------------------------------------------------------------

void draw_intro(Canvas* canvas, Game* game, uint32_t frameNo) {
    if(frameNo % 2 == 1) {
        if(game->move.frameNo < 100) game->move.frameNo++;
    }

    canvas_set_color(canvas, ColorBlack);
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

void draw_about(Canvas* canvas, Game* game, uint32_t frameNo) {
    if(frameNo % 10 == 9) {
        randomize_bg(&game->bg);
    }

    if(frameNo % 50 == 49) {
        r1 = rand();
        r2 = rand();
        randomize_bg(&game->bg);
    }

    uint8_t sx, sy;
    for(sy = 0; sy < SIZE_Y_BG; sy++) {
        for(sx = 0; sx < SIZE_X_BG; sx++) {
            canvas_draw_icon(
                canvas,
                (sx * TILE_SIZE) - (r1 % 7),
                sy * TILE_SIZE - (r2 % 7),
                tile_to_icon(game->bg[sy][sx], false));
        }
    }

    //gray_canvas(canvas);

    // back for os buttons
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_rbox(canvas, 44, 50, GUI_DISPLAY_WIDTH, GUI_DISPLAY_HEIGHT, 3);

    const uint8_t y = dialog_frame(canvas, 92, 43, false, true, "Exit game?");

    canvas_set_custom_u8g2_font(canvas, app_u8g2_font_squeezed_r6_tr);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_str_aligned(
        canvas, GUI_DISPLAY_WIDTH / 2, y + 2, AlignCenter, AlignTop, "(c) 2024 Dominik Dzienia");

    canvas_draw_str_aligned(
        canvas, GUI_DISPLAY_WIDTH / 2, y + 13, AlignCenter, AlignTop, "based on Vexed 2.2");

    canvas_draw_str_aligned(
        canvas, GUI_DISPLAY_WIDTH / 2, y + 21, AlignCenter, AlignTop, "(c) 2006 Vexed Dev Team");

    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);
    elements_button_center(canvas, "Exit");
    elements_button_right_back(canvas, "Back");
}

void draw_set_info(Canvas* canvas, Game* game) {
    BoundingBox box;
    const uint8_t w = 118;
    const uint8_t h = 46;
    const uint8_t x = (GUI_DISPLAY_WIDTH - w) / 2;

    const uint8_t y =
        dialog_frame(canvas, w, h, false, false, furi_string_get_cstr(game->levelSet->title));

    canvas_set_custom_u8g2_font(canvas, app_u8g2_font_squeezed_r6_tr);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_str_aligned(
        canvas,
        GUI_DISPLAY_WIDTH / 2,
        y,
        AlignCenter,
        AlignTop,
        furi_string_get_cstr(game->levelSet->author));

    canvas_draw_str_aligned(
        canvas,
        GUI_DISPLAY_WIDTH / 2,
        y + 8,
        AlignCenter,
        AlignTop,
        furi_string_get_cstr(game->levelSet->url));

    canvas_draw_hline_dotted(canvas, x, y + 16, w);

    set_bounding_box(&box, x + 3, y + 16, w - 6, 16);
    elements_multiline_text_aligned_limited(
        canvas,
        &box,
        box.width / 2,
        box.height / 2,
        2,
        AlignCenter,
        AlignCenter,
        furi_string_get_cstr(game->levelSet->description));
}

//-----------------------------------------------------------------------------

void draw_level_info(Canvas* canvas, Game* game) {
    int bufSize = 80;
    char buf[bufSize];

    memset(buf, 0, bufSize);
    snprintf(
        buf,
        sizeof(buf),
        "%s #%u",
        furi_string_get_cstr(game->levelSet->title),
        game->selectedLevel + 1);

    const uint8_t x = (GUI_DISPLAY_WIDTH - 100) / 2;
    const uint8_t y = dialog_frame(canvas, 100, 40, false, false, buf);

    canvas_set_color(canvas, ColorBlack);
    canvas_draw_vline_dotted(canvas, GUI_DISPLAY_CENTER_X, y, 30);

    canvas_set_font(canvas, FontSecondary);
    canvas_set_custom_u8g2_font(canvas, app_u8g2_font_squeezed_r7_tr);
    canvas_draw_str_aligned(canvas, x + 25, y + 4, AlignCenter, AlignTop, "Moves/Par");
    memset(buf, 0, bufSize);
    if(game->levelSet->scores[game->selectedLevel].moves == 0) {
        snprintf(
            buf,
            sizeof(buf),
            "??? / %u",

            game->levelSet->pars[game->selectedLevel]);
    } else {
        snprintf(
            buf,
            sizeof(buf),
            "%u / %u",
            game->levelSet->scores[game->selectedLevel].moves,
            game->levelSet->pars[game->selectedLevel]);
    }
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, x + 25, y + 22, AlignCenter, AlignBottom, buf);

    canvas_set_custom_u8g2_font(canvas, app_u8g2_font_squeezed_r7_tr);
    canvas_draw_str_aligned(canvas, x + 75, y + 4, AlignCenter, AlignTop, "Score");
    memset(buf, 0, bufSize);
    if(game->score == 0) {
        snprintf(buf, sizeof(buf), "on par");
    } else {
        snprintf(buf, sizeof(buf), "%+d", game->score);
    }
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, x + 75, y + 22, AlignCenter, AlignBottom, buf);
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

void draw_main_menu_continue(Canvas* canvas, Game* game) {
    int bufSize = 80;
    char buf[bufSize];
    int scorebufSize = 10;
    char scorebufSet[scorebufSize];
    bool hasNext = (game->continueLevel + 1) < game->levelSet->maxLevel;
    memset(scorebufSet, 0, scorebufSize);

    if(game->score == 0) {
        snprintf(scorebufSet, sizeof(scorebufSet), "par");
    } else {
        snprintf(scorebufSet, sizeof(scorebufSet), "%+d", game->score);
    }

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
            "%s (%s), #%d",
            furi_string_get_cstr(game->continueSet),
            scorebufSet,
            game->continueLevel + 2);
    } else {
        snprintf(buf, sizeof(buf), "%s finished!", furi_string_get_cstr(game->continueSet));
    }

    canvas_draw_str_aligned(canvas, GUI_DISPLAY_CENTER_X, 37, AlignCenter, AlignTop, buf);
}

//-----------------------------------------------------------------------------

void draw_main_menu_custom(Canvas* canvas, Game* game) {
    int bufSize = 80;
    char buf[bufSize];

    int scorebufSize = 10;
    char scorebuf[scorebufSize];

    canvas_set_color(canvas, ColorBlack);
    canvas_set_custom_u8g2_font(canvas, app_u8g2_font_squeezed_r7_tr);
    main_menu_pill(
        canvas,
        35,
        90,
        game->mainMenuBtn == LEVELSET_BTN,
        game->setPos > 0,
        game->setPos < game->setCount - 1,
        furi_string_get_cstr(game->selectedSet));

    memset(scorebuf, 0, scorebufSize);
    score_for_level(game, game->selectedLevel, scorebuf, scorebufSize);

    canvas_set_font(canvas, FontSecondary);
    memset(buf, 0, bufSize);
    snprintf(
        buf,
        sizeof(buf),
        "%u of %u (%s)",
        game->selectedLevel + 1,
        game->levelSet->maxLevel,
        scorebuf);
    main_menu_pill(
        canvas,
        50,
        90,
        game->mainMenuBtn == LEVELNO_BTN,
        game->selectedLevel > 0,
        game->selectedLevel < game->levelSet->maxLevel - 1,
        buf);
}

//-----------------------------------------------------------------------------

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
        game->mainMenuBtn == MODE_BTN,
        game->mainMenuMode != NEW_GAME,
        game->mainMenuMode != CUSTOM,
        game_mode_label(game->mainMenuMode));

    switch(game->mainMenuMode) {
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

        if(game->mainMenuBtn == LEVELSET_BTN) {
            draw_set_info(canvas, game);
        } else if(game->mainMenuBtn == LEVELNO_BTN) {
            draw_level_info(canvas, game);
        }

        canvas_set_color(canvas, ColorBlack);
        canvas_set_font(canvas, FontSecondary);
        elements_button_center(canvas, "Start");
        elements_button_right_back(canvas, "Back");
    }
}

//-----------------------------------------------------------------------------

void draw_playground(Canvas* canvas, Game* game) {
    Neighbors tiles;
    uint8_t tile, x, y, sx, sy, ex, ey;

    bool whiteB = (game->state == LEVEL_FINISHED) || (game->solutionMode);

    for(y = 0; y < SIZE_Y; y++) {
        for(x = 0; x < SIZE_X; x++) {
            tile = game->board[y][x];

            sx = x * TILE_SIZE;
            sy = y * TILE_SIZE;
            ex = ((x + 1) * TILE_SIZE) - 1;
            ey = ((y + 1) * TILE_SIZE) - 1;

            if(tile > 0) {
                if((game->state == MOVE_SIDES) && (x == game->move.x) && (y == game->move.y))
                    continue;
                if(((game->state == MOVE_GRAVITY) || (game->state == EXPLODE)) &&
                   (game->toAnimate[y][x] == 1))
                    continue;

                canvas_set_color(canvas, ColorBlack);
                canvas_draw_icon(canvas, sx, sy, tile_to_icon(tile, game->state == GAME_OVER));
            }
            if(tile == WALL_TILE) {
                tiles = find_neighbors(&game->board, x, y);

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

//-----------------------------------------------------------------------------

void draw_movable(Canvas* canvas, Game* game, uint32_t frameNo) {
    bool oddFrame = (frameNo % 20 < 10);
    if(game->currentMovable != MOVABLE_NOT_FOUND) {
        canvas_set_color(canvas, ColorBlack);
        uint8_t x = coord_x(game->currentMovable);
        uint8_t y = coord_y(game->currentMovable);
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

//-----------------------------------------------------------------------------

void draw_direction(Canvas* canvas, Game* game, uint32_t frameNo) {
    bool oddFrame = (frameNo % 20 < 10);
    if(game->currentMovable != MOVABLE_NOT_FOUND) {
        canvas_set_color(canvas, ColorBlack);
        uint8_t x = coord_x(game->currentMovable);
        uint8_t y = coord_y(game->currentMovable);

        if(oddFrame) {
            canvas_draw_icon(canvas, (x - 1) * TILE_SIZE, y * TILE_SIZE, &I_mov_l);
            canvas_draw_icon(canvas, ((x + 1) * TILE_SIZE) + 1, y * TILE_SIZE, &I_mov_r);
        }
    }
}

//-----------------------------------------------------------------------------

void draw_direction_solution(Canvas* canvas, Game* game, uint32_t frameNo) {
    bool oddFrame = (frameNo % 20 < 10);
    if(game->currentMovable != MOVABLE_NOT_FOUND) {
        canvas_set_color(canvas, ColorBlack);
        uint8_t x = coord_x(game->currentMovable);
        uint8_t y = coord_y(game->currentMovable);
        uint8_t how_movable = game->movables[y][x];

        if((how_movable & MOVABLE_LEFT) != 0) {
            canvas_draw_icon(
                canvas, (x - 1) * TILE_SIZE + (oddFrame ? 0 : 1), y * TILE_SIZE, &I_arr_l);
        }

        if((how_movable & MOVABLE_RIGHT) != 0) {
            canvas_draw_icon(
                canvas, (x + 1) * TILE_SIZE + (oddFrame ? 1 : 0), y * TILE_SIZE, &I_mov_r);
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

    game->move.frameNo--;
    if(game->move.frameNo == 0) {
        solution_move(game);
    }
}

//-----------------------------------------------------------------------------

void draw_ani_sides(Canvas* canvas, Game* game) {
    uint8_t tile, sx, sy, deltaX;

    if(game->state == MOVE_SIDES) {
        tile = game->board[game->move.y][game->move.x];
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

//-----------------------------------------------------------------------------

void draw_ani_gravity(Canvas* canvas, Game* game) {
    uint8_t tile, x, y, sx, sy;

    if(game->state == MOVE_GRAVITY) {
        for(y = 0; y < SIZE_Y; y++) {
            for(x = 0; x < SIZE_X; x++) {
                tile = game->board[y][x];

                sx = x * TILE_SIZE;
                sy = y * TILE_SIZE;

                if((tile > 0) && (game->toAnimate[y][x] == 1)) {
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

//-----------------------------------------------------------------------------

void draw_ani_explode(Canvas* canvas, Game* game) {
    uint8_t tile, x, y, sx, sy, cx, cy, s, o;

    if(game->state == EXPLODE) {
        for(y = 0; y < SIZE_Y; y++) {
            for(x = 0; x < SIZE_X; x++) {
                tile = game->board[y][x];

                if((tile > 0) && (game->toAnimate[y][x] == 1)) {
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

//-----------------------------------------------------------------------------

void draw_scores(Canvas* canvas, Game* game, uint32_t frameNo) {
    BoundingBox box;
    int bufSize = 80;
    char buf[bufSize];

    bool showScore = (frameNo % 200) < 100;

    canvas_set_color(canvas, ColorBlack);
    canvas_draw_rbox(canvas, 82, 1, 46, 17, 2);

    canvas_set_custom_u8g2_font(canvas, app_u8g2_font_squeezed_r6_tr);
    canvas_set_color(canvas, ColorWhite);
    set_bounding_box(&box, 82, 1, 46, 17);
    elements_multiline_text_aligned_limited(
        canvas,
        &box,
        box.width / 2,
        box.height / 2,
        2,
        AlignCenter,
        AlignCenter,
        furi_string_get_cstr(game->levelData->title));

    if(game->solutionMode) {
        canvas_set_color(canvas, ColorBlack);
        canvas_set_custom_u8g2_font(canvas, app_u8g2_font_wedge_tr);
        canvas_draw_str_aligned(canvas, 104, 27, AlignCenter, AlignTop, "Solution");
        canvas_set_custom_u8g2_font(canvas, app_u8g2_font_tom_thumb_4x6_mr);
        memset(buf, 0, bufSize);
        snprintf(buf, sizeof(buf), "%d of %d", game->solutionStep + 1, game->solutionTotal);
        canvas_draw_str_aligned(canvas, 104, 34, AlignCenter, AlignTop, buf);
    } else {
        canvas_set_color(canvas, ColorBlack);
        canvas_set_custom_u8g2_font(canvas, app_u8g2_font_wedge_tr);
        canvas_draw_str_aligned(
            canvas, 104, 20, AlignCenter, AlignTop, showScore ? "Score" : "Level");
        canvas_set_custom_u8g2_font(canvas, app_u8g2_font_tom_thumb_4x6_mr);
        memset(buf, 0, bufSize);
        if(showScore) {
            if(game->score == 0) {
                snprintf(buf, sizeof(buf), "on par");
            } else {
                snprintf(buf, sizeof(buf), "%+d", game->score);
            }
        } else {
            snprintf(buf, sizeof(buf), "%u/%u", game->currentLevel + 1, game->levelSet->maxLevel);
        }

        canvas_draw_str_aligned(canvas, 104, 27, AlignCenter, AlignTop, buf);

        canvas_set_custom_u8g2_font(canvas, app_u8g2_font_wedge_tr);
        canvas_draw_str_aligned(
            canvas, 104, 34, AlignCenter, AlignTop, showScore ? "Best" : "Moves");
        canvas_set_custom_u8g2_font(canvas, app_u8g2_font_tom_thumb_4x6_mr);
        memset(buf, 0, bufSize);

        if(showScore) {
            snprintf(buf, sizeof(buf), "%s", game->parLabel);
        } else {
            snprintf(buf, sizeof(buf), "%u/%u", game->gameMoves, game->levelData->gamePar);
        }

        canvas_draw_str_aligned(canvas, 104, 41, AlignCenter, AlignTop, buf);
    }
}

//-----------------------------------------------------------------------------

void draw_paused(Canvas* canvas, Game* game) {
    gray_canvas(canvas);

    menu_pill(
        canvas,
        0,
        MENU_PAUSED_COUNT,
        ((game->menuPausedPos == 0) && (game->undoMovable != MOVABLE_NOT_FOUND)),
        game->undoMovable == MOVABLE_NOT_FOUND,
        "Undo",
        &I_ico_undo);
    menu_pill(
        canvas, 1, MENU_PAUSED_COUNT, game->menuPausedPos == 1, false, "Restart", &I_ico_restart);
    menu_pill(canvas, 2, MENU_PAUSED_COUNT, game->menuPausedPos == 2, false, "Menu", &I_ico_home);
    menu_pill(canvas, 3, MENU_PAUSED_COUNT, game->menuPausedPos == 3, false, "Skip", &I_ico_skip);
    menu_pill(canvas, 4, MENU_PAUSED_COUNT, game->menuPausedPos == 4, false, "Count", &I_ico_hist);
    menu_pill(
        canvas, 5, MENU_PAUSED_COUNT, game->menuPausedPos == 5, false, "Solve", &I_ico_check);

    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);
    elements_button_right_back(canvas, "Back to game");
}

//-----------------------------------------------------------------------------

void draw_histogram(Canvas* canvas, Stats* stats) {
    gray_canvas(canvas);
    panel_histogram(canvas, furi_string_get_cstr(stats->bricksNonZero), stats->statsNonZero);
}

void draw_playfield_hint(Canvas* canvas, Game* game) {
    if(game->state == SELECT_BRICK) {
        if((game->currentMovable != MOVABLE_NOT_FOUND) &&
           (movable_dir(&game->movables, game->currentMovable) == MOVABLE_BOTH)) {
            hint_pill_double(canvas, "Select", "Choose", &I_hint_2);
        } else {
            hint_pill_double(canvas, "Select", "Move", &I_hint_1);
        }
    }

    if(game->state == SELECT_DIRECTION) {
        hint_pill_double(canvas, "Move", "Cancel", &I_hint_3);
    }

    if(game->state == SOLUTION_SELECT || game->solutionMode) {
        hint_pill_double(canvas, "ANY", "CANCEL", &I_hint_4);
    } else {
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
}

//-----------------------------------------------------------------------------

void draw_game_over(Canvas* canvas, GameOver gameOverReason) {
    gray_canvas(canvas);

    const uint8_t y = dialog_frame(canvas, 100, 38, true, false, "Game Over!");

    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);
    if(gameOverReason == CANNOT_MOVE) {
        canvas_draw_str_aligned(
            canvas, GUI_DISPLAY_CENTER_X, y + 8, AlignCenter, AlignTop, "Cannot move");
    } else if(gameOverReason == BRICKS_LEFT) {
        canvas_draw_str_aligned(
            canvas, GUI_DISPLAY_CENTER_X, y + 8, AlignCenter, AlignTop, "Unpaired bricks left");
    }

    elements_button_left(canvas, "Retry");
    elements_button_center(canvas, "Menu");
    elements_button_right_back(canvas, "Undo");
}

//-----------------------------------------------------------------------------

void draw_level_finished(Canvas* canvas, Game* game) {
    int bufSize = 80;
    char buf[bufSize];

    bool hasNext = game->currentLevel < game->levelSet->maxLevel - 1;

    gray_canvas(canvas);

    const uint8_t x = (GUI_DISPLAY_WIDTH - 100) / 2;
    const uint8_t y =
        dialog_frame(canvas, 100, 40, true, false, hasNext ? "Level finished!" : "Pack finished!");

    canvas_set_color(canvas, ColorBlack);
    canvas_draw_vline_dotted(canvas, GUI_DISPLAY_CENTER_X, y, 30);

    canvas_set_font(canvas, FontSecondary);
    canvas_set_custom_u8g2_font(canvas, app_u8g2_font_squeezed_r7_tr);
    canvas_draw_str_aligned(canvas, x + 25, y + 4, AlignCenter, AlignTop, "Moves/Par");
    memset(buf, 0, bufSize);
    snprintf(buf, sizeof(buf), "%u / %u", game->gameMoves, game->levelData->gamePar);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, x + 25, y + 22, AlignCenter, AlignBottom, buf);

    canvas_set_custom_u8g2_font(canvas, app_u8g2_font_squeezed_r7_tr);
    canvas_draw_str_aligned(canvas, x + 75, y + 4, AlignCenter, AlignTop, "Score");
    memset(buf, 0, bufSize);
    if(game->score == 0) {
        snprintf(buf, sizeof(buf), "on par");
    } else {
        snprintf(buf, sizeof(buf), "%+d", game->score);
    }
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, x + 75, y + 22, AlignCenter, AlignBottom, buf);

    canvas_set_font(canvas, FontSecondary);

    if(hasNext) {
        elements_button_center(canvas, "Next");
        elements_button_right_back(canvas, "Menu");
    } else {
        elements_button_center(canvas, "Menu");
    }
}

//-----------------------------------------------------------------------------

void draw_solution_prompt(Canvas* canvas, Game* game) {
    gray_canvas(canvas);

    const uint8_t y = dialog_frame(canvas, 100, 40, true, false, "Show solution?");
    const bool penalty = solution_will_have_penalty(game);

    canvas_set_color(canvas, ColorBlack);
    canvas_set_custom_u8g2_font(canvas, app_u8g2_font_squeezed_r7_tr);

    if(penalty) {
        canvas_draw_str_aligned(
            canvas, GUI_DISPLAY_CENTER_X, y + 4, AlignCenter, AlignTop, "It has one-time penalty");
        canvas_draw_str_aligned(
            canvas, GUI_DISPLAY_CENTER_X, y + 15, AlignCenter, AlignTop, "of additional 5 point");
    } else {
        canvas_draw_str_aligned(
            canvas, GUI_DISPLAY_CENTER_X, y + 4, AlignCenter, AlignTop, "Show solution?");
    }

    elements_button_center(canvas, "Show");
    elements_button_right_back(canvas, "Resign");
}

//-----------------------------------------------------------------------------

void draw_reset_prompt(Canvas* canvas, Game* game) {
    UNUSED(game);

    gray_canvas(canvas);

    const uint8_t y = dialog_frame(canvas, 110, 45, true, false, "Reset game?");

    canvas_set_color(canvas, ColorBlack);
    canvas_set_custom_u8g2_font(canvas, app_u8g2_font_squeezed_r7_tr);

    canvas_draw_str_aligned(
        canvas, GUI_DISPLAY_CENTER_X, y + 2, AlignCenter, AlignTop, "Starting new game will reset");
    canvas_draw_str_aligned(
        canvas, GUI_DISPLAY_CENTER_X, y + 10, AlignCenter, AlignTop, "all progress and scores!");

    canvas_draw_str_aligned(
        canvas, GUI_DISPLAY_CENTER_X, y + 21, AlignCenter, AlignTop, "Are you sure?");

    canvas_set_font(canvas, FontSecondary);
    elements_button_center(canvas, "Confirm");
    canvas_set_custom_u8g2_font(canvas, app_u8g2_font_squeezed_r7_tr);
    elements_button_right_back(canvas, "Back");
}

//-----------------------------------------------------------------------------

void draw_invalid_prompt(Canvas* canvas, Game* game) {
    UNUSED(game);

    gray_canvas(canvas);

    const uint8_t y = dialog_frame(canvas, 110, 45, true, false, "Invalid level");

    canvas_set_color(canvas, ColorBlack);
    canvas_set_custom_u8g2_font(canvas, app_u8g2_font_squeezed_r7_tr);

    canvas_draw_str_aligned(
        canvas, GUI_DISPLAY_CENTER_X, y + 2, AlignCenter, AlignTop, "Cannot load/parse level!");
    canvas_draw_str_aligned(
        canvas,
        GUI_DISPLAY_CENTER_X,
        y + 10,
        AlignCenter,
        AlignTop,
        furi_string_get_cstr(game->errorMsg));

    canvas_draw_str_aligned(
        canvas, GUI_DISPLAY_CENTER_X, y + 21, AlignCenter, AlignTop, "Repair or remove file!");

    canvas_set_font(canvas, FontSecondary);
    elements_button_center(canvas, "Understood");
}