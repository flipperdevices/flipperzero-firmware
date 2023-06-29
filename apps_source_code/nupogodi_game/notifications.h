#include <notification/notification.h>
#include <notification/notification_messages.h>
#include <notification/notification_messages_notes.h>

const NotificationSequence notification_egg1_sound = {
    &message_note_c5,
    &message_delay_100,
    NULL,
};
const NotificationSequence notification_egg2_sound = {
    &message_note_d5,
    &message_delay_100,
    NULL,
};
const NotificationSequence notification_egg3_sound = {
    &message_note_e5,
    &message_delay_100,
    NULL,
};
const NotificationSequence notification_egg4_sound = {
    &message_note_g5,
    &message_delay_100,
    NULL,
};

const NotificationSequence notification_egg_vibro = {
    &message_vibro_on,
    &message_delay_25,
    &message_vibro_off,
    NULL,
};

const NotificationSequence* notification_egg1[] = {
    &notification_egg1_sound,
    &notification_egg_vibro};

const NotificationSequence* notification_egg2[] = {
    &notification_egg2_sound,
    &notification_egg_vibro};

const NotificationSequence* notification_egg3[] = {
    &notification_egg3_sound,
    &notification_egg_vibro};

const NotificationSequence* notification_egg4[] = {
    &notification_egg4_sound,
    &notification_egg_vibro};

const NotificationSequence** notification_eggs[] =
    {notification_egg1, notification_egg2, notification_egg3, notification_egg4};

const NotificationSequence notification_done_sound = {
    &message_note_c6,
    &message_delay_100,
    NULL,
};

const NotificationSequence notification_done_vibro = {
    &message_vibro_on,
    &message_delay_50,
    &message_vibro_off,
    NULL,
};

const NotificationSequence* notification_done[] = {
    &notification_done_sound,
    &notification_done_vibro};

const NotificationSequence notification_fail_sound = {
    &message_note_c0,
    &message_delay_100,
    NULL,
};

const NotificationSequence notification_fail_vibro = {
    &message_vibro_on,
    &message_delay_100,
    &message_vibro_off,
    NULL,
};

const NotificationSequence* notification_fail[] = {
    &notification_fail_sound,
    &notification_fail_vibro};
