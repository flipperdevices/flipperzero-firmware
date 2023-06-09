#include "../nfc_app_i.h"
#include <dolphin/dolphin.h>

#define TAG "NfcMfClassicDictAttack"

typedef enum {
    DictAttackStateIdle,
    DictAttackStateUserDictInProgress,
    DictAttackStateFlipperDictInProgress,
} DictAttackState;

MfClassicPollerCommand nfc_dict_attack_worker_callback(MfClassicPollerEvent event, void* context) {
    furi_assert(context);

    MfClassicPollerCommand command = MfClassicPollerCommandContinue;

    NfcApp* nfc_app = context;
    if(event.type == MfClassicPollerEventTypeStart) {
        nfc_app->mf_dict_context.type = event.data->start_data.type;
        view_dispatcher_send_custom_event(
            nfc_app->view_dispatcher, NfcCustomEventDictAttackCardDetected);
    } else if(event.type == MfClassicPollerEventTypeCardDetected) {
        view_dispatcher_send_custom_event(
            nfc_app->view_dispatcher, NfcCustomEventDictAttackCardDetected);
    } else if(event.type == MfClassicPollerEventTypeCardNotDetected) {
        view_dispatcher_send_custom_event(
            nfc_app->view_dispatcher, NfcCustomEventDictAttackCardNotDetected);
    } else if(event.type == MfClassicPollerEventTypeRequestKey) {
        MfClassicKey key = {};
        if(mf_dict_get_next_key(nfc_app->mf_dict_context.dict, &key)) {
            event.data->key_request_data.key = key;
            event.data->key_request_data.key_provided = true;
            nfc_app->mf_dict_context.current_key++;
            if(nfc_app->mf_dict_context.current_key % 10 == 0) {
                view_dispatcher_send_custom_event(
                    nfc_app->view_dispatcher, NfcCustomEventDictAttackNewKeyBatch);
            }
        } else {
            event.data->key_request_data.key_provided = false;
        }
    } else if(event.type == MfClassicPollerEventTypeNewSector) {
        view_dispatcher_send_custom_event(
            nfc_app->view_dispatcher, NfcCustomEventDictAttackNewSector);
        mf_dict_rewind(nfc_app->mf_dict_context.dict);
        nfc_app->mf_dict_context.current_key = 0;
    } else if(event.type == MfClassicPollerEventTypeFoundKeyA) {
        view_dispatcher_send_custom_event(
            nfc_app->view_dispatcher, NfcCustomEventDictAttackFoundKeyA);
    } else if(event.type == MfClassicPollerEventTypeFoundKeyB) {
        view_dispatcher_send_custom_event(
            nfc_app->view_dispatcher, NfcCustomEventDictAttackFoundKeyB);
    } else if(event.type == MfClassicPollerEventTypeKeyAttackStart) {
        nfc_app->mf_dict_context.current_sector = event.data->key_attack_data.start_sector;
        view_dispatcher_send_custom_event(
            nfc_app->view_dispatcher, NfcCustomEventDictAttackKeyAttackStart);
    } else if(event.type == MfClassicPollerEventTypeKeyAttackStop) {
        mf_dict_rewind(nfc_app->mf_dict_context.dict);
        nfc_app->mf_dict_context.current_key = 0;
        view_dispatcher_send_custom_event(
            nfc_app->view_dispatcher, NfcCustomEventDictAttackKeyAttackStop);
    } else if(event.type == MfClassicPollerEventTypeKeyAttackNextSector) {
        view_dispatcher_send_custom_event(
            nfc_app->view_dispatcher, NfcCustomEventDictAttackKeyAttackNextSector);
    } else if(event.type == MfClassicPollerEventTypeReadComplete) {
        view_dispatcher_send_custom_event(
            nfc_app->view_dispatcher, NfcCustomEventDictAttackComplete);
        command = MfClassicPollerCommandStop;
    }

    return command;
}

void nfc_dict_attack_dict_attack_result_callback(void* context) {
    furi_assert(context);
    NfcApp* nfc = context;
    view_dispatcher_send_custom_event(nfc->view_dispatcher, NfcCustomEventDictAttackSkip);
}

static void nfc_scene_mf_classic_dict_attack_update_view(NfcApp* nfc) {
    MfClassicData* data = nfc->nfc_dev_data.mf_classic_data;
    uint8_t sectors_read = 0;
    uint8_t keys_found = 0;

    // Calculate found keys and read sectors
    mf_classic_get_read_sectors_and_keys(data, &sectors_read, &keys_found);
    dict_attack_set_keys_found(nfc->dict_attack, keys_found);
    dict_attack_set_sector_read(nfc->dict_attack, sectors_read);
}

