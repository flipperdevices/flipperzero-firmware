#include "../helpers/protocol_support/nfc_protocol_support.h"

#include "nfc_app_i.h"

#include <cfw/cfw.h>

FuriTimer* timer_auto_exit = NULL;

void nfc_scene_emulate_timer_callback(void* context) {
    NfcApp* instance = context;

    view_dispatcher_send_custom_event(
        instance->view_dispatcher, NfcCustomEventEmulationTimeExpired);
}

void nfc_scene_emulate_on_enter(void* context) {
    NfcApp* instance = context;

    nfc_protocol_support_on_enter(NfcProtocolSupportSceneEmulate, context);

    if(instance->fav_timeout) {
        timer_auto_exit =
            furi_timer_alloc(nfc_scene_emulate_timer_callback, FuriTimerTypeOnce, instance);
        furi_timer_start(
            timer_auto_exit, cfw_settings.favorite_timeout * furi_kernel_get_tick_frequency());
    }
}

bool nfc_scene_emulate_on_event(void* context, SceneManagerEvent event) {
    NfcApp* instance = context;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NfcCustomEventEmulationTimeExpired) {
            if(!scene_manager_previous_scene(instance->scene_manager)) {
                scene_manager_stop(instance->scene_manager);
                view_dispatcher_stop(instance->view_dispatcher);
            } else {
                scene_manager_previous_scene(instance->scene_manager);
            }
            return true;
        }
    }
    return nfc_protocol_support_on_event(NfcProtocolSupportSceneEmulate, context, event);
}

void nfc_scene_emulate_on_exit(void* context) {
    if(timer_auto_exit) {
        furi_timer_stop(timer_auto_exit);
        furi_timer_free(timer_auto_exit);
        timer_auto_exit = NULL;
    }
    nfc_protocol_support_on_exit(NfcProtocolSupportSceneEmulate, context);
}
