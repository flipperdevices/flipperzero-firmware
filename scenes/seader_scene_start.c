#include "../seader_i.h"
enum SubmenuIndex {
    SubmenuIndexSamPresent,
    SubmenuIndexSamMissing,
};

static void detect_callback(void* context) {
    Seader* seader = context;
    view_dispatcher_send_custom_event(seader->view_dispatcher, SeaderWorkerEventSamMissing);
}

void seader_sam_check_worker_callback(SeaderWorkerEvent event, void* context) {
    Seader* seader = context;
    view_dispatcher_send_custom_event(seader->view_dispatcher, event);
}

void seader_scene_start_submenu_callback(void* context, uint32_t index) {
    Seader* seader = context;
    view_dispatcher_send_custom_event(seader->view_dispatcher, index);
}

void seader_scene_start_on_enter(void* context) {
    Seader* seader = context;

    Popup* popup = seader->popup;

    popup_set_context(seader->popup, seader);
    popup_set_callback(seader->popup, detect_callback);
    popup_set_header(popup, "Detecting SAM", 58, 48, AlignCenter, AlignCenter);
    popup_set_timeout(seader->popup, 2500);
    popup_enable_timeout(seader->popup);

    // Start worker
    seader_worker_start(
        seader->worker,
        SeaderWorkerStateCheckSam,
        seader->uart,
        seader->credential,
        seader_sam_check_worker_callback,
        seader);
    view_dispatcher_switch_to_view(seader->view_dispatcher, SeaderViewPopup);
}

bool seader_scene_start_on_event(void* context, SceneManagerEvent event) {
    Seader* seader = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SeaderWorkerEventSamPresent) {
            scene_manager_next_scene(seader->scene_manager, SeaderSceneSamPresent);
            consumed = true;
        } else if(event.event == SeaderWorkerEventSamMissing) {
            scene_manager_next_scene(seader->scene_manager, SeaderSceneSamMissing);
            consumed = true;
        } else if(event.event == SeaderWorkerEventSamWrong) {
            scene_manager_next_scene(seader->scene_manager, SeaderSceneSamWrong);
            consumed = true;
        }

        scene_manager_set_scene_state(seader->scene_manager, SeaderSceneStart, event.event);
    }

    return consumed;
}

void seader_scene_start_on_exit(void* context) {
    Seader* seader = context;
    submenu_reset(seader->submenu);
}
