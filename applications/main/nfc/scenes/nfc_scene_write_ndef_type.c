#include "../nfc_app_i.h"

enum SubmenuIndex {
    SubmenuIndexUri = NdefWriteRecordTypeUri,
    SubmenuIndexEmail = NdefWriteRecordTypeEmail,
    SubmenuIndexPhone = NdefWriteRecordTypePhone,
    SubmenuIndexText = NdefWriteRecordTypeText,
    SubmenuIndexWifi = NdefWriteRecordTypeWifi,
};

static void nfc_scene_write_ndef_type_submenu_callback(void* context, uint32_t index) {
    NfcApp* instance = context;
    view_dispatcher_send_custom_event(instance->view_dispatcher, index);
}

void nfc_scene_write_ndef_type_on_enter(void* context) {
    NfcApp* instance = context;
    Submenu* submenu = instance->submenu;

    submenu_add_item(
        submenu, "URL", SubmenuIndexUri, nfc_scene_write_ndef_type_submenu_callback, instance);
    submenu_add_item(
        submenu, "Email", SubmenuIndexEmail, nfc_scene_write_ndef_type_submenu_callback, instance);
    submenu_add_item(
        submenu, "Phone", SubmenuIndexPhone, nfc_scene_write_ndef_type_submenu_callback, instance);
    submenu_add_item(
        submenu, "Text", SubmenuIndexText, nfc_scene_write_ndef_type_submenu_callback, instance);
    submenu_add_item(
        submenu,
        "Wi-Fi (WPA2)",
        SubmenuIndexWifi,
        nfc_scene_write_ndef_type_submenu_callback,
        instance);

    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(instance->scene_manager, NfcSceneWriteNdefType));
    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcViewMenu);
}

bool nfc_scene_write_ndef_type_on_event(void* context, SceneManagerEvent event) {
    NfcApp* instance = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        instance->ndef_write.record_type = (NdefWriteRecordType)event.event;
        instance->ndef_write.input_step = NdefWriteInputStepPrimary;
        furi_string_reset(instance->ndef_write.primary);
        furi_string_reset(instance->ndef_write.secondary);
        scene_manager_set_scene_state(instance->scene_manager, NfcSceneWriteNdefType, event.event);
        scene_manager_next_scene(instance->scene_manager, NfcSceneWriteNdefInput);
        consumed = true;
    }

    return consumed;
}

void nfc_scene_write_ndef_type_on_exit(void* context) {
    NfcApp* instance = context;
    submenu_reset(instance->submenu);
}
