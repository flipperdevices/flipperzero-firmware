#include "../picopass_i.h"
#include <dolphin/dolphin.h>

NfcCommand picopass_scene_listener_callback(PicopassListenerEvent event, void* context) {
    UNUSED(event);
    UNUSED(context);

    return NfcCommandContinue;
}

void picopass_scene_emulate_on_enter(void* context) {
    Picopass* picopass = context;
    dolphin_deed(DolphinDeedNfcEmulate);

    Widget* widget = picopass->widget;
    widget_reset(widget);
    widget_add_icon_element(widget, 0, 3, &I_RFIDDolphinSend_97x61);
    widget_add_string_element(widget, 89, 32, AlignCenter, AlignTop, FontPrimary, "Emulating");
    widget_add_string_element(widget, 89, 42, AlignCenter, AlignTop, FontPrimary, "PicoPass");

    view_dispatcher_switch_to_view(picopass->view_dispatcher, PicopassViewWidget);
    picopass_blink_emulate_start(picopass);

    picopass->listener = picopass_listener_alloc(picopass->nfc, &picopass->dev->dev_data);
    picopass_listener_start(picopass->listener, picopass_scene_listener_callback, picopass);
}

bool picopass_scene_emulate_on_event(void* context, SceneManagerEvent event) {
    Picopass* picopass = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == PicopassCustomEventWorkerExit) {
            consumed = true;
        } else if(event.event == PicopassCustomEventNrMacSaved) {
            scene_manager_next_scene(picopass->scene_manager, PicopassSceneNrMacSaved);
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
    picopass_listener_stop(picopass->listener);
    picopass_listener_free(picopass->listener);

    // Clear view
    widget_reset(picopass->widget);
}
