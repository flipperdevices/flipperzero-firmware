#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/icon_i.h>
#include <gui/elements.h>
#include <input/input.h>
#include <stdlib.h>
#include <stdio.h>
#include <dolphin/dolphin.h>

#include "trex_runner_icons.h"

#define DINO_START_X 10
#define DINO_START_Y 34 // 64 - 22 - BACKGROUND_H / 2 - 2

#define FPS 20

#define DINO_RUNNING_MS_PER_FRAME 500

#define GRAVITY 60
#define JUMP_SPEED 30

#define CACTUS_W 10
#define CACTUS_H 10
#define START_x_speed 25

#define BACKGROUND_W 128
#define BACKGROUND_H 12

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} PluginEvent;

typedef struct {
    FuriTimer* timer;
    uint32_t last_tick;
    const Icon* dino_icon;
    int dino_frame_ms;
    FuriMutex* mutex;

    // Dino info
    float y_position;
    float y_speed;
    int y_acceleration;
    float x_speed;

    // Cactus info
    int cactus_position;
    int has_cactus;

    // Horizontal line
    int background_position;

    int lost;

    int score;
} GameState;

static void timer_callback(void* ctx) {
    GameState* game_state = ctx;
    furi_mutex_acquire(game_state->mutex, FuriWaitForever);

    if(game_state == NULL) {
        return;
    }

    uint32_t ticks_elapsed = furi_get_tick() - game_state->last_tick;
    game_state->last_tick = furi_get_tick();
    int delta_time_ms = ticks_elapsed * 1000 / furi_kernel_get_tick_frequency();

    // dino update
    game_state->dino_frame_ms += delta_time_ms;
    // TODO: switch by dino state
    if(game_state->dino_frame_ms >= DINO_RUNNING_MS_PER_FRAME) {
        if(game_state->dino_icon == &I_DinoRun0) {
            game_state->dino_icon = &I_DinoRun1;
        } else {
            game_state->dino_icon = &I_DinoRun0;
        }
        game_state->dino_frame_ms = 0;
    }

    // Compute dino dynamics
    game_state->y_acceleration = game_state->y_acceleration - GRAVITY * delta_time_ms / 1000;
    game_state->y_speed = game_state->y_speed + game_state->y_acceleration * delta_time_ms / 1000;
    game_state->y_position = game_state->y_position - game_state->y_speed * delta_time_ms / 1000;

    // Touch ground
    if(game_state->y_position >= DINO_START_Y) {
        game_state->y_acceleration = 0;
        game_state->y_speed = 0;
        game_state->y_position = DINO_START_Y;
    }

    // Update Cactus state
    if(game_state->has_cactus) {
        game_state->cactus_position =
            game_state->cactus_position - game_state->x_speed * delta_time_ms / 1000;
        if(game_state->cactus_position <= 0) {
            game_state->has_cactus = 0;
            game_state->score = game_state->score + 1;
        }
    }
    // Create cactus (not random)
    else {
        game_state->has_cactus = 1;
        game_state->cactus_position = 120;
    }

    // Move horizontal line
    if(game_state->background_position <= -BACKGROUND_W)
        game_state->background_position += BACKGROUND_W;
    game_state->background_position =
        game_state->background_position - game_state->x_speed * delta_time_ms / 1000;

    // Lose condition
    if((game_state->y_position + 22 >= (64 - CACTUS_H)) &&
       ((DINO_START_X + 20) >= game_state->cactus_position) &&
       (DINO_START_X <= (game_state->cactus_position + CACTUS_W)))
        game_state->lost = 1;

    furi_mutex_release(game_state->mutex);
}

static void input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);

    PluginEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void render_callback(Canvas* const canvas, void* ctx) {
    const GameState* game_state = ctx;
    furi_mutex_acquire(game_state->mutex, FuriWaitForever);

    if(game_state == NULL) {
        return;
    }

    char score_string[12];
    if(!game_state->lost) {
        // Show Ground
        canvas_draw_icon(
            canvas, game_state->background_position, 64 - BACKGROUND_H, &I_HorizonLine0);
        canvas_draw_icon(
            canvas,
            game_state->background_position + BACKGROUND_W,
            64 - BACKGROUND_H,
            &I_HorizonLine0);

        // Show DINO
        canvas_draw_icon(canvas, DINO_START_X, game_state->y_position, game_state->dino_icon);

        // Show cactus
        if(game_state->has_cactus)
            //canvas_draw_triangle(canvas, game_state->cactus_position, 64 - BACKGROUND_H + CACTUS_W, CACTUS_W, CACTUS_H, CanvasDirectionBottomToTop);
            canvas_draw_icon(
                canvas,
                game_state->cactus_position,
                64 - BACKGROUND_H / 2 - CACTUS_H - 2,
                &I_Cactus);

        // Show score
        if(game_state->score == 0) canvas_set_font(canvas, FontSecondary);
        snprintf(score_string, 12, "Score: %d", game_state->score);
        canvas_draw_str_aligned(canvas, 85, 5, AlignLeft, AlignTop, score_string);

        if(game_state->score % 10 == 0) {
            dolphin_deed(getRandomDeed());
        }
    } else {
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 64, 32, AlignCenter, AlignBottom, "You lost :c");
    }

    furi_mutex_release(game_state->mutex);
}

static void game_state_init(GameState* const game_state) {
    game_state->last_tick = furi_get_tick();
    game_state->dino_frame_ms = 0;
    game_state->dino_icon = &I_Dino;
    game_state->y_acceleration = game_state->y_speed = 0;
    game_state->y_position = DINO_START_Y;
    game_state->has_cactus = 0;
    game_state->background_position = 0;
    game_state->lost = 0;
    game_state->x_speed = START_x_speed;
    game_state->score = 0;
    game_state->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
}

int32_t trexrunner_app() {
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(PluginEvent));

    GameState* game_state = malloc(sizeof(GameState));
    game_state_init(game_state);

    if(!game_state->mutex) {
        FURI_LOG_E("T-rex runner", "cannot create mutex\r\n");
        free(game_state);
        return 255;
    }
    // BEGIN IMPLEMENTATION

    // Set system callbacks
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, game_state);
    view_port_input_callback_set(view_port, input_callback, event_queue);
    game_state->timer = furi_timer_alloc(timer_callback, FuriTimerTypePeriodic, game_state);

    furi_timer_start(game_state->timer, (uint32_t)furi_kernel_get_tick_frequency() / FPS);

    // Open GUI and register view_port
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    PluginEvent event;
    for(bool processing = true; processing && !game_state->lost;) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 100);
        if(event_status == FuriStatusOk) {
            // press events
            if(event.type == EventTypeKey) {
                if(event.input.type == InputTypeShort) {
                    switch(event.input.key) {
                    case InputKeyUp:
                        break;
                    case InputKeyDown:
                        break;
                    case InputKeyLeft:
                        break;
                    case InputKeyRight:
                        break;
                    case InputKeyOk:
                        if(game_state->y_position == DINO_START_Y)
                            game_state->y_speed = JUMP_SPEED;
                        break;
                    case InputKeyBack:
                        // Exit the app
                        processing = false;
                        break;
                    case InputKeyMAX:
                        break;
                    }
                }
            }
        }
        if(game_state->lost) {
            furi_message_queue_get(
                event_queue, &event, 1500); //Sleep to show  the "you lost" message
        }
        view_port_update(view_port);
        furi_mutex_release(game_state->mutex);
    }

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close(RECORD_GUI);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);
    furi_mutex_free(game_state->mutex);
    furi_timer_free(game_state->timer);
    free(game_state);

    return 0;
}
