#include "../nfc_i.h"
#include "nfc_device.h"

void nfc_scene_nfcf_read_success_widget_callback(
    GuiButtonType result,
    InputType type,
    void* context) {
    furi_assert(context);
    Nfc* nfc = context;

    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(nfc->view_dispatcher, result);
    }
}

void nfc_scene_nfcf_read_success_on_enter(void* context) {
    Nfc* nfc = context;

    FuriHalNfcDevData* data = &nfc->dev->dev_data.nfc_data;
    NfcDeviceData* dev_data = &nfc->dev->dev_data;

    // Setup view
    Widget* widget = nfc->widget;
    widget_add_button_element(
        widget, GuiButtonTypeLeft, "Retry", nfc_scene_nfcf_read_success_widget_callback, nfc);
    widget_add_button_element(
        widget, GuiButtonTypeRight, "More", nfc_scene_nfcf_read_success_widget_callback, nfc);

    FuriString* temp_str = furi_string_alloc();

    if(dev_data->protocol == NfcDeviceProtocolFelica) {
        furi_string_cat_printf(temp_str, "\e#%s", nfc_felica_type(dev_data->felica_data.type));
    } else {
        furi_string_cat_printf(temp_str, "\e#Unknown ISO tag");
    }

    furi_string_cat_printf(temp_str, "\nISO 18092 (NFC-F)");

    furi_string_cat_printf(temp_str, "\nCIN:");
    // NFC-F Card Identification Number (CIN) starts at "UID" byte 2.
    for(size_t i = 2; i < data->uid_len; i++) {
        furi_string_cat_printf(temp_str, " %02X", data->uid[i]);
    }

    // The first 2 bytes of the "UID" are Manufacturer Code (MC)
    furi_string_cat_printf(
        temp_str,
        "\nMC: %02X %02X  ROM: %02X  IC: %02X",
        data->uid[0],
        data->uid[1],
        data->f_data.pmm[0],
        data->f_data.pmm[1]);

    widget_add_text_scroll_element(widget, 0, 0, 128, 52, furi_string_get_cstr(temp_str));
    furi_string_free(temp_str);

    notification_message_block(nfc->notifications, &sequence_set_green_255);

    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewWidget);
}

bool nfc_scene_nfcf_read_success_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeLeft) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneRetryConfirm);
            consumed = true;
        } else if(event.event == GuiButtonTypeRight) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneNfcfMenu);
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_next_scene(nfc->scene_manager, NfcSceneExitConfirm);
        consumed = true;
    }
    return consumed;
}

void nfc_scene_nfcf_read_success_on_exit(void* context) {
    Nfc* nfc = context;

    notification_message_block(nfc->notifications, &sequence_reset_green);

    // Clear view
    widget_reset(nfc->widget);
}
