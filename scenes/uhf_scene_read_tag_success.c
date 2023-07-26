#include "../uhf_app_i.h"

void uhf_read_tag_success_worker_callback(UHFWorkerEvent event, void* ctx) {
    UNUSED(event);
    UNUSED(ctx);
}

void uhf_scene_read_tag_success_on_enter(void* ctx) {
    // UNUSED(ctx);
    UHFApp* uhf_app = ctx;

    widget_add_string_element(
        uhf_app->widget,
        20,
        20,
        AlignCenter,
        AlignCenter,
        FontPrimary,
        convertToHexString(uhf_app->worker->data->data->data, uhf_app->worker->data->data->length));

    view_dispatcher_switch_to_view(uhf_app->view_dispatcher, UHFViewWidget);
}

bool uhf_scene_read_tag_success_on_event(void* ctx, SceneManagerEvent event) {
    UHFApp* uhf_app = ctx;
    bool consumed = false;
    // FURI_LOG_E("33", "uhf_scene_read_tag_on_event was called! event.event: %d", event.type);
    // FURI_LOG_E("33", "uhf_scene_read_tag_on_event was called! event.event: %lu", event.event);
    if(event.event == SceneManagerEventTypeBack) {
        // FURI_LOG_E("36", "Back button was pressed");
        uhf_app->worker->state = UHFWorkerStateStop;
    }
    if(event.type == SceneManagerEventTypeCustom) {
        // FURI_LOG_E("36", "SceneManagerEventTypeCustom");
        if(event.event == UHFCustomEventWorkerExit) {
            // FURI_LOG_E("38", "UHFCustomEventWorkerExit");
            // if(memcmp(uhf_app->dev->dev_data.pacs.key, uhf_factory_debit_key, PICOPASS_BLOCK_LEN) ==
            //    0) {
            //     scene_manager_next_scene(uhf_app->scene_manager, PicopassSceneReadFactorySuccess);
            // } else {
            //     scene_manager_next_scene(uhf_app->scene_manager, PicopassSceneReadCardSuccess);
            // }
            // scene_manager_next_scene(uhf_app->scene_manager, UHFSceneStart);
            consumed = true;
        }
    }
    return consumed;
}

void uhf_scene_read_tag_success_on_exit(void* ctx) {
    UHFApp* uhf_app = ctx;

    // // Stop worker
    // uhf_worker_stop(uhf_app->worker);
    // Clear view
    popup_reset(uhf_app->popup);
    // clear widget
    widget_reset(uhf_app->widget);

    // uhf_blink_stop(uhf_app);
}