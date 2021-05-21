#pragma once
#include "stdint.h"
#include "stdbool.h"

typedef struct NotificationApp NotificationApp;

typedef struct {
    float frequency;
    float pwm;
} NotificationMessageDataSound;

typedef struct {
    uint8_t value;
} NotificationMessageDataLed;

typedef struct {
    bool on;
} NotificationMessageDataVibro;

typedef struct {
    bool on;
} NotificationMessageDataDisplay;

typedef struct {
    uint32_t length;
} NotificationMessageDataDelay;

typedef union {
    NotificationMessageDataSound sound;
    NotificationMessageDataLed led;
    NotificationMessageDataVibro vibro;
    NotificationMessageDataDelay delay;
    NotificationMessageDataDisplay display;
} NotificationMessageData;

typedef enum {
    NotificationMessageTypeVibro,
    NotificationMessageTypeSound,
    NotificationMessageTypeLedRed,
    NotificationMessageTypeLedGreen,
    NotificationMessageTypeLedBlue,
    NotificationMessageTypeDelay,
    NotificationMessageTypeDisplay,
} NotificationMessageType;

typedef struct {
    NotificationMessageType type;
    NotificationMessageData data;
} NotificationMessage;

typedef const NotificationMessage* NotificationSequence[];

void notification_message(NotificationApp* app, const NotificationSequence* sequence);