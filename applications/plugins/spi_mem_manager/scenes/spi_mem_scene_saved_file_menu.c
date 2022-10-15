#include "../spi_mem_app.h"

typedef enum {
    SPIMemSceneSavedFileSubmenuIndexWrite,
    SPIMemSceneSavedFileSubmenuIndexDelete,
    SPIMemSceneSavedFileSubmenuIndexInfo,
} SPIMemSceneSavedFileSubmenuIndex;

static void spi_mem_scene_saved_file_submenu_callback(void* context, uint32_t index) {
    SPIMemApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void spi_mem_scene_saved_file_menu_on_enter(void* context) {
    SPIMemApp* app = context;
    submenu_add_item(
        app->submenu,
        "Write",
        SPIMemSceneSavedFileSubmenuIndexWrite,
        spi_mem_scene_saved_file_submenu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "Delete",
        SPIMemSceneSavedFileSubmenuIndexDelete,
        spi_mem_scene_saved_file_submenu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "Info",
        SPIMemSceneSavedFileSubmenuIndexInfo,
        spi_mem_scene_saved_file_submenu_callback,
        app);
    submenu_set_selected_item(
        app->submenu, scene_manager_get_scene_state(app->scene_manager, SPIMemSceneSavedFileMenu));
    view_dispatcher_switch_to_view(app->view_dispatcher, SPIMemViewSubmenu);
}

bool spi_mem_scene_saved_file_menu_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void spi_mem_scene_saved_file_menu_on_exit(void* context) {
    SPIMemApp* app = context;
    submenu_reset(app->submenu);
}
