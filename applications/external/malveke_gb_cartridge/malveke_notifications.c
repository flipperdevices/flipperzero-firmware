#include "malveke_notifications.h"

const NotificationMessage message_delay_note_325 = {
    .type = NotificationMessageTypeDelay,
    .data.delay.length = 325,
};
const NotificationMessage message_delay_note_108 = {
    .type = NotificationMessageTypeDelay,
    .data.delay.length = 108,
};
const NotificationMessage message_delay_note_216 = {
    .type = NotificationMessageTypeDelay,
    .data.delay.length = 216,
};

const NotificationMessage message_delay_note_1302 = {
    .type = NotificationMessageTypeDelay,
    .data.delay.length = 1302,
};
const NotificationMessage message_note_523 = {
    .type = NotificationMessageTypeSoundOn,
    .data.sound.frequency = 523.00f,
    .data.sound.volume = 1.0f,
};
const NotificationMessage message_note_622 = {
    .type = NotificationMessageTypeSoundOn,
    .data.sound.frequency = 622.00f,
    .data.sound.volume = 1.0f,
};
const NotificationMessage message_note_831 = {
    .type = NotificationMessageTypeSoundOn,
    .data.sound.frequency = 831.00f,
    .data.sound.volume = 1.0f,
};
const NotificationMessage message_note_1047 = {
    .type = NotificationMessageTypeSoundOn,
    .data.sound.frequency = 1047.00f,
    .data.sound.volume = 1.0f,
};
const NotificationMessage message_note_1109 = {
    .type = NotificationMessageTypeSoundOn,
    .data.sound.frequency = 1109.00f,
    .data.sound.volume = 1.0f,
};

static const NotificationSequence sequence_get_item = {

    &message_vibro_on,

    &message_note_1047,
    &message_delay_note_325,
    &message_sound_off,

    &message_vibro_off,

    &message_note_1047,
    &message_delay_note_108,
    &message_sound_off,

    &message_note_1047,
    &message_delay_note_108,
    &message_sound_off,

    &message_note_1047,
    &message_delay_note_108,
    &message_sound_off,

    &message_note_1109,
    &message_delay_note_216,
    &message_sound_off,

    &message_note_1109,
    &message_delay_note_216,
    &message_sound_off,

    &message_note_1109,
    &message_delay_note_216,
    &message_sound_off,

    &message_note_1047,
    &message_delay_note_1302,
    &message_sound_off,

    NULL,
};

void notification_success(NotificationApp* notification) {
    notification_message(notification, &sequence_get_item);
}