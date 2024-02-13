#include "app_i.h"

#include <furi.h>

typedef enum {
    SceneStartIndexInstallDefault,
    SceneStartIndexInstallCustom,
} SceneStartIndex;

void scene_start_on_enter(void* context) {
    App* app = context;

    if(!furi_string_empty(app->file_path)) {
        // File path is set, go directly to firmware install
        scene_manager_next_scene(app->scene_manager, SceneInstall);
        return;
    }

    submenu_add_item(
        app->submenu,
        "Install Official Firmware",
        SceneStartIndexInstallDefault,
        submenu_item_common_callback,
        app);
    submenu_add_item(
        app->submenu,
        "Install Firmware from File",
        SceneStartIndexInstallCustom,
        submenu_item_common_callback,
        app);

    view_dispatcher_switch_to_view(app->view_dispatcher, ViewIdSubmenu);
}

bool scene_start_on_event(void* context, SceneManagerEvent event) {
    furi_assert(context);

    App* app = context;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SceneStartIndexInstallDefault) {
            furi_string_set(app->file_path, VGM_DEFAULT_FW_FILE);
            scene_manager_next_scene(app->scene_manager, SceneConfirm);
        } else if(event.event == SceneStartIndexInstallCustom) {
            scene_manager_next_scene(app->scene_manager, SceneFileSelect);
        }

        return true;
    } else if(event.type == SceneManagerEventTypeBack) {
        view_dispatcher_stop(app->view_dispatcher);
        return true;
    }

    return false;
}

void scene_start_on_exit(void* context) {
    App* app = context;
    submenu_reset(app->submenu);
}
