#pragma once

#include <math.h>
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/elements.h>
// #include <gui/view.h>
// #include <gui/view_dispatcher.h>
#include <input/input.h>
#include <locale/locale.h>
#include <dolphin/dolphin.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include "applications/settings/desktop_settings/desktop_settings_app.h"

#define DAB_TIMER_ISO_DATE_FORMAT "%.4d-%.2d-%.2d"
#define DAB_TIMER_RFC_DATE_FORMAT "%.2d-%.2d-%.4d"
#define DAB_TIMER_TIME_FORMAT "%.2d:%.2d:%.2d"
#define MERIDIAN_FORMAT "%s"
#define MERIDIAN_STRING_AM "AM"
#define MERIDIAN_STRING_PM "PM"
#define TIME_LEN 12
#define DATE_LEN 14
#define MERIDIAN_LEN 3
#define PI 3.14
// #define DAB_TIMER_SETTINGS_PATH CFG_PATH("dab_timer_settings.txt")

typedef struct {
    uint8_t x;
    uint8_t y;
} Vector2;

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} PluginEvent;

typedef enum {
    FaceStylePwn,
    FaceStyleOriginal,
    FaceStyleOriginalSmall,
    FaceStyleCircle,
    FaceStyleBinary,
    FaceStylePwnInverted,
    FaceStyleOriginalInverted,
    FaceStyleOriginalSmallInverted,
    FaceStyleCircleInverted,
    FaceStyleBinaryInverted,
    FaceStyleCount,
} FaceStyle;

typedef enum {
    SoundAlertOff,
    SoundAlertByMin,
    SoundAlertMario,
    SoundAlertGoGoPoRa,
    SoundAlertCont,
    SoundAlertCount,
} SoundAlert;

typedef struct {
    FuriMutex* mutex;
    FuriMessageQueue* event_queue;
    FuriHalRtcDateTime datetime;
    DesktopSettings* desktop_settings;
    LocaleDateFormat date_format;
    LocaleTimeFormat time_format;
    // ViewDispatcher* view_dispatcher;
    uint32_t timer_start_timestamp;
    uint32_t lastexp_timestamp;
    uint32_t timer_stopped_seconds;
    uint32_t timerSecs;
    uint32_t alert_time;
    uint8_t codeSequence;
    uint8_t curEmotiveFace;
    bool timer_running;
    bool w_test;
    FaceStyle faceType;
    SoundAlert songSelect;
} DabTimerState;

