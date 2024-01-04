#include <furi.h>
#include <furi_hal.h>
#include <furi/core/string.h>

#include <gui/gui.h>

#include <input/input.h>
#include <stdlib.h>

#include "common.h"
#include "game.h"
#include "utils.h"
#include "load.h"
#include "move.h"
#include "fonts.h"
#include "ui.h"
#include "draw.h"
#include "events.h"

//-----------------------------------------------------------------------------

void game_tick(void* ctx) {
    furi_assert(ctx);
    const Game* game = ctx;
    view_port_update(game->viewPort);
}

static void app_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

static void app_draw_callback(Canvas* canvas, void* ctx) {
    furi_assert(ctx);
    Game* game = ctx;
    furi_mutex_acquire(game->mutex, FuriWaitForever);
    draw_app(canvas, game);
    furi_mutex_release(game->mutex);
}

//-----------------------------------------------------------------------------

int32_t game_vexed_app(void* p) {
    UNUSED(p);
    int error;
    bool running = true;
    bool paused = false;
    InputEvent event;

    Game* game = alloc_game_state(&error);
    if(error > 0) {
        return error;
    }

    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    // Configure view port
    game->viewPort = view_port_alloc();
    view_port_draw_callback_set(game->viewPort, app_draw_callback, game);
    view_port_input_callback_set(game->viewPort, app_input_callback, event_queue);

    // Register view port in GUI
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, game->viewPort, GuiLayerFullscreen);

    // Create a timer. When non-paused, it trigers UI refresh
    FuriTimer* timer = furi_timer_alloc(game_tick, FuriTimerTypePeriodic, game);
    furi_timer_start(timer, furi_kernel_get_tick_frequency() / 20);

    initial_load_game(game);

    while(running) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, FuriWaitForever);

        if(event_status == FuriStatusOk) {
            furi_mutex_acquire(game->mutex, FuriWaitForever);

            if(((event.type == InputTypeLong) && (event.key == InputKeyBack)) ||
               ((game->state == ABOUT) && (event.key == InputKeyOk))) {
                running = false;
            } else {
                events_for_game(&event, game);
            }

            bool shouldBePaused = is_state_pause(game->state);

            if(paused != shouldBePaused) {
                paused = shouldBePaused;
                if(paused) {
                    furi_timer_stop(timer);
                    FURI_LOG_D(TAG, "PAUSE - timer stoped");
                } else {
                    furi_timer_start(timer, furi_kernel_get_tick_frequency() / 20);
                    FURI_LOG_D(TAG, "UNPAUSE - timer started");
                }
            }

            if(shouldBePaused) {
                view_port_update(game->viewPort);
            }
            furi_mutex_release(game->mutex);
        }
    }

    furi_timer_free(timer);
    view_port_enabled_set(game->viewPort, false);
    gui_remove_view_port(gui, game->viewPort);
    furi_message_queue_free(event_queue);
    free_game_state(game);
    furi_record_close(RECORD_GUI);

    return 0;
}
