#include <furi.h>
#include <input/input.h>
#include <stdlib.h>

#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/text_input.h>

#define TEXT_BUFFER_SIZE 256

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} PluginEvent;

typedef struct {
  ViewDispatcher* view_dispatcher;
  TextInput* text_input;
  char input[TEXT_BUFFER_SIZE];
} CaesarState;

//static void render_callback(Canvas* const canvas, void* ctx) {
//    const CaesarState* caesar_state = acquire_mutex((ValueMutex*)ctx, 25);
//    if(caesar_state == NULL) {
//        return;
//    }
//    // border around the edge of the screen
//    canvas_draw_frame(canvas, 0, 0, 128, 64);
//
//    canvas_set_font(canvas, FontPrimary);
//
//    release_mutex((ValueMutex*)ctx, caesar_state);
//}

//static void input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
//    furi_assert(event_queue);
//
//    PluginEvent event = {.type = EventTypeKey, .input = *input_event};
//    furi_message_queue_put(event_queue, &event, FuriWaitForever);
//}

static void text_input_callback(void* ctx) {
    const CaesarState* caesar_state = acquire_mutex((ValueMutex*)ctx, 25);
    FURI_LOG_D("caesar_cipher", "Input text: %s", caesar_state->input);
    release_mutex((ValueMutex*)ctx, caesar_state);
}

static void caesar_cipher_state_init(CaesarState* const caesar_state) {
  caesar_state->view_dispatcher = view_dispatcher_alloc();
  caesar_state->text_input = text_input_alloc();
  // caesar_state->input = ""; - this is not python 
}

static void caesar_cipher_state_free(CaesarState* const caesar_state) {
  text_input_free(caesar_state->text_input);
  view_dispatcher_remove_view(caesar_state->view_dispatcher, 0);
  view_dispatcher_free(caesar_state->view_dispatcher);
  free(caesar_state);
}

int32_t caesar_cipher_app() {
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(PluginEvent));

    CaesarState* caesar_state = malloc(sizeof(CaesarState));
    
    FURI_LOG_D("caesar_cipher", "Running caesar_cipher_state_init");
    caesar_cipher_state_init(caesar_state);

    ValueMutex state_mutex;
    if(!init_mutex(&state_mutex, caesar_state, sizeof(CaesarState))) {
        FURI_LOG_E("caesar_cipher", "cannot create mutex\r\n");
        free(caesar_state);
        return 255;
    }

    // Set system callbacks
    //ViewPort* view_port = view_port_alloc();
    //view_port_draw_callback_set(view_port, render_callback, &state_mutex);
    //view_port_input_callback_set(view_port, input_callback, event_queue);

    FURI_LOG_D("caesar_cipher", "Assigning text input callback");
    text_input_set_result_callback(
        caesar_state->text_input,
        text_input_callback,
        &state_mutex,
        caesar_state->input,
        TEXT_BUFFER_SIZE,
        //clear default text
        true
    );
    text_input_set_header_text(caesar_state->text_input, "Input");

    // Open GUI and register view_port
    Gui* gui = furi_record_open("gui");
    //gui_add_view_port(gui, view_port, GuiLayerFullscreen);
    
    //FURI_LOG_D("caesar_cipher", "Enabling view dispatcher queue");
    //view_dispatcher_enable_queue(caesar_state->view_dispatcher);

    FURI_LOG_D("caesar_cipher", "Adding text input view to dispatcher");
    view_dispatcher_add_view(caesar_state->view_dispatcher, 0, text_input_get_view(caesar_state->text_input)); 
    FURI_LOG_D("caesar_cipher", "Attaching view dispatcher to GUI");
    view_dispatcher_attach_to_gui(caesar_state->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    FURI_LOG_D("ceasar_cipher", "starting view dispatcher");
    //view_dispatcher_run(caesar_state->view_dispatcher);
    view_dispatcher_switch_to_view(caesar_state->view_dispatcher, 0);

    PluginEvent event;
    for(bool processing = true; processing;) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 100);

        CaesarState* caesar_state = (CaesarState*)acquire_mutex_block(&state_mutex);

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
                        break;
                    case InputKeyBack:
                        processing = false;
                        break;
                    }
                }
            }
        } else {
            FURI_LOG_D("caesar_cipher", "FuriMessageQueue: event timeout");
            // event timeout
        }

        //view_port_update(view_port);
        release_mutex(&state_mutex, caesar_state);
    }

    //view_port_enabled_set(view_port, false);
    //gui_remove_view_port(gui, view_port);
    furi_record_close("gui");
    //view_port_free(view_port);
    furi_message_queue_free(event_queue);
    delete_mutex(&state_mutex);
    caesar_cipher_state_free(caesar_state);

    return 0;
}
