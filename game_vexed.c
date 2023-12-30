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
#include "load.h"
#include "move.h"
#include "fonts.h"
#include "ui.h"

struct neighbors {
    uint8_t u;
    uint8_t l;
    uint8_t d;
    uint8_t r;
    uint8_t dl;
    uint8_t ur;
    uint8_t ul;
    uint8_t dr;
};

enum GameState {
    SELECT_BRICK,
    SELECT_DIRECTION,
    MOVE_SIDES,
    MOVE_GRAVITY,
    EXPLODE,
    GAME_OVER,
    LEVEL_FINISHED,
};

enum GameOver {
    NOT_GAME_OVER = 0,
    CANNOT_MOVE = 1,
    BRICKS_LEFT = 2,
};

LevelData* levelData;
PlayGround board_curr;
PlayGround board_prev;
PlayGround board_undo;
PlayGround board_ani;
PlayGround movables;
uint8_t stats[WALL_TILE];

uint8_t undo_movable = MOVABLE_NOT_FOUND;
uint8_t current_movable = MOVABLE_NOT_FOUND;
uint8_t next_movable = MOVABLE_NOT_FOUND;
uint8_t current_level = 4;
uint8_t max_level = 30;
unsigned int gameMoves = 0;
unsigned int gamePar = 0;

enum GameState gameState = SELECT_BRICK;
enum GameOver gameOverReason = NOT_GAME_OVER;

u_int32_t frameNo = 0;

struct MoveInfo {
    u_int32_t frameNo;
    u_int32_t dir;
    u_int32_t x;
    u_int32_t y;
    u_int32_t delay;
};

struct MoveInfo move;

const Icon* tile_to_icon(uint8_t tile) {
    switch(tile) {
    case 1:
        return &I_a;
    case 2:
        return &I_b;
    case 3:
        return &I_c;
    case 4:
        return &I_alt_d;
    case 5:
        return &I_e;
    case 6:
        return &I_f;
    case 7:
        return &I_g;
    case 8:
        return &I_h;
    case 9:
    default:

        return (gameState == GAME_OVER) ? &I_w_black : &I_w;
    };
}

static void gray_canvas(Canvas* const canvas) {
    canvas_set_color(canvas, ColorWhite);
    for(int x = 0; x < 128; x += 2) {
        for(int y = 0; y < 64; y++) {
            canvas_draw_dot(canvas, x + (y % 2 == 1 ? 0 : 1), y);
        }
    }
}

//-----------------------------------------------------------------------------

bool compare_level(PlayGround Array1, PlayGround Array2) {
    uint8_t x, y;

    for(y = 0; y < SIZE_Y; y++) {
        for(x = 0; x < SIZE_X; x++) {
            if(Array1[y][x] != Array2[y][x]) {
                return 1;
            }
        }
    }
    return 0;
}

//-----------------------------------------------------------------------------

void copy_level(PlayGround Target, PlayGround Source) {
    uint8_t x, y;

    for(y = 0; y < SIZE_Y; y++) {
        for(x = 0; x < SIZE_X; x++) {
            Target[y][x] = Source[y][x];
        }
    }
    return;
}

void movement_stoped();
void start_explosion();

void update_board_stats() {
    map_movability(&board_curr, &movables);
    memset(stats, '\0', sizeof(stats));
    uint8_t x, y, tile;
    for(y = 0; y < SIZE_Y; y++) {
        for(x = 0; x < SIZE_X; x++) {
            tile = board_curr[y][x];
            if(is_block(tile)) {
                stats[tile]++;
            }
        }
    }
}

enum GameOver is_game_over() {
    uint8_t sumMov = 0;
    uint8_t sum = 0;
    uint8_t x, y;
    for(uint8_t i = 0; i < WALL_TILE; i++) {
        sum += stats[i];
    }
    for(y = 0; y < SIZE_Y; y++) {
        for(x = 0; x < SIZE_X; x++) {
            sumMov += movables[y][x];
        }
    }
    if((sum > 0) && (sumMov == 0)) {
        return CANNOT_MOVE;
    }
    for(uint8_t i = 0; i < WALL_TILE; i++) {
        if(stats[i] == 1) return BRICKS_LEFT;
    }
    return NOT_GAME_OVER;
}

