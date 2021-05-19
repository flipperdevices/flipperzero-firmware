#include <furi.h>
#include <api-hal.h>
#include "../notification/notification_i.h"

#define BACKLIGHT_TIME 30000
#define BACKLIGHT_FLAG_ACTIVITY 0x00000001U

static void event_cb(const void* value, void* ctx) {
    osThreadFlagsSet((osThreadId_t)ctx, BACKLIGHT_FLAG_ACTIVITY);
}

int32_t backlight_control(void* p) {
    // open record
    NotificationApp* notifications = furi_record_open("notification");
    PubSub* event_record = furi_record_open("input_events");
    subscribe_pubsub(event_record, event_cb, (void*)osThreadGetId());

    notification_internal_display_on(notifications);

    while(1) {
        // wait for event
        if(osThreadFlagsWait(BACKLIGHT_FLAG_ACTIVITY, osFlagsWaitAny, BACKLIGHT_TIME) ==
           BACKLIGHT_FLAG_ACTIVITY) {
            notification_internal_display_on(notifications);
        } else {
            notification_internal_display_off(notifications);
        }
    }

    return 0;
}