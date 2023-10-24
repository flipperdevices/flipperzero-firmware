#include "../mag_i.h"
#include "../helpers/mag_helpers.h"

#define TAG "MagSceneEmulate"

void cat_trackstr(FuriString* str, uint8_t calls, uint8_t i, FuriString* trackstr) {
    furi_string_cat_printf(
        str,
        "%sTrack %d:%s%s\n",
        (calls == 0) ? "" : "\n", // if first line, don't prepend a "\n"
        (i + 1),
        furi_string_empty(trackstr) ? "  " : "\n",
        furi_string_empty(trackstr) ? "< empty >" : furi_string_get_cstr(trackstr));
}

void mag_scene_emulate_on_enter(void* context) {
    Mag* mag = context;
    Widget* widget = mag->widget;

    FuriString* tmp_str;
    tmp_str = furi_string_alloc();

    // Use strlcpy instead perhaps, to truncate to screen width, then add ellipses if needed?
    furi_string_printf(tmp_str, "%s\r\n", mag->mag_dev->dev_name);

    // TODO: Display other relevant config settings (namely RFID vs GPIO)?

    widget_add_icon_element(widget, 1, 1, &I_mag_file_10px);
    widget_add_string_element(
        widget, 13, 2, AlignLeft, AlignTop, FontPrimary, furi_string_get_cstr(tmp_str));
    furi_string_reset(tmp_str);

    FURI_LOG_D(TAG, "%d", mag->setting->reverse);

    // print relevant data
    uint8_t cat_count = 0;
    for(uint8_t i = 0; i < MAG_DEV_TRACKS; i++) {
        FuriString* trackstr = mag->mag_dev->dev_data.track[i].str;

        // still messy / dumb way to do this, but slightly cleaner than before.
        // will clean up more later
        switch(mag->setting->track) {
        case MagTrackStateOne:
            if(i == 0) cat_trackstr(tmp_str, cat_count++, i, trackstr);
            break;
        case MagTrackStateTwo:
            if(i == 1) cat_trackstr(tmp_str, cat_count++, i, trackstr);
            break;
        case MagTrackStateThree:
            if(i == 2) cat_trackstr(tmp_str, cat_count++, i, trackstr);
            break;
        case MagTrackStateOneAndTwo:
            if((i == 0) | (i == 1)) cat_trackstr(tmp_str, cat_count++, i, trackstr);
            break;
        }
    }

    widget_add_text_scroll_element(widget, 0, 15, 128, 49, furi_string_get_cstr(tmp_str));

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
        }
    }

    return consumed;
}

void mag_scene_emulate_on_exit(void* context) {
    Mag* mag = context;
    notification_message(mag->notifications, &sequence_blink_stop);
    widget_reset(mag->widget);
}