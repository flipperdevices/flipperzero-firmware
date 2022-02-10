#include <notification/notification.h>
#include <notification/notification_messages.h>
#include <stddef.h>
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>

#include "desktop_helpers.h"
#include "desktop_i.h"

static const NotificationSequence sequence_pin_fail = {
    &message_display_on,

    &message_red_255,
    &message_vibro_on,
    &message_delay_100,
    &message_vibro_off,
    &message_red_0,

    &message_delay_250,

    &message_red_255,
    &message_vibro_on,
    &message_delay_100,
    &message_vibro_off,
    &message_red_0,
    NULL,
};

void desktop_helpers_emit_error_notification() {
    NotificationApp* notification = furi_record_open("notification");
    notification_message(notification, &sequence_pin_fail);
    furi_record_close("notification");
}

void desktop_helpers_lock_system(Desktop* desktop, bool hard_lock) {
    view_port_enabled_set(desktop->lock_viewport, true);
    if (hard_lock) {
        furi_hal_rtc_set_flag(FuriHalRtcFlagLock);
        furi_hal_usb_disable();
    }

    Gui* gui = furi_record_open("gui");
    gui_set_lockdown(gui, true);
    furi_record_close("gui");
}

void desktop_helpers_unlock_system(Desktop* desktop) {
    furi_hal_rtc_reset_flag(FuriHalRtcFlagLock);
    furi_hal_usb_enable();
    view_port_enabled_set(desktop->lock_viewport, false);

    Gui* gui = furi_record_open("gui");
    gui_set_lockdown(gui, false);
    furi_record_close("gui");
}

