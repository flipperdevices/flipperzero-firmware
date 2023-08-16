#include "../can_lin_tools_app_i.h"

void can_lin_tools_scene_about_widget_callback(GuiButtonType result, InputType type, void* context) {
    CanLinToolsApp* app = context;
    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(app->view_dispatcher, result);
    }
}

void can_lin_tools_scene_about_on_enter(void* context) {
    CanLinToolsApp* app = context;

    FuriString* temp_str;
    temp_str = furi_string_alloc();
    furi_string_printf(temp_str, "\e#%s\n", "Information");

    furi_string_cat_printf(temp_str, "Version: %s\n", CAN_LIN_TOOLS_VERSION_APP);
    furi_string_cat_printf(temp_str, "Developed by: %s\n", CAN_LIN_TOOLS_DEVELOPED);
    furi_string_cat_printf(temp_str, "Github: %s\n\n", CAN_LIN_TOOLS_GITHUB);

    furi_string_cat_printf(temp_str, "\e#%s\n", "Description");
    furi_string_cat_printf(
        temp_str,
        "This is a companion app for an external CAN/CAN-FD/LIN external module. It allows you to connect Flipper with the\nmodule to PC programs and\nuse is as an CAN?CAN-FD/\nLIN external adapter.\n\n");
    furi_string_cat_printf(temp_str, "\e#%s\n", "Supported programs");
    furi_string_cat_printf(temp_str, "CAN Hacker 2.xx\nCARBUS Analyzer????\netc...\n");

    widget_add_text_box_element(
        app->widget,
        0,
        0,
        128,
        14,
        AlignCenter,
        AlignBottom,
        "\e#\e!                                                      \e!\n",
        false);
    widget_add_text_box_element(
        app->widget,
        0,
        2,
        128,
        14,
        AlignCenter,
        AlignBottom,
        "\e#\e!      CAN-FD/LIN Tools       \e!\n",
        false);
    widget_add_text_scroll_element(app->widget, 0, 16, 128, 50, furi_string_get_cstr(temp_str));
    furi_string_free(temp_str);

    view_dispatcher_switch_to_view(app->view_dispatcher, CanLinToolsViewWidget);
}

bool can_lin_tools_scene_about_on_event(void* context, SceneManagerEvent event) {
    CanLinToolsApp* app = context;
    bool consumed = false;
    UNUSED(app);
    UNUSED(event);

    return consumed;
}

void can_lin_tools_scene_about_on_exit(void* context) {
    CanLinToolsApp* app = context;

    // Clear views
    widget_reset(app->widget);
}
