#include <furi.h>
#include <api-hal.h>
#include "notification_i.h"
#include "notification.h"
#include "notification-messages.h"

typedef enum {
    ForegroundMessage,
    BackgroundMessage,
} NotificationAppMessageType;

typedef struct {
    const NotificationMessage** messages;
    NotificationAppMessageType type;
} NotificationAppMessage;

typedef enum {
    LayerInternal = 0,
    LayerNotification = 1,
    LayerMAX = 2,
} NotificationLedLayerIndex;

typedef struct {
    uint8_t value[LayerMAX];
    NotificationLedLayerIndex index;
    Light light;
} NotificationLedLayer;

typedef struct {
    uint8_t display_brightness;
    float led_brightness;
} NotificationSettings;

struct NotificationApp {
    osMessageQueueId_t queue;

    NotificationLedLayer display;
    NotificationLedLayer red;
    NotificationLedLayer green;
    NotificationLedLayer blue;

    NotificationSettings settings;
};

void notification_send_messages_async(NotificationApp* app, const NotificationMessage** messages) {
    NotificationAppMessage m = {.type = ForegroundMessage, .messages = messages};
    furi_check(osMessageQueuePut(app->queue, &m, 0, osWaitForever) == osOK);
};

void notification_internal_display_on(NotificationApp* app) {
    const NotificationMessage** messages;
    messages = message_display_on_sequence;
    NotificationAppMessage m = {.type = BackgroundMessage, .messages = messages};
    furi_check(osMessageQueuePut(app->queue, &m, 0, osWaitForever) == osOK);
};

void notification_internal_display_off(NotificationApp* app) {
    const NotificationMessage** messages;
    messages = message_display_off_sequence;
    NotificationAppMessage m = {.type = BackgroundMessage, .messages = messages};
    furi_check(osMessageQueuePut(app->queue, &m, 0, osWaitForever) == osOK);
};

void notification_apply_internal_led_layer(NotificationLedLayer* layer, uint8_t layer_value) {
    furi_assert(layer);
    furi_assert(layer->index < LayerMAX);

    // set value
    layer->value[LayerInternal] = layer_value;

    // apply if current layer is BG
    if(layer->index == LayerInternal) {
        api_hal_light_set(layer->light, layer->value[layer->index]);
    }
}

uint8_t get_rgb_led_brightness(NotificationApp* app, uint8_t value) {
    return (value * app->settings.led_brightness);
}

NotificationApp* notification_app_alloc() {
    NotificationApp* app = furi_alloc(sizeof(NotificationApp));
    app->queue = osMessageQueueNew(8, sizeof(NotificationAppMessage), NULL);

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
    NotificationApp* app = notification_app_alloc();
    notification_apply_internal_led_layer(&app->display, 0x00);
    notification_apply_internal_led_layer(&app->red, 0x00);
    notification_apply_internal_led_layer(&app->green, 0x00);
    notification_apply_internal_led_layer(&app->blue, 0x00);

    furi_record_create("notification", app);

    NotificationAppMessage message;
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