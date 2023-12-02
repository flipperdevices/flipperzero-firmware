#include "../scope_app_i.h"

void scope_scene_about_widget_callback(GuiButtonType result, InputType type, void* context) {
    ScopeApp* app = context;
    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(app->view_dispatcher, result);
    }
}

void scope_scene_about_on_enter(void* context) {
    ScopeApp* app = context;

    FuriString* temp_str;
    temp_str = furi_string_alloc();
    furi_string_printf(temp_str, "\e#%s\n", "Information");
    furi_string_cat_printf(
        temp_str,
        "Provide signal to pin 16/PC0, with a voltage ranging from 0V to 2.5V and ground to pin 18/GND.\n\n");
    furi_string_cat_printf(temp_str, "Developed by: %s\n", S_DEVELOPED);
    furi_string_cat_printf(temp_str, "Github: %s\n\n", S_GITHUB);

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
        "\e#\e!          flipperscope             \e!\n",
        false);
    widget_add_text_scroll_element(app->widget, 0, 16, 128, 50, furi_string_get_cstr(temp_str));
    furi_string_free(temp_str);

    view_dispatcher_switch_to_view(app->view_dispatcher, ScopeViewWidget);
}

bool scope_scene_about_on_event(void* context, SceneManagerEvent event) {
    ScopeApp* app = context;
    bool consumed = false;
    UNUSED(app);
    UNUSED(event);
    return consumed;
}

void scope_scene_about_on_exit(void* context) {
    ScopeApp* app = context;
    // Clear views
    widget_reset(app->widget);
}
