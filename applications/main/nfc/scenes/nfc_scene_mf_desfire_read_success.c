#include "../nfc_app_i.h"

void nfc_scene_mf_desfire_read_success_on_enter(void* context) {
    UNUSED(context);
}

bool nfc_scene_mf_desfire_read_success_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);

    bool consumed = false;
    return consumed;
}

void nfc_scene_mf_desfire_read_success_on_exit(void* context) {
    UNUSED(context);
}
