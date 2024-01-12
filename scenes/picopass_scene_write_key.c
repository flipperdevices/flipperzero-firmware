#include "../picopass_i.h"
#include <dolphin/dolphin.h>

NfcCommand picopass_scene_write_key_poller_callback(PicopassPollerEvent event, void* context) {
    NfcCommand command = NfcCommandContinue;
    Picopass* picopass = context;

    if(event.type == PicopassPollerEventTypeRequestMode) {
        event.data->req_mode.mode = PicopassPollerModeWriteKey;
    } else if(event.type == PicopassPollerEventTypeRequestKey) {
        event.data->req_key.is_key_provided = true;
        memcpy(event.data->req_key.key, picopass->dev->dev_data.pacs.key, PICOPASS_KEY_LEN);
        event.data->req_key.is_elite_key = picopass->dev->dev_data.pacs.elite_kdf;
    } else if(event.type == PicopassPollerEventTypeRequestWriteKey) {
        event.data->req_write_key.data = &picopass->dev->dev_data;
        memcpy(
            event.data->req_write_key.key,
            picopass->write_key_context.key_to_write,
            PICOPASS_KEY_LEN);
        event.data->req_write_key.is_elite_key = picopass->write_key_context.is_elite;
    } else if(event.type == PicopassPollerEventTypeSuccess) {
        view_dispatcher_send_custom_event(
            picopass->view_dispatcher, PicopassCustomEventPollerSuccess);
    } else if(
        event.type == PicopassPollerEventTypeFail ||
        event.type == PicopassPollerEventTypeAuthFail) {
        view_dispatcher_send_custom_event(
            picopass->view_dispatcher, PicopassCustomEventPollerFail);
    }

    return command;
}

void picopass_scene_write_key_on_enter(void* context) {
    Picopass* picopass = context;
    dolphin_deed(DolphinDeedNfcSave);

    // Setup view
    Popup* popup = picopass->popup;
    popup_set_header(popup, "Writing\niClass\nkey", 68, 30, AlignLeft, AlignTop);
    popup_set_icon(popup, 0, 3, &I_RFIDDolphinSend_97x61);

    // Start worker
    view_dispatcher_switch_to_view(picopass->view_dispatcher, PicopassViewPopup);
    picopass_blink_start(picopass);

    picopass->poller = picopass_poller_alloc(picopass->nfc);
    picopass_poller_start(picopass->poller, picopass_scene_write_key_poller_callback, picopass);
}

bool picopass_scene_write_key_on_event(void* context, SceneManagerEvent event) {
    Picopass* picopass = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == PicopassCustomEventPollerSuccess) {
            scene_manager_next_scene(picopass->scene_manager, PicopassSceneWriteCardSuccess);
            consumed = true;
        } else if(event.event == PicopassCustomEventPollerFail) {
            scene_manager_next_scene(picopass->scene_manager, PicopassSceneWriteCardFailure);
            consumed = true;
        }
    }
    return consumed;
}

void picopass_scene_write_key_on_exit(void* context) {
    Picopass* picopass = context;

    // Stop worker
    picopass_poller_stop(picopass->poller);
    picopass_poller_free(picopass->poller);

    // Clear view
    popup_reset(picopass->popup);

    picopass_blink_stop(picopass);
}
