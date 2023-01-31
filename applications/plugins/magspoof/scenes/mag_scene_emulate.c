#include "../mag_i.h"
#include "../helpers/mag_helpers.h"

void mag_scene_emulate_on_enter(void* context) {
    Mag* mag = context;
    Widget* widget = mag->widget;

    FuriString* tmp_str;
    tmp_str = furi_string_alloc();

    // Use strlcpy instead perhaps, to truncate to screen width, then add ellipses if needed?
    furi_string_printf(tmp_str, "%s\r\n", mag->mag_dev->dev_name);

    // TODO: Display other relevant config settings (namely RFID vs GPIO)?

    widget_add_icon_element(widget, 2, 1, &I_mag_10px);
    widget_add_string_element(
        widget, 13, 2, AlignLeft, AlignTop, FontPrimary, furi_string_get_cstr(tmp_str));
    furi_string_reset(tmp_str);

    for(uint8_t i = 0; i < MAG_DEV_TRACKS; i++) {
        FuriString* trackstr = mag->mag_dev->dev_data.track[i].str;
        // there's definitely a better way to do this...
        bool is_active_one = (mag->setting->track == MagTrackStateOne) & (i == 0);
        bool is_active_two = (mag->setting->track == MagTrackStateTwo) & (i == 1);
        bool is_active_both = (mag->setting->track == MagTrackStateAll);

        if(is_active_one | is_active_two | is_active_both) {
            furi_string_cat_printf(
                tmp_str,
                "Track %d:%s%s%s",
                (i + 1),
                furi_string_empty(trackstr) ? "  " : "\n",
                furi_string_empty(trackstr) ? "< empty >" : furi_string_get_cstr(trackstr),
                (i + 1 == MAG_DEV_TRACKS) ? "\n" : "\n\n");
        }
    }

    widget_add_text_scroll_element(widget, 0, 15, 128, 49, furi_string_get_cstr(tmp_str));

    widget_add_button_element(widget, GuiButtonTypeLeft, "Config", mag_widget_callback, mag);
    widget_add_button_element(widget, GuiButtonTypeRight, "Send", mag_widget_callback, mag);
    widget_add_button_element(widget, GuiButtonTypeCenter, "Bitwise", mag_widget_callback, mag);

    view_dispatcher_switch_to_view(mag->view_dispatcher, MagViewWidget);
    furi_string_free(tmp_str);
}

bool mag_scene_emulate_on_event(void* context, SceneManagerEvent event) {
    Mag* mag = context;
    SceneManager* scene_manager = mag->scene_manager;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case GuiButtonTypeLeft:
            consumed = true;
            scene_manager_next_scene(scene_manager, MagSceneEmulateConfig);
            break;
        case GuiButtonTypeRight:
            consumed = true;
            notification_message(mag->notifications, &sequence_blink_start_cyan);
            mag_spoof(mag);
            notification_message(mag->notifications, &sequence_blink_stop);
            break;
        case GuiButtonTypeCenter:
            consumed = true;
            notification_message(mag->notifications, &sequence_blink_start_cyan);
            mag_spoof_bitwise(mag);
            notification_message(mag->notifications, &sequence_blink_stop);
            break;
        }
    }

    return consumed;
}

void mag_scene_emulate_on_exit(void* context) {
    Mag* mag = context;
    notification_message(mag->notifications, &sequence_blink_stop);
    widget_reset(mag->widget);
}