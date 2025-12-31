#pragma once

#include "power.h"

#include <stdint.h>
#include <gui/view_dispatcher.h>
#include <gui/gui.h>
#include <assets_icons.h>
#include <loader/loader.h>

#include <gui/modules/popup.h>
#include "views/power_off.h"
#include <power/power_settings.h>
#include "views/power_unplug_usb.h"

#include <notification/notification_messages.h>

#define POWER_BATTERY_HEALTHY_LEVEL 70

typedef enum {
    PowerStateNotCharging,
    PowerStateCharging,
    PowerStateCharged,
} PowerState;

struct Power {
    ViewDispatcher* view_dispatcher;
    PowerOff* power_off;
    PowerUnplugUsb* power_unplug_usb;

    ViewPort* battery_view_port;
    Gui* gui;
    NotificationApp* notification;
    FuriPubSub* event_pubsub;
<<<<<<< HEAD
    FuriPubSub* settings_events;
    FuriPubSub* input_events_pubsub;
    FuriPubSubSubscription* input_events_subscription;
    FuriPubSubSubscription* app_start_stop_subscription;
    FuriPubSubSubscription* settings_events_subscription;
=======
>>>>>>> origin/upstream-pr-2141-doom/2991-e2e-runner
    PowerEvent event;

    PowerState state;
    PowerInfo info;

    bool battery_low;
    bool show_low_bat_level_message;
    uint8_t battery_level;
    uint8_t power_off_timeout;

<<<<<<< HEAD
    uint32_t shutdown_idle_delay_ms;
    FuriTimer* auto_shutdown_timer;
    Loader* loader;
=======
>>>>>>> origin/upstream-pr-2141-doom/2991-e2e-runner
    FuriMutex* api_mtx;
};

typedef enum {
    PowerViewOff,
    PowerViewUnplugUsb,
} PowerView;
