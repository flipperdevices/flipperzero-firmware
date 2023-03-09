#include "../avr_isp_app_i.h"
#include "../helpers/avr_isp_types.h"

void avr_isp_scene_about_widget_callback(GuiButtonType result, InputType type, void* context) {
    AvrIspApp* app = context;
    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(app->view_dispatcher, result);
    }
}

void avr_isp_scene_about_on_enter(void* context) {
    AvrIspApp* app = context;

    FuriString* temp_str;
    temp_str = furi_string_alloc();
    furi_string_printf(temp_str, "\e#%s\n", "Information");

    furi_string_cat_printf(temp_str, "Version: %s\n", AVR_ISP_VERSION_APP);
    furi_string_cat_printf(temp_str, "Developed by: %s\n", AVR_ISP_DEVELOPED);
    furi_string_cat_printf(temp_str, "Github: %s\n\n", AVR_ISP_GITHUB);

    furi_string_cat_printf(temp_str, "\e#%s\n", "Description");
    furi_string_cat_printf(temp_str, "Avr Isp Programmer\nBla Bla Bla\nbla bla bla\n\n");

    furi_string_cat_printf(temp_str, "Supported protocols:\n");

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
        "\e#\e!     Avr Isp Programmer   \e!\n",
        false);
    widget_add_text_scroll_element(app->widget, 0, 16, 128, 50, furi_string_get_cstr(temp_str));
    furi_string_free(temp_str);

    view_dispatcher_switch_to_view(app->view_dispatcher, AvrIspViewWidget);
}

bool avr_isp_scene_about_on_event(void* context, SceneManagerEvent event) {
    AvrIspApp* app = context;
    bool consumed = false;
    UNUSED(app);
    UNUSED(event);

    return consumed;
}

void avr_isp_scene_about_on_exit(void* context) {
    AvrIspApp* app = context;

    // Clear views
    widget_reset(app->widget);
}
