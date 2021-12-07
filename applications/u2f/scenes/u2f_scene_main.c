#include "../u2f_app_i.h"
#include "../views/u2f_view.h"
#include "furi-hal.h"

void u2f_scene_main_ok_callback(InputType type, void* context) {
    furi_assert(context);
    U2FApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, type);
}

bool u2f_scene_main_on_event(void* context, SceneManagerEvent event) {
    //U2FApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
    } else if(event.type == SceneManagerEventTypeTick) {
        //u2f_set_state(app->u2f_view, u2f_script_get_state(app->u2f_script));
    }
    return consumed;
}

void u2f_scene_main_on_enter(void* context) {
    U2FApp* app = context;

    //u2f_set_state(app->u2f_view, u2f_script_get_state(app->u2f_script));
    app->u2f_hid = u2f_hid_start();

    u2f_set_ok_callback(app->u2f_view, u2f_scene_main_ok_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, U2FAppViewMain);
}

void u2f_scene_main_on_exit(void* context) {
    U2FApp* app = context;
    u2f_hid_stop(app->u2f_hid);
}
