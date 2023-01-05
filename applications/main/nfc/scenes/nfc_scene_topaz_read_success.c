#include "../nfc_i.h"

void nfc_scene_topaz_read_success_widget_callback(
    GuiButtonType result,
    InputType type,
    void* context) {
    Nfc* nfc = context;

    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(nfc->view_dispatcher, result);
    }
}

void nfc_scene_topaz_read_success_on_enter(void* context) {
    Nfc* nfc = context;

    // Setup widget view
    FuriHalNfcDevData* data = &nfc->dev->dev_data.nfc_data;
    TopazData* topaz_data = &nfc->dev->dev_data.topaz_data;
    Widget* widget = nfc->widget;
    widget_add_button_element(
        widget, GuiButtonTypeLeft, "Retry", nfc_scene_topaz_read_success_widget_callback, nfc);
    widget_add_button_element(
        widget, GuiButtonTypeRight, "More", nfc_scene_topaz_read_success_widget_callback, nfc);

    FuriString* temp_str = NULL;
    if(furi_string_size(nfc->dev->dev_data.parsed_data)) {
        temp_str = furi_string_alloc_set(nfc->dev->dev_data.parsed_data);
    } else {
        temp_str = furi_string_alloc_printf("\e#%s\n", nfc_topaz_type(topaz_data->type));
        furi_string_cat_printf(temp_str, "UID:");
        for(size_t i = 0; i < data->uid_len; i++) {
            furi_string_cat_printf(temp_str, " %02X", data->uid[i]);
        }
        furi_string_cat_printf(
            temp_str, "\nHR0: %02X\nHR1: %02X", topaz_data->hr[0], topaz_data->hr[1]);
    }
    widget_add_text_scroll_element(widget, 0, 0, 128, 52, furi_string_get_cstr(temp_str));
    furi_string_free(temp_str);

    notification_message_block(nfc->notifications, &sequence_set_green_255);

    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewWidget);
}

bool nfc_scene_topaz_read_success_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeLeft) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneRetryConfirm);
            consumed = true;
        } else if(event.event == GuiButtonTypeRight) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneTopazMenu);
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_next_scene(nfc->scene_manager, NfcSceneExitConfirm);
        consumed = true;
    }

    return consumed;
}

void nfc_scene_topaz_read_success_on_exit(void* context) {
    Nfc* nfc = context;

    notification_message_block(nfc->notifications, &sequence_reset_green);

    // Clean view
    widget_reset(nfc->widget);
}
