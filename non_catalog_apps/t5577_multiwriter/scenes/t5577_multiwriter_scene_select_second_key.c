#include "../t5577_multiwriter_i.h"

void t5577_multiwriter_scene_select_second_key_on_enter(void* context) {
    LfRfid* app = context;

    if(t5577_multiwriter_load_key_from_file_select(app)) {
        scene_manager_next_scene(app->scene_manager, LfRfidSceneWriteSecondKey);
    } else {
        scene_manager_previous_scene(app->scene_manager);
    }
}

bool t5577_multiwriter_scene_select_second_key_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    bool consumed = false;
    return consumed;
}

void t5577_multiwriter_scene_select_second_key_on_exit(void* context) {
    UNUSED(context);
}