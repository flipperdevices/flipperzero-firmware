#include "../nfc_magic_i.h"

void nfc_magic_scene_file_select_on_enter(void* context) {
    NfcMagic* nfc_magic = context;
    // Process file_select return
    nfc_device_set_loading_callback(nfc_magic->nfc_dev, nfc_magic_show_loading_popup, nfc_magic);

    uint32_t next_scene = NfcMagicSceneWrongCard;
    if(nfc_file_select(nfc_magic->nfc_dev)) {
        if(nfc_magic->nfc_dev->format == NfcDeviceSaveFormatMifareClassic) {
            scene_manager_set_scene_state(nfc_magic->scene_manager, NfcMagicSceneWriteMenu, 0);
            next_scene = NfcMagicSceneWriteMenu;
        }
    }
    scene_manager_next_scene(nfc_magic->scene_manager, next_scene);
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
