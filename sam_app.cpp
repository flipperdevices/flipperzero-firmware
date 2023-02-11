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
} TextState;

static void say_something(char* something) {
    if(furi_hal_speaker_is_mine() || furi_hal_speaker_acquire(1000)) {
        voice.begin();
        voice.say(something);
        furi_hal_speaker_release();
    }
    // return 0;
}

static void text_input_callback(void* ctx) {
    TextState* text_state = (TextState*)acquire_mutex((ValueMutex*)ctx, 25);
    FURI_LOG_D("SAM", "Input text: %s", text_state->input);
    FURI_LOG_D("SAM", "Upper text: %s", text_state->input);
    text_box_set_text(text_state->text_box, text_state->input);
    view_dispatcher_switch_to_view(text_state->view_dispatcher, 1);

    release_mutex((ValueMutex*)ctx, text_state);
}

static bool back_event_callback(void* ctx) {
    const TextState* text_state = (TextState*)acquire_mutex((ValueMutex*)ctx, 25);
    view_dispatcher_stop(text_state->view_dispatcher);
    release_mutex((ValueMutex*)ctx, text_state);
    return true;
}

static void sam_state_init(TextState* const text_state) {
    text_state->view_dispatcher = view_dispatcher_alloc();
    text_state->text_input = text_input_alloc();
    text_state->text_box = text_box_alloc();
    text_box_set_font(text_state->text_box, TextBoxFontText);
}

static void sam_state_free(TextState* const text_state) {
    text_input_free(text_state->text_input);
    text_box_free(text_state->text_box);
    view_dispatcher_remove_view(text_state->view_dispatcher, 0);
    view_dispatcher_remove_view(text_state->view_dispatcher, 1);
    view_dispatcher_free(text_state->view_dispatcher);
    free(text_state);
}

extern "C" int32_t sam_app(void* p) {
    UNUSED(p);
    TextState* text_state = (TextState*)malloc(sizeof(TextState));

    FURI_LOG_D("SAM", "Running sam_state_init");
    sam_state_init(text_state);

    ValueMutex state_mutex;
    if(!init_mutex(&state_mutex, text_state, sizeof(TextState))) {
        FURI_LOG_E("SAM", "cannot create mutex\r\n");
        free(text_state);
        return 255;
    }

    FURI_LOG_D("SAM", "Assigning text input callback");
    text_input_set_result_callback(
        text_state->text_input,
        text_input_callback,
        &state_mutex,
        text_state->input,
        TEXT_BUFFER_SIZE,
        //clear default text
        true);
    text_input_set_header_text(text_state->text_input, "Input");

    // Open GUI and register view_port
    Gui* gui = (Gui*)furi_record_open("gui");
    //gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    FURI_LOG_D("SAM", "Enabling view dispatcher queue");
    view_dispatcher_enable_queue(text_state->view_dispatcher);

    FURI_LOG_D("SAM", "Adding text input view to dispatcher");
    view_dispatcher_add_view(
        text_state->view_dispatcher, 0, text_input_get_view(text_state->text_input));
    view_dispatcher_add_view(
        text_state->view_dispatcher, 1, text_box_get_view(text_state->text_box));
    FURI_LOG_D("SAM", "Attaching view dispatcher to GUI");
    view_dispatcher_attach_to_gui(text_state->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    FURI_LOG_D("ceasar_cipher", "starting view dispatcher");
    view_dispatcher_set_navigation_event_callback(
        text_state->view_dispatcher, back_event_callback);
    view_dispatcher_set_event_callback_context(text_state->view_dispatcher, &state_mutex);
    view_dispatcher_switch_to_view(text_state->view_dispatcher, 0);
    view_dispatcher_run(text_state->view_dispatcher);

    // for(bool running = true; running;) {
    //     PluginEvent event;
    //     FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(PluginEvent));
    //     FuriStatus event_status = furi_message_queue_get(event_queue, &event, FuriWaitForever);
    //     if(event_status == FuriStatusOk) {
    //         if(event.input.key == InputKeyOk) {
    say_something(text_state->input);
    //         }
    //         if(event.input.key == InputKeyBack) {
    //             running = false;
    //         }
    //     }
    // }

    furi_record_close("gui");
    delete_mutex(&state_mutex);
    sam_state_free(text_state);

    return 0;
}
