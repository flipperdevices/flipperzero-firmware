#include <stdio.h>
#include <furi.h>
#include <furi_hal.h>
#include <dolphin/dolphin.h>
#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification_messages.h>
#include <storage/storage.h>

#define TAG "sudoku"

#define BOARD_SIZE 9
#define BOARD_SIZE_3 BOARD_SIZE / 3
#define FONT_SIZE 6

#define VALUE_MASK 0x0F
#define FLAGS_MASK ~VALUE_MASK
#define USER_INPUT_FLAG 0x80

static_assert(USER_INPUT_FLAG > VALUE_MASK);

#define EASY_GAPS 37
#define NORMAL_GAPS 44
#define HARD_GAPS 51

typedef enum {
    GameStateRunning,
    GameStatePaused,
    GameStateVictory,
    GameStateRestart, // util state
} GameState;

typedef struct {
    FuriMutex* mutex;
    uint8_t board[BOARD_SIZE][BOARD_SIZE];

    int8_t cursorX;
    int8_t cursorY;
    uint16_t horizontalFlags;
    uint16_t vertivalFlags;
    GameState state;
    int8_t menuCursor;
    int8_t lastGameMode;
    bool blockInputUntilRelease;
} SudokuState;

#define MENU_ITEMS_COUNT 5
const char* MENU_ITEMS[] = {
    "Continue",
    "Easy game",
    "Nornal game",
    "Hard game",
    "Save+Exit",
};

/*
Fontname: -Raccoon-Fixed4x6-Medium-R-Normal--6-60-75-75-P-40-ISO10646-1
Copyright: 
Glyphs: 95/203
BBX Build Mode: 0
*/
const uint8_t u8g2_font_tom_thumb_4x6_tr[725] =
    "_\0\2\2\2\3\3\4\4\3\6\0\377\5\377\5\0\0\352\1\330\2\270 \5\340\315\0!\6\265\310"
    "\254\0\42\6\213\313$\25#\10\227\310\244\241\206\12$\10\227\310\215\70b\2%\10\227\310d\324F\1"
    "&\10\227\310(\65R\22'\5\251\313\10(\6\266\310\251\62)\10\226\310\304\224\24\0*\6\217\312\244"
    "\16+\7\217\311\245\225\0,\6\212\310)\0-\5\207\312\14.\5\245\310\4/\7\227\310Ve\4\60"
    "\7\227\310-k\1\61\6\226\310\255\6\62\10\227\310h\220\312\1\63\11\227\310h\220\62X\0\64\10\227"
    "\310$\65b\1\65\10\227\310\214\250\301\2\66\10\227\310\315\221F\0\67\10\227\310\314TF\0\70\10\227"
    "\310\214\64\324\10\71\10\227\310\214\64\342\2:\6\255\311\244\0;\7\222\310e\240\0<\10\227\310\246\32"
    "d\20=\6\217\311l\60>\11\227\310d\220A*\1\77\10\227\310\314\224a\2@\10\227\310UC\3"
    "\1A\10\227\310UC\251\0B\10\227\310\250\264\322\2C\7\227\310\315\32\10D\10\227\310\250d-\0"
    "E\10\227\310\214\70\342\0F\10\227\310\214\70b\4G\10\227\310\315\221\222\0H\10\227\310$\65\224\12"
    "I\7\227\310\254X\15J\7\227\310\226\252\2K\10\227\310$\265\222\12L\7\227\310\304\346\0M\10\227"
    "\310\244\61\224\12N\10\227\310\244q\250\0O\7\227\310UV\5P\10\227\310\250\264b\4Q\10\227\310"
    "Uj$\1R\10\227\310\250\64V\1S\10\227\310m\220\301\2T\7\227\310\254\330\2U\7\227\310$"
    "W\22V\10\227\310$\253L\0W\10\227\310$\65\206\12X\10\227\310$\325R\1Y\10\227\310$U"
    "V\0Z\7\227\310\314T\16[\7\227\310\214X\16\134\10\217\311d\220A\0]\7\227\310\314r\4^"
    "\5\213\313\65_\5\207\310\14`\6\212\313\304\0a\7\223\310\310\65\2b\10\227\310D\225\324\2c\7"
    "\223\310\315\14\4d\10\227\310\246\245\222\0e\6\223\310\235\2f\10\227\310\246\264b\2g\10\227\307\35"
    "\61%\0h\10\227\310D\225\254\0i\6\265\310\244\1j\10\233\307f\30U\5k\10\227\310\304\264T"
    "\1l\7\227\310\310\326\0m\7\223\310<R\0n\7\223\310\250d\5o\7\223\310U\252\2p\10\227"
    "\307\250\244V\4q\10\227\307-\225d\0r\6\223\310\315\22s\10\223\310\215\70\22\0t\10\227\310\245"
    "\25\243\0u\7\223\310$+\11v\10\223\310$\65R\2w\7\223\310\244q\4x\7\223\310\244\62\25"
    "y\11\227\307$\225dJ\0z\7\223\310\254\221\6{\10\227\310\251\32D\1|\6\265\310(\1}\11"
    "\227\310\310\14RR\0~\6\213\313\215\4\0\0\0\4\377\377\0";

