#include "../uhf_app_i.h"

enum SubmenuIndex {
    SubmenuIndexSave,
    SubmenuIndexChangeKey,
};

void uhf_scene_card_menu_submenu_callback(void* context, uint32_t index) {
    UHFApp* uhf_app = context;
    view_dispatcher_send_custom_event(uhf_app->view_dispatcher, index);
}

void uhf_scene_card_menu_on_enter(void* context) {
    UHFApp* uhf_app = context;

    Submenu* submenu = uhf_app->submenu;

    submenu_add_item(
        submenu, "Save", SubmenuIndexSave, uhf_scene_card_menu_submenu_callback, uhf_app);
    // if(picopass->dev->dev_data.pacs.record.valid) {
    //     submenu_add_item(
    //         submenu,
    //         "Save as LF",
    //         SubmenuIndexSaveAsLF,
    //         picopass_scene_card_menu_submenu_callback,
    //         picopass);
    // }
    submenu_add_item(
        submenu,
        "Change Key",
        SubmenuIndexChangeKey,
        uhf_scene_card_menu_submenu_callback,
        uhf_app);

    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(uhf_app->scene_manager, UHFSceneCardMenu));

    view_dispatcher_switch_to_view(uhf_app->view_dispatcher, UHFViewMenu);
}

bool uhf_scene_card_menu_on_event(void* context, SceneManagerEvent event) {
    UHFApp* uhf_app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        FURI_LOG_E("LOG", "%lu", event.event);
        if(event.event == SubmenuIndexSave) {
            scene_manager_set_scene_state(
                uhf_app->scene_manager, UHFSceneCardMenu, SubmenuIndexSave);
            scene_manager_next_scene(uhf_app->scene_manager, UHFSceneSaveName);
            consumed = true;
        }
        // else if(event.event == SubmenuIndexChangeKey) {
        //     scene_manager_set_scene_state(
        //         picopass->scene_manager, UHFSceneCardMenu, SubmenuIndexChangeKey);
        //     scene_manager_next_scene(picopass->scene_manager, PicopassSceneKeyMenu);
        //     consumed = true;
        // }
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = scene_manager_search_and_switch_to_previous_scene(
            uhf_app->scene_manager, UHFSceneStart);
    }

    return consumed;
}

void uhf_scene_card_menu_on_exit(void* context) {
    UHFApp* uhf_app = context;

    submenu_reset(uhf_app->submenu);
}
