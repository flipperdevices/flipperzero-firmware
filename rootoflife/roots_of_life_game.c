#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>
#include <gui/view.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>

#include "roots_of_life_game_icons.h"

#define TAG "RootsOfLife"

// Flipper
#define FLIPPER_LCD_WIDTH 128
#define FLIPPER_LCD_HEIGHT 64

// General
#define GROUND_HEIGHT 10
#define CELL_SIZE 3
#define FIELD_START_X 0
#define FIELD_START_Y (GROUND_HEIGHT + 1)
#define CELLS_X (FLIPPER_LCD_WIDTH / CELL_SIZE)
#define CELLS_Y ((FLIPPER_LCD_HEIGHT - GROUND_HEIGHT) / CELL_SIZE)
#define CELLS_TOTAL (CELLS_Y * CELLS_X)
#define CELL(Y, X) (Y * CELLS_X + X)

// Root Spawn
#define ROOT_SIZE_X 7
#define ROOT_SIZE_Y 7
#define ROOT(Y, X) ((Y)*ROOT_SIZE_X + (X))

#define SPAWN_DIRECTIONS 2
#define GROW_STEPS 4
#define GROW_SAME_DIRECTION_CHANCE 70
#define RANDOM_GROW_ATTEMPTS 4
#define RANDOM_GROW_CHANCE 50

// UI
#define BLINK_PERIOD 12
#define BLINK_HIDE_FRAMES 5
#define TREE_HEIGHT 10
#define PICKUP_FREQUENCY 10

// Game
#define REROLLS_MAX 5
#define SCORE_FACTOR 10

#define PICKUPS_MIN 1
#define PICKUPS_MAX 5
#define PICKUPS_POINTS_FACTOR 10

typedef enum { EventTypeTick, EventTypeKey } EventType;

typedef enum {
    R_NONE = 0,
    R_UP = 0b1000,
    R_DOWN = 0b0100,
    R_LEFT = 0b0010,
    R_RIGHT = 0b0001
} Direction;

typedef enum { StageStart, StageRun, StageOver } GameStage;

typedef struct {
    bool initialDraw;

    GameStage stage;
    int tick;

    bool* filledCells;
    char* cells;
    bool* pickups;
    int collectedPickups;

    bool* filledRootBase;
    char* rootBase;

    int rootSizeX;
    int rootSizeY;
    bool* filledRoot;
    char* root;

    int pX, pY;

    int rerolls;
    int score;

    FuriMutex* mutex;
} GameState;

typedef struct {
    EventType type;
    InputEvent input;
} GameEvent;

static Direction rand_dir() {
    int r = rand() % 4;
    return 1 << r;
}

static Direction reverse_dir(Direction dir) {
    switch(dir) {
    case R_UP:
        return R_DOWN;
    case R_DOWN:
        return R_UP;
    case R_LEFT:
        return R_RIGHT;
    case R_RIGHT:
        return R_LEFT;

    default:
        return R_NONE;
    }
}

static int rand_range(int min, int max) {
    return min + rand() % (max - min);
}
static bool rand_chance(int chance) {
    return (rand() % 100) < chance;
}

static bool has_intersection(char cellA, char cellB) {
    return cellA & cellB;
}

static int root_index(GameState* state, int y, int x) {
    return y * state->rootSizeX + x;
}

static void set_cell(GameState* state, int y, int x, char cellRoot) {
    int c = CELL(y, x);
    state->filledCells[c] = true;
    state->cells[c] = cellRoot;
}

static void game_state_init(GameState* state) {
    state->initialDraw = false;
    state->tick = 0;

    // Init field arrays
    state->filledCells = (bool*)malloc(CELLS_TOTAL * sizeof(bool));
    state->cells = (char*)malloc(CELLS_TOTAL * sizeof(char));
    state->pickups = (bool*)malloc(CELLS_TOTAL * sizeof(char));

    state->rootBase = (char*)malloc(ROOT_SIZE_X * ROOT_SIZE_Y * sizeof(char));
    state->filledRootBase = (bool*)malloc(ROOT_SIZE_X * ROOT_SIZE_Y * sizeof(bool));
    state->root = NULL;
    state->filledRoot = NULL;

    for(int i = 0; i < CELLS_TOTAL; i++) {
        state->filledCells[i] = false;
        state->cells[i] = R_NONE;
        state->pickups[i] = false;
    }
}

