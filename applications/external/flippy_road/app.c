#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <furi_hal_random.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include <storage/storage.h>
#include <dolphin/dolphin.h>

#include "flippy_road_icons.h"

#define TAG "Flippy Road"
#define SAVE_FILE APP_DATA_PATH("flippy_road.save")

#define WIDTH 128
#define HEIGHT 64
#define GRID_UNIT 8
#define ROWS (HEIGHT / GRID_UNIT)
#define COLS (WIDTH / GRID_UNIT)
#define MAX_OBSTACLES 3
#define LOG_LENGTH 3

typedef enum {
    TickEvent,
    KeyEvent,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} Event;

typedef enum {
    Grass,
    Road,
    River,
} TerrainType;

typedef struct {
    TerrainType type;
    uint8_t y;
    uint8_t speed;
    uint8_t obstacles[MAX_OBSTACLES];
} Terrain;

typedef struct {
    FuriMutex* mutex;
    Terrain terrains[ROWS];
    Terrain* lastTerrain;
    Terrain* currTerrain;
    uint8_t x;
    uint16_t pos;
    uint16_t score;
    bool gameOver;
    bool deathNotification;
} GameState;

static const NotificationSequence sequence_fail = {
    &message_vibro_on,

    &message_note_ds4,
    &message_delay_10,
    &message_sound_off,
    &message_delay_10,

    &message_note_ds4,
    &message_delay_10,
    &message_sound_off,
    &message_delay_10,

    &message_note_ds4,
    &message_delay_10,
    &message_sound_off,
    &message_delay_10,

    &message_vibro_off,
    NULL,
};

static uint16_t high_score;

static bool storage_load() {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    storage_common_migrate(storage, EXT_PATH("apps/Games/flippy_road.save"), SAVE_FILE);
    File* file = storage_file_alloc(storage);

    uint16_t bytes_read = 0;
    if(storage_file_open(file, SAVE_FILE, FSAM_READ, FSOM_OPEN_EXISTING)) {
        bytes_read = storage_file_read(file, &high_score, sizeof(high_score));
    }

    storage_file_close(file);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
    return bytes_read == sizeof(high_score);
}

static void storage_save() {
    Storage* storage = furi_record_open(RECORD_STORAGE);

    File* file = storage_file_alloc(storage);
    if(storage_file_open(file, SAVE_FILE, FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
        storage_file_write(file, &high_score, sizeof(high_score));
    }
    storage_file_close(file);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
}

static void draw_road(Canvas* canvas, Terrain* road) {
    // Draw road
    canvas_draw_line(canvas, 0, road->y, WIDTH, road->y);
    for(float i = 0.5; i < 10; i += 2) {
        canvas_draw_line(
            canvas,
            WIDTH * i / 10,
            road->y + GRID_UNIT / 2,
            WIDTH * (i + 1) / 10,
            road->y + GRID_UNIT / 2);
    }
    canvas_draw_line(canvas, 0, road->y + GRID_UNIT, WIDTH, road->y + GRID_UNIT);

    // Draw cars
    for(uint8_t i = 0; i < MAX_OBSTACLES; i++) {
        canvas_draw_box(
            canvas, road->obstacles[i] * GRID_UNIT, road->y + 2, GRID_UNIT, GRID_UNIT - 3);
    }
}

static void draw_river(Canvas* canvas, Terrain* river) {
    // Draw river
    uint8_t river_tiles[COLS];
    memset(river_tiles, true, COLS);
    for(uint8_t i = 0; i < MAX_OBSTACLES; i++) {
        for(uint8_t j = 0; j < LOG_LENGTH; j++) {
            if(river->obstacles[i] == COLS - 1) {
                river_tiles[0] = false;
            }
            river_tiles[(river->obstacles[i] + j) % COLS] = false;
        }
    }

    for(uint8_t i = 0; i < COLS; i++) {
        if(river_tiles[i]) {
            canvas_draw_line(
                canvas,
                i * GRID_UNIT + 2,
                river->y + GRID_UNIT / 2,
                i * GRID_UNIT + 6,
                river->y + GRID_UNIT / 2 - 1);
        }
    }

    // Draw logs
    for(uint8_t i = 0; i < MAX_OBSTACLES; i++) {
        canvas_draw_frame(
            canvas, river->obstacles[i] * GRID_UNIT, river->y, GRID_UNIT * LOG_LENGTH, GRID_UNIT);
        if(river->obstacles[i] > COLS - LOG_LENGTH) {
            canvas_draw_frame(
                canvas,
                0,
                river->y,
                GRID_UNIT * (LOG_LENGTH - (COLS - river->obstacles[i])),
                GRID_UNIT);
        }
    }
}

static void draw_callback(Canvas* canvas, void* model) {
    furi_assert(model);
    GameState* state = (GameState*)model;

    furi_mutex_acquire(state->mutex, FuriWaitForever);

    canvas_draw_icon(canvas, state->x * GRID_UNIT, state->currTerrain->y, &I_frog);
    canvas_draw_frame(canvas, 0, 0, WIDTH, HEIGHT);

    // Draw terrain
    for(uint8_t i = 0; i < ROWS; i++) {
        Terrain* terrain = &(state->terrains[i]);
        switch(terrain->type) {
        case Road:
            draw_road(canvas, terrain);
            break;
        case Grass:
            break;
        case River:
            draw_river(canvas, terrain);
            break;
        }
    }

    // Game over screen
    if(state->gameOver) {
        canvas_set_color(canvas, ColorWhite);
        canvas_draw_box(canvas, 34, 14, 62, 36);
        canvas_set_color(canvas, ColorBlack);
        canvas_draw_frame(canvas, 34, 14, 62, 36);

        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 37, 25, "Game Over");

        canvas_set_font(canvas, FontSecondary);
        FuriString* score_str = furi_string_alloc();
        furi_string_printf(score_str, "Score: %u", state->score);
        canvas_draw_str_aligned(
            canvas, 64, 35, AlignCenter, AlignBottom, furi_string_get_cstr(score_str));
        furi_string_printf(score_str, " Best: %u", high_score);
        canvas_draw_str_aligned(
            canvas, 64, 45, AlignCenter, AlignBottom, furi_string_get_cstr(score_str));
        furi_string_free(score_str);
    }

    furi_mutex_release(state->mutex);
}

