#include "../spi_mem_app.h"

static void spi_mem_scene_delete_success_popup_callback(void* context) {
    SPIMemApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, SPIMemCustomEventPopupBack);
}

void spi_mem_scene_delete_success_on_enter(void* context) {
    SPIMemApp* app = context;
    popup_set_icon(app->popup, 0, 2, &I_DolphinMafia_115x62);
    popup_set_header(app->popup, "Deleted", 83, 19, AlignLeft, AlignBottom);
    popup_set_callback(app->popup, spi_mem_scene_delete_success_popup_callback);
    popup_set_context(app->popup, app);
    popup_set_timeout(app->popup, 1500);
    popup_enable_timeout(app->popup);
    view_dispatcher_switch_to_view(app->view_dispatcher, SPIMemViewPopup);
}

bool spi_mem_scene_delete_success_on_event(void* context, SceneManagerEvent event) {
    SPIMemApp* app = context;
    bool success = false;
    if(event.type == SceneManagerEventTypeCustom) {
        success = true;
        if(event.event == SPIMemCustomEventPopupBack) {
            scene_manager_search_and_switch_to_previous_scene(
                app->scene_manager, SPIMemSceneSelectFile);
        }
    }
    return success;
}

void spi_mem_scene_delete_success_on_exit(void* context) {
    SPIMemApp* app = context;
    popup_reset(app->popup);
}
