#include "../can_lin_tools_app_i.h"

typedef enum {
    SubmenuIndexCanLinToolsCanHacker20,
    SubmenuIndexCanLinToolsLinHacker,
    SubmenuIndexCanLinToolsCarBusAnalyzer,
    SubmenuIndexCanLinToolsAbout,
} SubmenuIndex;

void can_lin_tools_scene_start_submenu_callback(void* context, uint32_t index) {
    CanLinToolsApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void can_lin_tools_scene_start_on_enter(void* context) {
    UNUSED(context);
    CanLinToolsApp* app = context;
    Submenu* submenu = app->submenu;

    submenu_add_item(
        submenu,
        "CAN Hacker 2.0",
        SubmenuIndexCanLinToolsCanHacker20,
        can_lin_tools_scene_start_submenu_callback,
        app);
    submenu_add_item(
        submenu,
        "LIN Hacker",
        SubmenuIndexCanLinToolsLinHacker,
        can_lin_tools_scene_start_submenu_callback,
        app);
    submenu_add_item(
        submenu,
        "CarBus Analyzer",
        SubmenuIndexCanLinToolsCarBusAnalyzer,
        can_lin_tools_scene_start_submenu_callback,
        app);
    submenu_add_item(
        submenu,
        "About",
        SubmenuIndexCanLinToolsAbout,
        can_lin_tools_scene_start_submenu_callback,
        app);

    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(app->scene_manager, CanLinToolsSceneStart));

    view_dispatcher_switch_to_view(app->view_dispatcher, CanLinToolsViewSubmenu);
}

bool can_lin_tools_scene_start_on_event(void* context, SceneManagerEvent event) {
    CanLinToolsApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexCanLinToolsAbout) {
            scene_manager_next_scene(app->scene_manager, CanLinToolsSceneAbout);
            consumed = true;
        } else if(event.event == SubmenuIndexCanLinToolsCanHacker20) {
            scene_manager_next_scene(app->scene_manager, CanLinToolsSceneCanHacker2);
            consumed = true;
        } else if(event.event == SubmenuIndexCanLinToolsLinHacker) {
            scene_manager_next_scene(app->scene_manager, CanLinToolsSceneLinHacker);
            consumed = true;
        } else if(event.event == SubmenuIndexCanLinToolsCarBusAnalyzer) {
            scene_manager_next_scene(app->scene_manager, CanLinToolsSceneCarBusAnalyzer);
            consumed = true;
        }
        scene_manager_set_scene_state(app->scene_manager, CanLinToolsSceneStart, event.event);
    }

    return consumed;
}

void can_lin_tools_scene_start_on_exit(void* context) {
    CanLinToolsApp* app = context;
    submenu_reset(app->submenu);
}
