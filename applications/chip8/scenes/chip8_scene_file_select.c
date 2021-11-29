#include "../chip8_app_i.h"
#include "furi-hal-power.h"

static bool chip8_file_select(Chip8App* chip8) {
    furi_assert(chip8);

    bool res = dialog_file_select_show(
        chip8->dialogs,
        CHIP8_APP_PATH_FOLDER,
        CHIP8_APP_EXTENSION,
        chip8->file_name,
        sizeof(chip8->file_name),
        NULL);
    return res;
}

void chip8_scene_file_select_on_enter(void* context) {
    Chip8App* chip8 = context;

    if(chip8_file_select(chip8)) {
        scene_manager_next_scene(chip8->scene_manager, Chip8WorkView);
    } else {
        view_dispatcher_stop(chip8->view_dispatcher);
    }
}

bool chip8_scene_file_select_on_event(void* context, SceneManagerEvent event) {
    return false;
}

void chip8_scene_file_select_on_exit(void* context) {
}
