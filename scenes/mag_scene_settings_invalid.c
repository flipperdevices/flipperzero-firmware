#include "../mag_i.h"
#include "../helpers/mag_helpers.h"

void mag_scene_settings_invalid_on_enter(void* context) {
    Mag* mag = context;
    Widget* widget = mag->widget;

    FuriString* tmp_str;
    tmp_str = furi_string_alloc();

    furi_string_printf(tmp_str, "\e#Invalid Pin Configuration!#\e");

    //TODO: print concise summary of data on card? Would need to vary by card/track type

    widget_add_text_box_element(
        widget, 0, 0, 128, 27, AlignCenter, AlignCenter, furi_string_get_cstr(tmp_str), true);
    widget_add_button_element(widget, GuiButtonTypeLeft, "Modify", mag_widget_callback, mag);
    widget_add_button_element(widget, GuiButtonTypeRight, "Reset", mag_widget_callback, mag);

    view_dispatcher_switch_to_view(mag->view_dispatcher, MagViewWidget);

    furi_string_free(tmp_str);
}

bool mag_scene_settings_invalid_on_event(void* context, SceneManagerEvent event) {
    Mag* mag = context;
    SceneManager* scene_manager = mag->scene_manager;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeRight) {
            consumed = true;
            // reset to defaults here
            // once impl'd saved settings, reset to last save? or still to defaults?
            mag_state_gpio_reset(&mag->state);

            scene_manager_search_and_switch_to_previous_scene(mag->scene_manager, MagSceneStart);
        } else if(event.event == GuiButtonTypeLeft) {
            consumed = true;
            scene_manager_previous_scene(scene_manager);
        }
    }

    return consumed;
}

void mag_scene_settings_invalid_on_exit(void* context) {
    Mag* mag = context;
    widget_reset(mag->widget);
}
