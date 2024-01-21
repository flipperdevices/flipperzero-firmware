#include "flipper.h"
#include "app_state.h"
#include "scenes.h"
#include "scene_select_code.h"
#include <furi_hal_cortex.h>


static void file_browser_callback(void* context) {
    App* app = context;
    furi_assert(app);
    scene_manager_next_scene(app->scene_manager, FcomSendCodeScene);
    //view_dispatcher_send_custom_event(app->view_dispatcher, SceneManagerEventTypeCustom);
}

void fcom_select_code_scene_on_enter(void* context) {
    FURI_LOG_I(TAG, "fcom_read_scene_on_enter");
    App* app = context;

    file_browser_set_callback(app->file_browser, file_browser_callback, app);

    file_browser_start(app->file_browser, app->file_path);

    view_dispatcher_switch_to_view(app->view_dispatcher, FcomFileSelectView);
}

bool fcom_select_code_scene_on_event(void* context, SceneManagerEvent event) {
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

void fcom_select_code_scene_on_exit(void* context) {
    FURI_LOG_I(TAG, "fcom_read_scene_on_exit");
    UNUSED(context);
    App* app = context;
    UNUSED(app);

    file_browser_stop(app->file_browser);
    // shut down dcomm
    // clean up
}


