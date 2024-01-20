#include "flipper.h"
#include "app_state.h"
#include "scenes.h"
#include "scene_serial.h"
#include <furi_hal_cortex.h>


void fcom_serial_scene_on_enter(void* context) {
    FURI_LOG_I(TAG, "fcom_read_scene_on_enter");
    App* app = context;
    // initialize dcomm
    // start dcomm thread in read mode


    view_dispatcher_switch_to_view(app->view_dispatcher, FcomSerialView);
}

bool fcom_serial_scene_on_event(void* context, SceneManagerEvent event) {
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

void fcom_serial_scene_on_exit(void* context) {
    FURI_LOG_I(TAG, "fcom_read_scene_on_exit");
    UNUSED(context);
    App* app = context;
    UNUSED(app);
    // shut down dcomm
    // clean up
}