static int get_mode_gaps(int index) {
    if(index <= 0) {
        return EASY_GAPS;
    }
    if(index == 1) {
        return NORMAL_GAPS;
    }
    return HARD_GAPS;
}

#define SAVE_VERSION 2
#define SAVE_FILE APP_DATA_PATH("save.dat")

static bool load_game(SudokuState* state) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(storage);
    bool res = false;

    if(storage_file_open(file, SAVE_FILE, FSAM_READ, FSOM_OPEN_EXISTING)) {
        uint16_t version = 0;
        uint64_t expectedSize = sizeof(version) + sizeof(SudokuState);
        uint64_t fileSize = storage_file_size(file);
        if(fileSize >= expectedSize) {
            storage_file_read(file, &version, sizeof(version));
            if(version != SAVE_VERSION) {
                storage_simply_remove(storage, SAVE_FILE);
            } else {
                res = storage_file_read(file, state, sizeof(SudokuState)) == sizeof(SudokuState);
            }
        }
    }

    storage_file_free(file); // Closes the file if it was open.
    furi_record_close(RECORD_STORAGE);
    return res;
}

static void save_game(SudokuState* app) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(storage);

    if(storage_file_open(file, SAVE_FILE, FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
        uint16_t version = SAVE_VERSION;
        storage_file_write(file, &version, sizeof(version));
        storage_file_write(file, app, sizeof(SudokuState));
    }

    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
}

// inspired by game_2048
static void gray_canvas(Canvas* const canvas) {
    canvas_set_color(canvas, ColorWhite);
    for(int x = 0, mx = canvas_width(canvas); x < mx; x += 2) {
        for(int y = 0, my = canvas_height(canvas); y != my; y++) {
            canvas_draw_dot(canvas, x + (y % 2 == 1 ? 0 : 1), y);
        }
    }
}

