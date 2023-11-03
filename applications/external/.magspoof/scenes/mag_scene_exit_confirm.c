#include "../mag_i.h"

void mag_scene_exit_confirm_on_enter(void* context) {
    Mag* mag = context;
    UNUSED(mag);
}

bool mag_scene_exit_confirm_on_event(void* context, SceneManagerEvent event) {
    Mag* mag = context;
    UNUSED(mag);
    UNUSED(event);
    bool consumed = false;

    return consumed;
}

void mag_scene_exit_confirm_on_exit(void* context) {
    Mag* mag = context;
    UNUSED(mag);
}
