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
    FuriHalRtcDateTime datetime;
    uint32_t timerStartTime;
    uint32_t timerLastRunTime;
    bool timerStarted;
    int timerSecs;
    int timerTempSecs;
} ClockState;

static void clock_input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);
    PluginEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void clock_render_callback(Canvas* const canvas, void* ctx) {
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    ClockState* state = (ClockState*)acquire_mutex((ValueMutex*)ctx, 25);
    if(state == NULL) {
        return;
    }
    char strings[3][20];
    state->timerTempSecs = state->timerSecs;
    if(state->timerStarted) state->timerTempSecs = state->timerSecs + (int) ((furi_hal_rtc_datetime_to_timestamp(&state->datetime) - state->timerStartTime));
    int curMin = (state->timerTempSecs / 60);
    int curSec = state->timerTempSecs - (curMin * 60);
    snprintf(
        strings[0],
        20,
        "%.4d-%.2d-%.2d",
        state->datetime.year,
        state->datetime.month,
        state->datetime.day);
    snprintf(
        strings[1],
        20,
        "%.2d:%.2d:%.2d",
        state->datetime.hour,
        state->datetime.minute,
        state->datetime.second);
    snprintf(strings[2], 20, "%.2d:%.2d", curMin, curSec);
    canvas_set_font(canvas, FontBigNumbers);
    if(state->timerStarted || state->timerTempSecs!=0) {
        canvas_draw_str_aligned(canvas, 64, 8, AlignCenter, AlignCenter, strings[1]); // DRAW TIME
        canvas_draw_str_aligned(canvas, 64, 32, AlignCenter, AlignTop, strings[2]); // DRAW TIMER
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(canvas, 64, 20, AlignCenter, AlignTop, strings[0]); // DRAW DATE
    } else {
        canvas_draw_str_aligned(canvas, 64, 26, AlignCenter, AlignCenter, strings[1]); // DRAW TIME
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(canvas, 64, 38, AlignCenter, AlignTop, strings[0]); // DRAW DATE
    }
    if(state->timerStarted) {
        elements_button_center(canvas, "Stop");
    } else {
        elements_button_center(canvas, "Start");
    }
    if(state->timerTempSecs!=0) elements_button_left(canvas, "Reset");
    release_mutex((ValueMutex*)ctx, state);
}

static void clock_state_init(ClockState* const state) {
    furi_hal_rtc_get_datetime(&state->datetime);
    state->timerStarted = false;
    state->timerSecs = 0;
    state->timerTempSecs = 0;
    state->timerStartTime = 0;
}

// Runs every 1000ms by default
static void clock_tick(void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    PluginEvent event = {.type = EventTypeTick};
    furi_message_queue_put(event_queue, &event, 0);
}

int32_t clock_app(void* p) {
    UNUSED(p);
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(PluginEvent));
    ClockState* plugin_state = malloc(sizeof(ClockState));
    clock_state_init(plugin_state);
    ValueMutex state_mutex;
    if(!init_mutex(&state_mutex, plugin_state, sizeof(ClockState))) {
        FURI_LOG_E(TAG, "cannot create mutex\r\n");
        furi_message_queue_free(event_queue);
        free(plugin_state);
        return 255;
    }
    // Set system callbacks
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, clock_render_callback, &state_mutex);
    view_port_input_callback_set(view_port, clock_input_callback, event_queue);
    FuriTimer* timer = furi_timer_alloc(clock_tick, FuriTimerTypePeriodic, event_queue);
    furi_timer_start(timer, furi_kernel_get_tick_frequency());
    // Open GUI and register view_port
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);
    // Main loop
    PluginEvent event;
    for(bool processing = true; processing;) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 100);
        ClockState* plugin_state = (ClockState*)acquire_mutex_block(&state_mutex);
        if(event_status == FuriStatusOk) {
            // press events
            if(event.type == EventTypeKey) {
                if(event.input.type == InputTypeShort || event.input.type == InputTypeRepeat) {
                    switch(event.input.key) {
                    case InputKeyUp:
                        break;
                    case InputKeyDown:
                        break;
                    case InputKeyRight:
                        break;
                    case InputKeyLeft:
                        plugin_state->timerStartTime = furi_hal_rtc_datetime_to_timestamp(&plugin_state->datetime);
                        plugin_state->timerSecs = 0;
                        plugin_state->timerTempSecs = 0;
                        break;
                    case InputKeyOk:
                        if(plugin_state->timerStarted) {
                            plugin_state->timerStarted = false;
                            plugin_state->timerSecs = plugin_state->timerSecs + (int) ((furi_hal_rtc_datetime_to_timestamp(&plugin_state->datetime) - plugin_state->timerStartTime));
                        } else {
                            plugin_state->timerStarted = true;
                            plugin_state->timerStartTime = furi_hal_rtc_datetime_to_timestamp(&plugin_state->datetime);
                        }
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
        } else {
            FURI_LOG_D(TAG, "osMessageQueue: event timeout");
        }
        view_port_update(view_port);
        release_mutex(&state_mutex, plugin_state);
    }
    furi_timer_free(timer);
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close(RECORD_GUI);
    view_port_free(view_port);
    delete_mutex(&state_mutex);
    free(plugin_state);
    furi_message_queue_free(event_queue);
    return 0;
}
