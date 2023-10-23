#include "../infrared_i.h"

static void infrared_scene_edit_move_button_callback(
    uint32_t index_old,
    uint32_t index_new,
    void* context) {
    InfraredRemote* remote = context;
    furi_assert(remote);
    // TODO: Emit an event instead
    infrared_remote_move_signal(remote, index_old, index_new);
}

void infrared_scene_edit_move_on_enter(void* context) {
    Infrared* infrared = context;
    InfraredRemote* remote = infrared->remote;

    for(size_t i = 0; i < infrared_remote_get_signal_count(remote); ++i) {
        infrared_move_view_add_item(
            infrared->move_view, infrared_remote_get_signal_name(remote, i));
    }

    infrared_move_view_set_callback(
        infrared->move_view, infrared_scene_edit_move_button_callback, remote);

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
    Infrared* infrared = context;
    infrared_move_view_reset(infrared->move_view);
}