static void draw_callback(Canvas* canvas, void* ctx) {
    SudokuState* state = ctx;
    furi_mutex_acquire(state->mutex, FuriWaitForever);

    canvas_clear(canvas);
    canvas_set_custom_u8g2_font(canvas, u8g2_font_tom_thumb_4x6_tr);

    int gapX = 0;
    int xOffset = 2;
    int yOffset = -2;
    for(int i = 0; i != BOARD_SIZE; ++i) {
        int gapY = 0;
        bool vflag = state->vertivalFlags & (1 << i);
        if((i % 3) == 0) gapX += 2;
        if(vflag) {
            // draw vertical hint line
            canvas_set_color(canvas, ColorBlack);
            canvas_draw_line(
                canvas,
                i * FONT_SIZE + gapX + xOffset - 1,
                0,
                i * FONT_SIZE + gapX + xOffset + FONT_SIZE - 3,
                0);
        }
        for(int j = 0; j != BOARD_SIZE; ++j) {
            if((j % 3) == 0) gapY += 4;
            canvas_set_color(canvas, ColorBlack);
            if(i == 0) {
                bool hflag = state->horizontalFlags & (1 << j);
                if(hflag) {
                    // draw horizontal hint line
                    canvas_draw_line(
                        canvas,
                        0,
                        j * FONT_SIZE + gapY + yOffset + 1,
                        0,
                        j * FONT_SIZE + gapY + yOffset + FONT_SIZE - 1);
                }
            }
            bool userInput = state->board[i][j] & USER_INPUT_FLAG;
            bool cursor = i == state->cursorX && j == state->cursorY;
            if(!userInput) {
                int xBoxOffset = cursor ? -1 : 0;
                // draw black box around the locked number
                canvas_draw_box(
                    canvas,
                    i * FONT_SIZE + gapX - 1 + xBoxOffset + xOffset,
                    j * FONT_SIZE + gapY + yOffset,
                    FONT_SIZE - 1 - xBoxOffset * 2,
                    FONT_SIZE + 1);
                // text will be white
                canvas_set_color(canvas, ColorXOR);
            } else if(cursor) {
                // draw frame around the cursor
                canvas_draw_frame(
                    canvas,
                    i * FONT_SIZE + gapX - 2 + xOffset,
                    j * FONT_SIZE + gapY + yOffset,
                    FONT_SIZE + 1,
                    FONT_SIZE + 1);
            }
            int value = state->board[i][j] & VALUE_MASK;
            if(value != 0) {
                canvas_draw_glyph(
                    canvas,
                    i * FONT_SIZE + gapX + xOffset,
                    (j + 1) * FONT_SIZE + gapY + yOffset,
                    '0' + value);
            }
        }
    }
    canvas_set_color(canvas, ColorBlack);
    gapX = 0;
    int gapY = 0;
    yOffset = 2;
    for(int i = 1; i != BOARD_SIZE / 3; ++i) {
        gapX += i;
        gapY += i * 2;
        // vertical lines
        canvas_draw_line(
            canvas,
            i * FONT_SIZE * 3 + xOffset + gapX,
            yOffset,
            i * FONT_SIZE * 3 + xOffset + gapX,
            FONT_SIZE * BOARD_SIZE + 8 + yOffset);
        // horizontal lines
        canvas_draw_line(
            canvas,
            xOffset,
            i * FONT_SIZE * 3 + gapY + yOffset,
            FONT_SIZE * BOARD_SIZE + xOffset + 3,
            i * FONT_SIZE * 3 + gapY + yOffset);
    }

    if(state->state == GameStateVictory || state->state == GameStatePaused) {
        gray_canvas(canvas);
        canvas_set_color(canvas, ColorWhite);
        int w = canvas_width(canvas);
        int h = canvas_height(canvas);
        int winW = 58;
        int winH = 48;
        int winX = (w - winW) / 2;
        int winY = (h - winH) / 2;
        canvas_draw_rbox(canvas, winX, winY, winW, winH, 4);
        canvas_set_color(canvas, ColorBlack);
        canvas_draw_rframe(canvas, winX, winY, winW, winH, 4);

        int offX = 6;
        int offY = 3;
        int itemH = FONT_SIZE + 2;
        for(int i = 0; i < MENU_ITEMS_COUNT; i++) {
            if(i == state->menuCursor) {
                canvas_set_color(canvas, ColorBlack);
                canvas_draw_box(
                    canvas, winX + offX, winY + offY + itemH * i, winW - offX * 2, itemH);
            }

            canvas_set_color(canvas, i == state->menuCursor ? ColorWhite : ColorBlack);
            canvas_draw_str_aligned(
                canvas,
                w / 2,
                winY + offY + itemH * i + itemH / 2,
                AlignCenter,
                AlignCenter,
                i == 0 && state->state == GameStateVictory ? "VICTORY!" : MENU_ITEMS[i]);
        }
    }
    furi_mutex_release(state->mutex);
}