static void free_root(GameState* state) {
    if(state->root) free(state->root);
    if(state->filledRoot) free(state->filledRoot);
}

static void game_state_free(GameState* state) {
    free(state->filledCells);
    free(state->cells);
    free(state->pickups);

    free(state->rootBase);
    free(state->filledRootBase);

    free_root(state);
}

/*static bool has_root(GameState* state, int x, int y) {
    return x >= 0 && x < ROOT_SIZE_X && y >= 0 && y < ROOT_SIZE_Y &&
           state->filledRootBase[ROOT(y, x)];
}*/

static void generate_new_root(GameState* state) {
    for(int i = 0; i < ROOT_SIZE_X * ROOT_SIZE_Y; i++) {
        state->filledRootBase[i] = false;
        state->rootBase[i] = R_NONE;
    }

    int cX = ROOT_SIZE_X / 2;
    int cY = ROOT_SIZE_Y / 2;
    int c = ROOT(cY, cX);
    state->filledRootBase[c] = true;

    for(int i = 0; i < SPAWN_DIRECTIONS; i++) {
        int pX = cX, pY = cY;
        Direction oldDir = rand_dir();
        for(int g = 0; g < GROW_STEPS; g++) {
            Direction dir = rand_chance(GROW_SAME_DIRECTION_CHANCE) ? oldDir : rand_dir();
            oldDir = dir;

            int nX = pX - (dir & R_LEFT ? 1 : 0) + (dir & R_RIGHT ? 1 : 0);
            int nY = pY - (dir & R_UP ? 1 : 0) + (dir & R_DOWN ? 1 : 0);
            if(nX < 0 || nY < 0 || nX >= ROOT_SIZE_X || nY >= ROOT_SIZE_Y) continue;

            int n = ROOT(nY, nX);
            state->filledRootBase[n] = true;

            // Connect points
            int p = ROOT(pY, pX);
            state->rootBase[p] |= dir;
            state->rootBase[n] |= reverse_dir(dir);

            // Grow from new point
            pX = nX;
            pY = nY;
        }
    }

    for(int y = 0; y < ROOT_SIZE_Y; y++) {
        for(int x = 0; x < ROOT_SIZE_X; x++) {
            int c = ROOT(y, x);
            if(!state->filledRootBase[c]) continue;

            /*
            if(has_root(state, x - 1, y)) state->rootBase[c] |= R_LEFT;
            if(has_root(state, x + 1, y)) state->rootBase[c] |= R_RIGHT;
            if(has_root(state, x, y - 1)) state->rootBase[c] |= R_UP;
            if(has_root(state, x, y + 1)) state->rootBase[c] |= R_DOWN;
            */

            for(int r = 0; r < RANDOM_GROW_ATTEMPTS; r++) {
                if(!rand_chance(RANDOM_GROW_CHANCE)) continue;
                state->rootBase[c] |= rand_dir();
            }
        }
    }

    // Copy root to real root
    int minX = cX, maxX = cX, minY = cY, maxY = cY;
    for(int y = 0; y < ROOT_SIZE_Y; y++) {
        for(int x = 0; x < ROOT_SIZE_X; x++) {
            int r = ROOT(y, x);
            if(!state->filledRootBase[r]) continue;

            minX = MIN(minX, x);
            maxX = MAX(maxX, x);
            minY = MIN(minY, y);
            maxY = MAX(maxY, y);
        }
    }

    // Clone to real root
    state->rootSizeX = maxX - minX + 1;
    state->rootSizeY = maxY - minY + 1;
    free_root(state);

    state->root = (char*)malloc(state->rootSizeX * state->rootSizeY * sizeof(char));
    state->filledRoot = (bool*)malloc(state->rootSizeX * state->rootSizeY * sizeof(bool));
    for(int y = 0; y < state->rootSizeY; y++) {
        for(int x = 0; x < state->rootSizeX; x++) {
            int c = root_index(state, y, x);
            int r = ROOT(y + minY, x + minX);
            state->filledRoot[c] = state->filledRootBase[r];
            state->root[c] = state->rootBase[r];
        }
    }
}

