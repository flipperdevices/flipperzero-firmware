#pragma once

#include <stdatomic.h>
#include <furi.h>
#include <furi_hal.h>
#include "notification.h"
#include "notification_messages.h"
#include "notification_settings_filename.h"

#define NOTIFICATION_LED_COUNT      3
#define NOTIFICATION_EVENT_COMPLETE 0x00000001U

typedef enum {
    NotificationLayerMessage,
    InternalLayerMessage,
    SaveSettingsMessage,
    LoadSettingsMessage,
    RefreshRgbMessage,
} NotificationAppMessageType;

typedef struct {
    const NotificationSequence* sequence;
    NotificationAppMessageType type;
    FuriEventFlag* back_event;
} NotificationAppMessage;

typedef enum {
    LayerInternal = 0,
    LayerNotification = 1,
    LayerMAX = 2,
} NotificationLedLayerIndex;

typedef struct {
    uint8_t value_last[LayerMAX];
    uint8_t value[LayerMAX];
    NotificationLedLayerIndex index;
    Light light;
} NotificationLedLayer;

#define NOTIFICATION_SETTINGS_VERSION 0x04
#define NOTIFICATION_SETTINGS_PATH    INT_PATH(NOTIFICATION_SETTINGS_FILE_NAME)

typedef struct {
    uint8_t version;
    float display_brightness;
    float led_brightness;
    float speaker_volume;
    uint32_t display_off_delay_ms;
    int8_t contrast;
    bool vibro_on;
    bool smart_backlight; // A stored true suppresses BLE wake (RPC Backlight OFF).
    atomic_bool rpc_led;
} NotificationSettings;

struct NotificationApp {
    FuriMessageQueue* queue;
    FuriPubSub* event_record;
    FuriTimer* display_timer;

    NotificationLedLayer display;
    NotificationLedLayer led[NOTIFICATION_LED_COUNT];
    atomic_uint ble_screen_streams;
    uint8_t display_led_lock;

    NotificationSettings settings;
};

void notification_message_save_settings(NotificationApp* app);
void notification_ble_screen_stream_changed(NotificationApp* app, bool started);
void notification_set_rpc_led(NotificationApp* app, bool enabled);
