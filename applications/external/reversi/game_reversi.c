// Game "Reversi" for Flipper Zero
// Copyright 2023 Dmitry Matyukhin

#include <stdio.h>
#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <storage/storage.h>
#include "reversi.h"

#define FRAME_LEFT 3
#define FRAME_TOP 3
#define FRAME_CELL_SIZE 7

#define SAVING_DIRECTORY EXT_PATH("apps_data/reversi")
#define SAVING_FILENAME SAVING_DIRECTORY "/game_reversi.save"

typedef enum { AppScreenGame, AppScreenMenu } AppScreen;

typedef struct {
    GameState game;
    AppScreen screen;
    uint8_t selected_menu_item;
    FuriMutex* mutex;
} AppState;

#define MENU_ITEMS_COUNT 2
static const char* popup_menu_strings[] = {"Resume", "New Game"};

static void draw_menu(Canvas* const canvas, const AppState* app_state);
static void gray_canvas(Canvas* const canvas);

static void input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

static void draw_callback(Canvas* const canvas, void* ctx) {
    furi_assert(ctx);

    const AppState* app_state = ctx;
    furi_mutex_acquire(app_state->mutex, FuriWaitForever);
    if(app_state == NULL) return;
    const GameState* game_state = &app_state->game;

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);

    for(uint8_t i = 0; i <= BOARD_SIZE; i++) {
        canvas_draw_line(
            canvas,
            FRAME_LEFT + FRAME_CELL_SIZE * i,
            FRAME_TOP,
            FRAME_LEFT + FRAME_CELL_SIZE * i,
            FRAME_TOP + FRAME_CELL_SIZE * BOARD_SIZE);
        canvas_draw_line(
            canvas,
            FRAME_LEFT,
            FRAME_TOP + FRAME_CELL_SIZE * i,
            FRAME_LEFT + FRAME_CELL_SIZE * BOARD_SIZE,
            FRAME_TOP + FRAME_CELL_SIZE * i);
    }
    //
    // draw cursor
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_frame(
        canvas,
        FRAME_LEFT + FRAME_CELL_SIZE * game_state->cursor_x,
        FRAME_TOP + FRAME_CELL_SIZE * game_state->cursor_y,
        FRAME_CELL_SIZE + 1,
        FRAME_CELL_SIZE + 1);

    canvas_set_color(canvas, ColorBlack);
    // draw pieces
    int blacks = 0, whites = 0;
    const int radius = FRAME_CELL_SIZE >> 1;
    for(uint8_t i = 0; i < BOARD_SIZE; i++) {
        for(uint8_t j = 0; j < BOARD_SIZE; j++) {
            if(!game_state->board[i][j]) {
                continue;
            }
            if(game_state->board[i][j] == BLACK) {
                canvas_draw_disc(
                    canvas,
                    FRAME_LEFT + FRAME_CELL_SIZE * i + radius + 1,
                    FRAME_TOP + FRAME_CELL_SIZE * j + radius + 1,
                    radius);
                blacks++;
            } else {
                canvas_draw_circle(
                    canvas,
                    FRAME_LEFT + FRAME_CELL_SIZE * i + radius + 1,
                    FRAME_TOP + FRAME_CELL_SIZE * j + radius + 1,
                    radius);
                whites++;
            }
        }
    }

    canvas_set_font(canvas, FontPrimary);
    // draw score
    char score_str[25];
    memset(score_str, 0, sizeof(score_str));
    snprintf(score_str, sizeof(score_str), "%d - %d", whites, blacks);

    canvas_draw_str_aligned(canvas, 70, 3, AlignLeft, AlignTop, score_str);

    canvas_set_font(canvas, FontSecondary);
    if(game_state->is_game_over) {
        canvas_draw_str_aligned(canvas, 70, 20, AlignLeft, AlignTop, "Game over");

        canvas_draw_str_aligned(
            canvas,
            70,
            FRAME_TOP + FRAME_CELL_SIZE * BOARD_SIZE,
            AlignLeft,
            AlignBottom,
            "Press OK");

        canvas_set_font(canvas, FontPrimary);

        if(whites == blacks) {
            canvas_draw_str_aligned(canvas, 70, 30, AlignLeft, AlignTop, "DRAW");
        } else if(
            ((game_state->human_color == WHITE) && whites > blacks) ||
            ((game_state->human_color == BLACK) && blacks > whites)) {
            canvas_draw_str_aligned(canvas, 70, 30, AlignLeft, AlignTop, "YOU WIN");
        } else {
            canvas_draw_str_aligned(canvas, 70, 30, AlignLeft, AlignTop, "YOU LOSE");
        }
    } else if(game_state->current_player == game_state->human_color) {
        canvas_draw_str_aligned(canvas, 70, 12, AlignLeft, AlignTop, "Your turn");
    } else {
        canvas_draw_str_aligned(canvas, 70, 12, AlignLeft, AlignTop, "Computer turn");
    }

    if(app_state->screen == AppScreenMenu) {
        draw_menu(canvas, app_state);
    }

    furi_mutex_release(app_state->mutex);
}

static void draw_menu(Canvas* const canvas, const AppState* app_state) {
    gray_canvas(canvas);
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_rbox(canvas, 28, 16, 72, 32, 4);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_rframe(canvas, 28, 16, 72, 32, 4);

    for(int i = 0; i < MENU_ITEMS_COUNT; i++) {
        if(i == app_state->selected_menu_item) {
            canvas_set_color(canvas, ColorBlack);
            canvas_draw_box(canvas, 34, 20 + 12 * i, 60, 12);
        }

        canvas_set_color(canvas, i == app_state->selected_menu_item ? ColorWhite : ColorBlack);
        canvas_draw_str_aligned(
            canvas, 64, 26 + 12 * i, AlignCenter, AlignCenter, popup_menu_strings[i]);
    }
}

