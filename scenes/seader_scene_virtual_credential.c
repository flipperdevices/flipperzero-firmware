#include "../seader_i.h"
#include <dolphin/dolphin.h>

void seader_virtual_credential_worker_callback(SeaderWorkerEvent event, void* context) {
    Seader* seader = context;
    view_dispatcher_send_custom_event(seader->view_dispatcher, event);
}

void seader_scene_virtual_credential_on_enter(void* context) {
    Seader* seader = context;

    // Setup view
    Popup* popup = seader->popup;
    popup_set_header(popup, "Processing\nvirtual\npicopass", 68, 30, AlignLeft, AlignTop);

    // Start worker
    seader->credential->type = SeaderCredentialTypeVirtual;
    seader_worker_start(
        seader->worker,
        SeaderWorkerStateVirtualCredential,
        seader->uart,
        seader_virtual_credential_worker_callback,
        seader);

    view_dispatcher_switch_to_view(seader->view_dispatcher, SeaderViewPopup);
}

bool seader_scene_virtual_credential_on_event(void* context, SceneManagerEvent event) {
    Seader* seader = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SeaderCustomEventWorkerExit) {
            seader->credential->type = SeaderCredentialTypeVirtual;
            scene_manager_next_scene(seader->scene_manager, SeaderSceneReadCardSuccess);
            consumed = true;
        } else if(event.event == SeaderCustomEventPollerSuccess) {
            seader->credential->type = SeaderCredentialTypeVirtual;
            scene_manager_next_scene(seader->scene_manager, SeaderSceneReadCardSuccess);
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_search_and_switch_to_previous_scene(
            seader->scene_manager, SeaderSceneSavedMenu);
        consumed = true;
    }
    return consumed;
}

void seader_scene_virtual_credential_on_exit(void* context) {
    Seader* seader = context;

    // Clear view
    popup_reset(seader->popup);
}
