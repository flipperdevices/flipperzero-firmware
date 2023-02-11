#include <furi.h>
#include <furi_hal_speaker.h>
// #include <furi_hal.h>
#include <stdlib.h>

#include <input/input.h>

#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/text_input.h>
#include <gui/modules/text_box.h>

#include "stm32_sam.h"

#define TEXT_BUFFER_SIZE 256
STM32SAM voice;

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
    TextBox* text_box;
    char input[TEXT_BUFFER_SIZE];
} AppState;

static void say_something(char* something) {
    if(furi_hal_speaker_is_mine() || furi_hal_speaker_acquire(1000)) {
        voice.begin();
        voice.say(something);
        furi_hal_speaker_release();
    }
    // return 0;
}

// static void underscore_to_space(char* input) {
//     for(int i = 0; input[i] != '\0'; i++) {
//         if(input[i] == '_') {
//             app_state->input[i] = ' ';
//         } else {
//             app_state->input[i] = input[i];
//         }
//     }
// }

static void text_input_callback(void* ctx) {
    AppState* app_state = (AppState*)acquire_mutex((ValueMutex*)ctx, 25);
    FURI_LOG_D("SAM", "Input text: %s", app_state->input);

    // underscore_to_space(app_state->input);
    for(int i = 0; app_state->input[i] != '\0'; i++) {
        if(app_state->input[i] == '_') {
            app_state->input[i] = ' ';
        } else {
            app_state->input[i] = app_state->input[i];
        }
    }

    text_box_set_text(app_state->text_box, app_state->input);
    view_dispatcher_switch_to_view(app_state->view_dispatcher, 1);

    release_mutex((ValueMutex*)ctx, app_state);
}

static bool back_event_callback(void* ctx) {
    const AppState* app_state = (AppState*)acquire_mutex((ValueMutex*)ctx, 25);
    view_dispatcher_stop(app_state->view_dispatcher);
    release_mutex((ValueMutex*)ctx, app_state);
    return true;
}

static void sam_state_init(AppState* const app_state) {
    app_state->view_dispatcher = view_dispatcher_alloc();
    app_state->text_input = text_input_alloc();
    app_state->text_box = text_box_alloc();
    text_box_set_font(app_state->text_box, TextBoxFontText);
}

static void sam_state_free(AppState* const app_state) {
    text_input_free(app_state->text_input);
    text_box_free(app_state->text_box);
    view_dispatcher_remove_view(app_state->view_dispatcher, 0);
    view_dispatcher_remove_view(app_state->view_dispatcher, 1);
    view_dispatcher_free(app_state->view_dispatcher);
    free(app_state);
}

extern "C" int32_t sam_app(void* p) {
    UNUSED(p);
    AppState* app_state = (AppState*)malloc(sizeof(AppState));

    FURI_LOG_D("SAM", "Running sam_state_init");
    sam_state_init(app_state);

    ValueMutex state_mutex;
    if(!init_mutex(&state_mutex, app_state, sizeof(AppState))) {
        FURI_LOG_E("SAM", "cannot create mutex\r\n");
        free(app_state);
        return 255;
    }

    FURI_LOG_D("SAM", "Assigning text input callback");
    text_input_set_result_callback(
        app_state->text_input,
        text_input_callback,
        &state_mutex,
        app_state->input,
        TEXT_BUFFER_SIZE,
        //clear default text
        true);
    text_input_set_header_text(app_state->text_input, "Input");

    // Open GUI and register view_port
    Gui* gui = (Gui*)furi_record_open("gui");
    //gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    FURI_LOG_D("SAM", "Enabling view dispatcher queue");
    view_dispatcher_enable_queue(app_state->view_dispatcher);

    FURI_LOG_D("SAM", "Adding text input view to dispatcher");
    view_dispatcher_add_view(
        app_state->view_dispatcher, 0, text_input_get_view(app_state->text_input));
    view_dispatcher_add_view(
        app_state->view_dispatcher, 1, text_box_get_view(app_state->text_box));

    FURI_LOG_D("SAM", "Attaching view dispatcher to GUI");
    view_dispatcher_attach_to_gui(app_state->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    FURI_LOG_D("SAM", "starting view dispatcher");
    view_dispatcher_set_navigation_event_callback(app_state->view_dispatcher, back_event_callback);
    view_dispatcher_set_event_callback_context(app_state->view_dispatcher, &state_mutex);
    view_dispatcher_switch_to_view(app_state->view_dispatcher, 0);
    view_dispatcher_run(app_state->view_dispatcher);

    // for(bool running = true; running;) {
    //     PluginEvent event;
    //     FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(PluginEvent));
    //     FuriStatus event_status = furi_message_queue_get(event_queue, &event, FuriWaitForever);
    //     if(event_status == FuriStatusOk) {
    //         if(event.input.key == InputKeyOk) {
    say_something(app_state->input);
    FURI_LOG_D("SAM", "Spoken text: %s", app_state->input);
    //         }
    //         if(event.input.key == InputKeyBack) {
    //             running = false;
    //         }
    //     }
    // }

    furi_record_close("gui");
    delete_mutex(&state_mutex);
    sam_state_free(app_state);

    return 0;
}
