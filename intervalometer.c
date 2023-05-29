// An intervalometer application written for the Flipper Zero
//
// author: nitepone <sierra>

#include "intervalometer.h"

#include "gui/scene_manager.h"
#include <stdlib.h>
#include <stdio.h>
#include <furi.h>
#include <furi_hal.h>
#include <core/string.h>
#include <gui/gui.h>
#include <gui/elements.h>

#include <input/input.h>

#include <notification/notification.h>
#include <notification/notification_messages.h>

// app ui scenes
enum flipvalo_ui_scene {
    FVSceneMain,
    FVSceneConfig,
};

// run config for intervalometer
struct flipvalo_config {
    int init_delay_msec;          // initial delay to start capture
    int interval_delay_msec;      // time between shots
    int shot_count;               // total shots in run
    int burst_count;              // number of triggers in a shot
    int burst_delay_msec;         // time between triggers in a shot
    int tickrate;                 // tick rate in "ticks per second"

    // camera control functions.
    // a bit overkill atm, but this will allow us to drop in support
    // for other cameras, bluetooth, and more adv. functions later.
    int (*send_trigger_fn)(void* output_config);
    void* output_config;
};

// run time states for intervalometer
enum flipvalo_state {
    FVDone = 0, // done, 0 so it is default if state struct is zeroed
    FVWaitInitShot, // waiting for first shot
    FVWaitContShot, // waiting between "bursts" or "shots"
    FVWaitBurst, // waiting between shots in a "burst"
};

// run time data for intervalometer
// (this can be safely cleared between runs of the intervalometer)
struct flipvalo_run_state {
    enum flipvalo_state state;  // current state of the run
    int tick_cur;               // current tick count
    int tick_next;              // tick when next action will occur
    int shot_cur;               // current shot
    int burst_cur;              // current trigger in a burst
};

// private data of app
struct flipvalo_priv {
    struct flipvalo_config config;
    struct flipvalo_run_state run_state;
    enum flipvalo_ui_scene ui_scene;
    FuriTimer* timer;
    NotificationApp* notifications;
    FuriMutex* mutex;
};

enum event_type {
    EventTypeTick,
    EventTypeKey,
};

struct plugin_event {
    enum event_type type;
    InputEvent input;
};

static void flipvalo_run_state_init(struct flipvalo_run_state* fv_run_state) {
    fv_run_state->burst_cur = 0;
    fv_run_state->shot_cur = 0;
    fv_run_state->tick_next = 0;
    fv_run_state->state = FVDone;
    fv_run_state->tick_next = 0;
}

static int sony_ir_trigger(void* ctx) {
    UNUSED(ctx);
    return 0;
}

static void input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);
    struct plugin_event event = {.type = EventTypeKey, .input = *input_event };
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static inline bool flipvalo_intv_running(struct flipvalo_priv* fv_priv) {
    return fv_priv->run_state.state != FVDone;
}

static void flipvalo_intv_tick(struct flipvalo_priv* fv_priv) {
    struct flipvalo_config* conf = &fv_priv->config;
    struct flipvalo_run_state* run = &fv_priv->run_state;
    // check if action required
    if (run->tick_cur++ >= run->tick_next) {
        // call trigger function
        conf->send_trigger_fn(conf->output_config);
        run->shot_cur++;
        run->burst_cur++;
        // end of burst, prepare next shot
        if (run->burst_cur >= conf->burst_count) {
            run->state = FVWaitContShot;
            run->tick_next = run->tick_cur + ((conf->interval_delay_msec * conf->tickrate) / 1000);
        }
        else /*continue burst */ {
            run->state = FVWaitBurst;
            run->tick_next = run->tick_cur + ((conf->burst_delay_msec * conf->tickrate) / 1000);
        }
    }
}

static void flipvalo_intv_stop(struct flipvalo_priv* fv_priv) {
    fv_priv->run_state.state = FVDone;
}

static void flipvalo_intv_start(struct flipvalo_priv* fv_priv) {
    // clear struct
    furi_assert(fv_priv);
    flipvalo_run_state_init(&fv_priv->run_state);
    fv_priv->run_state.state = FVWaitInitShot;
    fv_priv->run_state.tick_next =
        ((fv_priv->config.init_delay_msec * fv_priv->config.tickrate) / 1000);
}

static void timer_callback(void* ctx) {
    furi_assert(ctx);
    struct flipvalo_priv* fv_priv = ctx;
    furi_mutex_acquire(fv_priv->mutex, FuriWaitForever);
    if (flipvalo_intv_running(fv_priv)) {
        flipvalo_intv_tick(fv_priv);
    }
    furi_mutex_release(fv_priv->mutex);
}

