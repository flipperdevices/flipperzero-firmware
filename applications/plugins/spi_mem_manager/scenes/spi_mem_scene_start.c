#include "../spi_mem_app.h"

typedef enum {
    SPIMemSceneStartSubmenuIndexRead,
    SPIMemSceneStartSubmenuIndexSaved,
    SPIMemSceneStartSubmenuIndexChipInfo
} SPIMemSceneStartSubmenuIndex;

static void spi_mem_scene_start_submenu_callback(void* context, uint32_t index) {
    SPIMemApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void spi_mem_scene_start_on_enter(void* context) {
    SPIMemApp* app = context;
    submenu_add_item(
        app->submenu,
        "Read",
        SPIMemSceneStartSubmenuIndexRead,
        spi_mem_scene_start_submenu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "Saved",
        SPIMemSceneStartSubmenuIndexSaved,
        spi_mem_scene_start_submenu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "Chip info",
        SPIMemSceneStartSubmenuIndexChipInfo,
        spi_mem_scene_start_submenu_callback,
        app);
    submenu_set_selected_item(
        app->submenu, scene_manager_get_scene_state(app->scene_manager, SPIMemSceneStart));
    view_dispatcher_switch_to_view(app->view_dispatcher, SPIMemViewSubmenu);
}

bool spi_mem_scene_start_on_event(void* context, SceneManagerEvent event) {
    SPIMemApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SPIMemSceneStartSubmenuIndexRead) {
            scene_manager_next_scene(app->scene_manager, SPIMemSceneChipDetect);
            consumed = true;
        } else if(event.event == SPIMemSceneStartSubmenuIndexSaved) {
            consumed = true;
        } else if(event.event == SPIMemSceneStartSubmenuIndexChipInfo) {
            consumed = true;
        }
        scene_manager_set_scene_state(app->scene_manager, SPIMemSceneStart, event.event);
    }

    return consumed;
}

void spi_mem_scene_start_on_exit(void* context) {
    SPIMemApp* app = context;
    submenu_reset(app->submenu);
}
