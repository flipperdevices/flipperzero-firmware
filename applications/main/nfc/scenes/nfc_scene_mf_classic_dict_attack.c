#include "../nfc_app_i.h"

#include <dolphin/dolphin.h>
#include <lib/nfc/protocols/mf_classic/mf_classic_poller.h>

#define TAG "NfcMfClassicDictAttack"

typedef enum {
    DictAttackStateIdle,
    DictAttackStateUserDictInProgress,
    DictAttackStateFlipperDictInProgress,
} DictAttackState;

NfcCommand nfc_dict_attack_worker_callback(NfcGenericEvent event, void* context) {
    furi_assert(context);
    furi_assert(event.data);
    furi_assert(event.instance);
    furi_assert(event.protocol == NfcProtocolMfClassic);

    NfcCommand command = NfcCommandContinue;
    MfClassicPollerEvent* mfc_event = event.data;

    NfcApp* instance = context;
    if(mfc_event->type == MfClassicPollerEventTypeCardDetected) {
        view_dispatcher_send_custom_event(
            instance->view_dispatcher, NfcCustomEventDictAttackCardDetected);
    } else if(mfc_event->type == MfClassicPollerEventTypeCardLost) {
        view_dispatcher_send_custom_event(instance->view_dispatcher, NfcCustomEventCardLost);
    } else if(mfc_event->type == MfClassicPollerEventTypeRequestMode) {
        mfc_event->data->poller_mode.mode = MfClassicPollerModeDictAttack;
        
    } else if(mfc_event->type == MfClassicPollerEventTypeRequestKey) {
        MfClassicKey key = {};
        if(mf_dict_get_next_key(instance->mf_dict_context.dict, &key)) {
            mfc_event->data->key_request_data.key = key;
            mfc_event->data->key_request_data.key_provided = true;
            instance->mf_dict_context.current_key++;
            if(instance->mf_dict_context.current_key % 10 == 0) {
                view_dispatcher_send_custom_event(
                    instance->view_dispatcher, NfcCustomEventDictAttackNewKeyBatch);
            }
        } else {
            mfc_event->data->key_request_data.key_provided = false;
        }
    } else if(mfc_event->type == MfClassicPollerEventTypeNextSector) {
        view_dispatcher_send_custom_event(
            instance->view_dispatcher, NfcCustomEventDictAttackNewSector);
        mf_dict_rewind(instance->mf_dict_context.dict);
        instance->mf_dict_context.current_key = 0;
    } else if(mfc_event->type == MfClassicPollerEventTypeFoundKeyA) {
        view_dispatcher_send_custom_event(
            instance->view_dispatcher, NfcCustomEventDictAttackFoundKeyA);
    } else if(mfc_event->type == MfClassicPollerEventTypeFoundKeyB) {
        view_dispatcher_send_custom_event(
            instance->view_dispatcher, NfcCustomEventDictAttackFoundKeyB);
    } else if(mfc_event->type == MfClassicPollerEventTypeKeyAttackStart) {
        instance->mf_dict_context.current_sector = mfc_event->data->key_attack_data.current_sector;
        view_dispatcher_send_custom_event(
            instance->view_dispatcher, NfcCustomEventDictAttackKeyAttackStart);
    } else if(mfc_event->type == MfClassicPollerEventTypeKeyAttackStop) {
        mf_dict_rewind(instance->mf_dict_context.dict);
        instance->mf_dict_context.current_key = 0;
        view_dispatcher_send_custom_event(
            instance->view_dispatcher, NfcCustomEventDictAttackKeyAttackStop);
    } else if(mfc_event->type == MfClassicPollerEventTypeKeyAttackNextSector) {
        view_dispatcher_send_custom_event(
            instance->view_dispatcher, NfcCustomEventDictAttackKeyAttackNextSector);
    } else if(mfc_event->type == MfClassicPollerEventTypeSuccess) {
        view_dispatcher_send_custom_event(
            instance->view_dispatcher, NfcCustomEventDictAttackComplete);
        command = NfcCommandStop;
    }

    return command;
}

void nfc_dict_attack_dict_attack_result_callback(void* context) {
    furi_assert(context);
    NfcApp* instance = context;

    view_dispatcher_send_custom_event(instance->view_dispatcher, NfcCustomEventDictAttackSkip);
}

static void nfc_scene_mf_classic_dict_attack_update_view(NfcApp* instance) {
    const MfClassicData* data = nfc_poller_get_data(instance->poller);
    uint8_t sectors_read = 0;
    uint8_t keys_found = 0;

    // Calculate found keys and read sectors
    mf_classic_get_read_sectors_and_keys(data, &sectors_read, &keys_found);
    dict_attack_set_keys_found(instance->dict_attack, keys_found);
    dict_attack_set_sector_read(instance->dict_attack, sectors_read);
}

