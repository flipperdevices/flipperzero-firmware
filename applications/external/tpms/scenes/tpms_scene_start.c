#include "../tpms_app_i.h"

typedef enum {
    SubmenuIndexTPMSReceiver,
    SubmenuIndexTPMSRelearn,
    SubmenuIndexTPMSAbout,
} SubmenuIndex;

void tpms_scene_start_submenu_callback(void* context, uint32_t index) {
    TPMSApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void tpms_scene_start_on_enter(void* context) {
    UNUSED(context);
    TPMSApp* app = context;
    Submenu* submenu = app->submenu;

    submenu_add_item(
        submenu, "Read TPMS", SubmenuIndexTPMSReceiver, tpms_scene_start_submenu_callback, app);
    submenu_add_item(
        submenu, "Relearn", SubmenuIndexTPMSRelearn, tpms_scene_start_submenu_callback, app);
    // Help
    submenu_add_item(
        submenu, "About", SubmenuIndexTPMSAbout, tpms_scene_start_submenu_callback, app);

    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(app->scene_manager, TPMSSceneStart));

    view_dispatcher_switch_to_view(app->view_dispatcher, TPMSViewSubmenu);
}

bool tpms_scene_start_on_event(void* context, SceneManagerEvent event) {
    TPMSApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeBack) {
        // Exit application.
        scene_manager_stop(app->scene_manager);
        view_dispatcher_stop(app->view_dispatcher);
        consumed = true;
    } else if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexTPMSAbout) {
            scene_manager_next_scene(app->scene_manager, TPMSSceneAbout);
            consumed = true;
        } else if(event.event == SubmenuIndexTPMSReceiver) {
            scene_manager_next_scene(app->scene_manager, TPMSSceneReceiver);
            consumed = true;
        } else if(event.event == SubmenuIndexTPMSRelearn) {
            scene_manager_next_scene(app->scene_manager, TPMSSceneRelearn);
            consumed = true;
        }
        scene_manager_set_scene_state(app->scene_manager, TPMSSceneStart, event.event);
    }

    return consumed;
}

void tpms_scene_start_on_exit(void* context) {
    TPMSApp* app = context;
    submenu_reset(app->submenu);
}
