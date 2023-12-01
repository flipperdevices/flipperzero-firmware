#include "../uart_terminal_app_i.h"

void uart_terminal_scene_help_widget_callback(GuiButtonType result, InputType type, void* context) {
    UART_TerminalApp* app = context;
    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(app->view_dispatcher, result);
    }
}

void uart_terminal_scene_help_on_enter(void* context) {
    UART_TerminalApp* app = context;

    FuriString* temp_str;
    temp_str = furi_string_alloc();
    furi_string_printf(
        temp_str,
        "\nUART terminal for Flipper\n\nI'm in github: cool4uma\n\nThis app is a modified\nWiFi Marauder companion,\nThanks 0xchocolate(github)\nfor great code and app.\n\n");
    furi_string_cat_printf(temp_str, "Press BACK to return\n");

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
        "\e#\e!         UART Terminal            \e!\n",
        false);
    widget_add_text_scroll_element(app->widget, 0, 16, 128, 50, furi_string_get_cstr(temp_str));
    furi_string_free(temp_str);

    view_dispatcher_switch_to_view(app->view_dispatcher, UART_TerminalAppViewHelp);
}

bool uart_terminal_scene_help_on_event(void* context, SceneManagerEvent event) {
    UART_TerminalApp* app = context;
    bool consumed = false;
    UNUSED(app);
    UNUSED(event);
    return consumed;
}

void uart_terminal_scene_help_on_exit(void* context) {
    UART_TerminalApp* app = context;
    // Clear views
    widget_reset(app->widget);
}