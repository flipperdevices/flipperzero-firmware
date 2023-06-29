#include "../mag_i.h"
#include "../mag_device.h"

void mag_scene_delete_confirm_on_enter(void* context) {
    Mag* mag = context;
    Widget* widget = mag->widget;
    MagDevice* mag_dev = mag->mag_dev;

    FuriString* tmp_str;
    tmp_str = furi_string_alloc();

    furi_string_printf(tmp_str, "\e#Delete %s?\e#", mag_dev->dev_name);

    //TODO: print concise summary of data on card? Would need to vary by card/track type

    widget_add_text_box_element(
        widget, 0, 0, 128, 27, AlignCenter, AlignCenter, furi_string_get_cstr(tmp_str), true);
    widget_add_button_element(widget, GuiButtonTypeLeft, "Cancel", mag_widget_callback, mag);
    widget_add_button_element(widget, GuiButtonTypeRight, "Delete", mag_widget_callback, mag);

    view_dispatcher_switch_to_view(mag->view_dispatcher, MagViewWidget);

    furi_string_free(tmp_str);
}

bool mag_scene_delete_confirm_on_event(void* context, SceneManagerEvent event) {
    Mag* mag = context;
    SceneManager* scene_manager = mag->scene_manager;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeRight) {
            consumed = true;
            if(mag_device_delete(mag->mag_dev, true)) {
                scene_manager_next_scene(scene_manager, MagSceneDeleteSuccess);
            }
        } else if(event.event == GuiButtonTypeLeft) {
            consumed = true;
            scene_manager_previous_scene(scene_manager);
        }
    }

    return consumed;
}

void mag_scene_delete_confirm_on_exit(void* context) {
    Mag* mag = context;
    widget_reset(mag->widget);
}
