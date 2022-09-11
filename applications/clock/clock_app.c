#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <gui/elements.h>
#include <input/input.h>

#include "clock_settings.h"

#define TAG "Clock"
#define CLOCK_ISO_DATE_FORMAT "%.4d-%.2d-%.2d"
#define CLOCK_RFC_DATE_FORMAT "%.2d-%.2d-%.4d"
#define CLOCK_TIME_FORMAT "%.2d:%.2d:%.2d"
#define MAX_LEN 20

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} PluginEvent;

typedef struct {
    ClockSettings settings;
    FuriMutex* mutex;
    FuriMessageQueue* event_queue;
} ClockState;

static void clock_input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);
    PluginEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void clock_render_callback(Canvas* const canvas, void* ctx) {
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);

    ClockState* state = ctx;
    if(furi_mutex_acquire(state->mutex, 200) != FuriStatusOk) {
        FURI_LOG_D(TAG, "Can't obtain mutex, requeue render");
        PluginEvent event = {.type = EventTypeTick};
        furi_message_queue_put(state->event_queue, &event, 0);
        return;
    }

    FuriHalRtcDateTime datetime;
    furi_hal_rtc_get_datetime(&datetime);

    char strings[2][MAX_LEN];

    if (*(&state->settings.date_format) == Iso) {
        snprintf(
        strings[0],
        MAX_LEN,
        CLOCK_ISO_DATE_FORMAT,
        datetime.year,
        datetime.month,
        datetime.day);
    } else {
        snprintf(
        strings[0],
        MAX_LEN,
        CLOCK_RFC_DATE_FORMAT,
        datetime.day,
        datetime.month,
        datetime.year);
    }

    if (*(&state->settings.time_format) == H24) {
        snprintf(
        strings[1],
        MAX_LEN,
        CLOCK_TIME_FORMAT,
        datetime.hour,
        datetime.minute,
        datetime.second);
    } else {
        snprintf(
        strings[1],
        MAX_LEN,
        CLOCK_TIME_FORMAT,
        datetime.hour > 12 ? datetime.hour - 12 : datetime.hour,
        datetime.minute,
        datetime.second);
    }

    furi_mutex_release(state->mutex);

    canvas_set_font(canvas, FontBigNumbers);
    canvas_draw_str_aligned(canvas, 64, 42 - 16, AlignCenter, AlignCenter, strings[1]);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 64, 52 - 8, AlignCenter, AlignTop, strings[0]);
}

static void clock_state_init(ClockState* const state) {
    LOAD_CLOCK_SETTINGS(&state->settings);
    FURI_LOG_D(TAG, "Time format: %s", &state->settings.time_format == H12 ? "12h" : "24h");
    FURI_LOG_D(TAG, "Date format: %s", &state->settings.date_format == Iso ? "ISO 8601" : "RFC 5322");
}

// Runs every 1000ms by default
static void clock_tick(void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    PluginEvent event = {.type = EventTypeTick};
    // It's OK to loose this event if system overloaded
    furi_message_queue_put(event_queue, &event, 0);
}

int32_t clock_app(void* p) {
    UNUSED(p);
    ClockState* plugin_state = malloc(sizeof(ClockState));
    
    plugin_state->event_queue = furi_message_queue_alloc(8, sizeof(PluginEvent));
    if(plugin_state->event_queue == NULL) {
        FURI_LOG_E(TAG, "Cannot create event queue");
        free(plugin_state);
        return 255;
    }
    
    plugin_state->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if(plugin_state->mutex == NULL) {
        FURI_LOG_E(TAG, "Cannot create mutex");
        furi_message_queue_free(plugin_state->event_queue);
        free(plugin_state);
        return 255;
    }

    clock_state_init(plugin_state);

    // Set system callbacks
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, clock_render_callback, plugin_state);
    view_port_input_callback_set(view_port, clock_input_callback, plugin_state->event_queue);

    FuriTimer* timer =
        furi_timer_alloc(clock_tick, FuriTimerTypePeriodic, plugin_state->event_queue);

    if(timer == NULL) {
        FURI_LOG_E(TAG, "Cannot create timer");
        furi_mutex_free(plugin_state->mutex);
        furi_message_queue_free(plugin_state->event_queue);
        free(plugin_state);
        return 255;
    }

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
                    switch(event.input.key) {
                    case InputKeyUp:
                    case InputKeyDown:
                    case InputKeyRight:
                    case InputKeyLeft:
                    case InputKeyOk:
                        break;
                    case InputKeyBack:
                        // Exit the plugin
                        processing = false;
                        break;
                    }
                }
            } else if(event.type == EventTypeTick) {
                //furi_hal_rtc_get_datetime(&plugin_state->datetime);
            }
        } else {
            // event timeout
            FURI_LOG_D(TAG, "furi_message_queue: event timeout");
        }
        view_port_update(view_port);
        furi_mutex_release(plugin_state->mutex);
    }

    furi_timer_free(timer);
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close(RECORD_GUI);
    view_port_free(view_port);
    furi_message_queue_free(plugin_state->event_queue);
    furi_mutex_free(plugin_state->mutex);

    return 0;
}