static bool in_borders(int x, int y) {
    return x >= 0 && y >= 0 && x < CELLS_X && y < CELLS_Y;
}
static char get_cell(GameState* state, int x, int y) {
    if(!in_borders(x, y)) return R_NONE;
    return state->cells[CELL(y, x)];
}

static bool get_filled_cell(GameState* state, int x, int y) {
    if(!in_borders(x, y)) return false;
    return state->filledCells[CELL(y, x)];
}

static bool can_place_root(GameState* state) {
    bool hasConnection = false;
    for(int y = 0; y < state->rootSizeY; y++) {
        for(int x = 0; x < state->rootSizeX; x++) {
            int r = root_index(state, y, x);
            if(!state->filledRoot[r]) {
                continue;
            }
            char root = state->root[r];

            int rY = y + state->pY;
            int rX = x + state->pX;

            // Check if colliding
            if(get_filled_cell(state, rX, rY)) {
                char cell = get_cell(state, rX, rY);
                if(has_intersection(cell, root)) {
                    return false;
                }
                hasConnection = true;
            }

            // Check neighbours
            hasConnection |= (root & R_RIGHT) && (get_cell(state, rX + 1, rY) & R_LEFT);
            hasConnection |= (root & R_LEFT) && (get_cell(state, rX - 1, rY) & R_RIGHT);
            hasConnection |= (root & R_UP) && (get_cell(state, rX, rY - 1) & R_DOWN);
            hasConnection |= (root & R_DOWN) && (get_cell(state, rX, rY + 1) & R_UP);
        }
    }

    return hasConnection;
}

static bool try_place_root(GameState* state) {
    if(!can_place_root(state)) return false;

    for(int y = 0; y < state->rootSizeY; y++) {
        for(int x = 0; x < state->rootSizeX; x++) {
            int r = root_index(state, y, x);
            if(!state->filledRoot[r]) continue;

            int rY = y + state->pY;
            int rX = x + state->pX;

            // Root may be out of borders in rare cases (after new cpawn changed its size), just ignore that part
            if(in_borders(rX, rY)) {
                int c = CELL(rY, rX);

                state->filledCells[c] = true;
                state->cells[c] |= state->root[r];
            }
        }
    }

    return true;
}

static void reset_level(GameState* state) {
    state->stage = StageStart;
    state->tick = 0;

    for(int i = 0; i < CELLS_TOTAL; i++) {
        state->filledCells[i] = false;
        state->cells[i] = R_NONE;
    }

    generate_new_root(state);

    // Starting cells
    int midX = CELLS_X / 2;
    set_cell(state, 0, midX, R_UP | R_DOWN);
    set_cell(state, 1, midX, R_UP | R_DOWN | R_LEFT | R_RIGHT);
    set_cell(state, 1, midX - 1, R_RIGHT | R_DOWN);
    set_cell(state, 1, midX + 1, R_LEFT | R_DOWN);
    set_cell(state, 2, midX, R_UP);

    state->pX = midX;
    state->pY = 4;

    state->rerolls = REROLLS_MAX;
    state->score = 0;

    state->collectedPickups = 0;
    for(int i = 0, n = rand_range(PICKUPS_MIN, PICKUPS_MAX); i < n; i++) {
        int x = rand_range(0, CELLS_X);
        int y = rand_range(0, CELLS_Y);
        state->pickups[CELL(y, x)] = true;
    }
}

static void recalculate_score(GameState* state) {
    int score = 0;
    for(int i = 0; i < CELLS_TOTAL; i++) {
        if(state->filledCells[i]) score++;
    }

    for(int i = 0; i < CELLS_TOTAL; i++) {
        if(!state->pickups[i] || !state->filledCells[i]) continue;

        state->pickups[i] = false;
        state->collectedPickups++;
        state->rerolls++;
    }

    state->score = (score + state->collectedPickups * PICKUPS_POINTS_FACTOR) * SCORE_FACTOR;
}

