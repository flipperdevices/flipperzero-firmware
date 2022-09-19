#include "../dtmf_dolphin_i.h"

static void dtmf_dolphin_scene_start_main_menu_enter_callback(void* context, uint32_t index) {
    DTMFDolphinApp* app = context;
    if (index == DTMFDolphinSceneStateDialer) {
        view_dispatcher_send_custom_event(
            app->view_dispatcher,
            DTMFDolphinEventStartDialer
        );
    } else if (index == DTMFDolphinSceneStateBluebox) {
        view_dispatcher_send_custom_event(
            app->view_dispatcher,
            DTMFDolphinEventStartBluebox
        );
    }
}

void dtmf_dolphin_scene_start_on_enter(void* context) {
    DTMFDolphinApp* app = context;
    VariableItemList* var_item_list = app->main_menu_list;

    // VariableItem* item;
    variable_item_list_set_enter_callback(
        var_item_list,
        dtmf_dolphin_scene_start_main_menu_enter_callback,
        app);

    variable_item_list_add(var_item_list, "Dialer", 0, NULL, NULL);
    variable_item_list_add(var_item_list, "Bluebox", 0, NULL, NULL);
    variable_item_list_add(var_item_list, "Misc", 0, NULL, NULL);

    variable_item_list_set_selected_item(
        var_item_list,
        scene_manager_get_scene_state(app->scene_manager, DTMFDolphinSceneStart));

    view_dispatcher_switch_to_view(
        app->view_dispatcher,
        DTMFDolphinViewMainMenu);
}

bool dtmf_dolphin_scene_start_on_event(void* context, SceneManagerEvent event) {
    DTMFDolphinApp* app = context;
    UNUSED(app);
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if (event.event == DTMFDolphinEventStartDialer) {
            scene_manager_set_scene_state(app->scene_manager, DTMFDolphinSceneDialer, DTMFDolphinSceneStateDialer);
            scene_manager_next_scene(app->scene_manager, DTMFDolphinSceneDialer);
        } else if (event.event == DTMFDolphinEventStartBluebox) {
            scene_manager_set_scene_state(app->scene_manager, DTMFDolphinSceneDialer, DTMFDolphinSceneStateBluebox);
            scene_manager_next_scene(app->scene_manager, DTMFDolphinSceneDialer);
        } else if (event.event == DTMFDolphinEventStartMisc) {
            scene_manager_set_scene_state(app->scene_manager, DTMFDolphinSceneDialer, DTMFDolphinSceneStateMisc);
            scene_manager_next_scene(app->scene_manager, DTMFDolphinSceneDialer);
        }
        consumed = true;
    }
    return consumed;
}

void dtmf_dolphin_scene_start_on_exit(void* context) {
    DTMFDolphinApp* app = context;
    variable_item_list_reset(app->main_menu_list);
}
