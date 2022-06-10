#include "../infrared_i.h"

void infrared_scene_remote_list_on_enter(void* context) {
    Infrared* infrared = context;

    if(!infrared_select_remote_file(infrared)) {
        scene_manager_previous_scene(infrared->scene_manager);
    } else {
        scene_manager_next_scene(infrared->scene_manager, InfraredSceneRemote);
    }
}

bool infrared_scene_remote_list_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    bool consumed = false;

    return consumed;
}

void infrared_scene_remote_list_on_exit(void* context) {
    UNUSED(context);
}
