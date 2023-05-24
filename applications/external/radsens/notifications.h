#include <notification/notification_messages.h>

const NotificationSequence sequence_notification_imp0_novibro = {
    NULL,
};

const NotificationSequence sequence_notification_imp0_vibro = {
    NULL,
};

const NotificationSequence sequence_notification_imp1_novibro = {
    &message_blue_255,
    &message_delay_25,
    NULL,
};

const NotificationSequence sequence_notification_imp1_vibro = {
    &message_blue_255,
    &message_vibro_on,
    &message_delay_25,
    NULL,
};

const NotificationSequence sequence_notification_imp2_novibro = {
    &message_blue_255,
    &message_delay_25,

    &message_blue_0,
    &message_delay_250,

    &message_blue_255,
    &message_delay_25,
    NULL,
};

const NotificationSequence sequence_notification_imp2_vibro = {
    &message_blue_255,
    &message_vibro_on,
    &message_delay_25,

    &message_blue_0,
    &message_vibro_off,
    &message_delay_250,

    &message_blue_255,
    &message_vibro_on,
    &message_delay_25,
    NULL,
};

const NotificationSequence sequence_notification_imp3_novibro = {
    &message_blue_255,
    &message_delay_25,

    &message_blue_0,
    &message_delay_250,

    &message_blue_255,
    &message_delay_25,

    &message_blue_0,
    &message_delay_250,

    &message_blue_255,
    &message_delay_25,
    NULL,
};

const NotificationSequence sequence_notification_imp3_vibro = {
    &message_blue_255,
    &message_vibro_on,
    &message_delay_25,

    &message_blue_0,
    &message_vibro_off,
    &message_delay_250,

    &message_blue_255,
    &message_vibro_on,
    &message_delay_25,

    &message_blue_0,
    &message_vibro_off,
    &message_delay_250,

    &message_blue_255,
    &message_vibro_on,
    &message_delay_25,
    NULL,
};

const NotificationSequence sequence_notification_fail = {
    &message_display_backlight_on,
    &message_red_255,
    &message_delay_25,
    NULL,
};

const NotificationSequence* sequence_notification_imp0[] = {
    &sequence_notification_imp0_novibro,
    &sequence_notification_imp0_vibro};
const NotificationSequence* sequence_notification_imp1[] = {
    &sequence_notification_imp1_novibro,
    &sequence_notification_imp1_vibro};
const NotificationSequence* sequence_notification_imp2[] = {
    &sequence_notification_imp2_novibro,
    &sequence_notification_imp2_vibro};
const NotificationSequence* sequence_notification_imp3[] = {
    &sequence_notification_imp3_novibro,
    &sequence_notification_imp3_vibro};

const NotificationSequence** sequence_notification_imps[] = {
    sequence_notification_imp0,
    sequence_notification_imp1,
    sequence_notification_imp2,
    sequence_notification_imp3};
