#include <furi.h>
#include <furi_hal_speaker.h>
#include <stdlib.h>

#include <input/input.h>
#include <dialogs/dialogs.h>
#include <flipper_format/flipper_format.h>

#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/text_input.h>

#include "stm32_sam.h"

#define TAG "SAM"
#define SAM_SAVE_PATH EXT_PATH("apps_data/sam.txt")
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
    char input[TEXT_BUFFER_SIZE];
} AppState;

AppState* app_state;

static void say_something(char* something) {
    if(furi_hal_speaker_is_mine() || furi_hal_speaker_acquire(1000)) {
        voice.begin();
        voice.say(something);
        furi_hal_speaker_release();
    }
}

static void text_input_callback(void* ctx) {
    AppState* app_state = (AppState*)ctx;
    //FURI_LOG_D(TAG, "Input text: %s", app_state->input);

    // underscore_to_space(app_state->input);
    for(int i = 0; app_state->input[i] != '\0'; i++) {
        if(app_state->input[i] == '_') {
            app_state->input[i] = ' ';
        }
    }

    say_something(app_state->input);
}

static bool back_event_callback(void* ctx) {
    const AppState* app_state = (AppState*)ctx;
    view_dispatcher_stop(app_state->view_dispatcher);
    return true;
}

static void sam_state_init(AppState* const app_state) {
    app_state->view_dispatcher = view_dispatcher_alloc();
    app_state->text_input = text_input_alloc();
}

static void sam_state_free(AppState* const app_state) {
    text_input_free(app_state->text_input);
    view_dispatcher_remove_view(app_state->view_dispatcher, 0);
    view_dispatcher_free(app_state->view_dispatcher);
    free(app_state);
}

static void save_message(FuriString* save_string) {
    Storage* storage = (Storage*)furi_record_open(RECORD_STORAGE);
    storage_common_copy(storage, EXT_PATH("sam.txt"), SAM_SAVE_PATH);
    storage_common_remove(storage, EXT_PATH("sam.txt"));
    File* file = storage_file_alloc(storage);
    if(storage_file_open(file, SAM_SAVE_PATH, FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
        storage_file_write(file, save_string, TEXT_BUFFER_SIZE);
    }
    storage_file_close(file);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
}

static bool load_messages() {
    Storage* storage = (Storage*)furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(storage);
    uint16_t bytes_read = 0;
    if(storage_file_open(file, SAM_SAVE_PATH, FSAM_READ, FSOM_OPEN_EXISTING)) {
        bytes_read = storage_file_read(file, app_state->input, TEXT_BUFFER_SIZE);
    }
    storage_file_close(file);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
    return bytes_read == TEXT_BUFFER_SIZE;
}

extern "C" int32_t sam_app(void* p) {
    UNUSED(p);
    app_state = (AppState*)malloc(sizeof(AppState));

    FURI_LOG_D(TAG, "Running sam_state_init");
    sam_state_init(app_state);

    FURI_LOG_D(TAG, "Assigning text input callback");

    load_messages();
    text_input_set_result_callback(
        app_state->text_input,
        text_input_callback,
        app_state,
        app_state->input,
        TEXT_BUFFER_SIZE,
        false); //clear default text
    text_input_set_header_text(app_state->text_input, "Input");

    Gui* gui = (Gui*)furi_record_open(RECORD_GUI);

    view_dispatcher_enable_queue(app_state->view_dispatcher);

    FURI_LOG_D(TAG, "Adding text input view to dispatcher");
    view_dispatcher_add_view(
        app_state->view_dispatcher, 0, text_input_get_view(app_state->text_input));

    FURI_LOG_D(TAG, "Attaching view dispatcher to GUI");
    view_dispatcher_attach_to_gui(app_state->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    FURI_LOG_D(TAG, "starting view dispatcher");
    view_dispatcher_set_navigation_event_callback(app_state->view_dispatcher, back_event_callback);
    view_dispatcher_set_event_callback_context(app_state->view_dispatcher, app_state);
    view_dispatcher_switch_to_view(app_state->view_dispatcher, 0);
    view_dispatcher_run(app_state->view_dispatcher);

    save_message((FuriString*)app_state->input);

    furi_record_close(RECORD_GUI);
    sam_state_free(app_state);

    return 0;
}
