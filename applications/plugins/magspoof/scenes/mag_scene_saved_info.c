#include "../mag_i.h"

void mag_scene_saved_info_on_enter(void* context) {
    Mag* mag = context;
    Widget* widget = mag->widget;

    FuriString* tmp_str;
    tmp_str = furi_string_alloc();

    // Use strlcpy instead perhaps, to truncate to screen width, then add ellipses if needed?
    furi_string_printf(tmp_str, "%s\r\n", mag->mag_dev->dev_name);

    widget_add_icon_element(widget, 1, 1, &I_mag_10px);
    widget_add_string_element(
        widget, 13, 2, AlignLeft, AlignTop, FontPrimary, furi_string_get_cstr(tmp_str));
    furi_string_reset(tmp_str);

    furi_string_printf(tmp_str, furi_string_get_cstr(mag->mag_dev->dev_data.track[1].str));
    widget_add_string_multiline_element(
        widget, 0, 15, AlignLeft, AlignTop, FontSecondary, furi_string_get_cstr(tmp_str));

    widget_add_button_element(widget, GuiButtonTypeLeft, "Back", mag_widget_callback, mag);

    view_dispatcher_switch_to_view(mag->view_dispatcher, MagViewWidget);
    furi_string_free(tmp_str);
}

bool mag_scene_saved_info_on_event(void* context, SceneManagerEvent event) {
    Mag* mag = context;
    SceneManager* scene_manager = mag->scene_manager;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeLeft) {
            consumed = true;

            scene_manager_previous_scene(scene_manager);
        }
    }

    return consumed;
}

void mag_scene_saved_info_on_exit(void* context) {
    Mag* mag = context;
    widget_reset(mag->widget);
}
