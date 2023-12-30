#include <furi.h>
#include <furi_hal.h>
#include <furi/core/string.h>

#include <gui/gui.h>
#include <gui/icon.h>
#include <input/input.h>
#include <stdlib.h>

#include "game_vexed_icons.h"
#include "common.h"
#include "load.h"
#include "move.h"

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

LevelData* levelData;
PlayGround board_raw;
PlayGround movables;

uint8_t current_movable = MOVABLE_NOT_FOUND;
uint8_t current_level = 2;
bool whiteB = false;
u_int32_t frameNo = 0;

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
        return &I_w;
    };
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
    bool oddFrame = (frameNo % 10 < 5);
    if(current_movable != MOVABLE_NOT_FOUND) {
        canvas_set_color(canvas, ColorBlack);
        uint8_t x = coord_x(current_movable);
        uint8_t y = coord_y(current_movable);
        uint8_t how_movable = movables[y][x];

        if((how_movable & MOVABLE_LEFT) != 0) {
            canvas_draw_icon(
                canvas, (x - 1) * tileSize + (oddFrame ? 0 : 1), y * tileSize, &I_arr_l);
        }

        if((how_movable & MOVABLE_RIGTH) != 0) {
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

// Screen is 128x64 px
static void app_draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);

    canvas_clear(canvas);
    draw_playground(canvas, &board_raw);
    draw_movable(canvas);

    frameNo++;
}

static void app_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);

    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

void refresh_level() {
    // Open storage
    Storage* storage = furi_record_open(RECORD_STORAGE);
    if(load_level(
           storage, APP_ASSETS_PATH("levels/01 Classic Levels.vxl"), current_level, levelData)) {
        parse_level_notation(furi_string_get_cstr(levelData->board), &board_raw);
    }
    // Close storage
    furi_record_close(RECORD_STORAGE);

    map_movability(&board_raw, &movables);
    current_movable = find_movable(&movables);
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
        if(furi_message_queue_get(event_queue, &event, 100) == FuriStatusOk) {
            if((event.type == InputTypePress) || (event.type == InputTypeRepeat)) {
                switch(event.key) {
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
                    current_level++;
                    refresh_level();
                    break;
                case InputKeyBack:
                    current_level--;
                    refresh_level();
                default:
                    break;
                }
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
