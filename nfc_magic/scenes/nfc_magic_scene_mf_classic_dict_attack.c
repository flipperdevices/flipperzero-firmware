#include "../nfc_magic_app_i.h"

#include <dolphin/dolphin.h>
#include <lib/nfc/protocols/mf_classic/mf_classic_poller.h>

#include "views/dict_attack.h"

#define TAG "NfcMagicMfClassicDictAttack"

typedef enum {
    DictAttackStateUserDictInProgress,
    DictAttackStateSystemDictInProgress,
} DictAttackState;

NfcCommand nfc_dict_attack_worker_callback(NfcGenericEvent event, void* context) {
    furi_assert(context);
    furi_assert(event.event_data);
    furi_assert(event.instance);
    furi_assert(event.protocol == NfcProtocolMfClassic);

    NfcCommand command = NfcCommandContinue;
    MfClassicPollerEvent* mfc_event = event.event_data;

    NfcMagicApp* instance = context;
    if(mfc_event->type == MfClassicPollerEventTypeCardDetected) {
        instance->nfc_dict_context.is_card_present = true;
        view_dispatcher_send_custom_event(
            instance->view_dispatcher, NfcMagicAppCustomEventCardDetected);
    } else if(mfc_event->type == MfClassicPollerEventTypeCardLost) {
        instance->nfc_dict_context.is_card_present = false;
        view_dispatcher_send_custom_event(
            instance->view_dispatcher, NfcMagicAppCustomEventCardLost);
    } else if(mfc_event->type == MfClassicPollerEventTypeRequestMode) {
        const MfClassicData* mfc_data = nfc_poller_get_data(instance->poller);
        nfc_device_set_data(instance->target_dev, NfcProtocolMfClassic, mfc_data);
        FURI_LOG_D(TAG, "MFC type: %d", mfc_data->type);
        mfc_event->data->poller_mode.mode = MfClassicPollerModeDictAttack;
        mfc_event->data->poller_mode.data = mfc_data;
        instance->nfc_dict_context.sectors_total =
            mf_classic_get_total_sectors_num(mfc_data->type);
        FURI_LOG_D(TAG, "Total sectors: %d", mf_classic_get_total_sectors_num(mfc_data->type));
        mf_classic_get_read_sectors_and_keys(
            mfc_data,
            &instance->nfc_dict_context.sectors_read,
            &instance->nfc_dict_context.keys_found);
        view_dispatcher_send_custom_event(
            instance->view_dispatcher, NfcMagicAppCustomEventDictAttackDataUpdate);
    } else if(mfc_event->type == MfClassicPollerEventTypeRequestKey) {
        MfClassicKey key = {};
        if(keys_dict_get_next_key(
               instance->nfc_dict_context.dict, key.data, sizeof(MfClassicKey))) {
            mfc_event->data->key_request_data.key = key;
            mfc_event->data->key_request_data.key_provided = true;
            instance->nfc_dict_context.dict_keys_current++;
            if(instance->nfc_dict_context.dict_keys_current % 10 == 0) {
                view_dispatcher_send_custom_event(
                    instance->view_dispatcher, NfcMagicAppCustomEventDictAttackDataUpdate);
            }
        } else {
            mfc_event->data->key_request_data.key_provided = false;
        }
    } else if(mfc_event->type == MfClassicPollerEventTypeDataUpdate) {
        MfClassicPollerEventDataUpdate* data_update = &mfc_event->data->data_update;
        instance->nfc_dict_context.sectors_read = data_update->sectors_read;
        instance->nfc_dict_context.keys_found = data_update->keys_found;
        instance->nfc_dict_context.current_sector = data_update->current_sector;
        view_dispatcher_send_custom_event(
            instance->view_dispatcher, NfcMagicAppCustomEventDictAttackDataUpdate);
    } else if(mfc_event->type == MfClassicPollerEventTypeNextSector) {
        keys_dict_rewind(instance->nfc_dict_context.dict);
        instance->nfc_dict_context.dict_keys_current = 0;
        instance->nfc_dict_context.current_sector =
            mfc_event->data->next_sector_data.current_sector;
        view_dispatcher_send_custom_event(
            instance->view_dispatcher, NfcMagicAppCustomEventDictAttackDataUpdate);
    } else if(mfc_event->type == MfClassicPollerEventTypeFoundKeyA) {
        view_dispatcher_send_custom_event(
            instance->view_dispatcher, NfcMagicAppCustomEventDictAttackDataUpdate);
    } else if(mfc_event->type == MfClassicPollerEventTypeFoundKeyB) {
        view_dispatcher_send_custom_event(
            instance->view_dispatcher, NfcMagicAppCustomEventDictAttackDataUpdate);
    } else if(mfc_event->type == MfClassicPollerEventTypeKeyAttackStart) {
        instance->nfc_dict_context.key_attack_current_sector =
            mfc_event->data->key_attack_data.current_sector;
        instance->nfc_dict_context.is_key_attack = true;
        view_dispatcher_send_custom_event(
            instance->view_dispatcher, NfcMagicAppCustomEventDictAttackDataUpdate);
    } else if(mfc_event->type == MfClassicPollerEventTypeKeyAttackStop) {
        keys_dict_rewind(instance->nfc_dict_context.dict);
        instance->nfc_dict_context.is_key_attack = false;
        instance->nfc_dict_context.dict_keys_current = 0;
        view_dispatcher_send_custom_event(
            instance->view_dispatcher, NfcMagicAppCustomEventDictAttackDataUpdate);
    } else if(mfc_event->type == MfClassicPollerEventTypeSuccess) {
        const MfClassicData* mfc_data = nfc_poller_get_data(instance->poller);
        nfc_device_set_data(instance->target_dev, NfcProtocolMfClassic, mfc_data);
        view_dispatcher_send_custom_event(
            instance->view_dispatcher, NfcMagicAppCustomEventDictAttackComplete);
        command = NfcCommandStop;
    }

    return command;
}

