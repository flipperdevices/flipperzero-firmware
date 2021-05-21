#include <furi.h>
#include <api-hal.h>
#include "notification_i.h"
#include "notification.h"
#include "notification-messages.h"

#define NOTIFICATION_LED_COUNT 3

typedef enum {
    NotificationLayerMessage,
    InternalLayerMessage,
} NotificationAppMessageType;

typedef struct {
    const NotificationSequence* sequence;
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
    NotificationLedLayer led[NOTIFICATION_LED_COUNT];

    NotificationSettings settings;
};

void notification_message(NotificationApp* app, const NotificationSequence* sequence) {
    NotificationAppMessage m = {.type = NotificationLayerMessage, .sequence = sequence};
    furi_check(osMessageQueuePut(app->queue, &m, 0, osWaitForever) == osOK);
};

void notification_internal_message(NotificationApp* app, const NotificationSequence* sequence) {
    NotificationAppMessage m = {.type = InternalLayerMessage, .sequence = sequence};
    furi_check(osMessageQueuePut(app->queue, &m, 0, osWaitForever) == osOK);
};

void notification_apply_internal_led_layer(NotificationLedLayer* layer, uint8_t layer_value) {
    furi_assert(layer);
    furi_assert(layer->index < LayerMAX);

    // set value
    layer->value[LayerInternal] = layer_value;

    // apply if current layer is internal
    if(layer->index == LayerInternal) {
        api_hal_light_set(layer->light, layer->value[LayerInternal]);
    }
}

void notification_apply_notification_led_layer(
    NotificationLedLayer* layer,
    const uint8_t layer_value) {
    furi_assert(layer);
    furi_assert(layer->index < LayerMAX);

    // set value
    layer->index = LayerNotification;
    // set layer
    layer->value[LayerNotification] = layer_value;
    // apply
    api_hal_light_set(layer->light, layer->value[LayerNotification]);
}

void notification_reset_notification_led_layer(NotificationLedLayer* layer) {
    furi_assert(layer);
    furi_assert(layer->index < LayerMAX);

    // set value
    layer->index = LayerInternal;
    // apply
    api_hal_light_set(layer->light, layer->value[LayerInternal]);
}

uint8_t get_rgb_led_brightness(NotificationApp* app, uint8_t value) {
    return (value * app->settings.led_brightness);
}

bool is_any_led_layer_internal_and_not_empty(NotificationApp* app) {
    bool result = false;
    if((app->led[0].index == LayerInternal) || (app->led[1].index == LayerInternal) ||
       (app->led[2].index == LayerInternal)) {
        if((app->led[0].value[LayerInternal] != 0x00) ||
           (app->led[1].value[LayerInternal] != 0x00) ||
           (app->led[2].value[LayerInternal] != 0x00)) {
            result = true;
        }
    }

    return result;
}

void notification_apply_notification_leds(NotificationApp* app, const uint8_t* values) {
    for(uint8_t i = 0; i < NOTIFICATION_LED_COUNT; i++) {
        notification_apply_notification_led_layer(
            &app->led[i], get_rgb_led_brightness(app, values[i]));
    }
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

    app->led[0].value[LayerInternal] = 0x00;
    app->led[0].value[LayerNotification] = 0x00;
    app->led[0].index = LayerInternal;
    app->led[0].light = LightRed;

    app->led[1].value[LayerInternal] = 0x00;
    app->led[1].value[LayerNotification] = 0x00;
    app->led[1].index = LayerInternal;
    app->led[1].light = LightGreen;

    app->led[2].value[LayerInternal] = 0x00;
    app->led[2].value[LayerNotification] = 0x00;
    app->led[2].index = LayerInternal;
    app->led[2].light = LightBlue;

    return app;
};

int32_t notification_app(void* p) {
    NotificationApp* app = notification_app_alloc();
    notification_apply_internal_led_layer(&app->display, 0x00);
    notification_apply_internal_led_layer(&app->led[0], 0x00);
    notification_apply_internal_led_layer(&app->led[1], 0x00);
    notification_apply_internal_led_layer(&app->led[2], 0x00);

    furi_record_create("notification", app);

    const uint8_t minimal_delay = 100;
    const uint8_t led_off_values[NOTIFICATION_LED_COUNT] = {0x00, 0x00, 0x00};

    NotificationAppMessage message;
    while(1) {
        furi_check(osMessageQueueGet(app->queue, &message, NULL, osWaitForever) == osOK);

        if(message.type == NotificationLayerMessage) {
            uint32_t notification_message_index = 0;
            const NotificationMessage* notification_message;
            notification_message = (*message.sequence)[notification_message_index];

            bool led_active = false;
            uint8_t led_value[NOTIFICATION_LED_COUNT] = {0x00, 0x00, 0x00};

            while(notification_message != NULL) {
                switch(notification_message->type) {
                case NotificationMessageTypeDisplay:
                    // if on - switch on and start timer
                    // if off - switch off and stop timer
                    // on timer - switch off
                    break;
                case NotificationMessageTypeLedRed:
                    // store and send on delay or after seq
                    led_active = true;
                    led_value[0] = notification_message->data.led.value;
                    break;
                case NotificationMessageTypeLedGreen:
                    // store and send on delay or after seq
                    led_active = true;
                    led_value[1] = notification_message->data.led.value;
                    break;
                case NotificationMessageTypeLedBlue:
                    // store and send on delay or after seq
                    led_active = true;
                    led_value[2] = notification_message->data.led.value;
                    break;
                case NotificationMessageTypeVibro:
                    break;
                case NotificationMessageTypeSound:
                    break;
                case NotificationMessageTypeDelay:
                    if(led_active) {
                        if(is_any_led_layer_internal_and_not_empty(app)) {
                            notification_apply_notification_leds(app, led_off_values);
                            delay(minimal_delay);
                        }

                        led_active = false;
                        notification_apply_notification_leds(app, led_value);
                        delay(notification_message->data.delay.length);
                    }
                    break;
                }
                notification_message_index++;
                notification_message = (*message.sequence)[notification_message_index];
            };

            // send and do minimal delay
            if(led_active) {
                bool need_minimal_delay = false;
                if(is_any_led_layer_internal_and_not_empty(app)) {
                    need_minimal_delay = true;
                }

                led_active = false;
                notification_apply_notification_leds(app, led_value);

                if(need_minimal_delay) {
                    notification_apply_notification_leds(app, led_off_values);
                    delay(minimal_delay);
                }
            }

            notification_reset_notification_led_layer(&app->led[0]);
            notification_reset_notification_led_layer(&app->led[1]);
            notification_reset_notification_led_layer(&app->led[2]);

        } else if(message.type == InternalLayerMessage) {
            uint32_t notification_message_index = 0;
            const NotificationMessage* notification_message;
            notification_message = (*message.sequence)[notification_message_index];

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
                        &app->led[0],
                        get_rgb_led_brightness(app, notification_message->data.led.value));
                    break;
                case NotificationMessageTypeLedGreen:
                    notification_apply_internal_led_layer(
                        &app->led[1],
                        get_rgb_led_brightness(app, notification_message->data.led.value));
                    break;
                case NotificationMessageTypeLedBlue:
                    notification_apply_internal_led_layer(
                        &app->led[2],
                        get_rgb_led_brightness(app, notification_message->data.led.value));
                    break;
                default:
                    break;
                }
                notification_message_index++;
                notification_message = (*message.sequence)[notification_message_index];
            }
        }
    }

    return 0;
};