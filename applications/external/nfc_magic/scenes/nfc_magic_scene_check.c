#include "../nfc_magic_i.h"

enum {
    NfcMagicSceneCheckStateCardSearch,
    NfcMagicSceneCheckStateCardFound,
};

static bool nfc_magic_scene_check_is_file_suitable(NfcMagic* nfc_magic) {
    NfcDevice* nfc_dev = nfc_magic->source_dev;
    if(nfc_dev->format == NfcDeviceSaveFormatMifareClassic) {
        switch(nfc_magic->dev->type) {
        case MagicTypeClassicGen1:
        case MagicTypeClassicDirectWrite:
        case MagicTypeClassicAPDU:
            if((nfc_dev->dev_data.mf_classic_data.type != MfClassicType1k) ||
               (nfc_dev->dev_data.nfc_data.uid_len != nfc_magic->dev->uid_len)) {
                return false;
            }
            return true;

        case MagicTypeGen4:
            return true;
        default:
            return false;
        }
    } else if(
        (nfc_dev->format == NfcDeviceSaveFormatMifareUl) &&
        (nfc_dev->dev_data.nfc_data.uid_len == 7)) {
        switch(nfc_magic->dev->type) {
        case MagicTypeUltralightGen1:
        case MagicTypeUltralightDirectWrite:
        case MagicTypeUltralightC_Gen1:
        case MagicTypeUltralightC_DirectWrite:
        case MagicTypeGen4:
            switch(nfc_dev->dev_data.mf_ul_data.type) {
            case MfUltralightTypeNTAGI2C1K:
            case MfUltralightTypeNTAGI2C2K:
            case MfUltralightTypeNTAGI2CPlus1K:
            case MfUltralightTypeNTAGI2CPlus2K:
                return false;
            default:
                return true;
            }
        default:
            return false;
        }
    }

    return false;
}

bool nfc_magic_check_worker_callback(NfcMagicWorkerEvent event, void* context) {
    furi_assert(context);

    NfcMagic* nfc_magic = context;
    view_dispatcher_send_custom_event(nfc_magic->view_dispatcher, event);

    return true;
}

static void nfc_magic_scene_check_setup_view(NfcMagic* nfc_magic) {
    Popup* popup = nfc_magic->popup;
    popup_reset(popup);
    uint32_t state = scene_manager_get_scene_state(nfc_magic->scene_manager, NfcMagicSceneCheck);

    if(state == NfcMagicSceneCheckStateCardSearch) {
        popup_set_icon(nfc_magic->popup, 0, 8, &I_NFC_manual_60x50);
        popup_set_text(
            nfc_magic->popup, "Apply card to\nthe back", 128, 32, AlignRight, AlignCenter);
    } else {
        popup_set_icon(popup, 12, 23, &I_Loading_24);
        popup_set_header(popup, "Checking\nDon't move...", 52, 32, AlignLeft, AlignCenter);
    }

    view_dispatcher_switch_to_view(nfc_magic->view_dispatcher, NfcMagicViewPopup);
}

void nfc_magic_scene_check_on_enter(void* context) {
    NfcMagic* nfc_magic = context;

    scene_manager_set_scene_state(
        nfc_magic->scene_manager, NfcMagicSceneCheck, NfcMagicSceneCheckStateCardSearch);
    nfc_magic_scene_check_setup_view(nfc_magic);

    // Setup and start worker
    nfc_magic_worker_start(
        nfc_magic->worker,
        NfcMagicWorkerStateCheck,
        nfc_magic->dev,
        &nfc_magic->source_dev->dev_data,
        nfc_magic->new_password,
        nfc_magic_check_worker_callback,
        nfc_magic);
    nfc_magic_blink_start(nfc_magic);
}

bool nfc_magic_scene_check_on_event(void* context, SceneManagerEvent event) {
    NfcMagic* nfc_magic = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NfcMagicWorkerEventSuccess) {
            if(nfc_magic_scene_check_is_file_suitable(nfc_magic)) {
                if(nfc_magic->dev->type == MagicTypeClassicGen1) {
                    scene_manager_next_scene(nfc_magic->scene_manager, NfcMagicSceneWriteConfirm);
                } else {
                    scene_manager_next_scene(nfc_magic->scene_manager, NfcMagicSceneWrite);
                }
            } else {
                scene_manager_next_scene(nfc_magic->scene_manager, NfcMagicSceneWrongCard);
            }
            consumed = true;
        } else if(event.event == NfcMagicWorkerEventNotMagic) {
            scene_manager_next_scene(nfc_magic->scene_manager, NfcMagicSceneNotMagic);
            consumed = true;
        } else if(event.event == NfcMagicWorkerEventWrongCard) {
            scene_manager_next_scene(nfc_magic->scene_manager, NfcMagicSceneWrongCard);
            consumed = true;
        } else if(event.event == NfcMagicWorkerEventCardDetected) {
            scene_manager_set_scene_state(
                nfc_magic->scene_manager, NfcMagicSceneCheck, NfcMagicSceneCheckStateCardFound);
            nfc_magic_scene_check_setup_view(nfc_magic);
            consumed = true;
        } else if(event.event == NfcMagicWorkerEventNoCardDetected) {
            scene_manager_set_scene_state(
                nfc_magic->scene_manager, NfcMagicSceneCheck, NfcMagicSceneCheckStateCardSearch);
            nfc_magic_scene_check_setup_view(nfc_magic);
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = scene_manager_search_and_switch_to_previous_scene(
            nfc_magic->scene_manager, NfcMagicSceneFileSelect);
    }
    return consumed;
}

void nfc_magic_scene_check_on_exit(void* context) {
    NfcMagic* nfc_magic = context;

    nfc_magic_worker_stop(nfc_magic->worker);
    scene_manager_set_scene_state(
        nfc_magic->scene_manager, NfcMagicSceneCheck, NfcMagicSceneCheckStateCardSearch);
    // Clear view
    popup_reset(nfc_magic->popup);

    nfc_magic_blink_stop(nfc_magic);
}