static void render_callback(Canvas* const canvas, void* ctx) {
    furi_assert(ctx);
    struct flipvalo_priv* fv_priv = ctx;
    FuriString* temp_str = furi_string_alloc();
    furi_mutex_acquire(fv_priv->mutex, FuriWaitForever);

    if (fv_priv->ui_scene == FVSceneMain) {
        int countdown_msec = (1000 * (fv_priv->run_state.tick_next
                    - fv_priv->run_state.tick_cur))
                / fv_priv->config.tickrate;
        int elapsed_msec = (1000 * fv_priv->run_state.tick_cur)
                / fv_priv->config.tickrate;

        canvas_draw_frame(canvas, 0, 0, 128, 64);

        // draw countdown
        canvas_set_font(canvas, FontPrimary);
        furi_string_printf(
                temp_str, "%02d:%02d:%02d:%03d",
                countdown_msec / 3600000,
                (countdown_msec / 60000) % 60,
                (countdown_msec / 1000) % 60,
                countdown_msec % 1000);
        canvas_draw_str_aligned(
                canvas, 64, 24,
                AlignCenter, AlignCenter,
                furi_string_get_cstr(temp_str));

        // draw top and bottom status bars
        canvas_set_font(canvas, FontSecondary);
        furi_string_printf(
                temp_str, "%02d:%02d:%02d",
                elapsed_msec / 3600000,
                (elapsed_msec / 60000) % 60,
                (elapsed_msec / 1000) % 60);
        canvas_draw_str_aligned(
                canvas, 4, 8,
                AlignLeft, AlignCenter,
                furi_string_get_cstr(temp_str));
        furi_string_printf(temp_str, "Shot: %d", fv_priv->run_state.shot_cur);
        canvas_draw_str_aligned(
                canvas, 124, 8,
                AlignRight, AlignCenter,
                furi_string_get_cstr(temp_str));
        elements_button_left(canvas, "Cfg");
        elements_button_right(canvas, "Snap");
        if (fv_priv->run_state.state == FVDone) {
            elements_button_center(canvas, "Start");
        } else {
            elements_button_center(canvas, "Stop ");
        }
    }

    furi_string_free(temp_str);
    furi_mutex_release(fv_priv->mutex);
}

static void flipvalo_config_init(struct flipvalo_config* fv_conf) {
    fv_conf->init_delay_msec = 0;
    fv_conf->interval_delay_msec = 5000;
    fv_conf->shot_count = 10;
    fv_conf->burst_count = 1;
    fv_conf->burst_delay_msec = 0;
    fv_conf->tickrate = 60;
    fv_conf->send_trigger_fn = sony_ir_trigger;
    fv_conf->output_config = NULL;
}

static void flipvalo_priv_init(struct flipvalo_priv* fv_priv) {
    flipvalo_config_init(&(fv_priv->config));
    flipvalo_run_state_init(&(fv_priv->run_state));
    fv_priv->timer = NULL;
    fv_priv->notifications = NULL;
    fv_priv->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
}

int32_t flipvalo_app() {
    int ret = 0;
    ViewPort* view_port = NULL;
    Gui* gui = NULL;
    FuriStatus event_status = {0};
    struct plugin_event event = {0};

    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(struct plugin_event));
    struct flipvalo_priv* fv_priv = malloc(sizeof(*fv_priv));

    flipvalo_priv_init(fv_priv);

    if (!fv_priv->mutex) {
        FURI_LOG_E("Flipvalo", "Cannot create mutex\r\n");
        ret = 1;
        goto cleanup;
    }

    view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, fv_priv);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    fv_priv->timer = furi_timer_alloc(timer_callback, FuriTimerTypePeriodic, fv_priv);
    furi_timer_start(
            fv_priv->timer,
            (uint32_t) furi_kernel_get_tick_frequency() / fv_priv->config.tickrate);
    gui = furi_record_open("gui");
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    while (true) {
        event_status = furi_message_queue_get(event_queue, &event, 100);

        furi_mutex_acquire(fv_priv->mutex, FuriWaitForever);

        // catch event_status that is not Ok
        if (event_status == FuriStatusErrorTimeout) {
            // timeout, ignore
            goto next_event;
        }
        else if (event_status != FuriStatusOk) {
            FURI_LOG_E("Flipvalo", "Event Queue Error: %d\r\n", event_status);
            goto next_event;
            // TODO(luna) evaluate if we should exit here.
            //goto cleanup;
        }

        // handle input
        if (event.type == EventTypeKey) {
            if (event.input.type == InputTypeShort) {
                switch (event.input.key) {
                    case InputKeyUp:
                        break;
                    case InputKeyDown:
                        break;
                    case InputKeyLeft:
                        break;
                    case InputKeyRight:
                        fv_priv->config.send_trigger_fn(fv_priv->config.output_config);
                        break;
                    case InputKeyOk:
                        if (flipvalo_intv_running(fv_priv)) {
                            flipvalo_intv_stop(fv_priv);
                        } else {
                            flipvalo_intv_start(fv_priv);
                        }
                        break;
                    case InputKeyMAX:
                        break;
                    case InputKeyBack:
                        // Exit the app
                        goto cleanup;
                }
            }
        }

next_event:
        furi_mutex_release(fv_priv->mutex);
    }

cleanup:
    if (view_port) {
        view_port_enabled_set(view_port, false);
        if (gui) {
            gui_remove_view_port(gui, view_port);
            furi_record_close("gui");
        }
        view_port_free(view_port);
    }
    if (event_queue) {
        furi_message_queue_free(event_queue);
    }
    if (fv_priv) {
        furi_mutex_free(fv_priv->mutex);
        furi_timer_free(fv_priv->timer);
    }
    free(fv_priv);
    return ret;
}