static void draw_root_cell(Canvas* canvas, char root, int y, int x, bool isHidden) {
    int posX = FIELD_START_X + x * CELL_SIZE + 1, posY = FIELD_START_Y + y * CELL_SIZE + 1;
    canvas_draw_dot(canvas, posX, posY);

    if(isHidden) {
        canvas_set_color(canvas, ColorXOR);
    }

    if(root & R_UP) canvas_draw_dot(canvas, posX, posY - 1);
    if(root & R_DOWN) canvas_draw_dot(canvas, posX, posY + 1);
    if(root & R_LEFT) canvas_draw_dot(canvas, posX - 1, posY);
    if(root & R_RIGHT) canvas_draw_dot(canvas, posX + 1, posY);

    if(isHidden) {
        canvas_set_color(canvas, ColorBlack);
    }
}

static void draw_placed_roots(Canvas* canvas, GameState* state) {
    for(int y = 0; y < CELLS_Y; y++) {
        for(int x = 0; x < CELLS_X; x++) {
            int c = CELL(y, x);
            if(!state->filledCells[c]) continue;
            draw_root_cell(canvas, state->cells[c], y, x, false);
        }
    }
}

static void draw_pickup(Canvas* canvas, GameState* state, int y, int x) {
    int posX = FIELD_START_X + x * CELL_SIZE + 1, posY = FIELD_START_Y + y * CELL_SIZE + 1;

    int stage = state->tick / PICKUP_FREQUENCY;

    if(stage++ % 4 < 3) canvas_draw_dot(canvas, posX + 1, posY);
    if(stage++ % 4 < 3) canvas_draw_dot(canvas, posX, posY + 1);
    if(stage++ % 4 < 3) canvas_draw_dot(canvas, posX - 1, posY);
    if(stage++ % 4 < 3) canvas_draw_dot(canvas, posX, posY - 1);
}

static void draw_pickups(Canvas* canvas, GameState* state) {
    for(int y = 0; y < CELLS_Y; y++) {
        for(int x = 0; x < CELLS_X; x++) {
            int c = CELL(y, x);
            if(!state->pickups[c]) continue;
            draw_pickup(canvas, state, y, x);
        }
    }
}

static void draw_active_root(Canvas* canvas, GameState* state) {
    bool isHidden = (state->tick % BLINK_PERIOD) < BLINK_HIDE_FRAMES;

    for(int y = 0; y < state->rootSizeY; y++) {
        for(int x = 0; x < state->rootSizeX; x++) {
            int c = root_index(state, y, x);
            if(!state->filledRoot[c]) continue;

            int realX = x + state->pX;
            int realY = y + state->pY;
            draw_root_cell(canvas, state->root[c], realY, realX, isHidden);
        }
    }
}

#if DRAW_DEBUG
static void draw_generated_root(Canvas* canvas, GameState* state) {
    bool isHidden = (state->tick % BLINK_PERIOD) < BLINK_HIDE_FRAMES;

    for(int y = 0; y < ROOT_SIZE_Y; y++) {
        for(int x = 0; x < ROOT_SIZE_X; x++) {
            int c = ROOT(y, x);
            if(!state->filledRootBase[c]) continue;

            int realX = x + 1;
            int realY = y + 1;
            draw_root_cell(canvas, state->rootBase[c], realY, realX, isHidden);
        }
    }
}
#endif

static void draw_ground(Canvas* canvas, GameState* state) {
    canvas_draw_line(canvas, 0, GROUND_HEIGHT, FLIPPER_LCD_WIDTH, GROUND_HEIGHT);
    UNUSED(state);
}

static void draw_tree(Canvas* canvas, GameState* state) {
    canvas_draw_icon(canvas, FLIPPER_LCD_WIDTH / 2 - 5, GROUND_HEIGHT - TREE_HEIGHT, &I_tree);
    UNUSED(state);
}

