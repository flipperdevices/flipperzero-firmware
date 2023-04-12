#include "../nfc_i.h"

void nfc_scene_felica_read_success_widget_callback(
    GuiButtonType result,
    InputType type,
    void* context) {
    furi_assert(context);
    Nfc* nfc = context;

    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(nfc->view_dispatcher, result);
    }
}

void nfc_scene_felica_read_success_on_enter(void* context) {
    Nfc* nfc = context;
    FelicaData* felica_data = &nfc->dev->dev_data.felica_data;

    // Setup view
    Widget* widget = nfc->widget;
    widget_add_button_element(
        widget, GuiButtonTypeLeft, "Retry", nfc_scene_felica_read_success_widget_callback, nfc);
    widget_add_button_element(
        widget, GuiButtonTypeRight, "More", nfc_scene_felica_read_success_widget_callback, nfc);

    FuriString* temp_str = NULL;
    if(furi_string_size(nfc->dev->dev_data.parsed_data)) {
        temp_str = furi_string_alloc_set(nfc->dev->dev_data.parsed_data);
    } else {
        temp_str = furi_string_alloc_printf("\e#%s", nfc_felica_type(felica_data->type));

        furi_string_cat_printf(temp_str, "\nCIN:");
        for(size_t i = 2; i < nfc->dev->dev_data.nfc_data.uid_len; i++) {
            furi_string_cat_printf(temp_str, " %02X", nfc->dev->dev_data.nfc_data.uid[i]);
        }

        size_t num_systems = FelicaSystemArray_size(felica_data->systems);
        furi_string_cat_printf(temp_str, "\n%zd system", num_systems);
        if(num_systems != 1) {
            furi_string_cat_printf(temp_str, "s");
        }
    }

    widget_add_text_scroll_element(widget, 0, 0, 128, 52, furi_string_get_cstr(temp_str));
    furi_string_free(temp_str);

    notification_message_block(nfc->notifications, &sequence_set_green_255);

    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewWidget);
}

bool nfc_scene_felica_read_success_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeLeft) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneRetryConfirm);
            consumed = true;
        } else if(event.event == GuiButtonTypeRight) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneFelicaMenu);
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_next_scene(nfc->scene_manager, NfcSceneExitConfirm);
        consumed = true;
    }
    return consumed;
}

void nfc_scene_felica_read_success_on_exit(void* context) {
    Nfc* nfc = context;

    notification_message_block(nfc->notifications, &sequence_reset_green);

    // Clear view
    widget_reset(nfc->widget);
}
