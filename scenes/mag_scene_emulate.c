#include "../mag_i.h"
#include "../helpers/mag_helpers.h"

void mag_scene_emulate_on_enter(void* context) {
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

    widget_add_button_element(widget, GuiButtonTypeLeft, "Config", mag_widget_callback, mag);
    widget_add_button_element(widget, GuiButtonTypeRight, "Send", mag_widget_callback, mag);

    view_dispatcher_switch_to_view(mag->view_dispatcher, MagViewWidget);
    furi_string_free(tmp_str);
}

bool mag_scene_emulate_on_event(void* context, SceneManagerEvent event) {
    Mag* mag = context;
    SceneManager* scene_manager = mag->scene_manager;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeLeft) {
            consumed = true;

            scene_manager_next_scene(scene_manager, MagSceneEmulateConfig);
        } else if(event.event == GuiButtonTypeRight) {
            consumed = true;

            //FuriString* tmp_str;
            //tmp_str = furi_string_alloc_set_str(
            //    furi_string_get_cstr(mag->mag_dev->dev_data.track[1].str));

            // Assumes track 2 for temporary testing.
            // Will overhaul alongside file format and config system
            notification_message(mag->notifications, &sequence_blink_start_cyan);
            mag_spoof(mag);
            //mag_spoof_single_track_rfid(tmp_str, 1);
            notification_message(mag->notifications, &sequence_blink_stop);

            //furi_string_free(tmp_str);
        }
    }

    return consumed;
}

void mag_scene_emulate_on_exit(void* context) {
    Mag* mag = context;
    notification_message(mag->notifications, &sequence_blink_stop);
    widget_reset(mag->widget);
}