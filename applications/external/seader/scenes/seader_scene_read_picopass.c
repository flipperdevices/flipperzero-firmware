#include "../seader_i.h"
#include <dolphin/dolphin.h>

void seader_read_picopass_worker_callback(SeaderWorkerEvent event, void* context) {
    UNUSED(event);
    Seader* seader = context;
    view_dispatcher_send_custom_event(seader->view_dispatcher, SeaderCustomEventWorkerExit);
}

void seader_scene_read_picopass_on_enter(void* context) {
    Seader* seader = context;
    dolphin_deed(DolphinDeedNfcRead);

    // Setup view
    Popup* popup = seader->popup;
    popup_set_header(popup, "Detecting\npicopass\ncard", 68, 30, AlignLeft, AlignTop);
    popup_set_icon(popup, 0, 3, &I_RFIDDolphinReceive_97x61);

    // Start worker
    view_dispatcher_switch_to_view(seader->view_dispatcher, SeaderViewPopup);
    seader_worker_start(
        seader->worker,
        SeaderWorkerStateReadPicopass,
        seader->uart,
        seader->credential,
        seader_read_picopass_worker_callback,
        seader);

    seader_blink_start(seader);
}

bool seader_scene_read_picopass_on_event(void* context, SceneManagerEvent event) {
    Seader* seader = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SeaderCustomEventWorkerExit) {
            seader->credential->type = SeaderCredentialTypePicopass;
            scene_manager_next_scene(seader->scene_manager, SeaderSceneReadCardSuccess);
            consumed = true;
        }
    }
    return consumed;
}

void seader_scene_read_picopass_on_exit(void* context) {
    Seader* seader = context;

    // Stop worker
    seader_worker_stop(seader->worker);
    // Clear view
    popup_reset(seader->popup);

    seader_blink_stop(seader);
}
