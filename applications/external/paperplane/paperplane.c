#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_random.h>
#include <gui/gui.h>
#include <gui/icon_i.h>
#include <gui/elements.h>
#include <input/input.h>
#include <stdlib.h>
#include <stdio.h>
#include "game_state.h"
#include "paper_plane_icons.h"

#define FPS 20

#define SPRITE_SIZE 8

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} PluginEvent;

static void timer_callback(void* ctx) {
    GameState* game_state = ctx;
    furi_mutex_acquire(game_state->mutex, FuriWaitForever);

    if(game_state == NULL) {
        return;
    }

    uint32_t ticks_elapsed = furi_get_tick() - game_state->last_tick;
    game_state->last_tick = furi_get_tick();
    int delta_time_ms = ticks_elapsed * 1000 / furi_kernel_get_tick_frequency();

    if(!game_state->crash_flag) {
        update_position(game_state->paper, delta_time_ms);
        check_collision(game_state);
    }

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

    // draw map (this should probably be it's own function)
    float background_position = game_state->paper->y;
    for(int i = background_position; i < background_position + 10; i++) {
        /*
            using a uint32_t here so that bits 
            that have been shifted out can still 
            be read.
        */
        uint32_t currentRow = game_state->map[i];
        for(unsigned int j = 0; j < sizeof(uint16_t) * 8; j++) {
            /*
                0x8000 is 1 with 15 zeros
                00000000000000001000000000000000 - 0x8000
                00000000000000001111111001111111 - map data (currentRow)
                using & will result in:
                00000000000000001000000000000000
                the above number will evaluate to true
                
                OR

                00000000000000001000000000000000 - 0x8000
                00000000000000000111111001111111 - map data (currentRow)
                using & will result in:
                00000000000000000000000000000000
                the above number will result in false
            */
            if(currentRow & 0x8000) {
                const Icon* ground_to_draw = &I_Ground;

                // if the bit to the left is 0, use the right facing ground sprite
                if(!(currentRow & 0x4000)) {
                    ground_to_draw = &I_GroundRight;
                }

                // if the bit to the right is 0, use the left facing ground tile
                if(!(currentRow & 0x10000)) {
                    ground_to_draw = &I_GroundLeft;
                }

                canvas_draw_icon(
                    canvas,
                    j * SPRITE_SIZE,
                    i * SPRITE_SIZE - background_position * SPRITE_SIZE,
                    ground_to_draw);
            }

            // bit shift currentRow to the left, so the bit to the right will be drawn
            currentRow <<= 1;
        }
    }

    // draw plane
    canvas_draw_icon(
        canvas, game_state->paper->x * SPRITE_SIZE, PAPER_START_Y, game_state->paper->icon);

    // Show score
    char score_string[11]; // length is 11 b/c: Score: xxx\0
    canvas_draw_icon(canvas, 77, 2, &I_Score);
    snprintf(
        score_string, 11, "Score: %d", (int)game_state->paper->y); // copy score into score_string
    canvas_draw_str_aligned(canvas, 80, 5, AlignLeft, AlignTop, score_string);

    furi_mutex_release(game_state->mutex);
}

int32_t paperplane_app() {
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(PluginEvent));

    GameState* game_state = malloc(sizeof(GameState));
    game_state_init(game_state);

    if(!game_state->mutex) {
        FURI_LOG_E("Paper Plane", "cannot create mutex\r\n");
        // game crash, all initialized items must be freed.
        furi_message_queue_free(event_queue);
        //furi_timer_free(game_state->timer); this causes a null pointer dereference
        free(game_state->paper);
        free(game_state->map);
        free(game_state);
        return 255;
    }

    // Set system callbacks
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, game_state);
    view_port_input_callback_set(view_port, input_callback, event_queue);
    game_state->timer = furi_timer_alloc(timer_callback, FuriTimerTypePeriodic, game_state);

    furi_timer_start(game_state->timer, (uint32_t)furi_kernel_get_tick_frequency() / FPS);

    // Open GUI and register view_port
    Gui* gui = furi_record_open("gui");
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    PluginEvent event;
    for(bool processing = true; processing;) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 100);
        if(event_status == FuriStatusOk) {
            // press events
            if(event.type == EventTypeKey) {
                if(event.input.type == InputTypePress || event.input.type == InputTypeLong ||
                   event.input.type == InputTypeRepeat) {
                    switch(event.input.key) {
                    case InputKeyUp:
                        break;
                    case InputKeyDown:
                        break;
                    case InputKeyLeft:
                        rotate_left(game_state->paper);
                        break;
                    case InputKeyRight:
                        rotate_right(game_state->paper);
                        break;
                    case InputKeyOk:
                        if(game_state->crash_flag) {
                            game_state_reinit(game_state);
                            break;
                        }
                        break;
                    case InputKeyMAX:
                        break;
                    case InputKeyBack:
                        // Exit the app
                        processing = false;
                        break;
                    }
                }
            }
        }

        furi_mutex_release(game_state->mutex);
        view_port_update(view_port);
    }

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close("gui");
    view_port_free(view_port);
    furi_message_queue_free(event_queue);
    furi_mutex_free(game_state->mutex);
    furi_timer_free(game_state->timer);
    free(game_state->paper);
    free(game_state->map);
    free(game_state);

    return 0;
}