void nfc_dict_attack_dict_attack_result_callback(DictAttackEvent event, void* context) {
    furi_assert(context);
    NfcMagicApp* instance = context;

    if(event == DictAttackEventSkipPressed) {
        view_dispatcher_send_custom_event(
            instance->view_dispatcher, NfcMagicAppCustomEventDictAttackSkip);
    }
}

static void nfc_magic_scene_mf_classic_dict_attack_update_view(NfcMagicApp* instance) {
    NfcMagicAppMfClassicDictAttackContext* mfc_dict = &instance->nfc_dict_context;

    if(mfc_dict->is_key_attack) {
        dict_attack_set_key_attack(instance->dict_attack, mfc_dict->key_attack_current_sector);
    } else {
        dict_attack_reset_key_attack(instance->dict_attack);
        dict_attack_set_sectors_total(instance->dict_attack, mfc_dict->sectors_total);
        dict_attack_set_sectors_read(instance->dict_attack, mfc_dict->sectors_read);
        dict_attack_set_keys_found(instance->dict_attack, mfc_dict->keys_found);
        dict_attack_set_current_dict_key(instance->dict_attack, mfc_dict->dict_keys_current);
        dict_attack_set_current_sector(instance->dict_attack, mfc_dict->current_sector);
    }
}

static void nfc_magic_scene_mf_classic_dict_attack_prepare_view(NfcMagicApp* instance) {
    uint32_t state =
        scene_manager_get_scene_state(instance->scene_manager, NfcMagicSceneMfClassicDictAttack);
    if(state == DictAttackStateUserDictInProgress) {
        do {
            if(!keys_dict_check_presence(NFC_APP_MF_CLASSIC_DICT_USER_PATH)) {
                state = DictAttackStateSystemDictInProgress;
                break;
            }

            instance->nfc_dict_context.dict = keys_dict_alloc(
                NFC_APP_MF_CLASSIC_DICT_USER_PATH, KeysDictModeOpenAlways, sizeof(MfClassicKey));
            if(keys_dict_get_total_keys(instance->nfc_dict_context.dict) == 0) {
                keys_dict_free(instance->nfc_dict_context.dict);
                state = DictAttackStateSystemDictInProgress;
                break;
            }

            dict_attack_set_header(instance->dict_attack, "MF Classic User Dictionary");
        } while(false);
    }
    if(state == DictAttackStateSystemDictInProgress) {
        instance->nfc_dict_context.dict = keys_dict_alloc(
            NFC_APP_MF_CLASSIC_DICT_SYSTEM_PATH, KeysDictModeOpenExisting, sizeof(MfClassicKey));
        dict_attack_set_header(instance->dict_attack, "MF Classic System Dictionary");
    }

    instance->nfc_dict_context.dict_keys_total =
        keys_dict_get_total_keys(instance->nfc_dict_context.dict);
    dict_attack_set_total_dict_keys(
        instance->dict_attack, instance->nfc_dict_context.dict_keys_total);
    instance->nfc_dict_context.dict_keys_current = 0;

    dict_attack_set_callback(
        instance->dict_attack, nfc_dict_attack_dict_attack_result_callback, instance);
    nfc_magic_scene_mf_classic_dict_attack_update_view(instance);

    scene_manager_set_scene_state(
        instance->scene_manager, NfcMagicSceneMfClassicDictAttack, state);
}

