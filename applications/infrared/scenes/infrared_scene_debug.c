#include "../infrared_i.h"

void infrared_scene_debug_on_enter(void* context) {
    Infrared* infrared = context;
    view_dispatcher_switch_to_view(infrared->view_dispatcher, InfraredViewDebugView);
}

bool infrared_scene_debug_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    bool consumed = false;

    return consumed;
}

void infrared_scene_debug_on_exit(void* context) {
    UNUSED(context);
}
