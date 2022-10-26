#include "../spi_mem_app_i.h"

void spi_mem_scene_read_view_result_callback(void* context) {
    SPIMemApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, SPIMemCustomEventViewReadCancel);
}

static void spi_mem_scene_read_callback(void* context, SPIMemCustomEventWorker event) {
    SPIMemApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

void spi_mem_scene_read_on_enter(void* context) {
    SPIMemApp* app = context;
    spi_mem_view_read_set_callback(app->view_read, spi_mem_scene_read_view_result_callback, app);
    notification_message(app->notifications, &sequence_blink_start_cyan);
    spi_mem_view_read_set_chip_size(app->view_read, 4096);
    spi_mem_view_read_set_block_size(app->view_read, 256);
    view_dispatcher_switch_to_view(app->view_dispatcher, SPIMemViewRead);
    spi_mem_worker_start_thread(app->worker);
    spi_mem_worker_read_start(
        app->chip_info, app->worker, spi_mem_scene_read_callback, app->storage, app);
}

bool spi_mem_scene_read_on_event(void* context, SceneManagerEvent event) {
    SPIMemApp* app = context;
    UNUSED(app);
    bool success = false;
    if(event.type == SceneManagerEventTypeBack) {
        success = true;
    } else if(event.type == SceneManagerEventTypeCustom) {
        success = true;
        if(event.event == SPIMemCustomEventViewReadCancel) {
            scene_manager_search_and_switch_to_previous_scene(
                app->scene_manager, SPIMemSceneChipDetected);
        } else if(event.event == SPIMemCustomEventWorkerBlockReaded) {
            spi_mem_view_read_inc_progress(app->view_read);
        } else if(event.event == SPIMemCustomEventWorkerReadDone) {
            scene_manager_search_and_switch_to_previous_scene(
                app->scene_manager, SPIMemSceneChipDetected);
        }
    }
    return success;
}
void spi_mem_scene_read_on_exit(void* context) {
    SPIMemApp* app = context;
    spi_mem_worker_stop_thread(app->worker);
    // spi_mem_view_read_reset(app->view_read);
    notification_message(app->notifications, &sequence_blink_stop);
}
