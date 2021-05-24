#include <furi.h>
#include <api-hal.h>
#include "notification.h"
#include "notification-messages.h"

#define NOTIFICATION_LED_COUNT 3
#define NOTIFICATION_EVENT_COMPLETE 0x00000001U

typedef enum {
    NotificationLayerMessage,
    InternalLayerMessage,
} NotificationAppMessageType;

typedef struct {
    const NotificationSequence* sequence;
    NotificationAppMessageType type;
    osEventFlagsId_t back_event;
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
    float display_brightness;
    float led_brightness;
} NotificationSettings;

struct NotificationApp {
    osMessageQueueId_t queue;
    osTimerId_t display_timer;

    NotificationLedLayer display;
    NotificationLedLayer led[NOTIFICATION_LED_COUNT];

    NotificationSettings settings;
};

void notification_message(NotificationApp* app, const NotificationSequence* sequence) {
    NotificationAppMessage m = {
        .type = NotificationLayerMessage, .sequence = sequence, .back_event = NULL};
    furi_check(osMessageQueuePut(app->queue, &m, 0, osWaitForever) == osOK);
};

void notification_internal_message(NotificationApp* app, const NotificationSequence* sequence) {
    NotificationAppMessage m = {
        .type = InternalLayerMessage, .sequence = sequence, .back_event = NULL};
    furi_check(osMessageQueuePut(app->queue, &m, 0, osWaitForever) == osOK);
};

void notification_message_block(NotificationApp* app, const NotificationSequence* sequence) {
    NotificationAppMessage m = {
        .type = NotificationLayerMessage,
        .sequence = sequence,
        .back_event = osEventFlagsNew(NULL)};
    furi_check(osMessageQueuePut(app->queue, &m, 0, osWaitForever) == osOK);
    osEventFlagsWait(m.back_event, NOTIFICATION_EVENT_COMPLETE, osFlagsWaitAny, osWaitForever);
    osEventFlagsDelete(m.back_event);
};

void notification_internal_message_block(
    NotificationApp* app,
    const NotificationSequence* sequence) {
    NotificationAppMessage m = {
        .type = InternalLayerMessage, .sequence = sequence, .back_event = osEventFlagsNew(NULL)};
    furi_check(osMessageQueuePut(app->queue, &m, 0, osWaitForever) == osOK);
    osEventFlagsWait(m.back_event, NOTIFICATION_EVENT_COMPLETE, osFlagsWaitAny, osWaitForever);
    osEventFlagsDelete(m.back_event);
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
    layer->value[LayerNotification] = 0;
    // set layer
    layer->index = LayerInternal;

    // apply
    api_hal_light_set(layer->light, layer->value[LayerInternal]);
}

