#include "../dtmf_dolphin_i.h"

void dtmf_dolphin_scene_dialer_on_enter(void *context) {
    DTMFDolphinApp* app = context;

    view_dispatcher_switch_to_view(app->view_dispatcher, DTMFDolphinViewDialer);
}

bool dtmf_dolphin_scene_dialer_on_event(void* context, SceneManagerEvent event) {
    DTMFDolphinApp* app = context;
    UNUSED(app);
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
    }

    return consumed;
    return false;
}

void dtmf_dolphin_scene_dialer_on_exit(void* context) {
    UNUSED(context);
}