bool is_level_finished() {
    uint8_t sum = 0;
    for(uint8_t i = 0; i < WALL_TILE; i++) {
        sum += stats[i];
    }
    return (sum == 0);
}

void start_gravity() {
    uint8_t x, y;
    bool change = false;

    clear_board(&board_ani);

    // go through it bottom to top so as all the blocks tumble down on top of each other
    for(y = (SIZE_Y - 2); y > 0; y--) {
        for(x = (SIZE_X - 1); x > 0; x--) {
            if((is_block(board_curr[y][x])) && (board_curr[y + 1][x] == EMPTY_TILE)) {
                change = true;
                board_ani[y][x] = 1;
            }
        }
    }

    if(change) {
        move.frameNo = 0;
        move.delay = 5;
        gameState = MOVE_GRAVITY;
    } else {
        gameState = SELECT_BRICK;
        start_explosion();
    }
}

void stop_gravity() {
    uint8_t x, y;
    for(y = 0; y < SIZE_Y - 1; y++) {
        for(x = 0; x < SIZE_X; x++) {
            if(board_ani[y][x] == 1) {
                board_curr[y + 1][x] = board_curr[y][x];
                board_curr[y][x] = EMPTY_TILE;
            }
        }
    }

    copy_level(board_prev, board_curr);
    start_gravity();
}

void start_explosion() {
    uint8_t x, y;
    bool change = false;

    clear_board(&board_ani);

    // go through it bottom to top so as all the blocks tumble down on top of each other
    for(y = 0; y < SIZE_Y; y++) {
        for(x = 0; x < SIZE_X; x++) {
            if(is_block(board_curr[y][x])) {
                if(((y > 0) && (board_curr[y][x] == board_curr[y - 1][x])) ||
                   ((x > 0) && (board_curr[y][x] == board_curr[y][x - 1])) ||
                   ((y < SIZE_Y - 1) && (board_curr[y][x] == board_curr[y + 1][x])) ||
                   ((x < SIZE_X - 1) && (board_curr[y][x] == board_curr[y][x + 1]))) {
                    change = true;
                    board_ani[y][x] = 1;
                }
            }
        }
    }

    if(change) {
        move.frameNo = 0;
        move.delay = 12;
        gameState = EXPLODE;
    } else {
        gameState = SELECT_BRICK;
        movement_stoped();
    }
}

void stop_explosion() {
    uint8_t x, y;
    for(y = 0; y < SIZE_Y - 1; y++) {
        for(x = 0; x < SIZE_X; x++) {
            if(board_ani[y][x] == 1) {
                board_curr[y][x] = EMPTY_TILE;
            }
        }
    }

    copy_level(board_prev, board_curr);
    start_gravity();
}

bool undo() {
    if(undo_movable != MOVABLE_NOT_FOUND) {
        current_movable = undo_movable;
        undo_movable = MOVABLE_NOT_FOUND;
        copy_level(board_curr, board_undo);
        update_board_stats();
        gameMoves--;
        gameState = SELECT_BRICK;
        return true;
    } else {
        return false;
    }
}

void start_move(uint8_t direction) {
    undo_movable = current_movable;
    copy_level(board_undo, board_curr);
    gameMoves++;
    move.dir = direction;
    move.x = coord_x(current_movable);
    move.y = coord_y(current_movable);
    move.frameNo = 0;
    next_movable = coord_from((move.x + ((direction == MOVABLE_LEFT) ? -1 : 1)), move.y);
    gameState = MOVE_SIDES;
}

void movement_stoped() {
    update_board_stats();
    current_movable = next_movable;
    next_movable = MOVABLE_NOT_FOUND;
    if(!is_block(board_curr[coord_y(current_movable)][coord_x(current_movable)])) {
        find_movable_down(&movables, &current_movable);
    }
    if(!is_block(board_curr[coord_y(current_movable)][coord_x(current_movable)])) {
        find_movable_right(&movables, &current_movable);
    }
    if(!is_block(board_curr[coord_y(current_movable)][coord_x(current_movable)])) {
        current_movable = MOVABLE_NOT_FOUND;
    }

    gameOverReason = is_game_over();

    if(gameOverReason > NOT_GAME_OVER) {
        gameState = GAME_OVER;
    } else if(is_level_finished()) {
        gameState = LEVEL_FINISHED;
    } else {
        gameState = SELECT_BRICK;
    }
}

