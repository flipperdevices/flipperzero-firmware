#include "../wifi_marauder_app_i.h"

void wifi_marauder_scene_settings_init_widget_callback(
    GuiButtonType result,
    InputType type,
    void* context) {
    WifiMarauderApp* app = context;
    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(app->view_dispatcher, result);
    }
}

void wifi_marauder_scene_settings_init_on_enter(void* context) {
    WifiMarauderApp* app = context;
    Widget* widget = app->widget;

    widget_add_button_element(
        widget, GuiButtonTypeLeft, "No", wifi_marauder_scene_settings_init_widget_callback, app);
    widget_add_button_element(
        widget, GuiButtonTypeRight, "Yes", wifi_marauder_scene_settings_init_widget_callback, app);

    widget_add_string_element(app->widget, 0, 0, AlignLeft, AlignTop, FontPrimary, "Save pcaps?");
    widget_add_text_scroll_element(
        app->widget,
        0,
        12,
        128,
        38,
        "With compatible marauder\nfirmware, you can choose to\nsave captures (pcaps) to the\nflipper sd card here:\n/" MARAUDER_APP_FOLDER_USER
        "\n\nYou can change this setting in the app at any time. Would\nyou like to enable this feature now?");

    view_dispatcher_switch_to_view(app->view_dispatcher, WifiMarauderAppViewWidget);
}

bool wifi_marauder_scene_settings_init_on_event(void* context, SceneManagerEvent event) {
    WifiMarauderApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        // go back to start scene (main menu)
        app->need_to_prompt_settings_init = false;
        scene_manager_previous_scene(app->scene_manager);
        consumed = true;
    }

    return consumed;
}

void wifi_marauder_scene_settings_init_on_exit(void* context) {
    WifiMarauderApp* app = context;
    widget_reset(app->widget);
}
