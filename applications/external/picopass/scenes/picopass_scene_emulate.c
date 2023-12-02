#include "../picopass_i.h"
#include <dolphin/dolphin.h>

void picopass_emulate_worker_callback(PicopassWorkerEvent event, void* context) {
    furi_assert(context);
    Picopass* picopass = context;
    view_dispatcher_send_custom_event(picopass->view_dispatcher, event);
}

void picopass_scene_emulate_on_enter(void* context) {
    Picopass* picopass = context;
    dolphin_deed(DolphinDeedNfcEmulate);

    Widget* widget = picopass->widget;
    widget_reset(widget);
    widget_add_icon_element(widget, 0, 3, &I_RFIDDolphinSend_97x61);
    widget_add_string_element(widget, 89, 32, AlignCenter, AlignTop, FontPrimary, "Emulating");
    widget_add_string_element(widget, 89, 42, AlignCenter, AlignTop, FontPrimary, "PicoPass");

    // Setup view
    view_dispatcher_switch_to_view(picopass->view_dispatcher, PicopassViewWidget);

    // Start worker
    picopass_worker_start(
        picopass->worker,
        PicopassWorkerStateEmulate,
        &picopass->dev->dev_data,
        picopass_emulate_worker_callback,
        picopass);

    picopass_blink_emulate_start(picopass);
}

bool picopass_scene_emulate_on_event(void* context, SceneManagerEvent event) {
    Picopass* picopass = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == PicopassCustomEventWorkerExit) {
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = scene_manager_previous_scene(picopass->scene_manager);
    }
    return consumed;
}

void picopass_scene_emulate_on_exit(void* context) {
    Picopass* picopass = context;

    picopass_blink_stop(picopass);

    // Stop worker
    picopass_worker_stop(picopass->worker);

    // Clear view
    widget_reset(picopass->widget);
}