void stop_move() {
    uint8_t deltaX = ((move.dir & MOVABLE_LEFT) != 0) ? -1 : 1;
    uint8_t tile = board_curr[move.y][move.x];

    board_curr[move.y][move.x] = EMPTY_TILE;
    board_curr[move.y][cap_x(move.x + deltaX)] = tile;

    copy_level(board_prev, board_curr);
    start_gravity();
}

struct neighbors find_neighbors(PlayGround* pg, uint8_t x, uint8_t y) {
    struct neighbors ne;

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

void draw_playground(Canvas* canvas, PlayGround* pg) {
    struct neighbors tiles;
    uint8_t tile, x, y, sx, sy, ex, ey;

    bool whiteB = (gameState == LEVEL_FINISHED);

    //canvas_set_color(canvas, ColorWhite);
    ///canvas_draw_box(
    // canvas, start_x * tileSize, start_y * tileSize, draw_w * tileSize, draw_h * tileSize);

    for(y = 0; y < SIZE_Y; y++) {
        for(x = 0; x < SIZE_X; x++) {
            tile = (*pg)[y][x];

            sx = x * tileSize;
            sy = y * tileSize;
            ex = ((x + 1) * tileSize) - 1;
            ey = ((y + 1) * tileSize) - 1;

            if(tile > 0) {
                if((gameState == MOVE_SIDES) && (x == move.x) && (y == move.y)) continue;
                if(((gameState == MOVE_GRAVITY) || (gameState == EXPLODE)) &&
                   (board_ani[y][x] == 1))
                    continue;

                canvas_set_color(canvas, ColorBlack);
                canvas_draw_icon(canvas, sx, sy, tile_to_icon(tile));
            }
            if(tile == WALL_TILE) {
                tiles = find_neighbors(pg, x, y);

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

void draw_movable(Canvas* canvas) {
    bool oddFrame = (frameNo % 20 < 10);
    if(current_movable != MOVABLE_NOT_FOUND) {
        canvas_set_color(canvas, ColorBlack);
        uint8_t x = coord_x(current_movable);
        uint8_t y = coord_y(current_movable);
        uint8_t how_movable = movables[y][x];

        if((how_movable & MOVABLE_LEFT) != 0) {
            canvas_draw_icon(
                canvas, (x - 1) * tileSize + (oddFrame ? 0 : 1), y * tileSize, &I_arr_l);
        }

        if((how_movable & MOVABLE_RIGHT) != 0) {
            canvas_draw_icon(
                canvas, (x + 1) * tileSize + (oddFrame ? 1 : 0), y * tileSize, &I_arr_r);
        }

        canvas_draw_frame(canvas, x * tileSize - 1, y * tileSize - 1, tileSize + 3, tileSize + 3);

        if(oddFrame) {
            canvas_draw_frame(
                canvas, x * tileSize - 2, y * tileSize - 2, tileSize + 5, tileSize + 5);
        } else {
            canvas_draw_frame(canvas, x * tileSize, y * tileSize, tileSize + 1, tileSize + 1);
        }
    }
}

void draw_direction(Canvas* canvas) {
    bool oddFrame = (frameNo % 20 < 10);
    if(current_movable != MOVABLE_NOT_FOUND) {
        canvas_set_color(canvas, ColorBlack);
        uint8_t x = coord_x(current_movable);
        uint8_t y = coord_y(current_movable);

        if(oddFrame) {
            canvas_draw_icon(canvas, (x - 1) * tileSize, y * tileSize, &I_mov_l);
            canvas_draw_icon(canvas, ((x + 1) * tileSize) + 1, y * tileSize, &I_mov_r);
        }
    }
}

void draw_ani_sides(Canvas* canvas, PlayGround* pg) {
    uint8_t tile, sx, sy, deltaX;

    if(gameState == MOVE_SIDES) {
        tile = (*pg)[move.y][move.x];
        deltaX = ((move.dir & MOVABLE_LEFT) != 0) ? -1 : 1;

        sx = (move.x * tileSize) + (deltaX * move.frameNo);
        sy = move.y * tileSize;

        canvas_set_color(canvas, ColorBlack);
        canvas_draw_icon(canvas, sx, sy, tile_to_icon(tile));

        move.frameNo++;

        if(move.frameNo > tileSize) {
            stop_move();
        }
    }
}

void draw_ani_gravity(Canvas* canvas, PlayGround* pg, PlayGround* ani) {
    uint8_t tile, x, y, sx, sy;

    if(gameState == MOVE_GRAVITY) {
        for(y = 0; y < SIZE_Y; y++) {
            for(x = 0; x < SIZE_X; x++) {
                tile = (*pg)[y][x];

                sx = x * tileSize;
                sy = y * tileSize;

                if((tile > 0) && ((*ani)[y][x] == 1)) {
                    canvas_set_color(canvas, ColorBlack);
                    canvas_draw_icon(canvas, sx, sy + move.frameNo, tile_to_icon(tile));
                }
            }
        }

        if(move.delay > 0) {
            move.delay--;
            return;
        }

        move.frameNo++;
        if(move.frameNo > tileSize) {
            stop_gravity();
        }
    }
}

void draw_ani_explode(Canvas* canvas, PlayGround* pg, PlayGround* ani) {
    uint8_t tile, x, y, sx, sy, cx, cy, s, o;

    if(gameState == EXPLODE) {
        for(y = 0; y < SIZE_Y; y++) {
            for(x = 0; x < SIZE_X; x++) {
                tile = (*pg)[y][x];

                if((tile > 0) && ((*ani)[y][x] == 1)) {
                    sx = x * tileSize;
                    sy = y * tileSize;
                    cx = sx + 4;
                    cy = sy + 4;

                    if((move.delay % 4 < 2) || (move.delay > 8)) {
                        canvas_set_color(canvas, ColorBlack);
                        canvas_draw_icon(canvas, sx, sy, tile_to_icon(tile));
                    }

                    if(move.frameNo > 0) {
                        canvas_set_color(canvas, ColorXOR);
                        o = MIN(((move.frameNo - 1) / 2), (uint8_t)4);
                        s = (o * 2) + 1;
                        canvas_draw_box(canvas, cx - o, cy - o, s, s);
                    }
                }
            }
        }

        if(move.delay > 0) {
            move.delay--;
            return;
        }

        move.frameNo++;
        if(move.frameNo > 10) {
            stop_explosion();
        }
    }
}

// Screen is 128x64 px
static void app_draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);

    int bufSize = 80;
    char buf[bufSize];

    canvas_clear(canvas);
    draw_playground(canvas, &board_curr);

    if(gameState == SELECT_BRICK) {
        draw_movable(canvas);
    }

    if(gameState == SELECT_DIRECTION) {
        draw_direction(canvas);
    }

    if(gameState == MOVE_SIDES) {
        draw_ani_sides(canvas, &board_curr);
    }

    if(gameState == MOVE_GRAVITY) {
        draw_ani_gravity(canvas, &board_curr, &board_ani);
    }

    if(gameState == EXPLODE) {
        draw_ani_explode(canvas, &board_curr, &board_ani);
    }

    canvas_set_color(canvas, ColorBlack);

    canvas_draw_rbox(canvas, 82, 1, 46, 17, 2);

    canvas_set_custom_u8g2_font(canvas, u8g2_font_squeezed_r6_tr);
    canvas_set_color(canvas, ColorWhite);
    elements_multiline_text_aligned_limited(
        canvas, 105, 9, 2, AlignCenter, AlignCenter, furi_string_get_cstr(levelData->title));

    canvas_set_color(canvas, ColorBlack);
    //canvas_set_font(canvas, FontPrimary);
    canvas_set_custom_u8g2_font(canvas, u8g2_font_wedge_tr);
    canvas_draw_str_aligned(canvas, 104, 20, AlignCenter, AlignTop, "Level");
    canvas_set_custom_u8g2_font(canvas, u8g2_font_tom_thumb_4x6_mn);
    memset(buf, 0, bufSize);
    snprintf(buf, sizeof(buf), "%u/%u", current_level + 1, max_level);
    canvas_draw_str_aligned(canvas, 104, 27, AlignCenter, AlignTop, buf);

    canvas_set_custom_u8g2_font(canvas, u8g2_font_wedge_tr);
    canvas_draw_str_aligned(canvas, 104, 34, AlignCenter, AlignTop, "Moves");
    canvas_set_custom_u8g2_font(canvas, u8g2_font_tom_thumb_4x6_mn);
    memset(buf, 0, bufSize);
    snprintf(buf, sizeof(buf), "%u/%u", gameMoves, gamePar);
    canvas_draw_str_aligned(canvas, 104, 41, AlignCenter, AlignTop, buf);

    if(gameState == SELECT_BRICK) {
        if((current_movable != MOVABLE_NOT_FOUND) &&
           (movable_dir(&movables, current_movable) == MOVABLE_BOTH)) {
            hint_pill_double(canvas, "Select", "Choose", &I_hint_2);
        } else {
            hint_pill_double(canvas, "Select", "Move", &I_hint_1);
        }
    }

    if(gameState == SELECT_DIRECTION) {
        hint_pill_double(canvas, "Move", "Cancel", &I_hint_3);
    }

    if(gameState == MOVE_SIDES) {
        hint_pill_single(canvas, "moving..");
    }

    if(gameState == MOVE_GRAVITY) {
        hint_pill_single(canvas, "falling..");
    }

    if(gameState == EXPLODE) {
        hint_pill_single(canvas, "BOOM!");
    }

    /* SINGLE 
    canvas_draw_rframe(canvas, 82, 55, 46, 9, 3);
    canvas_set_custom_u8g2_font(canvas, u8g2_font_micro_tr);
    canvas_draw_str_aligned(canvas, 104, 57, AlignCenter, AlignTop, "Select");
    */

    if(gameState == GAME_OVER) {
        gray_canvas(canvas);

        canvas_set_color(canvas, ColorWhite);
        canvas_draw_rbox(canvas, 14, 8, 100, 38, 4);

        canvas_set_color(canvas, ColorBlack);
        canvas_draw_line(canvas, 14, 22, 112, 22);
        canvas_draw_rframe(canvas, 14, 8, 100, 38, 4);

        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 64, 11, AlignCenter, AlignTop, "Game Over");

        canvas_set_font(canvas, FontSecondary);
        //if(record_broken) {
        //    canvas_draw_str_aligned(canvas, 64, 29, AlignCenter, AlignTop, "New Top Score!!!");
        //} else {
        if(gameOverReason == CANNOT_MOVE) {
            canvas_draw_str_aligned(canvas, 64, 30, AlignCenter, AlignTop, "Cannot move");
        } else if(gameOverReason == BRICKS_LEFT) {
            canvas_draw_str_aligned(canvas, 64, 30, AlignCenter, AlignTop, "Unpaired bricks left");
        }

        //}

        elements_button_left(canvas, "Retry");
        elements_button_center(canvas, "Menu");
        elements_button_right_back(canvas, "Undo");
    }

    if(gameState == LEVEL_FINISHED) {
        gray_canvas(canvas);

        canvas_set_color(canvas, ColorWhite);
        canvas_draw_rbox(canvas, 14, 8, 100, 38, 4);

        canvas_set_color(canvas, ColorBlack);
        canvas_draw_line(canvas, 14, 22, 112, 22);
        canvas_draw_rframe(canvas, 14, 8, 100, 38, 4);

        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 64, 11, AlignCenter, AlignTop, "Level finished !");

        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(canvas, 64, 29, AlignCenter, AlignTop, "Moves / Par");
        memset(buf, 0, bufSize);
        snprintf(buf, sizeof(buf), "%u / %u", gameMoves, gamePar);
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 64, 48, AlignCenter, AlignBottom, buf);
    }

