#include "../nfc_magic_i.h"

#define TAG "Magic"

static bool nfc_magic_scene_file_select_is_file_suitable(NfcMagic* nfc_magic) {
    NfcDevice* nfc_dev = nfc_magic->source_dev;
    if((nfc_dev->dev_data.protocol != NfcDeviceProtocolMifareClassic) &&
       (nfc_dev->dev_data.protocol != NfcDeviceProtocolMifareUl)) {
        FURI_LOG_D(TAG, "Protocol not supported");
        FURI_LOG_D(TAG, "Protocol: %d", nfc_dev->dev_data.protocol);
        return false;
    }
    if(nfc_dev->dev_data.protocol == NfcDeviceProtocolMifareUl) {
        switch(nfc_dev->dev_data.mf_ul_data.type) {
        case MfUltralightTypeNTAGI2C1K:
        case MfUltralightTypeNTAGI2C2K:
        case MfUltralightTypeNTAGI2CPlus1K:
        case MfUltralightTypeNTAGI2CPlus2K:
            FURI_LOG_D(TAG, "Ultralight type not supported");
            return false;
        default:
            FURI_LOG_D(TAG, "Ultralight type supported");
            return true;
        }
    }

    FURI_LOG_D(TAG, "Mifare classic type supported");
    return true;
}

void nfc_magic_scene_file_select_on_enter(void* context) {
    NfcMagic* nfc_magic = context;
    // Process file_select return
    nfc_device_set_loading_callback(
        nfc_magic->source_dev, nfc_magic_show_loading_popup, nfc_magic);

    if(!furi_string_size(nfc_magic->source_dev->load_path)) {
        furi_string_set_str(nfc_magic->source_dev->load_path, NFC_APP_FOLDER);
    }
    if(nfc_file_select(nfc_magic->source_dev)) {
        if(nfc_magic_scene_file_select_is_file_suitable(nfc_magic)) {
            scene_manager_next_scene(nfc_magic->scene_manager, NfcMagicSceneCheck);
        } else {
            scene_manager_next_scene(nfc_magic->scene_manager, NfcMagicSceneFileNotSupported);
        }
    } else {
        scene_manager_previous_scene(nfc_magic->scene_manager);
    }
}

bool nfc_magic_scene_file_select_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void nfc_magic_scene_file_select_on_exit(void* context) {
    NfcMagic* nfc_magic = context;
    nfc_device_set_loading_callback(nfc_magic->source_dev, NULL, nfc_magic);
}