void nfc_magic_scene_mf_classic_dict_attack_on_enter(void* context) {
    NfcMagicApp* instance = context;

    scene_manager_set_scene_state(
        instance->scene_manager,
        NfcMagicSceneMfClassicDictAttack,
        DictAttackStateUserDictInProgress);
    nfc_magic_scene_mf_classic_dict_attack_prepare_view(instance);
    dict_attack_set_card_state(instance->dict_attack, true);
    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcMagicAppViewDictAttack);
    nfc_magic_app_blink_start(instance);
    notification_message(instance->notifications, &sequence_display_backlight_enforce_on);

    instance->poller = nfc_poller_alloc(instance->nfc, NfcProtocolMfClassic);
    nfc_poller_start(instance->poller, nfc_dict_attack_worker_callback, instance);
}

static void nfc_magic_scene_mf_classic_dict_attack_notify_read(NfcMagicApp* instance) {
    const MfClassicData* mfc_data = nfc_poller_get_data(instance->poller);
    bool is_card_fully_read = mf_classic_is_card_read(mfc_data);
    if(is_card_fully_read) {
        notification_message(instance->notifications, &sequence_success);
    } else {
        notification_message(instance->notifications, &sequence_semi_success);
    }
}

bool nfc_magic_scene_mf_classic_dict_attack_on_event(void* context, SceneManagerEvent event) {
    NfcMagicApp* instance = context;
    bool consumed = false;

    uint32_t state =
        scene_manager_get_scene_state(instance->scene_manager, NfcMagicSceneMfClassicDictAttack);
    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NfcMagicAppCustomEventDictAttackComplete) {
            if(state == DictAttackStateUserDictInProgress) {
                nfc_poller_stop(instance->poller);
                nfc_poller_free(instance->poller);
                keys_dict_free(instance->nfc_dict_context.dict);
                scene_manager_set_scene_state(
                    instance->scene_manager,
                    NfcMagicSceneMfClassicDictAttack,
                    DictAttackStateSystemDictInProgress);
                nfc_magic_scene_mf_classic_dict_attack_prepare_view(instance);
                instance->poller = nfc_poller_alloc(instance->nfc, NfcProtocolMfClassic);
                nfc_poller_start(instance->poller, nfc_dict_attack_worker_callback, instance);
                consumed = true;
            } else {
                nfc_magic_scene_mf_classic_dict_attack_notify_read(instance);
                if(instance->protocol == NfcMagicProtocolGen2) {
                    scene_manager_next_scene(instance->scene_manager, NfcMagicSceneGen2WriteCheck);
                } else {
                    scene_manager_next_scene(
                        instance->scene_manager, NfcMagicSceneMfClassicWriteCheck);
                }
                dolphin_deed(DolphinDeedNfcReadSuccess);
                consumed = true;
            }
        } else if(event.event == NfcMagicAppCustomEventCardDetected) {
            dict_attack_set_card_state(instance->dict_attack, true);
            consumed = true;
        } else if(event.event == NfcMagicAppCustomEventCardLost) {
            dict_attack_set_card_state(instance->dict_attack, false);
            consumed = true;
        } else if(event.event == NfcMagicAppCustomEventDictAttackDataUpdate) {
            nfc_magic_scene_mf_classic_dict_attack_update_view(instance);
        } else if(event.event == NfcMagicAppCustomEventDictAttackSkip) {
            const MfClassicData* mfc_data = nfc_poller_get_data(instance->poller);
            nfc_device_set_data(instance->target_dev, NfcProtocolMfClassic, mfc_data);
            if(state == DictAttackStateUserDictInProgress) {
                if(instance->nfc_dict_context.is_card_present) {
                    nfc_poller_stop(instance->poller);
                    nfc_poller_free(instance->poller);
                    keys_dict_free(instance->nfc_dict_context.dict);
                    scene_manager_set_scene_state(
                        instance->scene_manager,
                        NfcMagicSceneMfClassicDictAttack,
                        DictAttackStateSystemDictInProgress);
                    nfc_magic_scene_mf_classic_dict_attack_prepare_view(instance);
                    instance->poller = nfc_poller_alloc(instance->nfc, NfcProtocolMfClassic);
                    nfc_poller_start(instance->poller, nfc_dict_attack_worker_callback, instance);
                } else {
                    nfc_magic_scene_mf_classic_dict_attack_notify_read(instance);
                    if(instance->protocol == NfcMagicProtocolGen2) {
                        scene_manager_next_scene(
                            instance->scene_manager, NfcMagicSceneGen2WriteCheck);
                    } else {
                        scene_manager_next_scene(
                            instance->scene_manager, NfcMagicSceneMfClassicWriteCheck);
                    }
                    dolphin_deed(DolphinDeedNfcReadSuccess);
                }
                consumed = true;
            } else if(state == DictAttackStateSystemDictInProgress) {
                nfc_magic_scene_mf_classic_dict_attack_notify_read(instance);
                if(instance->protocol == NfcMagicProtocolGen2) {
                    scene_manager_next_scene(instance->scene_manager, NfcMagicSceneGen2WriteCheck);
                } else {
                    scene_manager_next_scene(
                        instance->scene_manager, NfcMagicSceneMfClassicWriteCheck);
                }
                dolphin_deed(DolphinDeedNfcReadSuccess);
                consumed = true;
            }
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_previous_scene(instance->scene_manager);
        consumed = true;
    }
    return consumed;
}

