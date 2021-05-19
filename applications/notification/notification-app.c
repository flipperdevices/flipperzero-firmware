#include <furi.h>
#include <api-hal.h>
#include "notifications_i.h"
#include "notifications.h"

const NotificationMessage message_display_on = {
    .type = NotificationMessageTypeDisplay,
    .data.display.on = true,
};

const NotificationMessage message_display_off = {
    .type = NotificationMessageTypeDisplay,
    .data.display.on = false,
};

const NotificationMessage* message_display_on_sequence[] = {
    &message_display_on,
    NULL,
};

const NotificationMessage* message_display_off_sequence[] = {
    &message_display_off,
    NULL,
};

typedef enum {
    ForegroundMessage,
    BackgroundMessage,
} NotificationsAppMessageType;

typedef struct {
    const NotificationMessage** messages;
    NotificationsAppMessageType type;
} NotificationsAppMessage;

typedef enum {
    LayerInternal = 0,
    LayerNotification = 1,
    LayerMAX = 2,
} NotificationsLedLayerIndex;

typedef struct {
    uint8_t value[LayerMAX];
    NotificationsLedLayerIndex index;
    Light light;
} NotificationsLedLayer;

typedef struct {
    uint8_t display_brightness;
    float led_brightness;
} NotificationsSettings;

struct NotificationsApp {
    osMessageQueueId_t queue;

    NotificationsLedLayer display;
    NotificationsLedLayer red;
    NotificationsLedLayer green;
    NotificationsLedLayer blue;

    NotificationsSettings settings;
};

void notification_send_messages_async(NotificationsApp* app, const NotificationMessage** messages) {
    NotificationsAppMessage m = {.type = ForegroundMessage, .messages = messages};
    furi_check(osMessageQueuePut(app->queue, &m, 0, osWaitForever) == osOK);
};

void notification_internal_display_on(NotificationsApp* app) {
    const NotificationMessage** messages;
    messages = message_display_on_sequence;
    NotificationsAppMessage m = {.type = BackgroundMessage, .messages = messages};
    furi_check(osMessageQueuePut(app->queue, &m, 0, osWaitForever) == osOK);
};

void notification_internal_display_off(NotificationsApp* app) {
    const NotificationMessage** messages;
    messages = message_display_off_sequence;
    NotificationsAppMessage m = {.type = BackgroundMessage, .messages = messages};
    furi_check(osMessageQueuePut(app->queue, &m, 0, osWaitForever) == osOK);
};

void notification_apply_internal_led_layer(NotificationsLedLayer* layer, uint8_t layer_value) {
    furi_assert(layer);
    furi_assert(layer->index < LayerMAX);

    // set value
    layer->value[LayerInternal] = layer_value;

    // apply if current layer is BG
    if(layer->index == LayerInternal) {
        api_hal_light_set(layer->light, layer->value[layer->index]);
    }
}

uint8_t get_rgb_led_brightness(NotificationsApp* app, uint8_t value) {
    return (value * app->settings.led_brightness);
}

NotificationsApp* notification_app_alloc() {
    NotificationsApp* app = furi_alloc(sizeof(NotificationsApp));
    app->queue = osMessageQueueNew(8, sizeof(NotificationsAppMessage), NULL);

    app->settings.display_brightness = 0xFF;
    app->settings.led_brightness = 1.0f;

    app->display.value[LayerInternal] = 0x00;
    app->display.value[LayerNotification] = app->settings.display_brightness;
    app->display.index = LayerInternal;
    app->display.light = LightBacklight;

    app->red.value[LayerInternal] = 0x00;
    app->red.value[LayerNotification] = 0x00;
    app->red.index = LayerInternal;
    app->red.light = LightRed;

    app->green.value[LayerInternal] = 0x00;
    app->green.value[LayerNotification] = 0x00;
    app->green.index = LayerInternal;
    app->green.light = LightGreen;

    app->blue.value[LayerInternal] = 0x00;
    app->blue.value[LayerNotification] = 0x00;
    app->blue.index = LayerInternal;
    app->blue.light = LightBlue;

    return app;
};

int32_t notification_app(void* p) {
    NotificationsApp* app = notification_app_alloc();
    furi_record_create("notifications", app);

    NotificationsAppMessage message;
    while(1) {
        furi_check(osMessageQueueGet(app->queue, &message, NULL, osWaitForever) == osOK);

        if(message.type == ForegroundMessage) {
        } else if(message.type == BackgroundMessage) {
            uint32_t notification_message_index = 0;
            const NotificationMessage* notification_message;
            notification_message = message.messages[notification_message_index];

            while(notification_message != NULL) {
                switch(notification_message->type) {
                case NotificationMessageTypeDisplay:
                    if(notification_message->data.display.on) {
                        notification_apply_internal_led_layer(
                            &app->display, app->settings.display_brightness);
                    } else {
                        notification_apply_internal_led_layer(&app->display, 0x00);
                    }
                    break;
                case NotificationMessageTypeLedRed:
                    notification_apply_internal_led_layer(
                        &app->red,
                        get_rgb_led_brightness(app, notification_message->data.led.value));
                    break;
                case NotificationMessageTypeLedGreen:
                    notification_apply_internal_led_layer(
                        &app->green,
                        get_rgb_led_brightness(app, notification_message->data.led.value));
                    break;
                case NotificationMessageTypeLedBlue:
                    notification_apply_internal_led_layer(
                        &app->blue,
                        get_rgb_led_brightness(app, notification_message->data.led.value));
                    break;
                default:
                    break;
                }
                notification_message_index++;
                notification_message = message.messages[notification_message_index];
            }
        }
    }

    return 0;
};