static void input_callback(InputEvent* input_event, void* ctx) {
    FuriMessageQueue* event_queue = ctx;

    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

static void init_board(SudokuState* state) {
    for(int i = 0; i != BOARD_SIZE; ++i) {
        for(int j = 0; j != BOARD_SIZE; ++j) {
            state->board[i][j] = 1 + (i * BOARD_SIZE_3 + i % BOARD_SIZE_3 + j) % 9;
        }
    }
}

static void shuffle_board(SudokuState* state, int times) {
    uint8_t tmp[BOARD_SIZE];
    for(int t = 0; t < times; ++t) {
        // swap numbers
        int swapX, swapY;
        do {
            swapX = 1 + furi_hal_random_get() % BOARD_SIZE;
            swapY = 1 + furi_hal_random_get() % BOARD_SIZE;
        } while(swapX == swapY);
        for(int i = 0; i != BOARD_SIZE; ++i) {
            for(int j = 0; j != BOARD_SIZE; ++j) {
                if(state->board[i][j] == swapX) {
                    state->board[i][j] = swapY;
                } else if(state->board[i][j] == swapY) {
                    state->board[i][j] = swapX;
                }
            }
        }
        // swap columns
        for(int i = 0; i != BOARD_SIZE_3; ++i) {
            int swapX, swapY;
            int offset = i * BOARD_SIZE_3;
            do {
                swapX = offset + furi_hal_random_get() % BOARD_SIZE_3;
                swapY = offset + furi_hal_random_get() % BOARD_SIZE_3;
            } while(swapX == swapY);
            memcpy(tmp, state->board[swapX], BOARD_SIZE);
            memcpy(state->board[swapX], state->board[swapY], BOARD_SIZE);
            memcpy(state->board[swapY], tmp, BOARD_SIZE);
        }
        // swap rows
        for(int i = 0; i != BOARD_SIZE_3; ++i) {
            int swapX, swapY;
            int offset = i * BOARD_SIZE_3;
            do {
                swapX = offset + furi_hal_random_get() % BOARD_SIZE_3;
                swapY = offset + furi_hal_random_get() % BOARD_SIZE_3;
            } while(swapX == swapY);
            for(int k = 0; k != BOARD_SIZE; ++k) {
                FURI_SWAP(state->board[k][swapX], state->board[k][swapY]);
            }
        }
    }
}

static void add_gaps(SudokuState* state, int inputCells) {
    for(int i = 0; i <= inputCells; ++i) {
        int x, y;
        do {
            x = furi_hal_random_get() % BOARD_SIZE;
            y = furi_hal_random_get() % BOARD_SIZE;
        } while(state->board[x][y] & USER_INPUT_FLAG);
        state->board[x][y] = USER_INPUT_FLAG;
    }
}

static bool validate_board(SudokuState* state) {
    bool res = true;
    // check vertical lines for duplicates
    state->vertivalFlags = 0;
    for(int i = 0; i != BOARD_SIZE; ++i) {
        uint flags = 0;
        bool ok = true;
        for(int j = 0; j != BOARD_SIZE; ++j) {
            int value = state->board[i][j] & VALUE_MASK;
            if(value == 0) {
                ok = false;
                res = false;
            }
            if(flags & (1 << value)) {
                ok = false;
                res = false;
            }
            flags |= 1 << value;
        }
        if(ok) {
            state->vertivalFlags |= 1 << i;
        }
    }
    // check horizontal lines for duplicates
    state->horizontalFlags = 0;
    for(int i = 0; i != BOARD_SIZE; ++i) {
        bool ok = true;
        uint flags = 0;
        for(int j = 0; j != BOARD_SIZE; ++j) {
            int value = state->board[j][i] & VALUE_MASK;
            if(value == 0) {
                ok = false;
                res = false;
            }
            if(flags & (1 << value)) {
                ok = false;
                res = false;
            }
            flags |= 1 << value;
        }
        if(ok) {
            state->horizontalFlags |= 1 << i;
        }
    }
    if(!res) {
        return res;
    }
    // check 3x3 squares for duplicates
    for(int i = 0; i != BOARD_SIZE_3; ++i) {
        for(int j = 0; j != BOARD_SIZE_3; ++j) {
            uint flags = 0;
            for(int k = 0; k != BOARD_SIZE_3; ++k) {
                for(int l = 0; l != BOARD_SIZE_3; ++l) {
                    int value = state->board[i * BOARD_SIZE_3 + k][j * BOARD_SIZE_3 + l] &
                                VALUE_MASK;
                    if(flags & (1 << value)) {
                        return false;
                    }
                    flags |= 1 << value;
                }
            }
        }
    }
    return true;
}

static bool start_game(SudokuState* state) {
    state->state = GameStateRunning;
    state->cursorX = 0;
    state->cursorY = 0;
    state->blockInputUntilRelease = false;
    init_board(state);
    shuffle_board(state, 10);
    add_gaps(state, get_mode_gaps(state->lastGameMode));
    return validate_board(state);
}

int32_t sudoku_main(void* p) {
    UNUSED(p);

    InputEvent event;
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    SudokuState* state = malloc(sizeof(SudokuState));
    if(!load_game(state) || state->state == GameStateRestart) {
        state->menuCursor = 0;
        if(state->state != GameStateRestart)
            state->lastGameMode = 1; // set normal game mode by default, except restart
        start_game(state);
    }
    state->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    furi_check(state->mutex, "mutex alloc failed");
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, draw_callback, state);
    view_port_input_callback_set(view_port, input_callback, event_queue);
    view_port_set_orientation(view_port, ViewPortOrientationVertical);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    dolphin_deed(DolphinDeedPluginGameStart);

    while(true) {
        furi_check(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk);

        furi_mutex_acquire(state->mutex, FuriWaitForever);

        if(state->state == GameStatePaused || state->state == GameStateVictory) {
            bool exit = false;
            if(event.type == InputTypePress || event.type == InputTypeLong ||
               event.type == InputTypeRepeat) {
                switch(event.key) {
                case InputKeyLeft:
                case InputKeyUp:
                    state->menuCursor =
                        (state->menuCursor + MENU_ITEMS_COUNT - 1) % MENU_ITEMS_COUNT;
                    break;
                case InputKeyRight:
                case InputKeyDown:
                    state->menuCursor = (state->menuCursor + 1) % MENU_ITEMS_COUNT;
                    break;
                case InputKeyOk:
                    if(state->state == GameStatePaused && state->menuCursor == 0) {
                        state->state = GameStateRunning;
                    } else if(state->menuCursor >= 1 && state->menuCursor <= 3) {
                        state->lastGameMode = state->menuCursor - 1;
                        state->menuCursor = 0;
                        start_game(state);
                    } else if(state->menuCursor == 4) {
                        exit = true;
                        break;
                    }
                    break;
                default:
                    break;
                }
            }
            if(exit) {
                furi_mutex_release(state->mutex);
                break;
            }
        } else if(state->state == GameStateRunning) {
            bool invalidField = false;
            bool userInput = state->board[state->cursorX][state->cursorY] & USER_INPUT_FLAG;
            if(event.key == InputKeyBack) {
                if(event.type == InputTypeLong) {
                    state->state = GameStatePaused;
                } else if(userInput && event.type == InputTypeShort) {
                    invalidField = state->board[state->cursorX][state->cursorY] & VALUE_MASK;
                    state->board[state->cursorX][state->cursorY] &= FLAGS_MASK;
                }
            }

            bool invalidLidAndRow =
                !(state->horizontalFlags & (1 << state->cursorY) ||
                  state->vertivalFlags & (1 << state->cursorX));

            if(event.type == InputTypePress || event.type == InputTypeLong ||
               event.type == InputTypeRepeat) {
                switch(event.key) {
                case InputKeyLeft:
                    state->blockInputUntilRelease = false;
                    state->cursorX = (state->cursorX + BOARD_SIZE - 1) % BOARD_SIZE;
                    break;
                case InputKeyRight:
                    state->blockInputUntilRelease = false;
                    state->cursorX = (state->cursorX + 1) % BOARD_SIZE;
                    break;
                case InputKeyUp:
                    state->blockInputUntilRelease = false;
                    state->cursorY = (state->cursorY + BOARD_SIZE - 1) % BOARD_SIZE;
                    break;
                case InputKeyDown:
                    state->blockInputUntilRelease = false;
                    state->cursorY = (state->cursorY + 1) % BOARD_SIZE;
                    break;
                case InputKeyOk:
                    if(userInput && !state->blockInputUntilRelease) {
                        int flags = state->board[state->cursorX][state->cursorY] & FLAGS_MASK;
                        int value = state->board[state->cursorX][state->cursorY] & VALUE_MASK;
                        state->board[state->cursorX][state->cursorY] = flags | ((value + 1) % 10);
                        invalidField = true;
                    }
                    break;
                default:
                    break;
                }
            } else if(event.type == InputTypeRelease) {
                state->blockInputUntilRelease = false;
            }
            if(invalidField) {
                if(validate_board(state)) {
                    dolphin_deed(DolphinDeedPluginGameWin);
                    state->state = GameStateVictory;
                    state->menuCursor = 0;
                    for(int i = 0; i != BOARD_SIZE; ++i) {
                        for(int j = 0; j != BOARD_SIZE; ++j) {
                            state->board[i][j] &= ~USER_INPUT_FLAG;
                        }
                    }
                } else {
                    bool isValidLineOrRow = state->horizontalFlags & (1 << state->cursorY) ||
                                            state->vertivalFlags & (1 << state->cursorX);
                    if(invalidLidAndRow && isValidLineOrRow) {
                        state->blockInputUntilRelease = true;
                    }
                }
            }
        }
        furi_mutex_release(state->mutex);
        view_port_update(view_port);
    }

    furi_message_queue_free(event_queue);

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);

    furi_mutex_free(state->mutex);
    if(state->state == GameStateVictory) {
        state->state = GameStateRestart;
    }
    state->menuCursor = 0; // reset menu cursor, because we stand on exit
    state->mutex = NULL;
    save_game(state);
    free(state);

    return 0;
}