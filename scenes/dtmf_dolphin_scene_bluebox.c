#include "../dtmf_dolphin_i.h"

void dtmf_dolphin_scene_bluebox_on_enter(void *context) {
    DTMFDolphinApp* app = context;
    view_dispatcher_switch_to_view(app->view_dispatcher, DTMFDolphinViewBluebox);
}

bool dtmf_dolphin_scene_bluebox_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void dtmf_dolphin_scene_bluebox_on_exit(void* context) {
    UNUSED(context);
}
