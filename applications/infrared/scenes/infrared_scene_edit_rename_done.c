#include "../infrared_i.h"

void infrared_scene_edit_rename_done_on_enter(void* context) {
    Infrared* infrared = context;
    UNUSED(infrared);
}

bool infrared_scene_edit_rename_done_on_event(void* context, SceneManagerEvent event) {
    Infrared* infrared = context;
    UNUSED(infrared);
    UNUSED(event);
    bool consumed = false;

    return consumed;
}

void infrared_scene_edit_rename_done_on_exit(void* context) {
    Infrared* infrared = context;
    UNUSED(infrared);
}
