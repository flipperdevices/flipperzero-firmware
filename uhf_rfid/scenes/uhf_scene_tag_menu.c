#include "../uhf_app_i.h"

enum SubmenuIndex {
    SubmenuIndexSave,
    SubmenuIndexChangeKey,
};

void uhf_scene_tag_menu_submenu_callback(void* ctx, uint32_t index) {
    UHFApp* uhf_app = ctx;
    view_dispatcher_send_custom_event(uhf_app->view_dispatcher, index);
}

void uhf_scene_tag_menu_on_enter(void* ctx) {
    UHFApp* uhf_app = ctx;

    Submenu* submenu = uhf_app->submenu;

    submenu_add_item(
        submenu, "Save", SubmenuIndexSave, uhf_scene_tag_menu_submenu_callback, uhf_app);
    submenu_add_item(
        submenu, "Change Key", SubmenuIndexChangeKey, uhf_scene_tag_menu_submenu_callback, uhf_app);

    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(uhf_app->scene_manager, UHFSceneTagMenu));

    view_dispatcher_switch_to_view(uhf_app->view_dispatcher, UHFViewMenu);
}

bool uhf_scene_tag_menu_on_event(void* ctx, SceneManagerEvent event) {
    UHFApp* uhf_app = ctx;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexSave) {
            scene_manager_set_scene_state(
                uhf_app->scene_manager, UHFSceneTagMenu, SubmenuIndexSave);
            scene_manager_next_scene(uhf_app->scene_manager, UHFSceneSaveName);
            consumed = true;
        }
        // else if(event.event == SubmenuIndexChangeKey) {
        //     scene_manager_set_scene_state(
        //         picopass->scene_manager, UHFSceneTagMenu, SubmenuIndexChangeKey);
        //     scene_manager_next_scene(picopass->scene_manager, PicopassSceneKeyMenu);
        //     consumed = true;
        // }
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = scene_manager_search_and_switch_to_previous_scene(
            uhf_app->scene_manager, UHFSceneStart);
    }

    return consumed;
}

void uhf_scene_tag_menu_on_exit(void* ctx) {
    UHFApp* uhf_app = ctx;

    submenu_reset(uhf_app->submenu);
}
