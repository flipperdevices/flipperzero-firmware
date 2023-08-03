#include "../picopass_i.h"
#include <dolphin/dolphin.h>

void picopass_loclass_worker_callback(PicopassWorkerEvent event, void* context) {
    furi_assert(context);
    Picopass* picopass = context;
    view_dispatcher_send_custom_event(picopass->view_dispatcher, event);
}

void picopass_loclass_result_callback(void* context) {
    furi_assert(context);
    Picopass* picopass = context;
    view_dispatcher_send_custom_event(picopass->view_dispatcher, PicopassCustomEventViewExit);
}

void picopass_scene_loclass_on_enter(void* context) {
    Picopass* picopass = context;
    dolphin_deed(DolphinDeedNfcEmulate);

    scene_manager_set_scene_state(picopass->scene_manager, PicopassSceneLoclass, 0);

    loclass_set_callback(picopass->loclass, picopass_loclass_result_callback, picopass);

    // Start worker
    picopass_worker_start(
        picopass->worker,
        PicopassWorkerStateLoclass,
        &picopass->dev->dev_data,
        picopass_loclass_worker_callback,
        picopass);

    picopass_blink_emulate_start(picopass);

    loclass_set_header(picopass->loclass, "Loclass");

    view_dispatcher_switch_to_view(picopass->view_dispatcher, PicopassViewLoclass);
}

bool picopass_scene_loclass_on_event(void* context, SceneManagerEvent event) {
    Picopass* picopass = context;
    bool consumed = false;

    uint32_t loclass_macs_collected =
        scene_manager_get_scene_state(picopass->scene_manager, PicopassSceneLoclass);

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == PicopassWorkerEventLoclassGotMac) {
            loclass_macs_collected++;
            scene_manager_set_scene_state(
                picopass->scene_manager, PicopassSceneLoclass, loclass_macs_collected);
            loclass_set_num_macs(picopass->loclass, loclass_macs_collected);
            if(loclass_macs_collected >= LOCLASS_MACS_TO_COLLECT) {
                scene_manager_previous_scene(picopass->scene_manager);
            }
            consumed = true;
        } else if(event.event == PicopassWorkerEventLoclassGotStandardKey) {
            loclass_set_header(picopass->loclass, "Loclass (Got Std Key)");
            consumed = true;
        } else if(event.event == PicopassCustomEventViewExit) {
            consumed = scene_manager_previous_scene(picopass->scene_manager);
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = scene_manager_previous_scene(picopass->scene_manager);
    }
    return consumed;
}

void picopass_scene_loclass_on_exit(void* context) {
    Picopass* picopass = context;

    picopass_blink_stop(picopass);

    // Stop worker
    picopass_worker_stop(picopass->worker);

    loclass_reset(picopass->loclass);

    // Clear view
    widget_reset(picopass->widget);
}
