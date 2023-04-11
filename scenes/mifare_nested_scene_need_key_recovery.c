#include "../mifare_nested_i.h"

void mifare_nested_scene_need_key_recovery_widget_callback(
    GuiButtonType result,
    InputType type,
    void* context) {
    MifareNested* mifare_nested = context;
    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(mifare_nested->view_dispatcher, result);
    }
}

void mifare_nested_scene_need_key_recovery_on_enter(void* context) {
    MifareNested* mifare_nested = context;
    Widget* widget = mifare_nested->widget;

    notification_message(mifare_nested->notifications, &sequence_error);

    widget_add_icon_element(widget, 74, 13, &I_DolphinCry);
    widget_add_string_element(
        widget, 1, 4, AlignLeft, AlignTop, FontPrimary, "You need to recover");
    widget_add_string_element(widget, 1, 16, AlignLeft, AlignTop, FontPrimary, "keys in desktop");
    widget_add_string_element(widget, 1, 28, AlignLeft, AlignTop, FontPrimary, "app");
    widget_add_string_element(widget, 1, 40, AlignLeft, AlignTop, FontPrimary, "Check \"About\"");
    widget_add_button_element(
        widget,
        GuiButtonTypeLeft,
        "Back",
        mifare_nested_scene_need_key_recovery_widget_callback,
        mifare_nested);

    // Setup and start worker
    view_dispatcher_switch_to_view(mifare_nested->view_dispatcher, MifareNestedViewWidget);
}

bool mifare_nested_scene_need_key_recovery_on_event(void* context, SceneManagerEvent event) {
    MifareNested* mifare_nested = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeCenter || event.event == GuiButtonTypeLeft) {
            scene_manager_search_and_switch_to_previous_scene(mifare_nested->scene_manager, 0);
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_search_and_switch_to_previous_scene(mifare_nested->scene_manager, 0);
        consumed = true;
    }

    return consumed;
}

void mifare_nested_scene_need_key_recovery_on_exit(void* context) {
    MifareNested* mifare_nested = context;

    widget_reset(mifare_nested->widget);
}
