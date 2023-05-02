#include "../nfc_app_i.h"

void nfc_scene_generate_info_widget_callback(GuiButtonType result, InputType type, void* context) {
    NfcApp* nfc = context;

    if(type == InputTypeShort) {
        if(result == GuiButtonTypeRight) {
            view_dispatcher_send_custom_event(nfc->view_dispatcher, result);
        }
    }
}

void nfc_scene_generate_info_on_enter(void* context) {
    NfcApp* nfc = context;

    NfcaData* nfca_data = NULL;
    if(nfc->nfc_dev_data.protocol == NfcDevProtocolMfUltralight) {
        nfca_data = &nfc->nfc_dev_data.mf_ul_data.nfca_data;
    } else {
        // TODO add Mf Classic
        furi_crash("Not supported protocol");
    }

    // Setup dialog view
    Widget* widget = nfc->widget;
    widget_add_button_element(
        widget, GuiButtonTypeRight, "More", nfc_scene_generate_info_widget_callback, nfc);

    // Create info text
    NfcDataGeneratorType type =
        scene_manager_get_scene_state(nfc->scene_manager, NfcSceneGenerateInfo);
    const char* name = nfc_data_generator_get_name(type);
    widget_add_string_element(widget, 0, 0, AlignLeft, AlignTop, FontPrimary, name);
    widget_add_string_element(widget, 0, 13, AlignLeft, AlignTop, FontSecondary, "NFC-A");

    FuriString* temp_str = furi_string_alloc_printf("UID:");
    // Append UID
    for(int i = 0; i < nfca_data->uid_len; i++) {
        furi_string_cat_printf(temp_str, " %02X", nfca_data->uid[i]);
    }
    widget_add_string_element(
        widget, 0, 25, AlignLeft, AlignTop, FontSecondary, furi_string_get_cstr(temp_str));
    furi_string_free(temp_str);

    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewWidget);
}

bool nfc_scene_generate_info_on_event(void* context, SceneManagerEvent event) {
    NfcApp* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeRight) {
            // Switch either to NfcSceneMfClassicMenu or NfcSceneMfUltralightMenu
            if(nfc->nfc_dev_data.protocol == NfcDevProtocolMfUltralight) {
                scene_manager_next_scene(nfc->scene_manager, NfcSceneMfUltralightMenu);
            } else {
                // TODO add classic
            }
            consumed = true;
        }
    }

    return consumed;
}

void nfc_scene_generate_info_on_exit(void* context) {
    NfcApp* nfc = context;

    // Clean views
    widget_reset(nfc->widget);
}
