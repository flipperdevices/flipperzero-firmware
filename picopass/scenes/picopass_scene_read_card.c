#include "../picopass_i.h"
#include <dolphin/dolphin.h>
#include "../picopass_keys.h"

enum {
    PicopassSceneReadCardDictStandard,
    PicopassSceneReadCardDictElite,
};

static bool picopass_read_card_change_dict(Picopass* picopass) {
    bool success = false;

    do {
        uint32_t scene_state =
            scene_manager_get_scene_state(picopass->scene_manager, PicopassSceneReadCard);
        keys_dict_free(picopass->dict);
        picopass->dict = NULL;
        if(scene_state == PicopassSceneReadCardDictElite) break;
        if(!keys_dict_check_presence(PICOPASS_ICLASS_ELITE_DICT_FLIPPER_NAME)) break;

        picopass->dict = keys_dict_alloc(
            PICOPASS_ICLASS_ELITE_DICT_FLIPPER_NAME, KeysDictModeOpenExisting, PICOPASS_KEY_LEN);
        scene_manager_set_scene_state(
            picopass->scene_manager, PicopassSceneReadCard, PicopassSceneReadCardDictElite);
        success = true;
    } while(false);

    return success;
}

NfcCommand picopass_read_card_worker_callback(PicopassPollerEvent event, void* context) {
    furi_assert(context);
    NfcCommand command = NfcCommandContinue;

    Picopass* picopass = context;

    if(event.type == PicopassPollerEventTypeRequestMode) {
        event.data->req_mode.mode = PicopassPollerModeRead;
    } else if(event.type == PicopassPollerEventTypeRequestKey) {
        uint8_t key[PICOPASS_KEY_LEN] = {};
        bool is_key_provided = true;
        if(!keys_dict_get_next_key(picopass->dict, key, PICOPASS_KEY_LEN)) {
            if(picopass_read_card_change_dict(picopass)) {
                is_key_provided = keys_dict_get_next_key(picopass->dict, key, PICOPASS_KEY_LEN);
            } else {
                is_key_provided = false;
            }
        }
        uint32_t scene_state =
            scene_manager_get_scene_state(picopass->scene_manager, PicopassSceneReadCard);
        memcpy(event.data->req_key.key, key, PICOPASS_KEY_LEN);
        event.data->req_key.is_elite_key = (scene_state == PicopassSceneReadCardDictElite);
        event.data->req_key.is_key_provided = is_key_provided;
    } else if(
        event.type == PicopassPollerEventTypeSuccess ||
        event.type == PicopassPollerEventTypeAuthFail) {
        const PicopassDeviceData* data = picopass_poller_get_data(picopass->poller);
        memcpy(&picopass->dev->dev_data, data, sizeof(PicopassDeviceData));
        view_dispatcher_send_custom_event(
            picopass->view_dispatcher, PicopassCustomEventPollerSuccess);
    } else if(event.type == PicopassPollerEventTypeFail) {
        // the poller will retry, but provide some feedback to the user
        uint32_t ticks = furi_get_tick();
        if(picopass->last_error_notify_ticks + furi_ms_to_ticks(500) < ticks) {
            picopass->last_error_notify_ticks = ticks;
            notification_message(picopass->notifications, &sequence_error);
        }
    }

    return command;
}

void picopass_scene_read_card_on_enter(void* context) {
    Picopass* picopass = context;
    dolphin_deed(DolphinDeedNfcRead);

    picopass->last_error_notify_ticks = 0;

    // Setup view
    Popup* popup = picopass->popup;
    popup_set_header(popup, "Detecting\npicopass\ncard", 68, 30, AlignLeft, AlignTop);
    popup_set_icon(popup, 0, 3, &I_RFIDDolphinReceive_97x61);

    picopass->dict = keys_dict_alloc(
        PICOPASS_ICLASS_STANDARD_DICT_FLIPPER_NAME, KeysDictModeOpenExisting, PICOPASS_KEY_LEN);
    scene_manager_set_scene_state(
        picopass->scene_manager, PicopassSceneReadCard, PicopassSceneReadCardDictStandard);
    // Start worker
    picopass->poller = picopass_poller_alloc(picopass->nfc);
    picopass_poller_start(picopass->poller, picopass_read_card_worker_callback, picopass);

    view_dispatcher_switch_to_view(picopass->view_dispatcher, PicopassViewPopup);
    picopass_blink_start(picopass);
}

bool picopass_scene_read_card_on_event(void* context, SceneManagerEvent event) {
    Picopass* picopass = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == PicopassCustomEventPollerSuccess) {
            if(memcmp(
                   picopass->dev->dev_data.pacs.key,
                   picopass_factory_debit_key,
                   PICOPASS_BLOCK_LEN) == 0) {
                scene_manager_next_scene(picopass->scene_manager, PicopassSceneReadFactorySuccess);
            } else {
                scene_manager_next_scene(picopass->scene_manager, PicopassSceneReadCardSuccess);
            }
            consumed = true;
        }
    }
    return consumed;
}

void picopass_scene_read_card_on_exit(void* context) {
    Picopass* picopass = context;

    if(picopass->dict) {
        keys_dict_free(picopass->dict);
        picopass->dict = NULL;
    }
    picopass_poller_stop(picopass->poller);
    picopass_poller_free(picopass->poller);

    // Clear view
    popup_reset(picopass->popup);
    scene_manager_set_scene_state(
        picopass->scene_manager, PicopassSceneReadCard, PicopassSceneReadCardDictStandard);

    picopass_blink_stop(picopass);
}
