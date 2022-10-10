#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <gui/elements.h>

#include "clock_app.h"

static void clock_app_input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);
    PluginEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void clock_app_render_callback(Canvas* const canvas, void* ctx) {
    ClockApp* clock_app = ctx;
    if(furi_mutex_acquire(clock_app->mutex, 200) != FuriStatusOk) {
        return;
    }

    ClockState* state = clock_app->clock_state;

    char time_string[TIME_LEN];
    char date_string[DATE_LEN];
    char meridian_string[MERIDIAN_LEN];

    if(clock_app->settings.time_format == H24) {
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

    if(clock_app->settings.date_format == Iso) {
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

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontBigNumbers);
    canvas_draw_str_aligned(canvas, 64, 28, AlignCenter, AlignCenter, time_string);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 64, 42, AlignCenter, AlignTop, date_string);

    if(clock_app->settings.time_format == H12)
        canvas_draw_str_aligned(canvas, 65, 12, AlignCenter, AlignCenter, meridian_string);

    furi_mutex_release(clock_app->mutex);
}

static void clock_app_state_init(ClockApp* const clock_app) {
    bool load_success = LOAD_CLOCK_SETTINGS(&clock_app->settings);
    if(!load_success) {
        FURI_LOG_D(TAG, "No setting file");
    }
    FURI_LOG_I(TAG, "Time format: %s", clock_app->settings.time_format == H12 ? "12h" : "24h");
    FURI_LOG_I(
        TAG, "Date format: %s", clock_app->settings.date_format == Iso ? "ISO 8601" : "RFC 5322");
    furi_hal_rtc_get_datetime(&clock_app->clock_state->datetime);
}

static void clock_app_tick(void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    PluginEvent event = {.type = EventTypeTick};
    // It's OK to loose this event if system overloaded
    furi_message_queue_put(event_queue, &event, 0);
}

int32_t clock_app(void* p) {
    UNUSED(p);
    ClockApp* clock_app = malloc(sizeof(ClockApp));
    clock_app->clock_state = malloc(sizeof(ClockState));

    clock_app->event_queue = furi_message_queue_alloc(8, sizeof(PluginEvent));
    FURI_LOG_D(TAG, "Event queue created");

    clock_app->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    FURI_LOG_D(TAG, "Mutex created");

    clock_app_state_init(clock_app);

    // Set system callbacks
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, clock_app_render_callback, clock_app);
    view_port_input_callback_set(view_port, clock_app_input_callback, clock_app->event_queue);

    FuriTimer* timer =
        furi_timer_alloc(clock_app_tick, FuriTimerTypePeriodic, clock_app->event_queue);
    FURI_LOG_D(TAG, "Timer created");

    // Open GUI and register view_port
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    furi_timer_start(timer, 100);
    FURI_LOG_D(TAG, "Timer started");

    // Main loop
    PluginEvent event;
    for(bool processing = true; processing;) {
        FuriStatus event_status =
            furi_message_queue_get(clock_app->event_queue, &event, FuriWaitForever);
        furi_check(event_status == FuriStatusOk);
        furi_mutex_acquire(clock_app->mutex, FuriWaitForever);
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
            furi_hal_rtc_get_datetime(&clock_app->clock_state->datetime);
        }

        furi_mutex_release(clock_app->mutex);
        view_port_update(view_port);
    }

    furi_timer_free(timer);
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close(RECORD_GUI);
    view_port_free(view_port);
    furi_message_queue_free(clock_app->event_queue);
    furi_mutex_free(clock_app->mutex);
    free(clock_app->clock_state);
    free(clock_app);
    return 0;
}