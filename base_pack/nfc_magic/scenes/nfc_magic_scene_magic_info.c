#include "../nfc_magic_app_i.h"
#include "magic/nfc_magic_scanner.h"

void nfc_magic_scene_magic_info_widget_callback(
    GuiButtonType result,
    InputType type,
    void* context) {
    NfcMagicApp* instance = context;

    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(instance->view_dispatcher, result);
    }
}

void nfc_magic_scene_magic_info_on_enter(void* context) {
    NfcMagicApp* instance = context;
    Widget* widget = instance->widget;

    notification_message(instance->notifications, &sequence_success);

    FuriString* message = furi_string_alloc();

    if(instance->protocol == NfcMagicProtocolClassic) {
        widget_add_string_element(
            widget, 0, 0, AlignLeft, AlignTop, FontPrimary, "It Might Be a Magic Card");
        furi_string_printf(message, "You can make sure the card is\nmagic by writing to it\n");
    } else {
        widget_add_string_element(
            widget, 0, 0, AlignLeft, AlignTop, FontPrimary, "Magic card detected!");
    }
    furi_string_cat_printf(
        message, "Magic Type: %s", nfc_magic_protocols_get_name(instance->protocol));
    widget_add_text_box_element(
        widget, 0, 10, 128, 54, AlignLeft, AlignTop, furi_string_get_cstr(message), false);

    if(instance->protocol == NfcMagicProtocolGen4) {
        gen4_copy(instance->gen4_data, nfc_magic_scanner_get_gen4_data(instance->scanner));

        furi_string_printf(
            message,
            "Revision: %02X %02X\n",
            instance->gen4_data->revision.data[3],
            instance->gen4_data->revision.data[4]);

        widget_add_string_element(
            widget, 0, 20, AlignLeft, AlignTop, FontSecondary, furi_string_get_cstr(message));

        furi_string_printf(
            message,
            "Configured As:\n%s",
            gen4_get_configuration_name(&instance->gen4_data->config));

        widget_add_string_multiline_element(
            widget, 0, 30, AlignLeft, AlignTop, FontSecondary, furi_string_get_cstr(message));
    }

    widget_add_button_element(
        widget, GuiButtonTypeLeft, "Retry", nfc_magic_scene_magic_info_widget_callback, instance);
    widget_add_button_element(
        widget, GuiButtonTypeRight, "More", nfc_magic_scene_magic_info_widget_callback, instance);

    furi_string_free(message);

    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcMagicAppViewWidget);
}

bool nfc_magic_scene_magic_info_on_event(void* context, SceneManagerEvent event) {
    NfcMagicApp* instance = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeLeft) {
            consumed = scene_manager_previous_scene(instance->scene_manager);
        } else if(event.event == GuiButtonTypeRight) {
            if(instance->protocol == NfcMagicProtocolGen1) {
                scene_manager_next_scene(instance->scene_manager, NfcMagicSceneGen1Menu);
                consumed = true;
            } else if(instance->protocol == NfcMagicProtocolGen4) {
                scene_manager_next_scene(instance->scene_manager, NfcMagicSceneGen4Menu);
                consumed = true;
            } else if(instance->protocol == NfcMagicProtocolGen2) {
                scene_manager_next_scene(instance->scene_manager, NfcMagicSceneGen2Menu);
                consumed = true;
            } else if(instance->protocol == NfcMagicProtocolClassic) {
                scene_manager_next_scene(instance->scene_manager, NfcMagicSceneMfClassicMenu);
                consumed = true;
            }
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = scene_manager_search_and_switch_to_previous_scene(
            instance->scene_manager, NfcMagicSceneStart);
    }
    return consumed;
}

void nfc_magic_scene_magic_info_on_exit(void* context) {
    NfcMagicApp* instance = context;

    widget_reset(instance->widget);
}
