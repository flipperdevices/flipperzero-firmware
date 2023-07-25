// An intervalometer application written for the Flipper Zero
//
// author: nitepone <sierra>

#include "intervalometer.h"
#include <stdlib.h>
#include <stdio.h>
#include <furi.h>
#include <furi_hal.h>
#include <core/string.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include <gui/icon.h>
#include <infrared_transmit.h>

#include <input/input.h>

#include <notification/notification.h>
#include <notification/notification_messages.h>

#include "intervalometer_icons.h"

// app ui scenes
enum flipvalo_ui_scene {
    FVSceneMain,
    FVSceneConfig,
};

// run config for intervalometer
struct flipvalo_config {
    int init_delay_msec; // initial delay to start capture
    int interval_delay_msec; // time between shots
    int shot_count; // total shots in run
    int burst_count; // number of triggers in a shot
    int burst_delay_msec; // time between triggers in a shot
    int tickrate; // tick rate in "ticks per second"

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
    enum flipvalo_state state; // current state of the run
    int tick_cur; // current tick count
    int tick_next; // tick when next action will occur
    int shot_cur; // current shot
    int burst_cur; // current trigger in a burst
};

enum flipvalo_config_edit_lines {
    FvConfigEditInitDelay,
    FvConfigEditMIN = FvConfigEditInitDelay,
    FvConfigEditShotCount,
    FvConfigEditShotDelay,
    FvConfigEditBurstCount,
    FvConfigEditBurstDelay,
    FvConfigEditMAX = FvConfigEditBurstDelay,
};

struct flipvalo_config_edit_view {
    // the `config` that is under edit
    struct flipvalo_config* config;
    // the `cur_index` of the selection
    // (e.g. editing the 3rd value of a number)
    int cur_index;
    // the `cur_line` of the selection
    enum flipvalo_config_edit_lines cur_line;
    // the current line that is at the top of the scrolled view
    enum flipvalo_config_edit_lines scroll_pos;
    // are we editing the selection?
    // (this is really only needed for number fields)
    bool edit_mode;
};

