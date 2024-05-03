#include "../pocsag_pager_app_i.h"
#include "../helpers/pocsag_pager_types.h"

void pocsag_pager_scene_about_widget_callback(GuiButtonType result, InputType type, void* context) {
    POCSAGPagerApp* app = context;
    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(app->view_dispatcher, result);
    }
}

void pocsag_pager_scene_about_on_enter(void* context) {
    POCSAGPagerApp* app = context;

    FuriString* temp_str;
    temp_str = furi_string_alloc();
    furi_string_printf(temp_str, "\e#%s\n", "Information");

    furi_string_cat_printf(temp_str, "Version: %s\n", PCSG_VERSION_APP);
    furi_string_cat_printf(temp_str, "Developed by:\n%s\n\n", PCSG_DEVELOPED);
    furi_string_cat_printf(temp_str, "Github: %s\n\n", PCSG_GITHUB);

    furi_string_cat_printf(temp_str, "\e#%s\n", "Description");
    furi_string_cat_printf(
        temp_str,
        "Receiving POCSAG Pager \nmessages \nDefault frequency is set to\nDAPNET - 439987500\n\nUse Config button to set frequency\nCustom frequencies usually can be found in the end of the list - use right arrow button to scroll to the end\n\nTo add new modulations and\nfrequencies create file\n/pocsag/settings.txt\nAnd copy \nsubghz/assets/setting_user.example\ncontents into new\nsettings.txt\n\n");

    furi_string_cat_printf(
        temp_str, "Supported protocols:\nPOCSAG 512\nPOCSAG 1200\nPOCSAG 2400\n");

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
        "\e#\e!        POCSAG Pager       \e!\n",
        false);
    widget_add_text_scroll_element(app->widget, 0, 16, 128, 50, furi_string_get_cstr(temp_str));
    furi_string_free(temp_str);

    view_dispatcher_switch_to_view(app->view_dispatcher, POCSAGPagerViewWidget);
}

bool pocsag_pager_scene_about_on_event(void* context, SceneManagerEvent event) {
    POCSAGPagerApp* app = context;
    bool consumed = false;
    UNUSED(app);
    UNUSED(event);

    return consumed;
}

void pocsag_pager_scene_about_on_exit(void* context) {
    POCSAGPagerApp* app = context;

    // Clear views
    widget_reset(app->widget);
}