const NotificationSequence dab_timer_alert_silent = {
    &message_vibro_on,
    &message_red_255,
    &message_green_255,
    &message_blue_255,
    &message_display_backlight_on,
    &message_vibro_off,
    &message_display_backlight_off,
    &message_delay_50,
    &message_display_backlight_on,
    NULL,
};
const NotificationSequence dab_timer_alert_pr1 = {
    &message_vibro_on,
    &message_red_255,
    &message_green_255,
    &message_blue_255,
    &message_display_backlight_on,
    &message_note_g5,
    &message_delay_100,
    &message_delay_100,
    &message_delay_50,
    &message_sound_off,
    &message_vibro_off,
    &message_display_backlight_off,
    &message_delay_50,
    &message_display_backlight_on,
    &message_note_g5,
    &message_delay_100,
    &message_delay_100,
    &message_delay_50,
    &message_sound_off,
    NULL,
};
const NotificationSequence dab_timer_alert_pr2 = {
    &message_vibro_on,
    &message_note_fs5,
    &message_delay_100,
    &message_delay_100,
    &message_sound_off,
    &message_display_backlight_off,
    &message_vibro_off,
    &message_delay_50,
    &message_note_g5,
    &message_delay_100,
    &message_delay_100,
    &message_sound_off,
    &message_display_backlight_on,
    &message_delay_50,
    &message_note_a5,
    &message_delay_100,
    &message_delay_100,
    &message_sound_off,
    NULL,
};
const NotificationSequence dab_timer_alert_pr3 = {
    &message_display_backlight_off,
    &message_note_g5,
    &message_delay_100,
    &message_delay_100,
    &message_sound_off,
    &message_delay_50,
    &message_red_255,
    &message_green_255,
    &message_blue_255,
    &message_display_backlight_on,
    &message_delay_100,
    NULL,
};
const NotificationSequence dab_timer_alert_mario1 = {
    &message_vibro_on,
    &message_red_255,
    &message_green_255,
    &message_blue_255,
    &message_display_backlight_on,
    &message_note_e5,
    &message_delay_100,
    &message_delay_100,
    &message_delay_50,
    &message_sound_off,
    &message_note_e5,
    &message_delay_100,
    &message_delay_100,
    &message_delay_50,
    &message_sound_off,
    &message_vibro_off,
    &message_display_backlight_off,
    &message_delay_100,
    &message_display_backlight_on,
    &message_delay_100,
    &message_note_e5,
    &message_delay_100,
    &message_delay_100,
    &message_delay_50,
    &message_sound_off,
    NULL,
};
const NotificationSequence dab_timer_alert_mario2 = {
    &message_vibro_on,
    &message_display_backlight_off,
    &message_delay_100,
    &message_display_backlight_on,
    &message_delay_100,
    &message_note_c5,
    &message_delay_100,
    &message_delay_100,
    &message_sound_off,
    &message_display_backlight_off,
    &message_vibro_off,
    &message_delay_50,
    &message_note_e5,
    &message_delay_100,
    &message_delay_100,
    &message_sound_off,
    &message_display_backlight_on,
    NULL,
};
const NotificationSequence dab_timer_alert_mario3 = {
    &message_display_backlight_off,
    &message_note_g5,
    &message_delay_100,
    &message_delay_100,
    &message_delay_100,
    &message_delay_100,
    &message_sound_off,
    &message_delay_50,
    &message_red_255,
    &message_green_255,
    &message_blue_255,
    &message_display_backlight_on,
    &message_delay_100,
    &message_note_g4,
    &message_delay_100,
    &message_delay_100,
    &message_delay_100,
    &message_delay_100,
    &message_sound_off,
    NULL,
};
const NotificationSequence dab_timer_alert_perMin = {
    &message_note_g5,
    &message_delay_100,
    &message_delay_50,
    &message_sound_off,
    &message_delay_10,
    &message_note_g4,
    &message_delay_50,
    &message_delay_10,
    &message_delay_10,
    &message_sound_off,
    NULL,
};
const NotificationSequence dab_timer_alert_startStop = {
    &message_red_255,
    &message_green_255,
    &message_blue_255,
    &message_note_d6,
    &message_delay_100,
    &message_delay_10,
    &message_delay_10,
    &message_sound_off,
    NULL,
};

const NotificationMessage message_red_127 = {
    .type = NotificationMessageTypeLedRed,
    .data.led.value = 0x7F,
};

const NotificationMessage message_green_127 = {
    .type = NotificationMessageTypeLedGreen,
    .data.led.value = 0x7F,
};

const NotificationMessage message_blue_127 = {
    .type = NotificationMessageTypeLedBlue,
    .data.led.value = 0x7F,
};

const NotificationSequence sequence_rainbow = {
    &message_red_255,   &message_green_0,   &message_blue_0,
    &message_delay_250, &message_red_255,   &message_green_127,
    &message_blue_0,    &message_delay_250, &message_red_255,
    &message_green_255, &message_blue_0,    &message_delay_250,
    &message_red_127,   &message_green_255, &message_blue_0,
    &message_delay_250, &message_red_0,     &message_green_255,
    &message_blue_0,    &message_delay_250, &message_red_0,
    &message_green_255, &message_blue_127,  &message_delay_250,
    &message_red_0,     &message_green_255, &message_blue_255,
    &message_delay_250, &message_red_0,     &message_green_127,
    &message_blue_255,  &message_delay_250, &message_red_0,
    &message_green_0,   &message_blue_255,  &message_delay_250,
    &message_red_127,   &message_green_0,   &message_blue_255,
    &message_delay_250, &message_red_255,   &message_green_0,
    &message_blue_255,  &message_delay_250, &message_red_255,
    &message_green_0,   &message_blue_127,  &message_delay_250,
    &message_red_127,   &message_green_127, &message_blue_127,
    &message_delay_250, &message_red_255,   &message_green_255,
    &message_blue_255,  &message_delay_250, NULL,
};

static Vector2 angle_to_vector2(float angle_in_degrees, uint8_t distance, Vector2 center);

void dab_timer_render_binary_face(
    Canvas* const canvas,
    int32_t value,
    int32_t height,
    uint32_t timer_start_timestamp);
