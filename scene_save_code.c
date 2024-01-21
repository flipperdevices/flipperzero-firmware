#include "flipper.h"
#include "app_state.h"
#include "scenes.h"
#include "scene_save_code.h"
#include <furi_hal_cortex.h>

void save_text_input_callback(void* context) {
    App* app = context;
    FURI_LOG_I(TAG, "save_text_input_callback %s", app->state->file_name_tmp);
    //view_dispatcher_send_custom_event(subghz->view_dispatcher, SubGhzCustomEventSceneSaveName);
}


void fcom_save_code_scene_on_enter(void* context) {
    FURI_LOG_I(TAG, "fcom_save_code_scene_on_enter");
    App* app = context;
    // initialize dcomm
    // start dcomm thread in read mode

    text_input_reset(app->text_input);
    text_input_set_header_text(app->text_input, "Set filename");
    text_input_set_result_callback(app->text_input,
                                   save_text_input_callback,
                                   app,
                                   app->state->file_name_tmp,
                                   MAX_FILENAME_LEN,
                                   true);
    view_dispatcher_switch_to_view(app->view_dispatcher, FcomKeyboardView);
}

bool fcom_save_code_scene_on_event(void* context, SceneManagerEvent event) {
    FURI_LOG_I(TAG, "fcom_save_code_scene_on_event");
    UNUSED(context);
    UNUSED(event);

    // wait for event, then transfer to
    // Display code and Retry/More
    // More goes to submenu "Save" / "Emulate" 
    // Save goes to text input ("Name the card")
    // Emulate goes to Send screen "Send" press OK sends the code
    //

    return false; //consumed event
}

void fcom_save_code_scene_on_exit(void* context) {
    FURI_LOG_I(TAG, "fcom_save_code_scene_on_exit");
    UNUSED(context);
    App* app = context;
    UNUSED(app);
    // shut down dcomm
    // clean up
}


