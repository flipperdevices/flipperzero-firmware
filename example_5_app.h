#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <notification/notification_messages.h>

#include "example_5_icons.h"

typedef enum {
    DRAW_ALL,
    DRAW_ONLY_TEXT,
    DRAW_ONLY_PICTURES,
    TOTAL_DRAW_MODES = 3,
} DrawMode;

struct Example5App {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    NotificationApp* notifications;

    DrawMode draw_mode;
};

typedef struct Example5App Example5App;

const NotificationSequence example_led_sequence = {
    &message_red_255,
    &message_blue_255,
    &message_delay_500,
    &message_red_0,
    &message_blue_0,
    &message_delay_500,
    &message_red_255,
    &message_blue_255,
    &message_delay_500,
    &message_red_0,
    &message_blue_0,
    &message_delay_500,
    &message_red_255,
    &message_blue_255,
    &message_delay_500,
    &message_red_0,
    &message_blue_0,
    NULL,
};

const NotificationSequence example_vibro_sequence = {
    &message_vibro_on,
    &message_do_not_reset,
    &message_delay_1000,
    &message_delay_1000,
    &message_delay_1000,
    &message_vibro_off,
    NULL,
};

const NotificationSequence example_sound_sequence = {
    &message_note_e5,   &message_delay_100, &message_note_d5,
    &message_delay_100, &message_note_fs4,  &message_delay_250,
    &message_note_gs4,  &message_delay_250, &message_note_cs5,
    &message_delay_100, &message_note_b4,   &message_delay_100,
    &message_note_d4,   &message_delay_250, &message_note_e4,
    &message_delay_250, &message_note_b4,   &message_delay_100,
    &message_note_a4,   &message_delay_100, &message_note_cs4,
    &message_delay_250, &message_note_e4,   &message_delay_250,
    &message_note_a4,   &message_delay_500, NULL,
};