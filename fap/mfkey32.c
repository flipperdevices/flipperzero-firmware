#include <furi.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include <input/input.h>
#include <stdlib.h>
#include "mfkey32_icons.h"

// TODO: Count total number of hashes
// TODO: Update progress bar
// TODO: Use Flipper dictionary
// TODO: Run attack using mfkeypaging.c

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} PluginEvent;

typedef struct {
    bool running;
    int cracked;
    int total;
} ProgramState;

// Screen is 128x64 px
static void render_callback(Canvas* const canvas, void* ctx) {
    UNUSED(ctx);
    const ProgramState* program_state = acquire_mutex((ValueMutex*)ctx, 25);
    if (program_state == NULL) {
        return;
    }
    char draw_str[32] = {};
    float dict_progress = (float) program_state->cracked / (float) program_state->total;
    canvas_clear(canvas);
    canvas_draw_frame(canvas, 0, 0, 128, 64);
    canvas_draw_frame(canvas, 0, 15, 128, 64);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 5, 4, AlignLeft, AlignTop, "Mfkey32");
    canvas_draw_icon(canvas, 114, 4, &I_mfkey);
    if (program_state->running) {
        elements_progress_bar_with_text(canvas, 5, 20, 118, dict_progress, draw_str);
        canvas_set_font(canvas, FontSecondary);
        snprintf(draw_str, sizeof(draw_str), "Keys found: %d/%d", 0, 20);
        canvas_draw_str_aligned(canvas, 26, 33, AlignLeft, AlignTop, draw_str);
        elements_button_center(canvas, "Stop");
    } else {
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(canvas, 50, 30, AlignLeft, AlignTop, "Ready");
        elements_button_center(canvas, "Start");
    }
    release_mutex((ValueMutex*)ctx, program_state);
}

static void input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);

    PluginEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void mfkey32_state_init(ProgramState* const program_state) {
    program_state->running = false;
    program_state->cracked = 0;
    program_state->total = 10; // Simulated
}

int32_t mfkey32_main() {
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(PluginEvent));

    ProgramState* program_state = malloc(sizeof(ProgramState));

    mfkey32_state_init(program_state);

    ValueMutex state_mutex;
    if(!init_mutex(&state_mutex, program_state, sizeof(ProgramState))) {
        FURI_LOG_E("mfkey32", "cannot create mutex\r\n");
        free(program_state);
        return 255;
    }

    // Set system callbacks
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, &state_mutex);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    // Open GUI and register view_port
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    PluginEvent event;
    for(bool main_loop = true; main_loop;) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 100);

        ProgramState* program_state = (ProgramState*)acquire_mutex_block(&state_mutex);

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
                        break;
                    case InputKeyLeft:
                        break;
                    case InputKeyOk:
                        program_state->running = !program_state->running;
                        break;
                    default:
                        main_loop = false;
                        break;
                    }
                }
            }
        } else {
            FURI_LOG_D("mfkey32", "FuriMessageQueue: event timeout");
            // event timeout
        }

        view_port_update(view_port);
        release_mutex(&state_mutex, program_state);
    }

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close("gui");
    view_port_free(view_port);
    furi_message_queue_free(event_queue);
    delete_mutex(&state_mutex);

    return 0;
}
