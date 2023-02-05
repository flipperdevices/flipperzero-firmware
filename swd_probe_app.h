#ifndef __ARHA_FLIPPERAPP_DEMO
#define __ARHA_FLIPPERAPP_DEMO

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_speaker.h>
#include <gui/gui.h>
#include <input/input.h>

#include <lib/subghz/receiver.h>
#include <lib/subghz/transmitter.h>
#include <lib/subghz/subghz_file_encoder_worker.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>

#define TAG "SWD"

#define TIMER_HZ 50

typedef enum { KeyNone, KeyUp, KeyRight, KeyDown, KeyLeft, KeyOK } KeyCode;

typedef enum {
    EventTimerTick,
    EventKeyPress,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} AppEvent;

typedef struct {
    KeyCode last_key;

    FuriTimer* _timer;
    FuriMessageQueue* _event_queue;
    NotificationApp* notification;

    uint8_t current_mask_id;
    uint32_t current_mask;
    uint8_t io_swc;
    uint8_t io_swd;
    uint32_t dpidr;
    uint32_t detected_timeout;
    bool detected;
    bool detected_device;
    bool detected_notified;
    char state_string[32];
} AppFSM;

const NotificationSequence seq_c_minor = {
    &message_note_c4,
    &message_delay_100,
    &message_sound_off,
    &message_delay_10,

    &message_note_ds4,
    &message_delay_100,
    &message_sound_off,
    &message_delay_10,

    &message_note_g4,
    &message_delay_100,
    &message_sound_off,
    &message_delay_10,

    &message_vibro_on,
    &message_delay_50,
    &message_vibro_off,
    NULL,
};

#endif