#include "../spi_mem_app.h"

void spi_mem_scene_read_view_result_callback(void* context) {
    SPIMemApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, SPIMemCustomEventViewReadCancel);
}

void spi_mem_scene_read_on_enter(void* context) {
    SPIMemApp* app = context;
    spi_mem_view_read_set_callback(app->view_read, spi_mem_scene_read_view_result_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, SPIMemViewRead);
}

bool spi_mem_scene_read_on_event(void* context, SceneManagerEvent event) {
    SPIMemApp* app = context;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeBack) {
        consumed = true;
    } else if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        if(event.event == SPIMemCustomEventViewReadCancel) {
            scene_manager_search_and_switch_to_previous_scene(
                app->scene_manager, SPIMemSceneChipDetected);
        }
    }
    return consumed;
}
void spi_mem_scene_read_on_exit(void* context) {
    UNUSED(context);
}
