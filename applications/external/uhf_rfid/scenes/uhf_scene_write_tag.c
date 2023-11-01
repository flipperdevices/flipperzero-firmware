#include "../uhf_app_i.h"
#include <dolphin/dolphin.h>

void uhf_write_tag_worker_callback(UHFWorkerEvent event, void* ctx) {
    UHFApp* uhf_app = ctx;
    if(event == UHFWorkerEventSuccess) {
        view_dispatcher_send_custom_event(uhf_app->view_dispatcher, UHFCustomEventWorkerExit);
    }
    // } else if(event == UHFWorkerEventAborted) {
    //     scene_manager_search_and_switch_to_previous_scene(uhf_app->scene_manager, UHFSceneStart);
    // }
}

void uhf_scene_write_tag_on_enter(void* ctx) {
    UHFApp* uhf_app = ctx;
    dolphin_deed(DolphinDeedNfcRead);

    // Setup view
    Popup* popup = uhf_app->popup;
    popup_set_header(popup, "Writing\n[UHF] RFID\nTag", 68, 30, AlignLeft, AlignTop);
    popup_set_icon(popup, 0, 3, &I_RFIDDolphinSend_97x61);

    // Start worker
    view_dispatcher_switch_to_view(uhf_app->view_dispatcher, UHFViewPopup);
    uhf_worker_start(
        uhf_app->worker, UHFWorkerStateWriteSingle, uhf_write_tag_worker_callback, uhf_app);

    uhf_blink_start(uhf_app);
}

bool uhf_scene_write_tag_on_event(void* ctx, SceneManagerEvent event) {
    UHFApp* uhf_app = ctx;
    bool consumed = false;
    if(event.event == UHFCustomEventWorkerExit) {
        scene_manager_next_scene(uhf_app->scene_manager, UHFSceneWriteTagSuccess);
        consumed = true;
    }
    return consumed;
}

void uhf_scene_write_tag_on_exit(void* ctx) {
    UHFApp* uhf_app = ctx;
    // Stop worker
    uhf_worker_stop(uhf_app->worker);
    // Clear view
    popup_reset(uhf_app->popup);

    uhf_blink_stop(uhf_app);
}
