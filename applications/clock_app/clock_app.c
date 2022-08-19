#include <furi.h>
#include <furi_hal.h>
#include <gui/elements.h>
#include <gui/gui.h>
#include <input/input.h>

#define TAG "Clock"

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} PluginEvent;

typedef struct {
    FuriMutex* mutex;
    FuriMessageQueue* event_queue;
    uint32_t timer_start_timestamp;
    uint32_t timer_stopped_seconds;
    bool timer_running;
    bool militaryTime; // 24 hour
} ClockState;

static void clock_input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);
    PluginEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void clock_render_callback(Canvas* const canvas, void* ctx) {
    ClockState* state = ctx;
    if(furi_mutex_acquire(state->mutex, 200) != FuriStatusOk) {
        // Can't obtain mutex, requeue render
        PluginEvent event = {.type = EventTypeTick};
        furi_message_queue_put(state->event_queue, &event, 0);
        return;
    }

    FuriHalRtcDateTime curr_dt;
    furi_hal_rtc_get_datetime(&curr_dt);
    uint32_t curr_ts = furi_hal_rtc_datetime_to_timestamp(&curr_dt);

    char strings[3][20];
    snprintf(strings[0], 20, "%.4d-%.2d-%.2d", curr_dt.year, curr_dt.month, curr_dt.day);

    uint8_t hour = curr_dt.hour;
    char strAMPM[3];
    snprintf(strAMPM, sizeof(strAMPM),"%s","AM");
    if(!state->militaryTime && hour > 12) {
        hour -= 12;
		snprintf(strAMPM, sizeof(strAMPM),"%s","PM");
    }
    snprintf(strings[1], 20, "%.2d:%.2d:%.2d", hour, curr_dt.minute, curr_dt.second);

    bool timer_running = state->timer_running;
    uint32_t timer_start_timestamp = state->timer_start_timestamp;
    uint32_t timer_stopped_seconds = state->timer_stopped_seconds;

    furi_mutex_release(state->mutex);

    canvas_set_font(canvas, FontBigNumbers);
    if(timer_start_timestamp != 0) {
        int32_t elapsed_secs = timer_running ? (curr_ts - timer_start_timestamp) :
                                               timer_stopped_seconds;
        snprintf(strings[2], 20, "%.2ld:%.2ld", elapsed_secs / 60, elapsed_secs % 60);
        canvas_draw_str_aligned(canvas, 64, 8, AlignCenter, AlignCenter, strings[1]); // DRAW TIME
        canvas_draw_str_aligned(canvas, 64, 32, AlignCenter, AlignTop, strings[2]); // DRAW TIMER
        canvas_set_font(canvas, FontSecondary);
	    if(!state->militaryTime) canvas_draw_str_aligned(canvas, 118, 5, AlignCenter, AlignCenter, strAMPM);
        canvas_draw_str_aligned(canvas, 64, 20, AlignCenter, AlignTop, strings[0]); // DRAW DATE
        elements_button_left(canvas, "Reset");
    } else {
        canvas_draw_str_aligned(canvas, 64, 26, AlignCenter, AlignCenter, strings[1]); // DRAW TIME
        canvas_set_font(canvas, FontSecondary);
	    if(!state->militaryTime) canvas_draw_str_aligned(canvas, 67, 15, AlignCenter, AlignCenter, strAMPM);
        canvas_draw_str_aligned(canvas, 64, 38, AlignCenter, AlignTop, strings[0]); // DRAW DATE
        elements_button_left(canvas, state->militaryTime ? "12h" : "24h");
    }
    if(timer_running) {
        elements_button_center(canvas, "Stop");
    } else {
        elements_button_center(canvas, "Start");
    }
}

static void clock_state_init(ClockState* const state) {
    memset(state, 0, sizeof(ClockState));
    state->militaryTime = true;
}

// Runs every 1000ms by default
static void clock_tick(void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    PluginEvent event = {.type = EventTypeTick};
    // It's OK to lose this event if system overloaded
    furi_message_queue_put(event_queue, &event, 0);
}

