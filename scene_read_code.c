#include "flipper.h"
#include "app_state.h"
#include "scenes.h"
#include "scene_read_code.h"
#include <furi_hal_cortex.h>


void read_code_dialog_callback(DialogExResult result, void* context) {
    furi_assert(context);
    App* app = context;
    UNUSED(app);
    if(result == DialogExResultRight) {
        FURI_LOG_I(TAG, "DialogExResultRight");
        scene_manager_next_scene(app->scene_manager, FcomSaveCodeScene);
    }
}

void fcom_read_code_scene_on_enter(void* context) {
    FURI_LOG_I(TAG, "fcom_read_scene_on_enter");
    App* app = context;
    dialog_ex_set_header(app->dialog, "Read Digimon Code", 64, 12, AlignCenter, AlignTop);
    dialog_ex_set_left_button_text(app->dialog, NULL);
    dialog_ex_set_right_button_text(app->dialog, "Save");
    dialog_ex_set_center_button_text(app->dialog, NULL);
    dialog_ex_set_result_callback(app->dialog, read_code_dialog_callback);
    dialog_ex_set_context(app->dialog, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, FcomReadCodeView);

    // start dcomm thread in read mode and flash LED.
    // when code is read, stop LED flashing and set right button to "Save"
}

bool fcom_read_code_scene_on_event(void* context, SceneManagerEvent event) {
    FURI_LOG_I(TAG, "fcom_read_scene_on_event");
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

void fcom_read_code_scene_on_exit(void* context) {
    FURI_LOG_I(TAG, "fcom_read_scene_on_exit");
    UNUSED(context);
    App* app = context;
    UNUSED(app);
    // shut down dcomm
    // clean up
}