void nfc_magic_scene_mf_classic_dict_attack_on_exit(void* context) {
    NfcMagicApp* instance = context;
    const MfClassicData* mfc_data = nfc_poller_get_data(instance->poller);
    nfc_device_set_data(instance->target_dev, NfcProtocolMfClassic, mfc_data);

    nfc_poller_stop(instance->poller);
    nfc_poller_free(instance->poller);

    dict_attack_reset(instance->dict_attack);
    scene_manager_set_scene_state(
        instance->scene_manager,
        NfcMagicSceneMfClassicDictAttack,
        DictAttackStateUserDictInProgress);

    keys_dict_free(instance->nfc_dict_context.dict);

    instance->nfc_dict_context.current_sector = 0;
    instance->nfc_dict_context.sectors_total = 0;
    instance->nfc_dict_context.sectors_read = 0;
    instance->nfc_dict_context.keys_found = 0;
    instance->nfc_dict_context.dict_keys_total = 0;
    instance->nfc_dict_context.dict_keys_current = 0;
    instance->nfc_dict_context.is_key_attack = false;
    instance->nfc_dict_context.key_attack_current_sector = 0;
    instance->nfc_dict_context.is_card_present = false;

    nfc_magic_app_blink_stop(instance);
    notification_message(instance->notifications, &sequence_display_backlight_enforce_auto);
}