static void input_callback(InputEvent* input_event, void* ctx) {
    FuriMessageQueue* event_queue = ctx;
    furi_assert(event_queue);
    Event event = {.type = KeyEvent, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void timer_callback(void* ctx) {
    FuriMessageQueue* event_queue = ctx;
    furi_assert(event_queue);
    Event event = {.type = TickEvent};
    furi_message_queue_put(event_queue, &event, 0);
}

static void generate_terrain(GameState* state, Terrain* terrain, uint8_t new_y, bool starter) {
    terrain->y = new_y;
    terrain->speed = (uint8_t)furi_hal_random_get() % 2 ? 1 : -1;

    if(starter) {
        terrain->type = Road;
    } else {
        uint8_t seed = (uint8_t)furi_hal_random_get() % 100;
        if(state->lastTerrain->type == River) {
            if(seed > 30) {
                terrain->speed = -(state->lastTerrain->speed);
                terrain->type = River;
            } else if(seed > 0) {
                terrain->type = Road;
            } else {
                terrain->type = Grass;
            }
        } else {
            if(seed > 30) {
                terrain->type = Road;
            } else if(seed > 5) {
                terrain->type = River;
            } else {
                terrain->type = Grass;
            }
        }
    }

    if(terrain->type == Road) {
        for(uint8_t i = 0; i < MAX_OBSTACLES; i++) {
            terrain->obstacles[i] = (uint8_t)furi_hal_random_get() % COLS;
        }
    }

    if(terrain->type == River) {
        uint8_t x = (uint8_t)furi_hal_random_get() % COLS;
        for(uint8_t i = 0; i < MAX_OBSTACLES; i++) {
            terrain->obstacles[i] = x;
            x += (uint8_t)furi_hal_random_get() % (COLS / MAX_OBSTACLES - (i + 1)) + LOG_LENGTH -
                 1 + 1;
        }
    }

    state->lastTerrain = terrain;
}

static void scroll(GameState* state, uint8_t scroll_pixels) {
    for(uint8_t i = 0; i < ROWS; i++) {
        Terrain* terrain = &(state->terrains[i]);
        terrain->y += scroll_pixels;
        if(terrain->y >= HEIGHT) {
            if(state->currTerrain->y == terrain->y) {
                state->gameOver = true;
            } else {
                generate_terrain(state, terrain, 0, false);
            }
        }
    }
}

static bool check_collision(GameState* state) {
    if(state->gameOver) {
        return true;
    }

    switch(state->currTerrain->type) {
    case Road:
        for(uint8_t i = 0; i < MAX_OBSTACLES; i++) {
            if(state->currTerrain->obstacles[i] == state->x) {
                return true;
            }
        }
        return false;
    case River:
        for(uint8_t i = 0; i < MAX_OBSTACLES; i++) {
            uint8_t obstacle_x = state->currTerrain->obstacles[i];
            if(state->x >= obstacle_x && (state->x - obstacle_x) < LOG_LENGTH) {
                return false;
            }
            if(obstacle_x > COLS - LOG_LENGTH && state->x < (obstacle_x + LOG_LENGTH) % COLS) {
                return false;
            }
        }
        return true;
    default:
        return false;
    }
}

static void process_tick(GameState* state) {
    if(state->gameOver) {
        return;
    }

    scroll(state, 1);

    for(uint8_t i = 0; i < ROWS; i++) {
        Terrain* terrain = &(state->terrains[i]);
        if(terrain->type == Road || terrain->type == River) {
            for(uint8_t j = 0; j < MAX_OBSTACLES; j++) {
                terrain->obstacles[j] += terrain->speed;
                terrain->obstacles[j] %= COLS;
            }
        }
    }

    if(state->currTerrain->type == River) {
        state->x += state->currTerrain->speed;
        if(state->x >= COLS) {
            state->gameOver = true;
        }
    }

    state->gameOver = check_collision(state);
}

static void move_player(GameState* state, InputKey input) {
    if(state->gameOver) {
        return;
    }

    switch(input) {
    case InputKeyRight:
        if(state->x + 1 < COLS) {
            state->x++;
        }
        break;
    case InputKeyLeft:
        if(state->x - 1 >= 0) {
            state->x--;
        }
        break;
    case InputKeyUp:
        if(state->currTerrain->y < ROWS / 2 * GRID_UNIT) {
            scroll(state, GRID_UNIT - (state->currTerrain->y) % GRID_UNIT);
        }
        state->currTerrain--;
        if(state->currTerrain < state->terrains) {
            state->currTerrain =
                state->terrains + ((state->currTerrain - state->terrains + ROWS) % ROWS);
        }
        state->pos++;
        if(state->pos > state->score) {
            state->score = state->pos;
        }
        break;
    case InputKeyDown:
        if(state->currTerrain->y + GRID_UNIT < HEIGHT) {
            state->currTerrain =
                state->terrains + ((state->currTerrain + 1 - state->terrains) % ROWS);
            state->pos--;
        }
        break;
    default:
        break;
    }

    state->gameOver = check_collision(state);
}

static void frogger_init(GameState* state) {
    state->x = COLS / 2;
    state->pos = 0;
    state->score = 0;
    state->gameOver = false;
    state->lastTerrain = state->terrains;
    state->deathNotification = false;

    const uint8_t num_start_rows = 3;
    for(uint8_t i = 0; i < ROWS - num_start_rows; i++) {
        generate_terrain(state, &(state->terrains[i]), i * GRID_UNIT, true);
    }
    for(uint8_t i = 1; i <= num_start_rows; i++) {
        state->terrains[ROWS - i] = (Terrain){.type = Grass, .y = HEIGHT - GRID_UNIT * i};
    }
    state->currTerrain = &(state->terrains[ROWS - num_start_rows + 1]);
}

int32_t flippy_road_app() {
    if(!storage_load()) {
        high_score = 0;
    }

    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(Event));
    GameState* state = malloc(sizeof(GameState));
    frogger_init(state);

    state->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if(!state->mutex) {
        FURI_LOG_E(TAG, "cannot create mutex\r\n");
        free(state);
        return 255;
    }

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, draw_callback, state);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    FuriTimer* timer = furi_timer_alloc(timer_callback, FuriTimerTypePeriodic, event_queue);
    furi_timer_start(timer, furi_kernel_get_tick_frequency());

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
    notification_message_block(notification, &sequence_display_backlight_enforce_on);

    dolphin_deed(DolphinDeedPluginGameStart);

    Event event;
    for(bool processing = true; processing;) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, FuriWaitForever);
        furi_mutex_acquire(state->mutex, FuriWaitForever);

        if(event_status == FuriStatusOk) {
            if(event.type == KeyEvent) {
                if(event.input.type == InputTypePress) {
                    switch(event.input.key) {
                    case InputKeyBack:
                        processing = false;
                        break;
                    case InputKeyOk:
                        if(state->gameOver) {
                            frogger_init(state);
                            furi_timer_start(timer, furi_kernel_get_tick_frequency());
                        }
                        break;
                    default:
                        move_player(state, event.input.key);
                        break;
                    }
                }
            } else if(event.type == TickEvent) {
                process_tick(state);
            }

            if(state->gameOver && !state->deathNotification) {
                notification_message_block(notification, &sequence_fail);
                furi_timer_stop(timer);
                if(state->score > high_score) {
                    high_score = state->score;
                    storage_save();
                }
                state->deathNotification = true;
            }
        }

        furi_mutex_release(state->mutex);
        view_port_update(view_port);
    }

    notification_message(notification, &sequence_display_backlight_enforce_auto);

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);
    furi_timer_free(timer);
    furi_mutex_free(state->mutex);
    free(state);

    return 0;
}
