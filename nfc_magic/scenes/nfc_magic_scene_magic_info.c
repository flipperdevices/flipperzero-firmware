#include "../nfc_magic_app_i.h"

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

    widget_add_icon_element(widget, 84, 22, &I_WarningDolphinFlip_45x42);
    widget_add_string_element(
        widget, 3, 4, AlignLeft, AlignTop, FontPrimary, "Magic card detected");
    widget_add_string_element(
        widget,
        3,
        17,
        AlignLeft,
        AlignTop,
        FontSecondary,
        nfc_magic_protocols_get_name(instance->protocol));
    widget_add_button_element(
        widget, GuiButtonTypeLeft, "Retry", nfc_magic_scene_magic_info_widget_callback, instance);
    widget_add_button_element(
        widget, GuiButtonTypeRight, "More", nfc_magic_scene_magic_info_widget_callback, instance);

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
            } else {
                scene_manager_next_scene(instance->scene_manager, NfcMagicSceneGen4Menu);
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
