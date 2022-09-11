#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <gui/elements.h>

#include "clock_app.h"

static void clock_input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);
    PluginEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void clock_render_callback(Canvas* const canvas, void* ctx) {
    FURI_LOG_T(TAG, "clock_render_callback - start");

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);

    ClockState* state = ctx;
    if(furi_mutex_acquire(state->mutex, 200) != FuriStatusOk) {
        FURI_LOG_D(TAG, "Can't obtain mutex, requeue render");
        PluginEvent event = {.type = EventTypeTick};
        furi_message_queue_put(state->event_queue, &event, 0);
        return;
    }

    char time_string[TIME_LEN];
    char date_string[DATE_LEN];
    char meridian_string[MERIDIAN_LEN];

    if(state->settings.time_format == H24) {
        snprintf(
            time_string,
            TIME_LEN,
            CLOCK_TIME_FORMAT,
            state->datetime.hour,
            state->datetime.minute,
            state->datetime.second);
    } else {
        bool pm = state->datetime.hour > 12;
        snprintf(
            time_string,
            TIME_LEN,
            CLOCK_TIME_FORMAT,
            pm ? state->datetime.hour - 12 : state->datetime.hour,
            state->datetime.minute,
            state->datetime.second);

        snprintf(
            meridian_string,
            MERIDIAN_LEN,
            MERIDIAN_FORMAT,
            pm ? MERIDIAN_STRING_PM : MERIDIAN_STRING_AM);
    }

    if(state->settings.date_format == Iso) {
        snprintf(
            date_string,
            DATE_LEN,
            CLOCK_ISO_DATE_FORMAT,
            state->datetime.year,
            state->datetime.month,
            state->datetime.day);
    } else {
        snprintf(
            date_string,
            DATE_LEN,
            CLOCK_RFC_DATE_FORMAT,
            state->datetime.day,
            state->datetime.month,
            state->datetime.year);
    }

    furi_mutex_release(state->mutex);

    canvas_set_font(canvas, FontBigNumbers);
    canvas_draw_str_aligned(canvas, 64, 28, AlignCenter, AlignCenter, time_string);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 64, 42, AlignCenter, AlignTop, date_string);

    if(state->settings.time_format == H12)
            canvas_draw_str_aligned(canvas, 65, 12, AlignCenter, AlignCenter, meridian_string);

    FURI_LOG_T(TAG, "clock_render_callback - end");
}

static void clock_state_init(ClockState* const state) {
    LOAD_CLOCK_SETTINGS(&state->settings);
    FURI_LOG_D(TAG, "Time format: %s", state->settings.time_format == H12 ? "12h" : "24h");
    FURI_LOG_D(
        TAG, "Date format: %s", state->settings.date_format == Iso ? "ISO 8601" : "RFC 5322");
    furi_hal_rtc_get_datetime(&state->datetime);
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
    FURI_LOG_D(TAG, "Event queue created");

    plugin_state->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if(plugin_state->mutex == NULL) {
        FURI_LOG_E(TAG, "Cannot create mutex");
        furi_message_queue_free(plugin_state->event_queue);
        free(plugin_state);
        return 255;
    }
    FURI_LOG_D(TAG, "Mutex created");

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
    FURI_LOG_D(TAG, "Timer created");

    // Open GUI and register view_port
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    furi_timer_start(timer, furi_kernel_get_tick_frequency());
    FURI_LOG_D(TAG, "Timer started");

    // Main loop
    PluginEvent event;
    for(bool processing = true; processing;) {
        FuriStatus event_status = furi_message_queue_get(plugin_state->event_queue, &event, 100);

        if(event_status != FuriStatusOk) continue;

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
            furi_hal_rtc_get_datetime(&plugin_state->datetime);
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