static void draw_placement(Canvas* canvas, GameState* state) {
    bool canPlace = can_place_root(state);
    canvas_draw_icon(canvas, FLIPPER_LCD_WIDTH - 10, 0, canPlace ? &I_place_ok : &I_place_error);
}

static void draw_rerolls(Canvas* canvas, GameState* state) {
    UNUSED(canvas);
    UNUSED(state);

    canvas_draw_icon(canvas, 0, 0, &I_root_reroll);

    // Ugh
    FuriString* tmp_string = furi_string_alloc();
    furi_string_printf(tmp_string, "%d", MAX(0, state->rerolls));
    canvas_draw_str(canvas, 11, 9, furi_string_get_cstr(tmp_string));
    furi_string_free(tmp_string);
}

static void draw_score(Canvas* canvas, GameState* state) {
    UNUSED(canvas);
    UNUSED(state);

    int x = FLIPPER_LCD_WIDTH / 2 + 15;
    canvas_draw_icon(canvas, x, 0, &I_score);

    // Ugh
    FuriString* tmp_string = furi_string_alloc();
    furi_string_printf(tmp_string, "%d", MAX(0, state->score));
    canvas_draw_str(canvas, x + 11, 9, furi_string_get_cstr(tmp_string));
    furi_string_free(tmp_string);
}

static void draw_gui(Canvas* canvas, GameState* state) {
    draw_ground(canvas, state);
    draw_tree(canvas, state);
    draw_placement(canvas, state);
    draw_rerolls(canvas, state);
    draw_score(canvas, state);
}

static void draw_center_box(Canvas* canvas, int w2, int h2, int margin) {
    int x = FLIPPER_LCD_WIDTH / 2 - w2;
    int y = FLIPPER_LCD_HEIGHT / 2 - h2;

    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(
        canvas, x - margin - 1, y - margin - 1, (w2 + margin + 1) * 2, (h2 + margin + 1) * 2);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_frame(canvas, x - margin, y - margin, (w2 + margin) * 2, (h2 + margin) * 2);
}

static void draw_start_ui(Canvas* canvas, GameState* state) {
    int w2 = 40;
    int margin = 3;
    int h2 = 10;
    draw_center_box(canvas, w2, h2, margin);

    int x = FLIPPER_LCD_WIDTH / 2 - w2;
    int y = FLIPPER_LCD_HEIGHT / 2 - h2;
    canvas_draw_str(canvas, x + 1, y + 9, "  Grow your roots  ");
    canvas_draw_str(canvas, x + 1, y + 18, "Press [OK] to start");

    UNUSED(state);
}

static void draw_end_ui(Canvas* canvas, GameState* state) {
    int w2 = 46;
    int margin = 3;
    int h2 = 15;
    draw_center_box(canvas, w2, h2, margin);

    int x = FLIPPER_LCD_WIDTH / 2 - w2;
    int y = FLIPPER_LCD_HEIGHT / 2 - h2;

    canvas_draw_str(canvas, x + 1, y + 9, "        Game Over        ");

    FuriString* tmp_string = furi_string_alloc();
    furi_string_printf(tmp_string, "You've got %d points", MAX(0, state->score));
    canvas_draw_str(canvas, x + 1, y + 19, furi_string_get_cstr(tmp_string));
    furi_string_free(tmp_string);

    canvas_draw_str(canvas, x + 2, y + 29, "Press [OK] to restart");

    int h = 13, w = 54;
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, 0, FLIPPER_LCD_HEIGHT - h, w + 1, h + 1);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_frame(canvas, 0, FLIPPER_LCD_HEIGHT - h, w, h);
    canvas_draw_str(canvas, 2, FLIPPER_LCD_HEIGHT - 3, "by @Xorboo");
    UNUSED(state);
}

static void roots_draw_callback(Canvas* const canvas, void* ctx) {
    furi_assert(ctx);
    GameState* state = ctx;
    furi_mutex_acquire(state->mutex, FuriWaitForever);

    if(!state->initialDraw) {
        state->initialDraw = true;

        canvas_set_font(canvas, FontSecondary);
        reset_level(state);
    }

    state->tick++;

    draw_gui(canvas, state);
    draw_placed_roots(canvas, state);
    draw_pickups(canvas, state);

    switch(state->stage) {
    case StageStart:
        draw_start_ui(canvas, state);
        break;

    case StageRun:
        draw_active_root(canvas, state);
#if DRAW_DEBUG
        draw_generated_root(canvas, state);
#endif
        break;

    case StageOver:
        draw_end_ui(canvas, state);
        break;
    }

    furi_mutex_release(state->mutex);
}

