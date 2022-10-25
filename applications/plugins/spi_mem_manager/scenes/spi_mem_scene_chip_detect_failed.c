#include "../spi_mem_app.h"

static void spi_mem_scene_chip_detect_failed_widget_callback(
    GuiButtonType result,
    InputType type,
    void* context) {
    SPIMemApp* app = context;
    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(app->view_dispatcher, result);
    }
}

void spi_mem_scene_chip_detect_failed_on_enter(void* context) {
    SPIMemApp* app = context;
    widget_add_button_element(
        app->widget,
        GuiButtonTypeCenter,
        "Retry",
        spi_mem_scene_chip_detect_failed_widget_callback,
        app);
    widget_add_string_element(
        app->widget, 64, 15, AlignCenter, AlignBottom, FontPrimary, "Detected");
    widget_add_string_element(
        app->widget, 64, 27, AlignCenter, AlignBottom, FontPrimary, "unknown SPI chip");
    view_dispatcher_switch_to_view(app->view_dispatcher, SPIMemViewWidget);
}

bool spi_mem_scene_chip_detect_failed_on_event(void* context, SceneManagerEvent event) {
    SPIMemApp* app = context;
    bool success = false;
    if(event.type == SceneManagerEventTypeBack) {
        success = true;
        scene_manager_search_and_switch_to_previous_scene(app->scene_manager, SPIMemSceneStart);
    } else if(event.type == SceneManagerEventTypeCustom) {
        success = true;
        if(event.event == GuiButtonTypeCenter) {
            scene_manager_previous_scene(app->scene_manager);
        }
    }
    return success;
}
void spi_mem_scene_chip_detect_failed_on_exit(void* context) {
    SPIMemApp* app = context;
    widget_reset(app->widget);
}
