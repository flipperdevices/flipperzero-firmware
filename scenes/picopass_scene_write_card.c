#include "../picopass_i.h"
#include <dolphin/dolphin.h>
#include "../picopass_keys.h"

#define PICOPASS_SCENE_WRITE_BLOCK_START 6
#define PICOPASS_SCENE_WRITE_BLOCK_STOP 10

NfcCommand picopass_scene_write_poller_callback(PicopassPollerEvent event, void* context) {
    Picopass* picopass = context;
    NfcCommand command = NfcCommandContinue;

    if(event.type == PicopassPollerEventTypeRequestMode) {
        event.data->req_mode.mode = PicopassPollerModeWrite;
    } else if(event.type == PicopassPollerEventTypeRequestKey) {
        memcpy(event.data->req_key.key, picopass_iclass_key, sizeof(picopass_iclass_key));
        event.data->req_key.is_elite_key = false;
        event.data->req_key.is_key_provided = true;
    } else if(event.type == PicopassPollerEventTypeRequestWriteBlock) {
        uint8_t block_num =
            scene_manager_get_scene_state(picopass->scene_manager, PicopassSceneWriteCard);
        if(block_num == PICOPASS_SCENE_WRITE_BLOCK_STOP) {
            event.data->req_write.perform_write = false;
        } else {
            event.data->req_write.block_num = block_num;
            event.data->req_write.block = &picopass->dev->dev_data.AA1[block_num];
            event.data->req_write.perform_write = true;
            block_num++;
            scene_manager_set_scene_state(
                picopass->scene_manager, PicopassSceneWriteCard, block_num);
        }
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

void picopass_scene_write_card_on_enter(void* context) {
    Picopass* picopass = context;
    dolphin_deed(DolphinDeedNfcSave);

    // Setup view
    Popup* popup = picopass->popup;
    popup_set_header(popup, "Writing\npicopass\ncard", 68, 30, AlignLeft, AlignTop);
    popup_set_icon(popup, 0, 3, &I_RFIDDolphinSend_97x61);
    scene_manager_set_scene_state(
        picopass->scene_manager, PicopassSceneWriteCard, PICOPASS_SCENE_WRITE_BLOCK_START);

    // Start worker
    view_dispatcher_switch_to_view(picopass->view_dispatcher, PicopassViewPopup);

    picopass->poller = picopass_poller_alloc(picopass->nfc);
    picopass_poller_start(picopass->poller, picopass_scene_write_poller_callback, picopass);

    picopass_blink_start(picopass);
}

bool picopass_scene_write_card_on_event(void* context, SceneManagerEvent event) {
    Picopass* picopass = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == PicopassCustomEventPollerFail) {
            scene_manager_next_scene(picopass->scene_manager, PicopassSceneWriteCardFailure);
            consumed = true;
        } else if(event.event == PicopassCustomEventPollerSuccess) {
            scene_manager_next_scene(picopass->scene_manager, PicopassSceneWriteCardSuccess);
            consumed = true;
        }
    }
    return consumed;
}

void picopass_scene_write_card_on_exit(void* context) {
    Picopass* picopass = context;

    // Stop worker
    picopass_poller_stop(picopass->poller);
    picopass_poller_free(picopass->poller);
    // Clear view
    popup_reset(picopass->popup);

    picopass_blink_stop(picopass);
}