    frameNo++;
}

static void app_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);

    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

void refresh_level() {
    clear_board(&board_curr);
    clear_board(&board_prev);
    clear_board(&board_undo);
    clear_board(&board_ani);

    // Open storage
    Storage* storage = furi_record_open(RECORD_STORAGE);
    if(load_level(
           storage, APP_ASSETS_PATH("levels/01 Classic Levels.vxl"), current_level, levelData)) {
        parse_level_notation(furi_string_get_cstr(levelData->board), &board_curr);
    }
    // Close storage
    furi_record_close(RECORD_STORAGE);

    gamePar = strlen(furi_string_get_cstr(levelData->solution)) / 2;
    update_board_stats();
    current_movable = find_movable(&movables);
    undo_movable = MOVABLE_NOT_FOUND;
    gameMoves = 0;
    gameState = SELECT_BRICK;
}

void click_selected() {
    const uint8_t dir = movable_dir(&movables, current_movable);
    switch(dir) {
    case MOVABLE_LEFT:
    case MOVABLE_RIGHT:
        start_move(dir);
        break;
    case MOVABLE_BOTH:
        gameState = SELECT_DIRECTION;
        break;
    default:
        break;
    }
}

void events_for_selection(InputEvent* event) {
    if((event->type == InputTypePress) || (event->type == InputTypeRepeat)) {
        switch(event->key) {
        case InputKeyLeft:
            //current_level--;
            //refresh_level();
            find_movable_left(&movables, &current_movable);
            break;
        case InputKeyRight:
            //current_level++;
            //refresh_level();
            find_movable_right(&movables, &current_movable);
            break;
        case InputKeyUp:
            //image_position.y -= 2;
            //whiteB = !whiteB;
            find_movable_up(&movables, &current_movable);
            break;
        case InputKeyDown:
            //image_position.y += 2;
            //whiteB = !whiteB;
            find_movable_down(&movables, &current_movable);
            break;
        case InputKeyOk:
            click_selected();
            break;
        case InputKeyBack:
            if(!undo()) {
                current_level--;
                refresh_level();
            }
        default:
            break;
        }
    }

    if(event->type == InputTypeLong) {
        switch(event->key) {
        case InputKeyOk:
            current_level++;
            refresh_level();
            break;
        default:
            break;
        }
    }
}

