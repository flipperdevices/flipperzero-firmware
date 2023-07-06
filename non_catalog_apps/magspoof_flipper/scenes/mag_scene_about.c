#include "../mag_i.h"

void mag_scene_about_on_enter(void* context) {
    Mag* mag = context;
    Widget* widget = mag->widget;

    FuriString* tmp_str;
    tmp_str = furi_string_alloc();

    furi_string_cat_printf(tmp_str, "Version: %s\n", MAG_VERSION_APP);
    furi_string_cat_printf(tmp_str, "Developer: %s\n", MAG_DEVELOPER);
    furi_string_cat_printf(tmp_str, "GitHub: %s\n\n", MAG_GITHUB);

    furi_string_cat_printf(
        tmp_str,
        "Unfinished port of Samy Kamkar's MagSpoof. Confer GitHub for updates; in the interim, use responsibly and at your own risk.");

    // TODO: Add credits

    widget_add_text_scroll_element(widget, 0, 0, 128, 64, furi_string_get_cstr(tmp_str));
    furi_string_free(tmp_str);

    view_dispatcher_switch_to_view(mag->view_dispatcher, MagViewWidget);
}

bool mag_scene_about_on_event(void* context, SceneManagerEvent event) {
    Mag* mag = context;
    SceneManager* scene_manager = mag->scene_manager;
    bool consumed = false;

    UNUSED(event);
    UNUSED(scene_manager);

    return consumed;
}

void mag_scene_about_on_exit(void* context) {
    Mag* mag = context;
    widget_reset(mag->widget);
}