// private data of app
struct flipvalo_priv {
    struct flipvalo_config config;
    struct flipvalo_config_edit_view config_edit_view;
    struct flipvalo_run_state run_state;
    enum flipvalo_ui_scene ui_scene;
    int gui_shutter_blink;
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

// XXX(luna) settings experimental ui kludge

enum flipvalo_config_edit_line_type {
    FvConfigEditTypeTimer,
    FvConfigEditTypeCount,
};

static void flipvalo_config_edit_view_init(struct flipvalo_config_edit_view* view) {
    view->config = NULL;
    view->cur_index = 0;
    view->cur_line = 0;
    view->scroll_pos = 0;
    view->edit_mode = false;
}

#define ITEM_H 64 / 3
#define ITEM_W 128
#define VALUE_X 100
#define VALUE_W 45
static void flipvalo_config_edit_draw(Canvas* canvas, struct flipvalo_config_edit_view* view) {
    int* line_value;
    char* line_label = NULL;
    FuriString* temp_str = furi_string_alloc();
    enum flipvalo_config_edit_line_type line_type;
    enum flipvalo_config_edit_lines selected_line;

    for(size_t line = 0; line < 3; line++) {
        selected_line = view->scroll_pos + line;
        switch(selected_line) {
        case FvConfigEditInitDelay:
            line_value = &view->config->init_delay_msec;
            line_type = FvConfigEditTypeTimer;
            line_label = "Init Time";
            break;
        case FvConfigEditShotDelay:
            line_value = &view->config->interval_delay_msec;
            line_type = FvConfigEditTypeTimer;
            line_label = "Seq Time";
            break;
        case FvConfigEditShotCount:
            line_value = &view->config->shot_count;
            line_type = FvConfigEditTypeCount;
            line_label = "Seq Count";
            break;
        case FvConfigEditBurstDelay:
            line_value = &view->config->burst_delay_msec;
            line_type = FvConfigEditTypeTimer;
            line_label = "Brst Time";
            break;
        case FvConfigEditBurstCount:
            line_value = &view->config->burst_count;
            line_type = FvConfigEditTypeCount;
            line_label = "Brst Count";
            break;
        default:
            continue;
        };

        canvas_set_color(canvas, ColorBlack);
        if((selected_line) == view->cur_line) {
            elements_slightly_rounded_box(canvas, 0, ITEM_H * line + 1, ITEM_W, ITEM_H - 1);
            canvas_set_color(canvas, ColorWhite);
        }

        uint8_t text_y = ITEM_H * line + ITEM_H / 2 + 2;

        canvas_draw_str_aligned(canvas, 6, text_y, AlignLeft, AlignCenter, line_label);

        switch(line_type) {
        case FvConfigEditTypeTimer:
            furi_string_printf(
                temp_str,
                "%02d:%02d:%02d:%03d",
                *line_value / 3600000,
                (*line_value / 60000) % 60,
                (*line_value / 1000) % 60,
                *line_value % 1000);
            canvas_set_font(canvas, FontKeyboard);
            canvas_draw_str_aligned(
                canvas, 124, text_y, AlignRight, AlignCenter, furi_string_get_cstr(temp_str));
            canvas_set_font(canvas, FontSecondary);
            if(view->edit_mode && view->cur_line == selected_line) {
                switch(view->cur_index) {
                case 0:
                    canvas_draw_icon(canvas, 117, text_y - 9, &I_ArrowUp_3x5);
                    canvas_draw_icon(canvas, 117, text_y + 5, &I_ArrowDown_3x5);
                    canvas_draw_icon(canvas, 112, text_y - 9, &I_ArrowUp_3x5);
                    canvas_draw_icon(canvas, 112, text_y + 5, &I_ArrowDown_3x5);
                    canvas_draw_icon(canvas, 107, text_y - 9, &I_ArrowUp_3x5);
                    canvas_draw_icon(canvas, 107, text_y + 5, &I_ArrowDown_3x5);
                    break;
                case 1:
                    canvas_draw_icon(canvas, 93, text_y - 9, &I_ArrowUp_3x5);
                    canvas_draw_icon(canvas, 93, text_y + 5, &I_ArrowDown_3x5);
                    canvas_draw_icon(canvas, 89, text_y - 9, &I_ArrowUp_3x5);
                    canvas_draw_icon(canvas, 89, text_y + 5, &I_ArrowDown_3x5);
                    break;
                case 2:
                    canvas_draw_icon(canvas, 75, text_y - 9, &I_ArrowUp_3x5);
                    canvas_draw_icon(canvas, 75, text_y + 5, &I_ArrowDown_3x5);
                    canvas_draw_icon(canvas, 71, text_y - 9, &I_ArrowUp_3x5);
                    canvas_draw_icon(canvas, 71, text_y + 5, &I_ArrowDown_3x5);
                    break;
                case 3:
                    canvas_draw_icon(canvas, 57, text_y - 9, &I_ArrowUp_3x5);
                    canvas_draw_icon(canvas, 57, text_y + 5, &I_ArrowDown_3x5);
                    canvas_draw_icon(canvas, 53, text_y - 9, &I_ArrowUp_3x5);
                    canvas_draw_icon(canvas, 53, text_y + 5, &I_ArrowDown_3x5);
                    break;
                }
            }
            break;
        case FvConfigEditTypeCount:
            furi_string_printf(temp_str, "%d", *line_value);
            canvas_draw_str_aligned(
                canvas, VALUE_X, text_y, AlignCenter, AlignCenter, furi_string_get_cstr(temp_str));
            // TODO(luna) 0 values are actually more special for shot count and burst count.
            // former being infinite, latter being uh.. nothing? not allowed?.. review this logic later.
            if(*line_value > 0) {
                canvas_draw_str_aligned(
                    canvas, VALUE_X - VALUE_W / 2, text_y, AlignCenter, AlignCenter, "<");
            }
            canvas_draw_str_aligned(
                canvas, VALUE_X + VALUE_W / 2, text_y, AlignCenter, AlignCenter, ">");
            break;
        }
    }

    furi_string_free(temp_str);
}

static void
    flipvalo_config_edit_input_move_cursor(struct flipvalo_config_edit_view* view, int dx, int dy) {
    enum flipvalo_config_edit_lines new_line = 0;

    int* line_value;
    enum flipvalo_config_edit_line_type line_type;

    switch(view->cur_line) {
    case FvConfigEditInitDelay:
        line_value = &view->config->init_delay_msec;
        line_type = FvConfigEditTypeTimer;
        break;
    case FvConfigEditShotDelay:
        line_value = &view->config->interval_delay_msec;
        line_type = FvConfigEditTypeTimer;
        break;
    case FvConfigEditShotCount:
        line_value = &view->config->shot_count;
        line_type = FvConfigEditTypeCount;
        break;
    case FvConfigEditBurstDelay:
        line_value = &view->config->burst_delay_msec;
        line_type = FvConfigEditTypeTimer;
        break;
    case FvConfigEditBurstCount:
        line_value = &view->config->burst_count;
        line_type = FvConfigEditTypeCount;
        break;
    default:
        return;
    };

    if(!view->edit_mode) {
        // Do `dy` behaviors
        new_line = view->cur_line + dy;
        if(new_line > FvConfigEditMAX) {
            // Out of bound cursor. No-op.
            return;
        }
        view->cur_line = new_line;

        // Handle moving scroll position.
        if(new_line < view->scroll_pos) {
            view->scroll_pos = new_line;
        } else if(new_line >= (view->scroll_pos + 3)) {
            view->scroll_pos += dy;
        }

        // Do `dx` behavior
        switch(line_type) {
        case FvConfigEditTypeCount:
            if(*line_value + dx >= 0) {
                *line_value += dx;
            }
            break;
        case FvConfigEditTypeTimer:
            // no-op unless edit mode
            break;
        }
    } else /* edit mode */ {
        switch(line_type) {
        case FvConfigEditTypeCount:
            // If current line does not edit mode.. why are we in edit mode?
            // Reaching this would be a bug, so lets go back to normal mode.
            view->edit_mode = false;
            return;
        case FvConfigEditTypeTimer:
            switch(view->cur_index) {
            case 0:
                if(*line_value + (dy * -10) >= 0) {
                    *line_value += (dy * -10);
                }
                break;
            case 1:
                if(*line_value + (dy * -1000) >= 0) {
                    *line_value += (dy * -1000);
                }
                break;
            case 2:
                if(*line_value + (dy * -60000) >= 0) {
                    *line_value += (dy * -60000);
                }
                break;
            case 3:
                if(*line_value + (dy * -3600000) >= 0) {
                    *line_value += (dy * -3600000);
                }
                break;
            }

            view->cur_index -= dx;
            if(view->cur_index < 0) {
                view->cur_index = 0;
            }
            if(view->cur_index > 3) {
                view->cur_index = 3;
            }
            break;
        }
    }
}

static int flipvalo_config_edit_input(InputEvent* event, struct flipvalo_config_edit_view* view) {
    // ignore all but short and repeats
    if(!(event->type == InputTypeShort || event->type == InputTypeRepeat)) {
        return 0;
    }
    switch(event->key) {
    case InputKeyRight:
        flipvalo_config_edit_input_move_cursor(view, 1, 0);
        break;
    case InputKeyLeft:
        flipvalo_config_edit_input_move_cursor(view, -1, 0);
        break;
    case InputKeyUp:
        flipvalo_config_edit_input_move_cursor(view, 0, -1);
        break;
    case InputKeyDown:
        flipvalo_config_edit_input_move_cursor(view, 0, 1);
        break;
    case InputKeyOk:
        //TODO(luna) Check if line supports edit mode before doing this.
        view->edit_mode = !view->edit_mode;
        break;
    case InputKeyBack:
        if(view->edit_mode) {
            view->edit_mode = false;
        } else {
            // exit config edit view
            return 1;
        }
    default:
        break;
    }
    return 0;
}

// XXX(luna) back to app

static void flipvalo_run_state_init(struct flipvalo_run_state* fv_run_state) {
    fv_run_state->burst_cur = 1;
    fv_run_state->shot_cur = 1;
    fv_run_state->tick_next = 0;
    fv_run_state->state = FVDone;
    fv_run_state->tick_next = 0;
    fv_run_state->tick_cur = 0;
}

static int sony_ir_trigger(void* ctx) {
    UNUSED(ctx);
    InfraredMessage message = {
        .address = 0x1E3A,
        .command = 0x2D,
        .protocol = InfraredProtocolSIRC20,
    };
    infrared_send(&message, 1);
    return 0;
}

static void input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);
    struct plugin_event event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static inline bool flipvalo_intv_running(struct flipvalo_priv* fv_priv) {
    return fv_priv->run_state.state != FVDone;
}

