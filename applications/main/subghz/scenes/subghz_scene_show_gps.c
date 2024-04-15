#include "../subghz_i.h"
#include "../helpers/subghz_custom_event.h"

void subghz_scene_show_gps_draw_satellites(void* context) {
    SubGhz* subghz = context;
    widget_reset(subghz->widget);

    float latitude, longitude;
    // Get from saved file or from history
    if(scene_manager_get_scene_state(subghz->scene_manager, SubGhzSceneShowGps)) {
        latitude = subghz_txrx_get_latitude(subghz->txrx);
        longitude = subghz_txrx_get_longitude(subghz->txrx);
    } else {
        latitude = subghz_history_get_latitude(subghz->history, subghz->idx_menu_chosen);
        longitude = subghz_history_get_longitude(subghz->history, subghz->idx_menu_chosen);
    }
    FuriString* text_str = furi_string_alloc_printf(
        "Captured at: %f,\r\n"
        "%f\r\n"
        "\r\n",
        (double)latitude,
        (double)longitude);

    if(subghz->gps) {
        DateTime datetime;
        furi_hal_rtc_get_datetime(&datetime);
        if((datetime.second - subghz->gps->fix_second) > 15) {
            subghz->gps->latitude = NAN;
            subghz->gps->longitude = NAN;
            subghz->gps->satellites = 0;
            subghz->gps->fix_hour = 0;
            subghz->gps->fix_minute = 0;
            subghz->gps->fix_second = 0;
        }

        subghz->gps->cat_realtime(subghz->gps, text_str, latitude, longitude);
    }

    widget_add_text_scroll_element(subghz->widget, 0, 0, 128, 64, furi_string_get_cstr(text_str));
    furi_string_free(text_str);
}

void subghz_scene_show_gps_on_enter(void* context) {
    SubGhz* subghz = context;

    subghz_scene_show_gps_draw_satellites(subghz);

    if(subghz->gps) {
        subghz->gps->timer =
            furi_timer_alloc(subghz_scene_show_gps_draw_satellites, FuriTimerTypePeriodic, subghz);
        furi_timer_start(subghz->gps->timer, 1000);
    }

    view_dispatcher_switch_to_view(subghz->view_dispatcher, SubGhzViewIdWidget);
}

bool subghz_scene_show_gps_on_event(void* context, SceneManagerEvent event) {
    SubGhz* subghz = context;

    // No rx blink if opened from saved file
    if(scene_manager_get_scene_state(subghz->scene_manager, SubGhzSceneShowGps)) return false;

    if(event.type == SceneManagerEventTypeTick) {
        if(subghz->state_notifications == SubGhzNotificationStateRx) {
            if(subghz->gps) {
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

    if(subghz->gps) {
        furi_timer_stop(subghz->gps->timer);
        furi_timer_free(subghz->gps->timer);
    }

    widget_reset(subghz->widget);
}