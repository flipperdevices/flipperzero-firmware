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

        FelicaSystemList_it_t it;
        for(FelicaSystemList_it(it, felica_data->systems); !FelicaSystemList_end_p(it);
            FelicaSystemList_next(it)) {
            FelicaSystem* current_system = *FelicaSystemList_ref(it);
            furi_string_cat_printf(
                temp_str, "\nSystem %04X (#%d):", current_system->code, current_system->number);
            furi_string_cat_printf(temp_str, "\nIDm:\n    ");
            for(size_t i = 0; i < 8; i++) {
                furi_string_cat_printf(temp_str, "%02X", current_system->idm[i]);
            }
            furi_string_cat_printf(temp_str, "\nPMm:\n    ");
            for(size_t i = 0; i < 8; i++) {
                furi_string_cat_printf(temp_str, "%02X", current_system->pmm[i]);
            }
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
