#include "../nfc_magic_i.h"

void nfc_magic_scene_file_select_on_enter(void* context) {
    NfcMagic* nfc_magic = context;
    // Process file_select return
    nfc_device_set_loading_callback(nfc_magic->nfc_dev, nfc_magic_show_loading_popup, nfc_magic);

    if(nfc_file_select(nfc_magic->nfc_dev)) {
        if(nfc_magic->nfc_dev->format == NfcDeviceSaveFormatMifareClassic) {
            scene_manager_next_scene(nfc_magic->scene_manager, NfcMagicSceneWriteConfirm);
        } else {
            scene_manager_next_scene(nfc_magic->scene_manager, NfcMagicSceneWrongCard);
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
    nfc_device_set_loading_callback(nfc_magic->nfc_dev, NULL, nfc_magic);
}