static void nfc_scene_mf_classic_dict_attack_prepare_view(NfcApp* nfc, DictAttackState state) {
    MfClassicData* data = nfc->nfc_dev_data.mf_classic_data;

    // Identify scene state
    if(state == DictAttackStateIdle) {
        if(mf_dict_check_presence(MfDictTypeUser)) {
            state = DictAttackStateUserDictInProgress;
            nfc->mf_dict_context.dict = mf_dict_alloc(MfDictTypeUser);
        } else {
            state = DictAttackStateFlipperDictInProgress;
            nfc->mf_dict_context.dict = mf_dict_alloc(MfDictTypeSystem);
        }
        nfc->mf_dict_context.total_keys = mf_dict_get_total_keys(nfc->mf_dict_context.dict);
        nfc->mf_dict_context.current_key = 0;
    } else if(state == DictAttackStateUserDictInProgress) {
        state = DictAttackStateFlipperDictInProgress;
    }

    // Setup view
    if(state == DictAttackStateUserDictInProgress) {
        dict_attack_set_header(nfc->dict_attack, "MF Classic User Dictionary");
    }
    if(state == DictAttackStateFlipperDictInProgress) {
        dict_attack_set_header(nfc->dict_attack, "MF Classic System Dictionary");
    }

    scene_manager_set_scene_state(nfc->scene_manager, NfcSceneMfClassicDictAttack, state);
    dict_attack_set_callback(nfc->dict_attack, nfc_dict_attack_dict_attack_result_callback, nfc);
    dict_attack_set_current_sector(nfc->dict_attack, 0);
    dict_attack_set_card_detected(nfc->dict_attack, data->type);
    dict_attack_set_total_dict_keys(nfc->dict_attack, nfc->mf_dict_context.total_keys);
    nfc_scene_mf_classic_dict_attack_update_view(nfc);
}

void nfc_scene_mf_classic_dict_attack_on_enter(void* context) {
    NfcApp* nfc = context;
    nfc_scene_mf_classic_dict_attack_prepare_view(nfc, DictAttackStateIdle);
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewDictAttack);
    nfc_blink_read_start(nfc);
    notification_message(nfc->notifications, &sequence_display_backlight_enforce_on);
    mf_classic_poller_dict_attack(nfc->mf_classic_poller, nfc_dict_attack_worker_callback, nfc);
}

bool nfc_scene_mf_classic_dict_attack_on_event(void* context, SceneManagerEvent event) {
    NfcApp* nfc = context;
    MfClassicData* mf_classic_data = nfc->nfc_dev_data.mf_classic_data;
    bool consumed = false;

    uint32_t state =
        scene_manager_get_scene_state(nfc->scene_manager, NfcSceneMfClassicDictAttack);
    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NfcCustomEventDictAttackComplete) {
            if(state == DictAttackStateUserDictInProgress) {
                nfc_scene_mf_classic_dict_attack_prepare_view(nfc, state);
                consumed = true;
            } else {
                notification_message(nfc->notifications, &sequence_success);
                scene_manager_next_scene(nfc->scene_manager, NfcSceneMfClassicReadSuccess);
                DOLPHIN_DEED(DolphinDeedNfcReadSuccess);
                consumed = true;
            }
        } else if(event.event == NfcCustomEventDictAttackCardDetected) {
            dict_attack_set_card_detected(nfc->dict_attack, nfc->mf_dict_context.type);
            consumed = true;
        } else if(event.event == NfcCustomEventDictAttackCardNotDetected) {
            dict_attack_set_card_removed(nfc->dict_attack);
            consumed = true;
        } else if(event.event == NfcCustomEventDictAttackFoundKeyA) {
            dict_attack_inc_keys_found(nfc->dict_attack);
            consumed = true;
        } else if(event.event == NfcCustomEventDictAttackFoundKeyB) {
            dict_attack_inc_keys_found(nfc->dict_attack);
            consumed = true;
        } else if(event.event == NfcCustomEventDictAttackNewSector) {
            mf_classic_poller_get_data(nfc->mf_classic_poller, mf_classic_data);
            nfc_scene_mf_classic_dict_attack_update_view(nfc);
            dict_attack_inc_current_sector(nfc->dict_attack);
            consumed = true;
        } else if(event.event == NfcCustomEventDictAttackNewKeyBatch) {
            mf_classic_poller_get_data(nfc->mf_classic_poller, mf_classic_data);
            nfc_scene_mf_classic_dict_attack_update_view(nfc);
            dict_attack_inc_current_dict_key(nfc->dict_attack, 10);
            consumed = true;
        } else if(event.event == NfcCustomEventDictAttackSkip) {
            if(state == DictAttackStateUserDictInProgress) {
                mf_classic_poller_stop(nfc->mf_classic_poller);
                consumed = true;
            } else if(state == DictAttackStateFlipperDictInProgress) {
                mf_classic_poller_stop(nfc->mf_classic_poller);
                consumed = true;
            }
        } else if(event.event == NfcCustomEventDictAttackKeyAttackStart) {
            dict_attack_set_key_attack(
                nfc->dict_attack, true, nfc->mf_dict_context.current_sector);
        } else if(event.event == NfcCustomEventDictAttackKeyAttackStop) {
            dict_attack_set_key_attack(nfc->dict_attack, false, 0);
        } else if(event.event == NfcCustomEventDictAttackKeyAttackNextSector) {
            dict_attack_inc_key_attack_current_sector(nfc->dict_attack);
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_next_scene(nfc->scene_manager, NfcSceneExitConfirm);
        consumed = true;
    }
    return consumed;
}

void nfc_scene_mf_classic_dict_attack_on_exit(void* context) {
    NfcApp* nfc = context;
    mf_classic_poller_stop(nfc->mf_classic_poller);
    nfc_blink_stop(nfc);
    notification_message(nfc->notifications, &sequence_display_backlight_enforce_auto);
}
