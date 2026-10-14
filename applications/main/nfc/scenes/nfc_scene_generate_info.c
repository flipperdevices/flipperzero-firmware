#include "../nfc_app_i.h"

void nfc_scene_generate_info_widget_callback(GuiButtonType result, InputType type, void* context) {
    NfcApp* instance = context;

    if(type == InputTypeShort) {
        if(result == GuiButtonTypeRight) {
            view_dispatcher_send_custom_event(instance->view_dispatcher, result);
        }
    }
}

void nfc_scene_generate_info_on_enter(void* context) {
    NfcApp* instance = context;

    NfcProtocol protocol = nfc_device_get_protocol(instance->nfc_device);

    const char* type_cstr;
    FuriString* id_str;

    switch(protocol) {
    case NfcProtocolMfUltralight:
    case NfcProtocolMfClassic: {
        const Iso14443_3aData* iso14443_3a_data =
            nfc_device_get_data(instance->nfc_device, NfcProtocolIso14443_3a);
        type_cstr = "NFC-A";
        id_str = furi_string_alloc_printf("UID:");
        // Append UID
        for(int i = 0; i < iso14443_3a_data->uid_len; i++) {
            furi_string_cat_printf(id_str, " %02X", iso14443_3a_data->uid[i]);
        }
        break;
    }

    case NfcProtocolFelica: {
        const FelicaData* felica_data =
            nfc_device_get_data(instance->nfc_device, NfcProtocolFelica);
        type_cstr = "FeliCa";
        id_str = furi_string_alloc_printf("IDm:");
        for(unsigned int i = 0; i < FELICA_IDM_SIZE; i++) {
            furi_string_cat_printf(id_str, " %02X", felica_data->idm.data[i]);
        }
        break;
    }

    default:
        furi_crash("NFC: Unhandled protocol.");
    }

    // Setup dialog view
    Widget* widget = instance->widget;
    widget_add_button_element(
        widget, GuiButtonTypeRight, "More", nfc_scene_generate_info_widget_callback, instance);

    // Create info text
    NfcDataGeneratorType type =
        scene_manager_get_scene_state(instance->scene_manager, NfcSceneGenerateInfo);
    const char* name = nfc_data_generator_get_name(type);
    widget_add_string_element(widget, 0, 0, AlignLeft, AlignTop, FontPrimary, name);
    widget_add_string_element(widget, 0, 13, AlignLeft, AlignTop, FontSecondary, type_cstr);

    widget_add_string_element(
        widget, 0, 25, AlignLeft, AlignTop, FontSecondary, furi_string_get_cstr(id_str));
    furi_string_free(id_str);

    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcViewWidget);
}

bool nfc_scene_generate_info_on_event(void* context, SceneManagerEvent event) {
    NfcApp* instance = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeRight) {
            scene_manager_next_scene(instance->scene_manager, NfcSceneReadMenu);
            consumed = true;
        }
    }

    return consumed;
}

void nfc_scene_generate_info_on_exit(void* context) {
    NfcApp* instance = context;

    // Clean views
    widget_reset(instance->widget);
}
