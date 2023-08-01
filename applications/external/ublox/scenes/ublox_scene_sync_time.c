#include "../ublox_i.h"
#include "../ublox_worker_i.h"

#define TAG "ublox_scene_sync_time"

void ublox_scene_sync_time_worker_callback(UbloxWorkerEvent event, void* context) {
    Ublox* ublox = context;

    view_dispatcher_send_custom_event(ublox->view_dispatcher, event);
}

void ublox_scene_sync_time_on_enter(void* context) {
    Ublox* ublox = context;

    view_dispatcher_switch_to_view(ublox->view_dispatcher, UbloxViewWidget);

    widget_add_string_element(
        ublox->widget, 3, 5, AlignLeft, AlignCenter, FontPrimary, "Syncing time...");

    ublox_worker_start(
        ublox->worker, UbloxWorkerStateSyncTime, ublox_scene_sync_time_worker_callback, ublox);
}

bool ublox_scene_sync_time_on_event(void* context, SceneManagerEvent event) {
    Ublox* ublox = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == UbloxWorkerEventDataReady) {
            widget_reset(ublox->widget);
            // We don't have a timezone (or even UTC offset) in the
            // RTC, so we can only update the minute and second---not
            // even the date.
            FuriHalRtcDateTime datetime;
            furi_hal_rtc_get_datetime(&datetime);
            datetime.minute = (ublox->nav_timeutc).min;
            datetime.second = (ublox->nav_timeutc).sec;
            furi_hal_rtc_set_datetime(&datetime);

            widget_add_string_element(
                ublox->widget, 3, 5, AlignLeft, AlignCenter, FontPrimary, "Updated min/sec to GPS");

            FuriString* s = furi_string_alloc();
            furi_string_cat_printf(s, "New date/time: ");

            FuriString* date = furi_string_alloc();
            locale_format_date(date, &datetime, locale_get_date_format(), "/");
            furi_string_cat_printf(date, " ");
            FuriString* time = furi_string_alloc();
            locale_format_time(time, &datetime, locale_get_time_format(), ":");

            furi_string_cat(date, time);
            widget_add_string_element(
                ublox->widget, 3, 25, AlignLeft, AlignTop, FontSecondary, furi_string_get_cstr(s));
            widget_add_string_element(
                ublox->widget,
                3,
                35,
                AlignLeft,
                AlignTop,
                FontSecondary,
                furi_string_get_cstr(date));
            furi_string_free(time);
            furi_string_free(date);
            furi_string_free(s);
        }
    }

    return consumed;
}

void ublox_scene_sync_time_on_exit(void* context) {
    Ublox* ublox = context;

    ublox_worker_stop(ublox->worker);

    widget_reset(ublox->widget);
}
