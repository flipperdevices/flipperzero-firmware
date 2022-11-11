#include "../spi_mem_app_i.h"

static void
    spi_mem_scene_chip_erase_widget_callback(GuiButtonType result, InputType type, void* context) {
    SPIMemApp* app = context;
    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(app->view_dispatcher, result);
    }
}

static void spi_mem_scene_chip_erase_callback(void* context, SPIMemCustomEventWorker event) {
    SPIMemApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

void spi_mem_scene_chip_erase_on_enter(void* context) {
    SPIMemApp* app = context;
    widget_add_button_element(
        app->widget, GuiButtonTypeLeft, "Cancel", spi_mem_scene_chip_erase_widget_callback, app);
    widget_add_string_element(
        app->widget, 64, 15, AlignCenter, AlignBottom, FontPrimary, "Erasing SPI chip");
    widget_add_string_element(
        app->widget, 64, 27, AlignCenter, AlignBottom, FontSecondary, "Please be patient");
    notification_message(app->notifications, &sequence_blink_start_magenta);
    view_dispatcher_switch_to_view(app->view_dispatcher, SPIMemViewWidget);
    spi_mem_worker_start_thread(app->worker);
    spi_mem_worker_chip_erase_start(
        app->chip_info, app->worker, spi_mem_scene_chip_erase_callback, app);
}

bool spi_mem_scene_chip_erase_on_event(void* context, SceneManagerEvent event) {
    SPIMemApp* app = context;
    bool success = false;
    if(event.type == SceneManagerEventTypeBack) {
        success = true;
        scene_manager_search_and_switch_to_previous_scene(
            app->scene_manager, SPIMemSceneSavedFileMenu);
    } else if(event.type == SceneManagerEventTypeCustom) {
        success = true;
        if(event.event == GuiButtonTypeLeft) {
            scene_manager_previous_scene(app->scene_manager);
        }
    }
    return success;
}
void spi_mem_scene_chip_erase_on_exit(void* context) {
    SPIMemApp* app = context;
    spi_mem_worker_stop_thread(app->worker);
    notification_message(app->notifications, &sequence_blink_stop);
    widget_reset(app->widget);
}
