#include "../ttt_multi.h"

enum SubmenuIndex { SubmenuIndexCreate, SubmenuIndexJoin, SubmenuIndexLocal };

void ttt_multi_scene_start_submenu_callback(void* context, uint32_t index) {
    TttMultiApp* ttt_multi = context;
    view_dispatcher_send_custom_event(ttt_multi->view_dispatcher, index);
}

void ttt_multi_scene_start_on_enter(void* context) {
    TttMultiApp* ttt_multi = context;

    Submenu* submenu = ttt_multi->submenu;
    submenu_add_item(
        submenu,
        "Create game",
        SubmenuIndexCreate,
        ttt_multi_scene_start_submenu_callback,
        ttt_multi);
    submenu_add_item(
        submenu, "Join game", SubmenuIndexJoin, ttt_multi_scene_start_submenu_callback, ttt_multi);

    submenu_add_item(
        submenu,
        "Local game",
        SubmenuIndexLocal,
        ttt_multi_scene_start_submenu_callback,
        ttt_multi);

    submenu_set_header(submenu, "Tic-Tac-Toe");

    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(ttt_multi->scene_manager, TttMultiSceneStart));
    view_dispatcher_switch_to_view(ttt_multi->view_dispatcher, TttMultiViewMenu);
}

bool ttt_multi_scene_start_on_event(void* context, SceneManagerEvent event) {
    TttMultiApp* ttt_multi = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexCreate) {
            // TODO: implement
            consumed = true;
        } else if(event.event == SubmenuIndexJoin) {
            // TODO: implement
            consumed = true;
        } else if(event.event == SubmenuIndexLocal) {
            scene_manager_next_scene(ttt_multi->scene_manager, TttMultiSceneLocal);
            consumed = true;
        }
        scene_manager_set_scene_state(ttt_multi->scene_manager, TttMultiSceneStart, event.event);
    }

    return consumed;
}

void ttt_multi_scene_start_on_exit(void* context) {
    TttMultiApp* ttt_multi = context;
    submenu_reset(ttt_multi->submenu);
}
