#include "../nfc_app_i.h"

void nfc_scene_mf_desfire_read_success_widget_callback(
    GuiButtonType result,
    InputType type,
    void* context) {
    furi_assert(context);
    NfcApp* nfc = context;

    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(nfc->view_dispatcher, result);
    }
}

void nfc_scene_mf_desfire_read_success_on_enter(void* context) {
    NfcApp* nfc = context;

    // Setup view
    nfc->nfc_dev_data.protocol = NfcDevProtocolMfDesfire;
    MfDesfireData* data = &nfc->nfc_dev_data.mf_desfire_data;
    NfcaData* iso14443_3a_data = &data->iso14443_4a_data.iso14443_3a_data;
    Widget* widget = nfc->widget;

    FuriString* temp_str;

    temp_str = furi_string_alloc_printf("\e#MIFARE DESfire\n");
    furi_string_cat_printf(temp_str, "UID:");
    for(size_t i = 0; i < iso14443_3a_data->uid_len; i++) {
        furi_string_cat_printf(temp_str, " %02X", iso14443_3a_data->uid[i]);
    }

    uint32_t bytes_total = 1UL << (data->version.sw_storage >> 1);
    uint32_t bytes_free = data->free_memory.bytes_free;
    furi_string_cat_printf(temp_str, "\n%lu", bytes_total);
    if(data->version.sw_storage & 1) {
        furi_string_push_back(temp_str, '+');
    }
    furi_string_cat_printf(temp_str, " bytes, %lu bytes free\n", bytes_free);

    uint16_t n_apps = 0;
    uint16_t n_files = 0;
    for(MfDesfireApplication* app = data->app_head; app; app = app->next) {
        n_apps++;
        for(MfDesfireFile* file = app->file_head; file; file = file->next) {
            n_files++;
        }
    }
    furi_string_cat_printf(temp_str, "%d Application", n_apps);
    if(n_apps != 1) {
        furi_string_push_back(temp_str, 's');
    }
    furi_string_cat_printf(temp_str, ", %d file", n_files);
    if(n_files != 1) {
        furi_string_push_back(temp_str, 's');
    }

    widget_add_text_scroll_element(widget, 0, 0, 128, 52, furi_string_get_cstr(temp_str));
    furi_string_free(temp_str);

    widget_add_button_element(
        widget, GuiButtonTypeLeft, "Retry", nfc_scene_mf_desfire_read_success_widget_callback, nfc);
    widget_add_button_element(
        widget, GuiButtonTypeRight, "More", nfc_scene_mf_desfire_read_success_widget_callback, nfc);

    notification_message_block(nfc->notifications, &sequence_set_green_255);
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewWidget);
}

bool nfc_scene_mf_desfire_read_success_on_event(void* context, SceneManagerEvent event) {
    NfcApp* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeLeft) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneRetryConfirm);
            consumed = true;
        } else if(event.event == GuiButtonTypeRight) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneMfDesfireMenu);
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_next_scene(nfc->scene_manager, NfcSceneExitConfirm);
        consumed = true;
    }

    return consumed;
}

void nfc_scene_mf_desfire_read_success_on_exit(void* context) {
    NfcApp* nfc = context;

    notification_message_block(nfc->notifications, &sequence_reset_green);

    // Clear view
    widget_reset(nfc->widget);
}