void events_for_direction(InputEvent* event) {
    if((event->type == InputTypePress) || (event->type == InputTypeRepeat)) {
        switch(event->key) {
        case InputKeyLeft:
            start_move(MOVABLE_LEFT);
            break;
        case InputKeyRight:
            start_move(MOVABLE_RIGHT);
            break;
        case InputKeyUp:
        case InputKeyDown:
        case InputKeyBack:
        case InputKeyOk:
            gameState = SELECT_BRICK;
            break;
        default:
            break;
        }
    }
}

void events_for_game_over(InputEvent* event) {
    if((event->type == InputTypePress) || (event->type == InputTypeRepeat)) {
        switch(event->key) {
        case InputKeyBack:
            undo();
            break;
        case InputKeyOk:
            current_level++;
            refresh_level();
            break;
        case InputKeyLeft:
            refresh_level();
            break;
        case InputKeyRight:
        case InputKeyUp:
        case InputKeyDown:
        default:
            break;
        }
    }
}

void events_for_level_finished(InputEvent* event) {
    if((event->type == InputTypePress) || (event->type == InputTypeRepeat)) {
        switch(event->key) {
        case InputKeyLeft:
        case InputKeyRight:
        case InputKeyUp:
        case InputKeyDown:
        case InputKeyBack:
        case InputKeyOk:
            current_level++;
            refresh_level();
            break;
        default:
            break;
        }
    }
}

int32_t game_vexed_app(void* p) {
    UNUSED(p);

    levelData = alloc_level_data();

    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    // Configure view port
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, app_draw_callback, view_port);
    view_port_input_callback_set(view_port, app_input_callback, event_queue);

    // Register view port in GUI
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    refresh_level();

    InputEvent event;

    bool running = true;
    while(running) {
        if(furi_message_queue_get(event_queue, &event, 50) == FuriStatusOk) {
            switch(gameState) {
            case SELECT_BRICK:
                events_for_selection(&event);
                break;
            case SELECT_DIRECTION:
                events_for_direction(&event);
                break;
            case GAME_OVER:
                events_for_game_over(&event);
                break;
            case LEVEL_FINISHED:
                events_for_level_finished(&event);
                break;
            default:
                break;
            }

            if(event.type == InputTypeLong) {
                switch(event.key) {
                case InputKeyBack:
                    running = false;
                    break;
                default:
                    break;
                }
            }
        }
        if(furi_message_queue_get_count(event_queue) > 0) {
            furi_message_queue_reset(event_queue);
        }
        view_port_update(view_port);
    }

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);

    furi_record_close(RECORD_GUI);
    free_level_data(levelData);

    return 0;
}
