#include "../nfc_app_i.h"

#include "../helpers/format/nfc_protocol_format.h"

static void
    nfc_scene_read_success_widget_callback(GuiButtonType result, InputType type, void* context) {
    furi_assert(context);
    NfcApp* nfc = context;

    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(nfc->view_dispatcher, result);
    }
}

void nfc_scene_read_success_on_enter(void* context) {
    NfcApp* nfc = context;
    Widget* widget = nfc->widget;

    FuriString* temp_str = furi_string_alloc();
    nfc_protocol_format_info(nfc->nfc_dev, NfcProtocolFormatTypeShort, temp_str);

    widget_add_text_scroll_element(widget, 0, 0, 128, 52, furi_string_get_cstr(temp_str));
    furi_string_free(temp_str);

    widget_add_button_element(
        widget, GuiButtonTypeLeft, "Retry", nfc_scene_read_success_widget_callback, nfc);
    widget_add_button_element(
        widget, GuiButtonTypeRight, "More", nfc_scene_read_success_widget_callback, nfc);

    notification_message_block(nfc->notifications, &sequence_set_green_255);
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewWidget);
}

static const NfcScene nfc_scene_read_success_menus[NfcProtocolTypeMax] = {
    [NfcProtocolTypeIso14443_3a] = NfcSceneNfcaMenu,
    [NfcProtocolTypeIso14443_4a] = NfcSceneNotImplemented, //TODO: ISO14443-4A menu
    [NfcProtocolTypeMfUltralight] = NfcSceneMfUltralightMenu,
    [NfcProtocolTypeMfClassic] = NfcSceneMfUltralightMenu,
    [NfcProtocolTypeMfDesfire] = NfcSceneMfDesfireMenu,
};

bool nfc_scene_read_success_on_event(void* context, SceneManagerEvent event) {
    NfcApp* nfc = context;

    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeLeft) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneRetryConfirm);
            consumed = true;
        } else if(event.event == GuiButtonTypeRight) {
            const NfcProtocolType protocol_type = nfc_dev_get_protocol_type(nfc->nfc_dev);
            const NfcScene menu_scene = nfc_scene_read_success_menus[protocol_type];
            scene_manager_next_scene(nfc->scene_manager, menu_scene);
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_next_scene(nfc->scene_manager, NfcSceneExitConfirm);
        consumed = true;
    }

    return consumed;
}

void nfc_scene_read_success_on_exit(void* context) {
    NfcApp* nfc = context;

    notification_message_block(nfc->notifications, &sequence_reset_green);
    widget_reset(nfc->widget);
}
