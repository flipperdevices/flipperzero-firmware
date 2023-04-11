#include "../mifare_nested_i.h"

void mifare_nested_scene_run_app_widget_callback(
    GuiButtonType result,
    InputType type,
    void* context) {
    MifareNested* mifare_nested = context;
    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(mifare_nested->view_dispatcher, result);
    }
}

void mifare_nested_scene_run_app_on_enter(void* context) {
    MifareNested* mifare_nested = context;
    Widget* widget = mifare_nested->widget;

    notification_message(mifare_nested->notifications, &sequence_success);

    widget_add_icon_element(widget, 73, 13, &I_DolphinCry);
    widget_add_string_element(widget, 3, 4, AlignLeft, AlignTop, FontPrimary, "You need to");
    widget_add_string_element(widget, 3, 16, AlignLeft, AlignTop, FontPrimary, "recover keys.");
    widget_add_string_element(widget, 3, 28, AlignLeft, AlignTop, FontPrimary, "Read README");
    widget_add_button_element(
        widget,
        GuiButtonTypeLeft,
        "Back",
        mifare_nested_scene_run_app_widget_callback,
        mifare_nested);

    // Setup and start worker
    view_dispatcher_switch_to_view(mifare_nested->view_dispatcher, MifareNestedViewWidget);
}

bool mifare_nested_scene_run_app_on_event(void* context, SceneManagerEvent event) {
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

void mifare_nested_scene_run_app_on_exit(void* context) {
    MifareNested* mifare_nested = context;

    widget_reset(mifare_nested->widget);
}
