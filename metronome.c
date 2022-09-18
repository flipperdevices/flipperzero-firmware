#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <m-string.h>
#include <stdlib.h>

#define BPM_STEP_SIZE 0.5d

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} PluginEvent;

typedef struct {
    double bpm;
    bool playing;
    FuriTimer* timer;
} MetronomeState;

static void render_callback(Canvas* const canvas, void* ctx) {
    const MetronomeState* metronome_state = acquire_mutex((ValueMutex*)ctx, 25);
    if(metronome_state == NULL) {
        return;
    }
    char* play_state;
    string_t tempStr;
    string_init(tempStr);

    // border around the edge of the screen
    canvas_draw_frame(canvas, 0, 0, 128, 64);
    canvas_set_font(canvas, FontPrimary);

    // draw playing state
    if (metronome_state->playing) {
        play_state = "Playing";
    } else {
        play_state = "Paused";
    }
    canvas_draw_str_aligned(canvas, 5, 10, AlignLeft, AlignBottom, play_state);

    // draw BPM value
    string_printf(tempStr, "%.2f", metronome_state->bpm);
    canvas_set_font(canvas, FontBigNumbers);
    canvas_draw_str_aligned(canvas, 64, 40, AlignCenter, AlignCenter, string_get_cstr(tempStr));
    string_reset(tempStr);

    string_clear(tempStr);
    release_mutex((ValueMutex*)ctx, metronome_state);
}

static void input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);

    PluginEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void timer_callback() {
    //UNUSED(metronome_state);
    furi_hal_speaker_start(440.0f, 1.0f);
    furi_delay_ms(40);
    furi_hal_speaker_stop();
}

static void metronome_state_init(MetronomeState* const metronome_state) {
    metronome_state->bpm = 120.0;
    metronome_state->playing = false;
    metronome_state->timer = furi_timer_alloc(timer_callback, FuriTimerTypePeriodic, metronome_state);
}

int32_t metronome_app() {
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(PluginEvent));

    MetronomeState* metronome_state = malloc(sizeof(MetronomeState));
    metronome_state_init(metronome_state);

    ValueMutex state_mutex;
    if(!init_mutex(&state_mutex, metronome_state, sizeof(MetronomeState))) {
        FURI_LOG_E("Metronome", "cannot create mutex\r\n");
        free(metronome_state);
        return 255;
    }

    // Set system callbacks
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, &state_mutex);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    // Open GUI and register view_port
    Gui* gui = furi_record_open("gui");
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    PluginEvent event;
    for(bool processing = true; processing;) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 100);

        MetronomeState* metronome_state = (MetronomeState*)acquire_mutex_block(&state_mutex);

        if(event_status == FuriStatusOk) {
            // press events
            if(event.type == EventTypeKey) {
                if(event.input.type == InputTypePress) {
                    switch(event.input.key) {
                    case InputKeyUp:
                        break;
                    case InputKeyDown:
                        break;
                    case InputKeyRight:
                        metronome_state->bpm += (double)BPM_STEP_SIZE;
                        break;
                    case InputKeyLeft:
                        metronome_state->bpm -= (double)BPM_STEP_SIZE;
                        break;
                    case InputKeyOk:
                        metronome_state->playing = !metronome_state->playing;
                        if (metronome_state->playing) {
                            // calculate time between beeps
                            uint32_t tps = furi_kernel_get_tick_frequency();
                            double bps = (double)metronome_state->bpm / 60;
                            uint32_t ticks_to_sleep = (uint32_t)round(tps / bps);
                            furi_timer_start(metronome_state->timer, ticks_to_sleep);
                        } else {
                          furi_timer_stop(metronome_state->timer);
                        }
                        break;
                    case InputKeyBack:
                        processing = false;
                        break;
                    }
                }
            }
        } else {
            FURI_LOG_D("Hello_world", "FuriMessageQueue: event timeout");
            // event timeout
        }

        view_port_update(view_port);
        release_mutex(&state_mutex, metronome_state);
    }

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close("gui");
    view_port_free(view_port);
    furi_message_queue_free(event_queue);
    delete_mutex(&state_mutex);
    furi_timer_free(metronome_state->timer);
    free(metronome_state);

    return 0;
}