static void flipvalo_intv_tick(struct flipvalo_priv* fv_priv) {
    struct flipvalo_config* conf = &fv_priv->config;
    struct flipvalo_run_state* run = &fv_priv->run_state;
    // check if action required
    if(run->tick_cur++ >= run->tick_next) {
        // call trigger function
        conf->send_trigger_fn(conf->output_config);
        fv_priv->gui_shutter_blink = 3;
        // end of burst, prepare next shot
        if(run->burst_cur >= conf->burst_count) {
            run->burst_cur = 1;
            run->shot_cur++;
            run->state = FVWaitContShot;
            run->tick_next = run->tick_cur + ((conf->interval_delay_msec * conf->tickrate) / 1000);
        } else /*continue burst */ {
            run->burst_cur++;
            run->state = FVWaitBurst;
            run->tick_next = run->tick_cur + ((conf->burst_delay_msec * conf->tickrate) / 1000);
        }
    }
    if(run->shot_cur > conf->shot_count) {
        run->state = FVDone;
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
    if(flipvalo_intv_running(fv_priv)) {
        flipvalo_intv_tick(fv_priv);
    }
    furi_mutex_release(fv_priv->mutex);
}

static void render_callback(Canvas* const canvas, void* ctx) {
    furi_assert(ctx);
    struct flipvalo_priv* fv_priv = ctx;
    FuriString* temp_str = furi_string_alloc();
    furi_mutex_acquire(fv_priv->mutex, FuriWaitForever);

    // invert screen if blinking
    if(fv_priv->gui_shutter_blink > 0) {
        fv_priv->gui_shutter_blink--;
        canvas_draw_box(canvas, 0, 0, 127, 63);
        canvas_set_color(canvas, ColorWhite);
    }

    if(fv_priv->ui_scene == FVSceneMain) {
        int countdown_msec =
            (1000 * (fv_priv->run_state.tick_next - fv_priv->run_state.tick_cur)) /
            fv_priv->config.tickrate;
        int elapsed_msec = (1000 * fv_priv->run_state.tick_cur) / fv_priv->config.tickrate;

        canvas_draw_frame(canvas, 0, 0, 128, 64);

        // draw countdown
        canvas_set_font(canvas, FontPrimary);
        furi_string_printf(
            temp_str,
            "%02d:%02d:%02d:%03d",
            countdown_msec / 3600000,
            (countdown_msec / 60000) % 60,
            (countdown_msec / 1000) % 60,
            countdown_msec % 1000);
        canvas_draw_str_aligned(
            canvas, 64, 24, AlignCenter, AlignCenter, furi_string_get_cstr(temp_str));

        // draw top and bottom status bars
        canvas_set_font(canvas, FontSecondary);
        furi_string_printf(
            temp_str,
            "%02d:%02d:%02d",
            elapsed_msec / 3600000,
            (elapsed_msec / 60000) % 60,
            (elapsed_msec / 1000) % 60);
        canvas_draw_str_aligned(
            canvas, 4, 8, AlignLeft, AlignCenter, furi_string_get_cstr(temp_str));
        furi_string_printf(temp_str, "Shot: %d", fv_priv->run_state.shot_cur);
        canvas_draw_str_aligned(
            canvas, 124, 8, AlignRight, AlignCenter, furi_string_get_cstr(temp_str));
        elements_button_left(canvas, "Cfg");
        elements_button_right(canvas, "Snap");
        if(fv_priv->run_state.state == FVDone) {
            elements_button_center(canvas, "Start");
        } else {
            elements_button_center(canvas, "Stop ");
        }
    } else if(fv_priv->ui_scene == FVSceneConfig) {
        flipvalo_config_edit_draw(canvas, &fv_priv->config_edit_view);
    }

    furi_string_free(temp_str);
    furi_mutex_release(fv_priv->mutex);
}

static void flipvalo_config_init(struct flipvalo_config* fv_conf) {
    fv_conf->init_delay_msec = 2000;
    fv_conf->interval_delay_msec = 0;
    fv_conf->shot_count = 1;
    fv_conf->burst_count = 1;
    fv_conf->burst_delay_msec = 0;
    fv_conf->tickrate = 125;
    fv_conf->send_trigger_fn = sony_ir_trigger;
    fv_conf->output_config = NULL;
}

static void flipvalo_priv_init(struct flipvalo_priv* fv_priv) {
    flipvalo_config_init(&fv_priv->config);
    flipvalo_config_edit_view_init(&fv_priv->config_edit_view);
    flipvalo_run_state_init(&fv_priv->run_state);
    fv_priv->gui_shutter_blink = 0;
    fv_priv->timer = NULL;
    fv_priv->notifications = NULL;
    fv_priv->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    fv_priv->gui_shutter_blink = 0;
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

    if(!fv_priv->mutex) {
        FURI_LOG_E("Flipvalo", "Cannot create mutex\r\n");
        ret = 1;
        goto cleanup;
    }

    view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, fv_priv);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    fv_priv->timer = furi_timer_alloc(timer_callback, FuriTimerTypePeriodic, fv_priv);
    furi_timer_start(
        fv_priv->timer, (uint32_t)furi_kernel_get_tick_frequency() / fv_priv->config.tickrate);
    gui = furi_record_open("gui");
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    while(true) {
        event_status = furi_message_queue_get(event_queue, &event, 100);

        furi_mutex_acquire(fv_priv->mutex, FuriWaitForever);

        // catch event_status that is not Ok
        if(event_status == FuriStatusErrorTimeout) {
            // timeout, ignore
            goto next_event;
        } else if(event_status != FuriStatusOk) {
            FURI_LOG_E("Flipvalo", "Event Queue Error: %d\r\n", event_status);
            goto next_event;
            // TODO(luna) evaluate if we should exit here.
            //goto cleanup;
        }

        // handle input
        if(/* long press back */
           event.type == EventTypeKey && event.input.type == InputTypeLong &&
           event.input.key == InputKeyBack) {
            goto cleanup;
        }
        switch(fv_priv->ui_scene) {
        case FVSceneMain:
            // TODO(luna) Maybe give this a function.. look howl clean FVSceneConfig is...
            if(event.type == EventTypeKey) {
                if(event.input.type == InputTypeShort) {
                    switch(event.input.key) {
                    case InputKeyUp:
                        break;
                    case InputKeyDown:
                        break;
                    case InputKeyLeft:
                        flipvalo_intv_stop(fv_priv);
                        fv_priv->config_edit_view.config = &fv_priv->config;
                        fv_priv->ui_scene = FVSceneConfig;
                        break;
                    case InputKeyRight:
                        fv_priv->gui_shutter_blink = 3;
                        fv_priv->config.send_trigger_fn(fv_priv->config.output_config);
                        break;
                    case InputKeyOk:
                        if(flipvalo_intv_running(fv_priv)) {
                            flipvalo_intv_stop(fv_priv);
                        } else {
                            flipvalo_intv_start(fv_priv);
                        }
                        break;
                    case InputKeyMAX:
                        break;
                    case InputKeyBack:
                        break;
                    }
                }
            }
            break;
        case FVSceneConfig:
            ret = flipvalo_config_edit_input(&event.input, &fv_priv->config_edit_view);
            if(ret) {
                fv_priv->ui_scene = FVSceneMain;
            }
            break;
        }

    next_event:
        furi_mutex_release(fv_priv->mutex);

        view_port_update(view_port);
    }

cleanup:
    if(view_port) {
        view_port_enabled_set(view_port, false);
        if(gui) {
            gui_remove_view_port(gui, view_port);
            furi_record_close("gui");
        }
        view_port_free(view_port);
    }
    if(event_queue) {
        furi_message_queue_free(event_queue);
    }
    if(fv_priv) {
        furi_mutex_free(fv_priv->mutex);
        furi_timer_free(fv_priv->timer);
    }
    free(fv_priv);
    return ret;
}
