#include "../uhf_app_i.h"
#include <dolphin/dolphin.h>

void uhf_read_tag_worker_callback(UHFWorkerEvent event, void* ctx) {
    UNUSED(event);
    UHFApp* uhf_app = ctx;
    view_dispatcher_send_custom_event(uhf_app->view_dispatcher, UHFCustomEventWorkerExit);
}

void uhf_scene_read_tag_on_enter(void* ctx) {
    // FURI_LOG_E("33", "uhf_scene_read_tag_on_enter was called!");
    UHFApp* uhf_app = ctx;
    dolphin_deed(DolphinDeedNfcRead);

    // Setup view
    Popup* popup = uhf_app->popup;
    popup_set_header(popup, "Detecting\n[UHF] RFID\nTag", 68, 30, AlignLeft, AlignTop);
    popup_set_icon(popup, 0, 3, &I_RFIDDolphinReceive_97x61);

    // Start worker
    view_dispatcher_switch_to_view(uhf_app->view_dispatcher, UHFViewPopup);
    uhf_worker_start(uhf_app->worker, UHFWorkerStateDetect, uhf_read_tag_worker_callback, uhf_app);

    uhf_blink_start(uhf_app);

    // furi_delay_ms(2000);
    // view_dispatcher_send_custom_event(uhf_app->view_dispatcher, UHFCustomEventWorkerExit);
}

bool uhf_scene_read_tag_on_event(void* ctx, SceneManagerEvent event) {
    UHFApp* uhf_app = ctx;
    bool consumed = false;
    uint8_t cmd[] = {0xBB, 0x00, 0x22, 0x00, 0x00, 0x22, 0x7E};
    // while(uhf_worker->state != UHFWorkerStateStop || !first_data->data_full) {
    FURI_LOG_E("uhf_scene_read_tag_on_event", "sending single polling command");
    furi_hal_uart_tx(FuriHalUartIdUSART1, cmd, 7);
    if(uhf_app->worker->data->data->data_full) {
        scene_manager_next_scene(uhf_app->scene_manager, UHFSceneReadTagSuccess);
        consumed = true;
    }
    // FURI_LOG_E("33", "uhf_scene_read_tag_on_event was called! event.event: %lu", event.event);
    if(event.type == SceneManagerEventTypeCustom) {
        // FURI_LOG_E("36", "SceneManagerEventTypeCustom");
        if(event.event == UHFCustomEventWorkerExit) {
            uhf_app->worker->state = UHFWorkerStateStop;
            // scene_manager_next_scene(uhf_app->scene_manager, UHFSceneReadTagSuccess);
            consumed = true;
        }
    }
    return consumed;
}

void uhf_scene_read_tag_on_exit(void* ctx) {
    UHFApp* uhf_app = ctx;
    // Stop worker
    uhf_worker_stop(uhf_app->worker);
    // Clear view
    popup_reset(uhf_app->popup);

    uhf_blink_stop(uhf_app);
}
