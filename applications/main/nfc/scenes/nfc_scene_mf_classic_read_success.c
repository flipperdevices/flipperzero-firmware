#include "../nfc_app_i.h"

void nfc_scene_mf_classic_read_success_widget_callback(
    GuiButtonType result,
    InputType type,
    void* context) {
    furi_assert(context);
    NfcApp* nfc = context;

    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(nfc->view_dispatcher, result);
    }
}

void nfc_scene_mf_classic_read_success_on_enter(void* context) {
    NfcApp* nfc = context;
    MfClassicData* mfc_data = &nfc->nfc_dev_data.mf_classic_data;

    // Setup view
    Widget* widget = nfc->widget;
    widget_add_button_element(
        widget, GuiButtonTypeLeft, "Retry", nfc_scene_mf_classic_read_success_widget_callback, nfc);
    widget_add_button_element(
        widget, GuiButtonTypeRight, "More", nfc_scene_mf_classic_read_success_widget_callback, nfc);

    FuriString* temp_str = NULL;
    // if(furi_string_size(nfc->dev->dev_data.parsed_data)) {
    //     temp_str = furi_string_alloc_set(nfc->dev->dev_data.parsed_data);
    // } else {
        // temp_str = furi_string_alloc_printf("\e#%s\n", nfc_mf_classic_type(mfc_data->type));
        furi_string_cat_printf(temp_str, "UID:");
        for(size_t i = 0; i < mfc_data->nfca_data.uid_len; i++) {
            furi_string_cat_printf(temp_str, " %02X", mfc_data->nfca_data.uid[i]);
        }
        uint8_t sectors_total = mf_classic_get_total_sectors_num(mfc_data->type);
        uint8_t keys_total = sectors_total * 2;
        uint8_t keys_found = 0;
        uint8_t sectors_read = 0;
        mf_classic_get_read_sectors_and_keys(mfc_data, &sectors_read, &keys_found);
        furi_string_cat_printf(temp_str, "\nKeys Found: %d/%d", keys_found, keys_total);
        furi_string_cat_printf(temp_str, "\nSectors Read: %d/%d", sectors_read, sectors_total);
    // }

    widget_add_text_scroll_element(widget, 0, 0, 128, 52, furi_string_get_cstr(temp_str));
    furi_string_free(temp_str);

    notification_message_block(nfc->notifications, &sequence_set_green_255);

    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewWidget);
}

bool nfc_scene_mf_classic_read_success_on_event(void* context, SceneManagerEvent event) {
    NfcApp* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeLeft) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneRetryConfirm);
            consumed = true;
        } else if(event.event == GuiButtonTypeRight) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneNotImplemented);
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_next_scene(nfc->scene_manager, NfcSceneExitConfirm);
        consumed = true;
    }

    return consumed;
}

void nfc_scene_mf_classic_read_success_on_exit(void* context) {
    NfcApp* nfc = context;

    notification_message_block(nfc->notifications, &sequence_reset_green);

    // Clear view
    widget_reset(nfc->widget);
}
