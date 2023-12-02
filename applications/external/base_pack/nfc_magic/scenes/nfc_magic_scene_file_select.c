#include "../nfc_magic_app_i.h"
#include <nfc/protocols/mf_classic/mf_classic.h>

static bool nfc_magic_scene_file_select_is_file_suitable(NfcMagicApp* instance) {
    NfcProtocol protocol = nfc_device_get_protocol(instance->source_dev);
    size_t uid_len = 0;
    nfc_device_get_uid(instance->source_dev, &uid_len);

    bool suitable = false;
    if(instance->protocol == NfcMagicProtocolGen1) {
        if((uid_len == 4) && (protocol == NfcProtocolMfClassic)) {
            const MfClassicData* mfc_data =
                nfc_device_get_data(instance->source_dev, NfcProtocolMfClassic);
            if(mfc_data->type == MfClassicType1k) {
                suitable = true;
            }
        }
    } else if(instance->protocol == NfcMagicProtocolGen4) {
        if(protocol == NfcProtocolMfClassic) {
            suitable = true;
        } else if(protocol == NfcProtocolMfUltralight) {
            const MfUltralightData* mfu_data =
                nfc_device_get_data(instance->source_dev, NfcProtocolMfUltralight);
            const Iso14443_3aData* iso3_data = mfu_data->iso14443_3a_data;
            if(iso3_data->uid_len == 7) {
                MfUltralightType mfu_type = mfu_data->type;
                suitable = (mfu_type != MfUltralightTypeNTAGI2C1K) &&
                           (mfu_type != MfUltralightTypeNTAGI2C2K) &&
                           (mfu_type != MfUltralightTypeNTAGI2CPlus1K) &&
                           (mfu_type != MfUltralightTypeNTAGI2CPlus2K);
            }
        }
    }

    return suitable;
}

void nfc_magic_scene_file_select_on_enter(void* context) {
    NfcMagicApp* instance = context;

    if(nfc_magic_load_from_file_select(instance)) {
        if(nfc_magic_scene_file_select_is_file_suitable(instance)) {
            scene_manager_next_scene(instance->scene_manager, NfcMagicSceneWriteConfirm);
        } else {
            scene_manager_next_scene(instance->scene_manager, NfcMagicSceneWrongCard);
        }
    } else {
        scene_manager_previous_scene(instance->scene_manager);
    }
}

bool nfc_magic_scene_file_select_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void nfc_magic_scene_file_select_on_exit(void* context) {
    UNUSED(context);
}