static void roots_input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);

    GameEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void roots_update_timer_callback(FuriMessageQueue* event_queue) {
    furi_assert(event_queue);

    GameEvent event = {.type = EventTypeTick};
    furi_message_queue_put(event_queue, &event, 0);
}

static void ProcessStartInput(GameState* state, InputKey key) {
    if(key == InputKeyOk) {
        state->stage = StageRun;
    }
}

static void ProcessRunInput(GameState* state, InputKey key) {
    switch(key) {
    case InputKeyRight:
        state->pX = MIN(state->pX + 1, CELLS_X - state->rootSizeX);
        break;
    case InputKeyLeft:
        state->pX = MAX(state->pX - 1, 0);
        break;
    case InputKeyUp:
        state->pY = MAX(state->pY - 1, 0);
        break;
    case InputKeyDown:
        state->pY = MIN(state->pY + 1, CELLS_Y - state->rootSizeY);
        break;
    case InputKeyOk: {
        bool rootPlaced = try_place_root(state);
        if(rootPlaced) {
            recalculate_score(state);
            generate_new_root(state);
        } else {
            state->rerolls--;
            if(state->rerolls >= 0) {
                generate_new_root(state);
            } else {
                state->stage = StageOver;
            }
        }
        break;
    }
    default:
        break;
    }
}

static void ProcessOverInput(GameState* state, InputKey key) {
    if(key == InputKeyOk) {
        state->stage = StageStart;
        reset_level(state);
    }
}

int32_t roots_of_life_game_app(void* p) {
    FURI_LOG_D(TAG, "Starting game...");

    UNUSED(p);
    int32_t return_code = 0;

    // Set random seed from interrR_UPts
    srand(DWT->CYCCNT);

    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(GameEvent));

    GameState* state = malloc(sizeof(GameState));
    game_state_init(state);

    state->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if(!state->mutex) {
        FURI_LOG_E(TAG, "Cannot create mutex\r\n");
        return_code = 255;
        goto free_and_exit;
    }

    // Set system callbacks
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, roots_draw_callback, state);
    view_port_input_callback_set(view_port, roots_input_callback, event_queue);

    FuriTimer* timer =
        furi_timer_alloc(roots_update_timer_callback, FuriTimerTypePeriodic, event_queue);
    furi_timer_start(timer, furi_kernel_get_tick_frequency() / 22);

    // Open GUI and register view_port
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    FURI_LOG_D(TAG, "Entering game loop...");
    GameEvent event;
    for(bool processing = true; processing;) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 100);
        furi_mutex_acquire(state->mutex, FuriWaitForever);

        if(event_status == FuriStatusOk) {
            // Key events
            if(event.type == EventTypeKey) {
                //FURI_LOG_D(TAG, "Got key: %d", event.input.key);
                if(event.input.type == InputTypePress || event.input.type == InputTypeLong ||
                   event.input.type == InputTypeRepeat) {
                    if(event.input.key == InputKeyBack) {
                        processing = false;
                    }

                    switch(state->stage) {
                    case StageStart:
                        ProcessStartInput(state, event.input.key);
                        break;
                    case StageRun:
                        ProcessRunInput(state, event.input.key);
                        break;
                    case StageOver:
                        ProcessOverInput(state, event.input.key);
                        break;
                    }
                }
            }
        }

        furi_mutex_release(state->mutex);
        view_port_update(view_port);
    }

    furi_timer_free(timer);
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);
    view_port_free(view_port);
    furi_mutex_free(state->mutex);

free_and_exit:
    //FURI_LOG_D(TAG, "Quitting game...");
    game_state_free(state);
    free(state);
    furi_message_queue_free(event_queue);

    return return_code;
}