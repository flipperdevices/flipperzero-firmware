#include <furi.h>
#include <api-hal.h>

typedef struct {
    float frequency;
    float pwm;
} NotificationMessageDataSound;

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} NotificationMessageDataLed;

typedef struct {
    bool on;
} NotificationMessageDataVibro;

typedef struct {
    uint32_t length;
} NotificationMessageDataDelay;

typedef union {
    NotificationMessageDataSound sound;
    NotificationMessageDataLed led;
    NotificationMessageDataVibro vibro;
    NotificationMessageDataDelay delay;
} NotificationMessageData;

typedef enum {
    NotificationMessageTypeVibro,
    NotificationMessageTypeSound,
    NotificationMessageTypeLed,
    NotificationMessageTypeDelay,
} NotificationMessageType;

typedef struct {
    NotificationMessageType type;
    NotificationMessageData data;
} NotificationMessage;

void send_notify_messages_async(const NotificationMessage* messages);