int32_t clock_app(void* p) {
    UNUSED(p);
    ClockState* plugin_state = malloc(sizeof(ClockState));
    clock_state_init(plugin_state);
    plugin_state->event_queue = furi_message_queue_alloc(8, sizeof(PluginEvent));
    if(plugin_state->event_queue == NULL) {
        FURI_LOG_E(TAG, "cannot create event queue\n");
        free(plugin_state);
        return 255;
    }

    plugin_state->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if(plugin_state->mutex == NULL) {
        FURI_LOG_E(TAG, "cannot create mutex\n");
        furi_message_queue_free(plugin_state->event_queue);
        free(plugin_state);
        return 255;
    }

    FuriTimer* timer =
        furi_timer_alloc(clock_tick, FuriTimerTypePeriodic, plugin_state->event_queue);
    if(timer == NULL) {
        FURI_LOG_E(TAG, "cannot create timer\n");
        furi_mutex_free(plugin_state->mutex);
        furi_message_queue_free(plugin_state->event_queue);
        free(plugin_state);
        return 255;
    }

    // Set system callbacks
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, clock_render_callback, plugin_state);
    view_port_input_callback_set(view_port, clock_input_callback, plugin_state->event_queue);

    // Open GUI and register view_port
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);
    furi_timer_start(timer, furi_kernel_get_tick_frequency());

    // Main loop
    PluginEvent event;
    for(bool processing = true; processing;) {
        FuriStatus event_status = furi_message_queue_get(plugin_state->event_queue, &event, 100);
        if(event_status == FuriStatusOk) {
            if(furi_mutex_acquire(plugin_state->mutex, FuriWaitForever) != FuriStatusOk) continue;
            // press events
            if(event.type == EventTypeKey) {
                if(event.input.type == InputTypeShort || event.input.type == InputTypeRepeat) {
                    if(event.input.key == InputKeyOk) {
                        // START/STOP TIMER
                        FuriHalRtcDateTime curr_dt;
                        furi_hal_rtc_get_datetime(&curr_dt);
                        uint32_t curr_ts = furi_hal_rtc_datetime_to_timestamp(&curr_dt);

                        if(plugin_state->timer_running) {
                            // Update stopped seconds
                            plugin_state->timer_stopped_seconds =
                                curr_ts - plugin_state->timer_start_timestamp;
                        } else {
                            if(plugin_state->timer_start_timestamp == 0) {
                                // Set starting timestamp if this is first time
                                plugin_state->timer_start_timestamp = curr_ts;
                            } else {
                                // Timer was already running, need to slightly readjust so we don't
                                // count the intervening time
                                plugin_state->timer_start_timestamp =
                                    curr_ts - plugin_state->timer_stopped_seconds;
                            }
                        }
                        plugin_state->timer_running = !plugin_state->timer_running;
                    } else if(event.input.key == InputKeyLeft) {
                        if(plugin_state->timer_start_timestamp != 0) {
                            // Reset seconds
                            plugin_state->timer_running = false;
                            plugin_state->timer_start_timestamp = 0;
                            plugin_state->timer_stopped_seconds = 0;
                        } else {
                            // Toggle 12/24 hours
                            plugin_state->militaryTime = !plugin_state->militaryTime;
                        }
                    } else if(event.input.key == InputKeyBack) {
                        // Exit the plugin
                        processing = false;
                    }
                }
            } else if(event.type == EventTypeTick) {
                // Do nothing, just need to update viewport
            }
            view_port_update(view_port);
            furi_mutex_release(plugin_state->mutex);
        } else {
            // event timeout
            // FURI_LOG_D(TAG, "osMessageQueue: event timeout");
        }
    }
    // Cleanup
    furi_timer_free(timer);
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close(RECORD_GUI);
    view_port_free(view_port);
    furi_message_queue_free(plugin_state->event_queue);
    furi_mutex_free(plugin_state->mutex);
    free(plugin_state);
    return 0;
}