uint8_t get_display_brightness(NotificationApp* app, uint8_t value) {
    return (value * app->settings.display_brightness);
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

void notification_vibro_on() {
    api_hal_vibro_on(true);
}

void notification_vibro_off() {
    api_hal_vibro_on(false);
}

void notification_sound_on(float pwm, float freq) {
    hal_pwm_set(pwm, freq, &SPEAKER_TIM, SPEAKER_CH);
}

void notification_sound_off() {
    hal_pwm_stop(&SPEAKER_TIM, SPEAKER_CH);
}

void display_timer(void* ctx) {
    furi_assert(ctx);
    NotificationApp* app = ctx;
    notification_message(app, &sequence_display_off);
}

NotificationApp* notification_app_alloc() {
    NotificationApp* app = furi_alloc(sizeof(NotificationApp));
    app->queue = osMessageQueueNew(8, sizeof(NotificationAppMessage), NULL);
    app->display_timer = osTimerNew(display_timer, osTimerOnce, app, NULL);

    app->settings.display_brightness = 1.0f;
    app->settings.led_brightness = 1.0f;

    app->display.value[LayerInternal] = 0x00;
    app->display.value[LayerNotification] = 0x00;
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

    notification_vibro_off();
    notification_sound_off();
    notification_apply_internal_led_layer(&app->display, 0x00);
    notification_apply_internal_led_layer(&app->led[0], 0x00);
    notification_apply_internal_led_layer(&app->led[1], 0x00);
    notification_apply_internal_led_layer(&app->led[2], 0x00);

    furi_record_create("notification", app);

    const uint8_t minimal_delay = 100;
    const uint32_t display_off_delay = 30000.0f / (1000.0f / osKernelGetTickFreq());
    const uint8_t led_off_values[NOTIFICATION_LED_COUNT] = {0x00, 0x00, 0x00};

    NotificationAppMessage message;
    while(1) {
        furi_check(osMessageQueueGet(app->queue, &message, NULL, osWaitForever) == osOK);

        if(message.type == NotificationLayerMessage) {
            uint32_t notification_message_index = 0;
            const NotificationMessage* notification_message;
            notification_message = (*message.sequence)[notification_message_index];

            bool led_active = false;
            uint8_t led_values[NOTIFICATION_LED_COUNT] = {0x00, 0x00, 0x00};
            bool reset_notifications = true;

            const uint8_t reset_red_mask = 1 << 0;
            const uint8_t reset_green_mask = 1 << 1;
            const uint8_t reset_blue_mask = 1 << 2;
            const uint8_t reset_vibro_mask = 1 << 3;
            const uint8_t reset_sound_mask = 1 << 4;
            const uint8_t reset_display_mask = 1 << 5;

            uint8_t reset_mask = 0;

            while(notification_message != NULL) {
                switch(notification_message->type) {
                case NotificationMessageTypeLedDisplay:
                    // if on - switch on and start timer
                    // if off - switch off and stop timer
                    // on timer - switch off
                    if(notification_message->data.led.value > 0x00) {
                        notification_apply_notification_led_layer(
                            &app->display,
                            get_display_brightness(app, notification_message->data.led.value));
                    } else {
                        notification_reset_notification_led_layer(&app->display);
                        if(osTimerIsRunning(app->display_timer)) {
                            osTimerStop(app->display_timer);
                        }
                    }
                    reset_mask |= reset_display_mask;
                    break;
                case NotificationMessageTypeLedRed:
                    // store and send on delay or after seq
                    led_active = true;
                    led_values[0] = notification_message->data.led.value;
                    reset_mask |= reset_red_mask;
                    break;
                case NotificationMessageTypeLedGreen:
                    // store and send on delay or after seq
                    led_active = true;
                    led_values[1] = notification_message->data.led.value;
                    reset_mask |= reset_green_mask;
                    break;
                case NotificationMessageTypeLedBlue:
                    // store and send on delay or after seq
                    led_active = true;
                    led_values[2] = notification_message->data.led.value;
                    reset_mask |= reset_blue_mask;
                    break;
                case NotificationMessageTypeVibro:
                    if(notification_message->data.vibro.on) {
                        notification_vibro_on();
                    } else {
                        notification_vibro_off();
                    }
                    reset_mask |= reset_vibro_mask;
                    break;
                case NotificationMessageTypeSoundOn:
                    notification_sound_on(
                        notification_message->data.sound.pwm,
                        notification_message->data.sound.frequency);
                    reset_mask |= reset_sound_mask;
                    break;
                case NotificationMessageTypeSoundOff:
                    notification_sound_off();
                    reset_mask |= reset_sound_mask;
                    break;
                case NotificationMessageTypeDelay:
                    if(led_active) {
                        if(is_any_led_layer_internal_and_not_empty(app)) {
                            notification_apply_notification_leds(app, led_off_values);
                            delay(minimal_delay);
                        }

                        led_active = false;

                        notification_apply_notification_leds(app, led_values);
                        reset_mask |= reset_red_mask;
                        reset_mask |= reset_green_mask;
                        reset_mask |= reset_blue_mask;
                    }

                    delay(notification_message->data.delay.length);
                    break;
                case NotificationMessageTypeDoNotReset:
                    reset_notifications = false;
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
                notification_apply_notification_leds(app, led_values);
                reset_mask |= reset_red_mask;
                reset_mask |= reset_green_mask;
                reset_mask |= reset_blue_mask;

                if(need_minimal_delay) {
                    notification_apply_notification_leds(app, led_off_values);
                    delay(minimal_delay);
                }
            }

            if(reset_notifications) {
                if(reset_mask & reset_red_mask) {
                    notification_reset_notification_led_layer(&app->led[0]);
                }
                if(reset_mask & reset_green_mask) {
                    notification_reset_notification_led_layer(&app->led[1]);
                }
                if(reset_mask & reset_blue_mask) {
                    notification_reset_notification_led_layer(&app->led[2]);
                }
                if(reset_mask & reset_vibro_mask) {
                    notification_vibro_off();
                }
                if(reset_mask & reset_sound_mask) {
                    notification_sound_off();
                }
                if(reset_mask & reset_display_mask) {
                    osTimerStart(app->display_timer, display_off_delay);
                }
            }

            if(message.back_event != NULL) {
                osEventFlagsSet(message.back_event, NOTIFICATION_EVENT_COMPLETE);
            }
        } else if(message.type == InternalLayerMessage) {
            uint32_t notification_message_index = 0;
            const NotificationMessage* notification_message;
            notification_message = (*message.sequence)[notification_message_index];

            while(notification_message != NULL) {
                switch(notification_message->type) {
                case NotificationMessageTypeLedDisplay:
                    notification_apply_internal_led_layer(
                        &app->display,
                        get_display_brightness(app, notification_message->data.led.value));
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

            if(message.back_event != NULL) {
                osEventFlagsSet(message.back_event, NOTIFICATION_EVENT_COMPLETE);
            }
        }
    }

    return 0;
};