static void
    nfc_scene_mf_classic_dict_attack_prepare_view(NfcApp* instance, DictAttackState state) {
    // Identify scene state
    if(state == DictAttackStateIdle) {
        bool use_user_dict = false;
        if(mf_dict_check_presence(MfDictTypeUser)) {
            instance->mf_dict_context.dict = mf_dict_alloc(MfDictTypeUser);
            state = DictAttackStateUserDictInProgress;
            use_user_dict = mf_dict_get_total_keys(instance->mf_dict_context.dict) > 0;
            if(!use_user_dict) {
                mf_dict_free(instance->mf_dict_context.dict);
                state = DictAttackStateFlipperDictInProgress;
                instance->mf_dict_context.dict = mf_dict_alloc(MfDictTypeSystem);
            }
        }
        instance->mf_dict_context.total_keys =
            mf_dict_get_total_keys(instance->mf_dict_context.dict);
        instance->mf_dict_context.current_key = 0;
    } else if(state == DictAttackStateUserDictInProgress) {
        state = DictAttackStateFlipperDictInProgress;
    }

    // Setup view
    if(state == DictAttackStateUserDictInProgress) {
        dict_attack_set_header(instance->dict_attack, "MF Classic User Dictionary");
    }
    if(state == DictAttackStateFlipperDictInProgress) {
        dict_attack_set_header(instance->dict_attack, "MF Classic System Dictionary");
    }

    scene_manager_set_scene_state(instance->scene_manager, NfcSceneMfClassicDictAttack, state);
    dict_attack_set_callback(
        instance->dict_attack, nfc_dict_attack_dict_attack_result_callback, instance);
    dict_attack_set_current_sector(instance->dict_attack, 0);
    dict_attack_set_total_dict_keys(instance->dict_attack, instance->mf_dict_context.total_keys);
    // nfc_scene_mf_classic_dict_attack_update_view(instance);
}

void nfc_scene_mf_classic_dict_attack_on_enter(void* context) {
    NfcApp* instance = context;

    nfc_scene_mf_classic_dict_attack_prepare_view(instance, DictAttackStateIdle);
    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcViewDictAttack);
    nfc_blink_read_start(instance);
    notification_message(instance->notifications, &sequence_display_backlight_enforce_on);

    instance->poller = nfc_poller_alloc(instance->nfc, NfcProtocolMfClassic);
    nfc_poller_start(instance->poller, nfc_dict_attack_worker_callback, instance);
}

bool nfc_scene_mf_classic_dict_attack_on_event(void* context, SceneManagerEvent event) {
    NfcApp* instance = context;
    bool consumed = false;

    uint32_t state =
        scene_manager_get_scene_state(instance->scene_manager, NfcSceneMfClassicDictAttack);
    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NfcCustomEventDictAttackComplete) {
            if(state == DictAttackStateUserDictInProgress) {
                nfc_scene_mf_classic_dict_attack_prepare_view(instance, state);
                consumed = true;
            } else {
                notification_message(instance->notifications, &sequence_success);
                scene_manager_next_scene(instance->scene_manager, NfcSceneReadSuccess);
                dolphin_deed(DolphinDeedNfcReadSuccess);
                consumed = true;
            }
        } else if(event.event == NfcCustomEventDictAttackCardDetected) {
            dict_attack_set_card_detected(instance->dict_attack, MfClassicType1k);
            consumed = true;
        } else if(event.event == NfcCustomEventDictAttackCardNotDetected) {
            dict_attack_set_card_removed(instance->dict_attack);
            consumed = true;
        } else if(event.event == NfcCustomEventDictAttackFoundKeyA) {
            dict_attack_inc_keys_found(instance->dict_attack);
            consumed = true;
        } else if(event.event == NfcCustomEventDictAttackFoundKeyB) {
            dict_attack_inc_keys_found(instance->dict_attack);
            consumed = true;
        } else if(event.event == NfcCustomEventDictAttackNewSector) {
            nfc_device_set_data(
                instance->nfc_device, NfcProtocolMfClassic, nfc_poller_get_data(instance->poller));
            nfc_scene_mf_classic_dict_attack_update_view(instance);
            dict_attack_inc_current_sector(instance->dict_attack);
            consumed = true;
        } else if(event.event == NfcCustomEventDictAttackNewKeyBatch) {
            nfc_device_set_data(
                instance->nfc_device, NfcProtocolMfClassic, nfc_poller_get_data(instance->poller));
            nfc_scene_mf_classic_dict_attack_update_view(instance);
            dict_attack_inc_current_dict_key(instance->dict_attack, 10);
            consumed = true;
        } else if(event.event == NfcCustomEventDictAttackSkip) {
            if(state == DictAttackStateUserDictInProgress) {
                nfc_poller_stop(instance->poller);
                consumed = true;
            } else if(state == DictAttackStateFlipperDictInProgress) {
                nfc_poller_stop(instance->poller);
                consumed = true;
            }
        } else if(event.event == NfcCustomEventDictAttackKeyAttackStart) {
            dict_attack_set_key_attack(
                instance->dict_attack, true, instance->mf_dict_context.current_sector);
        } else if(event.event == NfcCustomEventDictAttackKeyAttackStop) {
            dict_attack_set_key_attack(instance->dict_attack, false, 0);
        } else if(event.event == NfcCustomEventDictAttackKeyAttackNextSector) {
            dict_attack_inc_key_attack_current_sector(instance->dict_attack);
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_next_scene(instance->scene_manager, NfcSceneExitConfirm);
        consumed = true;
    }
    return consumed;
}

void nfc_scene_mf_classic_dict_attack_on_exit(void* context) {
    NfcApp* instance = context;

    nfc_poller_stop(instance->poller);
    nfc_poller_free(instance->poller);

    nfc_blink_stop(instance);
    notification_message(instance->notifications, &sequence_display_backlight_enforce_auto);
}
