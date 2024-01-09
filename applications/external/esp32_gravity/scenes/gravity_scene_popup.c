#include "../gravity_app_i.h"

uint32_t timeout = 2000;

/* Popup contents must be set prior to activating the scene */
void gravity_scene_popup_on_enter(void* context) {
    GravityApp* app = context;
    popup_set_context(app->popup, app);
    popup_set_timeout(app->popup, timeout);
    popup_enable_timeout(app->popup);
    view_dispatcher_switch_to_view(app->view_dispatcher, Gravity_AppViewPopup);
}

bool gravity_scene_popup_on_event(void* context, SceneManagerEvent event) {
    return false;
}

void gravity_scene_popup_on_exit(void* context) {
    GravityApp* app = context;
    popup_reset(app->popup);
}