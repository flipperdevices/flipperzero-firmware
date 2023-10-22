#include "../infrared_i.h"

static void infrared_scene_edit_move_button_callback(
    uint32_t index_old,
    uint32_t index_new,
    void* context) {
    InfraredRemote* remote = context;
    furi_assert(remote);
    infrared_remote_move_signal(remote, index_old, index_new);
}

static const char* infrared_scene_edit_move_get_name_callback(uint32_t index, void* context) {
    InfraredRemote* remote = context;
    furi_assert(remote);
    return infrared_remote_get_signal_name(remote, index);
}

void infrared_scene_edit_move_on_enter(void* context) {
    Infrared* infrared = context;
    InfraredRemote* remote = infrared->remote;

    infrared_move_view_set_callback(infrared->move_view, infrared_scene_edit_move_button_callback);

    uint32_t btn_count = infrared_remote_get_signal_count(remote);
    infrared_move_view_list_init(
        infrared->move_view, btn_count, infrared_scene_edit_move_get_name_callback, remote);
    infrared_move_view_list_update(infrared->move_view);

    view_dispatcher_switch_to_view(infrared->view_dispatcher, InfraredViewMove);
}

bool infrared_scene_edit_move_on_event(void* context, SceneManagerEvent event) {
    Infrared* infrared = context;
    bool consumed = false;

    UNUSED(event);
    UNUSED(infrared);

    return consumed;
}

void infrared_scene_edit_move_on_exit(void* context) {
    UNUSED(context);
}