static void gray_canvas(Canvas* const canvas) {
    canvas_set_color(canvas, ColorWhite);
    for(int x = 0; x < 128; x += 2) {
        for(int y = 0; y < 64; y++) {
            canvas_draw_dot(canvas, x + (y % 2 == 1 ? 0 : 1), y);
        }
    }
}

bool load_game(GameState* game_state) {
    Storage* storage = furi_record_open(RECORD_STORAGE);

    File* file = storage_file_alloc(storage);
    uint16_t bytes_readed = 0;
    if(storage_file_open(file, SAVING_FILENAME, FSAM_READ, FSOM_OPEN_EXISTING)) {
        bytes_readed = storage_file_read(file, game_state, sizeof(GameState));
    }
    storage_file_close(file);
    storage_file_free(file);

    furi_record_close(RECORD_STORAGE);

    return bytes_readed == sizeof(GameState);
}

void save_game(const GameState* game_state) {
    Storage* storage = furi_record_open(RECORD_STORAGE);

    if(storage_common_stat(storage, SAVING_DIRECTORY, NULL) == FSE_NOT_EXIST) {
        if(!storage_simply_mkdir(storage, SAVING_DIRECTORY)) {
            return;
        }
    }

    File* file = storage_file_alloc(storage);
    if(storage_file_open(file, SAVING_FILENAME, FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
        storage_file_write(file, game_state, sizeof(GameState));
    }
    storage_file_close(file);
    storage_file_free(file);

    furi_record_close(RECORD_STORAGE);
}

bool handle_key_game(GameState* game_state, InputKey key) {
    switch(key) {
    case InputKeyBack:
        save_game(game_state);
        return false;
        break;
    case InputKeyOk:
        if(game_state->is_game_over) {
            init_game(game_state);
            save_game(game_state);
        } else {
            human_move(game_state);
        }
        break;
    case InputKeyUp:
        if(game_state->cursor_y > 0) {
            game_state->cursor_y--;
        } else {
            game_state->cursor_y = BOARD_SIZE - 1;
        }
        break;
    case InputKeyDown:
        if(game_state->cursor_y < BOARD_SIZE - 1) {
            game_state->cursor_y++;
        } else {
            game_state->cursor_y = 0;
        }
        break;
    case InputKeyLeft:
        if(game_state->cursor_x > 0) {
            game_state->cursor_x--;
        } else {
            game_state->cursor_x = BOARD_SIZE - 1;
        }
        break;
    case InputKeyRight:
        if(game_state->cursor_x < BOARD_SIZE - 1) {
            game_state->cursor_x++;
        } else {
            game_state->cursor_x = 0;
        }
        break;
    default:
        break;
    }
    return true;
}

bool handle_key_menu(AppState* app_state, InputKey key) {
    switch(key) {
    case InputKeyUp:
        if(app_state->selected_menu_item > 0) {
            app_state->selected_menu_item--;
        }
        break;
    case InputKeyDown:
        if(app_state->selected_menu_item < MENU_ITEMS_COUNT - 1) {
            app_state->selected_menu_item++;
        }
        break;
    case InputKeyOk:
        if(app_state->selected_menu_item == 1) {
            // new game
            init_game(&app_state->game);
            save_game(&app_state->game);
        }
        app_state->screen = AppScreenGame;
        break;
    default:
        break;
    }
    return true;
}

// returns `true` if the event loop should keep going
bool handle_key(AppState* app_state, InputKey key) {
    GameState* game_state = &app_state->game;

    switch(app_state->screen) {
    case AppScreenGame:
        return handle_key_game(game_state, key);
        break;
    case AppScreenMenu:
        return handle_key_menu(app_state, key);
        break;
    }
    return true;
}

int32_t game_reversi_app() {
    AppState app_state;
    app_state.screen = AppScreenGame;
    if(!load_game(&app_state.game)) {
        init_game(&app_state.game);
    }

    app_state.mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if(!app_state.mutex) {
        return 255;
    }
    InputEvent input;
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, draw_callback, &app_state);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);
    bool is_finished = false;

    while(!is_finished) {
        // check if it's computer's turn
        if(!app_state.game.is_game_over &&
           (app_state.game.current_player != app_state.game.human_color)) {
            computer_move(&app_state.game);
        }
        FuriStatus event_status = furi_message_queue_get(event_queue, &input, FuriWaitForever);
        if(event_status == FuriStatusOk) {
            // handle only press event, ignore repeat/release events

            if(input.type == InputTypeLong && input.key == InputKeyOk &&
               app_state.screen == AppScreenGame) {
                furi_mutex_acquire(app_state.mutex, FuriWaitForever);
                app_state.selected_menu_item = 0;
                app_state.screen = AppScreenMenu;
                view_port_update(view_port);
                furi_mutex_release(app_state.mutex);
                continue;
            }
            if(input.type != InputTypePress) continue;

            furi_mutex_acquire(app_state.mutex, FuriWaitForever);
            is_finished = !handle_key(&app_state, input.key);
            view_port_update(view_port);
            furi_mutex_release(app_state.mutex);
        }
    }

    gui_remove_view_port(gui, view_port);
    furi_record_close(RECORD_GUI);

    view_port_free(view_port);

    furi_message_queue_free(event_queue);

    furi_mutex_free(app_state.mutex);

    return 0;
}
