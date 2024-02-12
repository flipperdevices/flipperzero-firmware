#include "../subghz_i.h"
#include "../helpers/subghz_custom_event.h"

void subghz_scene_show_gps_draw_satellites(void* context) {
    SubGhz* subghz = context;

    FuriString* text = furi_string_alloc();
    FuriString* time_text = furi_string_alloc();

    FuriHalRtcDateTime datetime;
    furi_hal_rtc_get_datetime(&datetime);
    if((datetime.second - subghz->gps->fix_second) > 15) {
        subghz->gps->latitude = NAN;
        subghz->gps->longitude = NAN;
        subghz->gps->satellites = 0;
        subghz->gps->fix_hour = 0;
        subghz->gps->fix_minute = 0;
        subghz->gps->fix_second = 0;
    }

    subghz_history_get_time_item_menu(subghz->history, time_text, subghz->idx_menu_chosen);

    double distance = subghz_gps_calc_distance(
        (double)subghz_history_get_latitude(subghz->history, subghz->idx_menu_chosen),
        (double)subghz_history_get_longitude(subghz->history, subghz->idx_menu_chosen),
        (double)subghz->gps->latitude,
        (double)subghz->gps->longitude);

    float angle = subghz_gps_calc_angle(
        (double)subghz_history_get_latitude(subghz->history, subghz->idx_menu_chosen),
        (double)subghz_history_get_longitude(subghz->history, subghz->idx_menu_chosen),
        (double)subghz->gps->latitude,
        (double)subghz->gps->longitude);

    char* angle_str = "?";
    if(angle > -22.5 && angle <= 22.5) {
        angle_str = "E";
    } else if(angle > 22.5 && angle <= 67.5) {
        angle_str = "NE";
    } else if(angle > 67.5 && angle <= 112.5) {
        angle_str = "N";
    } else if(angle > 112.5 && angle <= 157.5) {
        angle_str = "NW";
    } else if(angle < -22.5 && angle >= -67.5) {
        angle_str = "SE";
    } else if(angle < -67.5 && angle >= -112.5) {
        angle_str = "S";
    } else if(angle < -112.5 && angle >= -157.5) {
        angle_str = "SW";
    } else if(angle < -157.5 || angle >= 157.5) {
        angle_str = "W";
    }

    FuriString* lat_str = furi_string_alloc();
    FuriString* lon_str = furi_string_alloc();

    if(isnan(subghz_history_get_latitude(subghz->history, subghz->idx_menu_chosen))) {
        furi_string_printf(lat_str, "N/A");
    } else {
        furi_string_printf(
            lat_str,
            "%f",
            (double)subghz_history_get_latitude(subghz->history, subghz->idx_menu_chosen));
    }

    if(isnan(subghz_history_get_longitude(subghz->history, subghz->idx_menu_chosen))) {
        furi_string_printf(lon_str, "N/A");
    } else {
        furi_string_printf(
            lon_str,
            "%f",
            (double)subghz_history_get_longitude(subghz->history, subghz->idx_menu_chosen));
    }

    furi_string_printf(
        text,
        "Captured at: %s,\r\n%s Time: %s\r\n\r\nRealtime:  Sats: %d\r\nDistance: %.2f%s Dir: %s\r\nGPS time: %02d:%02d:%02d UTC",
        furi_string_get_cstr(lat_str),
        furi_string_get_cstr(lon_str),
        furi_string_get_cstr(time_text),
        subghz->gps->satellites,
        subghz->gps->satellites > 0 ? distance > 1 ? distance : distance * 1000 : 0,
        distance > 1 ? "km" : "m",
        angle_str,
        subghz->gps->fix_hour,
        subghz->gps->fix_minute,
        subghz->gps->fix_second);

    widget_add_text_scroll_element(subghz->widget, 0, 0, 128, 64, furi_string_get_cstr(text));

    furi_string_free(lat_str);
    furi_string_free(lon_str);

    furi_string_free(text);
    furi_string_free(time_text);
}

void subghz_scene_show_gps_refresh_screen(void* context) {
    SubGhz* subghz = context;
    widget_reset(subghz->widget);
    subghz_scene_show_gps_draw_satellites(subghz);
}

void subghz_scene_show_gps_on_enter(void* context) {
    SubGhz* subghz = context;

    subghz_scene_show_gps_draw_satellites(subghz);

    if(subghz->last_settings->gps_baudrate != 0) {
        subghz->gps->timer =
            furi_timer_alloc(subghz_scene_show_gps_refresh_screen, FuriTimerTypePeriodic, subghz);
        furi_timer_start(subghz->gps->timer, 1000);
    }
}

bool subghz_scene_show_gps_on_event(void* context, SceneManagerEvent event) {
    SubGhz* subghz = context;
    if(event.type == SceneManagerEventTypeTick) {
        if(subghz->state_notifications == SubGhzNotificationStateRx) {
            if(subghz->last_settings->gps_baudrate != 0) {
                if(subghz->gps->satellites > 0) {
                    notification_message(subghz->notifications, &sequence_blink_green_10);
                } else {
                    notification_message(subghz->notifications, &sequence_blink_red_10);
                }
            } else {
                notification_message(subghz->notifications, &sequence_blink_cyan_10);
            }
        }
    }
    return false;
}

void subghz_scene_show_gps_on_exit(void* context) {
    SubGhz* subghz = context;

    if(subghz->last_settings->gps_baudrate != 0) {
        furi_timer_stop(subghz->gps->timer);
        furi_timer_free(subghz->gps->timer);
    }

    widget_reset(subghz->widget);
}