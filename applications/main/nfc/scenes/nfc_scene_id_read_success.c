#include "../nfc_i.h"
#include <dolphin/dolphin.h>

void nfc_scene_id_read_success_widget_callback(GuiButtonType result, InputType type, void* context) {
    Nfc* nfc = context;
    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(nfc->view_dispatcher, result);
    }
}

void nfc_scene_id_read_success_on_enter(void* context) {
    Nfc* nfc = context;
    FuriHalNfcDevData data = nfc->dev->dev_data.nfc_data;
    DOLPHIN_DEED(DolphinDeedNfcReadSuccess);

    // Setup Custom Widget view
    widget_add_button_element(
        nfc->widget, GuiButtonTypeLeft, "Retry", nfc_scene_id_read_success_widget_callback, nfc);

    notification_message_block(nfc->notifications, &sequence_set_green_255);

    widget_add_string_element(
        nfc->widget, 64, 12, AlignCenter, AlignBottom, FontPrimary, "Passport or ID card");

    FuriString* temp_str;
    temp_str = furi_string_alloc();
    if(data.type == FuriHalNfcTypeA) {
        furi_string_set_str(temp_str, "NFC-A UID:");
    } else if(data.type == FuriHalNfcTypeB) {
        furi_string_set_str(temp_str, "NFC-B UID:");
    }
    for(uint8_t i = 0; i < data.uid_len; i++) {
        furi_string_cat_printf(temp_str, " %02X", data.uid[i]);
    }
    widget_add_string_element(
        nfc->widget,
        64,
        28,
        AlignCenter,
        AlignCenter,
        FontSecondary,
        furi_string_get_cstr(temp_str));
    widget_add_string_element(
        nfc->widget, 64, 40, AlignCenter, AlignCenter, FontSecondary, "(Probably random)");
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewWidget);
}

bool nfc_scene_id_read_success_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeLeft) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneRetryConfirm);
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_next_scene(nfc->scene_manager, NfcSceneExitConfirm);
        consumed = true;
    }
    return consumed;
}

void nfc_scene_id_read_success_on_exit(void* context) {
    Nfc* nfc = context;

    notification_message_block(nfc->notifications, &sequence_reset_green);

    // Clear view
    widget_reset(nfc->widget);
}
