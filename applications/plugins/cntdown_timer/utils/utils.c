#include "utils.h"

static const NotificationSequence sequence_finish = {
    &message_display_backlight_on,
    &message_blue_255,
    &message_vibro_on,
    &message_note_c5,
    &message_delay_250,
    &message_note_c5,
    &message_delay_500,
    &message_note_c5,
    &message_delay_100,
    &message_note_c5,
    &message_delay_500,
    &message_sound_off,
    &message_vibro_off,
    &message_delay_250,
    NULL,
};

static const NotificationSequence sequence_beep = {
    &message_blue_255,
    &message_note_d5,
    &message_delay_100,
    &message_sound_off,

    NULL,
};

void notification_beep_once() {
    notification_message(furi_record_open(RECORD_NOTIFICATION), &sequence_beep);
    notification_off();
}

void notification_off() {
    furi_record_close(RECORD_NOTIFICATION);
}

void notification_timeup() {
    notification_message(furi_record_open(RECORD_NOTIFICATION), &sequence_finish);
}
