#include <math.h>
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include <input/input.h>
#include <locale/locale.h>
#include <dolphin/dolphin.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include "applications/settings/desktop_settings/desktop_settings_app.h"
#include "dab_timer_icons.h"

#define TAG "DabTimer"
#define CLOCK_ISO_DATE_FORMAT "%.4d-%.2d-%.2d"
#define CLOCK_RFC_DATE_FORMAT "%.2d-%.2d-%.4d"
#define CLOCK_TIME_FORMAT "%.2d:%.2d:%.2d"
#define MERIDIAN_FORMAT "%s"
#define MERIDIAN_STRING_AM "AM"
#define MERIDIAN_STRING_PM "PM"
#define TIME_LEN 12
#define DATE_LEN 14
#define MERIDIAN_LEN 3
#define PI 3.14

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

typedef struct {
    FuriMutex* mutex;
    FuriMessageQueue* event_queue;
    DesktopSettings* desktop_settings;
    uint32_t timer_start_timestamp;
    uint32_t lastexp_timestamp;
    uint32_t timer_stopped_seconds;
    uint32_t songSelect;
    uint32_t codeSequence;
    uint32_t timerSecs;
    uint32_t alert_time;
    uint32_t faceType;
    uint32_t curEmotiveFace;
    bool timer_running;
    bool w_test;
    LocaleDateFormat date_format;
    LocaleTimeFormat time_format;
    FuriHalRtcDateTime datetime;
} ClockState;

const NotificationSequence clock_alert_silent = {
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
const NotificationSequence clock_alert_pr1 = {
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
const NotificationSequence clock_alert_pr2 = {
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
const NotificationSequence clock_alert_pr3 = {
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
const NotificationSequence clock_alert_mario1 = {
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
const NotificationSequence clock_alert_mario2 = {
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
const NotificationSequence clock_alert_mario3 = {
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
const NotificationSequence clock_alert_perMin = {
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
const NotificationSequence clock_alert_startStop = {
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

static void desktop_view_main_dumbmode_changed(DesktopSettings* settings) {
    settings->is_dumbmode = !settings->is_dumbmode;
    DESKTOP_SETTINGS_SAVE(settings);
}

static void clock_input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);
    PluginEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static Vector2 angle_to_vector2(float angle_in_degrees, uint8_t distance, Vector2 center) {
    float radians = (angle_in_degrees - 90) * (PI / 180);

    Vector2 vec = {
        .x = center.x + cos(radians) * distance,
        .y = center.y + sin(radians) * distance,
    };

    return vec;
}

static void clock_render_callback(Canvas* const canvas, void* ctx) {
    ClockState* state = ctx;
    if(furi_mutex_acquire(state->mutex, 200) != FuriStatusOk) {
        PluginEvent event = {.type = EventTypeTick};
        furi_message_queue_put(state->event_queue, &event, 0);
        return;
    }
    FuriHalRtcDateTime curr_dt;
    furi_hal_rtc_get_datetime(&curr_dt);
    uint32_t curr_ts = furi_hal_rtc_datetime_to_timestamp(&curr_dt);
    char time_string[TIME_LEN];
    char date_string[DATE_LEN];
    char meridian_string[MERIDIAN_LEN];
    char timer_string[20];
    if(state->time_format == LocaleTimeFormat24h) {
        snprintf(
            time_string, TIME_LEN, CLOCK_TIME_FORMAT, curr_dt.hour, curr_dt.minute, curr_dt.second);
    } else {
        bool pm = curr_dt.hour > 12;
        bool pm12 = curr_dt.hour >= 12;
        bool am12 = curr_dt.hour == 0;
        snprintf(
            time_string,
            TIME_LEN,
            CLOCK_TIME_FORMAT,
            pm ? curr_dt.hour - 12 : (am12 ? 12 : curr_dt.hour),
            curr_dt.minute,
            curr_dt.second);

        snprintf(
            meridian_string,
            MERIDIAN_LEN,
            MERIDIAN_FORMAT,
            pm12 ? MERIDIAN_STRING_PM : MERIDIAN_STRING_AM);
    }
    if(state->date_format == LocaleDateFormatYMD) {
        snprintf(
            date_string, DATE_LEN, CLOCK_ISO_DATE_FORMAT, curr_dt.year, curr_dt.month, curr_dt.day);
    } else if(state->date_format == LocaleDateFormatMDY) {
        snprintf(
            date_string, DATE_LEN, CLOCK_RFC_DATE_FORMAT, curr_dt.month, curr_dt.day, curr_dt.year);
    } else {
        snprintf(
            date_string, DATE_LEN, CLOCK_RFC_DATE_FORMAT, curr_dt.day, curr_dt.month, curr_dt.year);
    }
    bool timer_running = state->timer_running;
    uint32_t songSelect = state->songSelect;
    int alert_time = (int)state->alert_time;
    uint32_t timer_start_timestamp = state->timer_start_timestamp;
    uint32_t timer_stopped_seconds = state->timer_stopped_seconds;
    char alertTime[4];
    snprintf(alertTime, sizeof(alertTime), "%d", alert_time);
    furi_mutex_release(state->mutex);
    if(state->faceType == 0 || state->faceType == 4) {
        if(state->faceType == 4) {
            canvas_draw_icon(canvas, 0, 0, &I_black);
            if(timer_start_timestamp != 0) {
                elements_button_left(canvas, "Reset");
            } else {
                elements_button_left(canvas, "F5");
            }
            canvas_set_color(canvas, ColorWhite);
        } else {
            canvas_set_color(canvas, ColorWhite);
            if(timer_start_timestamp != 0) {
                elements_button_left(canvas, "Reset");
            } else {
                elements_button_left(canvas, "F1");
            }
            canvas_set_color(canvas, ColorBlack);
        }
        canvas_set_font(canvas, FontBigNumbers);
        int32_t elapsed_secs = timer_running ? (curr_ts - timer_start_timestamp) :
                                               timer_stopped_seconds;
        snprintf(timer_string, 20, "%.2ld:%.2ld", elapsed_secs / 60, elapsed_secs % 60);
        canvas_draw_str_aligned(canvas, 64, 8, AlignCenter, AlignCenter, time_string); // DRAW TIME
        if(state->w_test && timer_start_timestamp != 0) {
            int32_t elapsed_secs_img = (elapsed_secs % 60) % 5;
            static const Icon* const count_anim[5] = {
                &I_HappyFlipper_128x64, &I_G0ku, &I_g0ku_1, &I_g0ku_2, &I_g0ku_3};
            canvas_draw_icon(canvas, -5, 15, count_anim[elapsed_secs_img]);
            canvas_draw_str_aligned(
                canvas, 96, 31, AlignCenter, AlignTop, timer_string); // DRAW TIMER
        } else if(timer_start_timestamp != 0) {
            canvas_draw_str_aligned(
                canvas, 96, 32, AlignCenter, AlignTop, timer_string); // DRAW TIMER
            static const Icon* const flip_face[25] = {
                &I_angry_flipagotchi,        &I_awake_flipagotchi,
                &I_bored_flipagotchi,        &I_broken_flipagotchi,
                &I_cool_flipagotchi,         &I_debug_flipagotchi,
                &I_demotivated_flipagotchi,  &I_excited_flipagotchi,
                &I_friend_flipagotchi,       &I_grateful_flipagotchi,
                &I_happy_flipagotchi,        &I_intense_flipagotchi,
                &I_lonely_flipagotchi,       &I_look_l_flipagotchi,
                &I_look_l_happy_flipagotchi, &I_look_r_flipagotchi,
                &I_look_r_happy_flipagotchi, &I_motivated_flipagotchi,
                &I_sad_flipagotchi,          &I_sleep2_flipagotchi,
                &I_sleep_flipagotchi,        &I_smart_flipagotchi,
                &I_upload1_flipagotchi,      &I_upload2_flipagotchi,
                &I_upload_flipagotchi};
            canvas_draw_icon(canvas, 1, 32, flip_face[state->curEmotiveFace]);
        } else {
            canvas_draw_icon(canvas, 1, 32, &I_cool_flipagotchi);
        }
        canvas_set_font(canvas, FontBatteryPercent);
        canvas_draw_str_aligned(canvas, 117, 11, AlignCenter, AlignCenter, alertTime);
        canvas_set_font(canvas, FontSecondary);
        canvas_set_font(canvas, FontBatteryPercent);
        if(state->time_format == LocaleTimeFormat12h)
            canvas_draw_str_aligned(canvas, 117, 4, AlignCenter, AlignCenter, meridian_string);
        canvas_draw_str_aligned(canvas, 96, 20, AlignCenter, AlignTop, date_string); // DRAW DATE
    } else if(state->faceType == 1 || state->faceType == 5) {
        canvas_set_font(canvas, FontSecondary);
        if(state->faceType == 5) {
            canvas_draw_icon(canvas, 0, 0, &I_black);
            if(timer_start_timestamp != 0) {
                elements_button_left(canvas, "Reset");
            } else {
                elements_button_left(canvas, "F6");
            }
            canvas_set_color(canvas, ColorWhite);
        } else {
            canvas_set_color(canvas, ColorWhite);
            if(timer_start_timestamp != 0) {
                elements_button_left(canvas, "Reset");
            } else {
                elements_button_left(canvas, "F2");
            }
            canvas_set_color(canvas, ColorBlack);
        }
        canvas_set_font(canvas, FontBigNumbers);
        if(timer_start_timestamp != 0 && !state->w_test) {
            int32_t elapsed_secs = timer_running ? (curr_ts - timer_start_timestamp) :
                                                   timer_stopped_seconds;
            snprintf(timer_string, 20, "%.2ld:%.2ld", elapsed_secs / 60, elapsed_secs % 60);
            canvas_draw_str_aligned(
                canvas, 64, 8, AlignCenter, AlignCenter, time_string); // DRAW TIME
            canvas_draw_str_aligned(
                canvas, 64, 32, AlignCenter, AlignTop, timer_string); // DRAW TIMER
            canvas_set_font(canvas, FontBatteryPercent);
            if(state->time_format == LocaleTimeFormat12h)
                canvas_draw_str_aligned(canvas, 117, 4, AlignCenter, AlignCenter, meridian_string);
            canvas_draw_str_aligned(canvas, 117, 11, AlignCenter, AlignCenter, alertTime);
            canvas_draw_str_aligned(
                canvas, 64, 20, AlignCenter, AlignTop, date_string); // DRAW DATE
            canvas_set_font(canvas, FontSecondary);
        } else {
            if(state->w_test) canvas_set_font(canvas, FontBatteryPercent);
            if(state->w_test && timer_start_timestamp != 0) {
                int32_t elapsed_secs = timer_running ? (curr_ts - timer_start_timestamp) :
                                                       timer_stopped_seconds;
                snprintf(timer_string, 20, "%.2ld:%.2ld", elapsed_secs / 60, elapsed_secs % 60);
                int32_t elapsed_secs_img = (elapsed_secs % 60) % 5;
                int32_t elapsed_secs_img2 = (elapsed_secs % 60) % 4;
                static const Icon* const count_anim[5] = {
                    &I_HappyFlipper_128x64, &I_G0ku, &I_g0ku_1, &I_g0ku_2, &I_g0ku_3};
                static const Icon* const count_anim2[4] = {
                    &I_EviWaiting1_18x21,
                    &I_EviWaiting2_18x21,
                    &I_EviSmile1_18x21,
                    &I_EviSmile2_18x21};
                static const Icon* const count_anim3[4] = {
                    &I_frame_01, &I_frame_02, &I_frame_03, &I_frame_02};
                canvas_draw_icon(canvas, -5, 15, count_anim[elapsed_secs_img]);
                canvas_draw_icon(canvas, 90, 0, count_anim2[elapsed_secs_img2]);
                canvas_draw_icon(canvas, 110, 5, count_anim3[elapsed_secs_img2]);
                canvas_draw_str_aligned(
                    canvas, 64, 32, AlignCenter, AlignTop, timer_string); // DRAW TIMER
            }
            canvas_draw_str_aligned(
                canvas, 64, 26, AlignCenter, AlignCenter, time_string); // DRAW TIME
            canvas_set_font(canvas, FontBatteryPercent);
            if(state->time_format == LocaleTimeFormat12h)
                canvas_draw_str_aligned(canvas, 69, 15, AlignCenter, AlignCenter, meridian_string);
            if(!state->w_test)
                canvas_draw_str_aligned(
                    canvas, 64, 38, AlignCenter, AlignTop, date_string); // DRAW DATE
            canvas_set_font(canvas, FontSecondary);
        }
    } else if(state->faceType == 2 || state->faceType == 6) {
        if(state->faceType == 6) {
            canvas_draw_icon(canvas, 0, 0, &I_black);
            if(timer_start_timestamp != 0) {
                elements_button_left(canvas, "Reset");
            } else {
                elements_button_left(canvas, "F7");
            }
            canvas_set_color(canvas, ColorWhite);
        } else {
            canvas_set_color(canvas, ColorWhite);
            if(timer_start_timestamp != 0) {
                elements_button_left(canvas, "Reset");
            } else {
                elements_button_left(canvas, "F3");
            }
            canvas_set_color(canvas, ColorBlack);
        }
        canvas_set_font(canvas, FontBatteryPercent);
        if(timer_start_timestamp != 0) {
            int32_t elapsed_secs = timer_running ? (curr_ts - timer_start_timestamp) :
                                                   timer_stopped_seconds;
            snprintf(timer_string, 20, "%.2ld:%.2ld", elapsed_secs / 60, elapsed_secs % 60);
            canvas_draw_str_aligned(
                canvas, 64, 31, AlignCenter, AlignTop, timer_string); // DRAW TIMER
        }
        canvas_draw_str_aligned(
            canvas, 64, 26, AlignCenter, AlignCenter, time_string); // DRAW TIME
        canvas_set_font(canvas, FontBatteryPercent);
        if(state->time_format == LocaleTimeFormat12h)
            canvas_draw_str_aligned(canvas, 69, 15, AlignCenter, AlignCenter, meridian_string);
        if(!state->w_test)
            canvas_draw_str_aligned(
                canvas, 64, 38, AlignCenter, AlignTop, date_string); // DRAW DATE
        canvas_set_font(canvas, FontSecondary);
    } else {
        if(state->faceType == 7) {
            canvas_draw_icon(canvas, 0, 0, &I_black);
            if(timer_start_timestamp != 0) {
                elements_button_left(canvas, "Reset");
            } else {
                elements_button_left(canvas, "F8");
            }
            canvas_set_color(canvas, ColorWhite);
        } else {
            canvas_set_color(canvas, ColorWhite);
            if(timer_start_timestamp != 0) {
                elements_button_left(canvas, "Reset");
            } else {
                elements_button_left(canvas, "F4");
            }
            canvas_set_color(canvas, ColorBlack);
        }
        uint8_t width = canvas_width(canvas);
        uint8_t height = canvas_height(canvas);
        Vector2 clock_center = {
            .x = 28 + width / 2,
            .y = height / 2,
        };
        uint8_t radius = MIN(width, height) / 2 - 2;
        canvas_draw_circle(canvas, clock_center.x, clock_center.y, radius);
        FuriString* str = furi_string_alloc();
        for(uint8_t i = 3; i <= 12; i += 3) {
            Vector2 pos = angle_to_vector2(360 / 12 * i, radius - 4, clock_center);

            furi_string_printf(str, "%i", i);

            canvas_draw_str_aligned(
                canvas, pos.x, pos.y, AlignCenter, AlignCenter, furi_string_get_cstr(str));
        }
        Vector2 hour_vec =
            angle_to_vector2(((curr_dt.hour % 12) / 12.f * 360.f), radius - 8, clock_center);
        canvas_draw_line(canvas, clock_center.x, clock_center.y, hour_vec.x, hour_vec.y);
        Vector2 minute_vec =
            angle_to_vector2((curr_dt.minute / 60.f * 360.f), radius - 4, clock_center);
        canvas_draw_line(canvas, clock_center.x, clock_center.y, minute_vec.x, minute_vec.y);
        Vector2 second_vec =
            angle_to_vector2((curr_dt.second / 60.f * 360.f), radius - 2, clock_center);
        canvas_draw_line(canvas, clock_center.x, clock_center.y, second_vec.x, second_vec.y);
        canvas_set_font(canvas, FontSecondary); // app->date_time
        locale_format_date(str, &curr_dt, locale_get_date_format(), ".");
        uint16_t date_str_width = canvas_string_width(canvas, furi_string_get_cstr(str));
        canvas_draw_frame(canvas, 0, 41, date_str_width + 6, 13);
        canvas_draw_str(canvas, 3, 51, furi_string_get_cstr(str));
        if(timer_start_timestamp != 0) {
            int32_t elapsed_secs = timer_running ? (curr_ts - timer_start_timestamp) :
                                                   timer_stopped_seconds;
            snprintf(timer_string, 20, "%.2ld:%.2ld", elapsed_secs / 60, elapsed_secs % 60);
            canvas_draw_str_aligned(
                canvas, 15, 32, AlignCenter, AlignTop, timer_string); // DRAW TIMER
        }
    }
    if(state->faceType >= 4) {
        canvas_set_color(canvas, ColorBlack);
    }
    if(state->faceType < 4) {
        canvas_set_color(canvas, ColorWhite);
    }
    if(!state->desktop_settings->is_dumbmode && !state->w_test) {
        if(timer_running) {
            elements_button_center(canvas, "Stop");
        } else {
            elements_button_center(canvas, "Start");
        }
    }
    if(timer_running && !state->w_test) {
        if(songSelect == 0) {
            elements_button_right(canvas, "S:OFF");
        } else if(songSelect == 1) {
            elements_button_right(canvas, "S:PoRa");
        } else if(songSelect == 2) {
            elements_button_right(canvas, "S:Mario");
        } else if(songSelect == 3) {
            elements_button_right(canvas, "S:ByMin");
        }
    }
    // if(state->faceType >= 4) canvas_set_color(canvas, ColorBlack);
    // if(state->faceType < 4) canvas_set_color(canvas, ColorWhite);
    if(state->w_test && state->desktop_settings->is_dumbmode) {
        canvas_draw_icon(canvas, 0, 0, &I_GameMode_11x8);
    }
}

static void clock_state_init(ClockState* const state) {
    memset(state, 0, sizeof(ClockState));
    state->songSelect = 2;
    state->codeSequence = 0;
    state->lastexp_timestamp = 0;
    state->timer_start_timestamp = 0;
    state->timer_stopped_seconds = 0;
    state->timerSecs = 0;
    state->faceType = 0;
    state->curEmotiveFace = 0;
    state->alert_time = 80;
    state->desktop_settings = malloc(sizeof(DesktopSettings));
    state->w_test = false;
    state->time_format = locale_get_time_format();
    state->date_format = locale_get_date_format();
}

// Runs every 1000ms by default
static void clock_tick(void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    PluginEvent event = {.type = EventTypeTick};
    // It's OK to lose this event if system overloaded
    furi_message_queue_put(event_queue, &event, 0);
}

int32_t clock_app(void* p) {
    UNUSED(p);
    ClockState* plugin_state = malloc(sizeof(ClockState));
    clock_state_init(plugin_state);
    plugin_state->event_queue = furi_message_queue_alloc(8, sizeof(PluginEvent));
    if(plugin_state->event_queue == NULL) {
        FURI_LOG_E(TAG, "cannot create event queue\n");
        free(plugin_state);
        return 255;
    }
    plugin_state->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if(plugin_state->mutex == NULL) {
        FURI_LOG_E(TAG, "cannot create mutex\n");
        furi_message_queue_free(plugin_state->event_queue);
        free(plugin_state);
        return 255;
    }
    FuriTimer* timer =
        furi_timer_alloc(clock_tick, FuriTimerTypePeriodic, plugin_state->event_queue);
    if(timer == NULL) {
        FURI_LOG_E(TAG, "cannot create timer\n");
        furi_mutex_free(plugin_state->mutex);
        furi_message_queue_free(plugin_state->event_queue);
        free(plugin_state);
        return 255;
    }
    DESKTOP_SETTINGS_LOAD(plugin_state->desktop_settings);
    // Set system callbacks
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, clock_render_callback, plugin_state);
    view_port_input_callback_set(view_port, clock_input_callback, plugin_state->event_queue);
    NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
    // Open GUI and register view_port
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);
    furi_timer_start(timer, furi_kernel_get_tick_frequency());
    // Main loop
    PluginEvent event;
    for(bool processing = true; processing;) {
        FuriStatus event_status = furi_message_queue_get(plugin_state->event_queue, &event, 100);
        if(event_status == FuriStatusOk) {
            if(furi_mutex_acquire(plugin_state->mutex, FuriWaitForever) != FuriStatusOk) continue;
            // press events
            if(event.type == EventTypeKey) {
                if(event.input.type == InputTypeShort || event.input.type == InputTypeRepeat) {
                    switch(event.input.key) {
                    case InputKeyUp:
                        if(plugin_state->codeSequence == 0 || plugin_state->codeSequence == 1) {
                            plugin_state->codeSequence++;
                            if(plugin_state->timer_running)
                                plugin_state->alert_time = plugin_state->alert_time + 5;
                        } else {
                            plugin_state->codeSequence = 0;
                            if(plugin_state->timer_running)
                                plugin_state->alert_time = plugin_state->alert_time + 5;
                        }
                        break;
                    case InputKeyDown:
                        if(plugin_state->codeSequence == 2 || plugin_state->codeSequence == 3) {
                            plugin_state->codeSequence++;
                            if(plugin_state->timer_running)
                                plugin_state->alert_time = plugin_state->alert_time - 5;
                        } else {
                            plugin_state->codeSequence = 0;
                            if(plugin_state->timer_running)
                                plugin_state->alert_time = plugin_state->alert_time - 5;
                        }
                        break;
                    case InputKeyRight:
                        if(plugin_state->codeSequence == 5 || plugin_state->codeSequence == 7) {
                            plugin_state->codeSequence++;
                            if(plugin_state->codeSequence == 8) {
                                desktop_view_main_dumbmode_changed(plugin_state->desktop_settings);
                                plugin_state->w_test = true; // OH HEY NOW LETS GAIN EXP & MORE FUN
                            }
                        } else {
                            plugin_state->codeSequence = 0;
                            if(plugin_state->songSelect <= 2) {
                                plugin_state->songSelect = plugin_state->songSelect + 1;
                            } else {
                                plugin_state->songSelect = 0;
                            }
                        }
                        break;
                    case InputKeyLeft:
                        if(plugin_state->codeSequence == 4 || plugin_state->codeSequence == 6) {
                            plugin_state->codeSequence++;
                        } else {
                            plugin_state->codeSequence = 0;
                            if(plugin_state->timer_start_timestamp != 0) {
                                FuriHalRtcDateTime curr_dt;
                                furi_hal_rtc_get_datetime(&curr_dt);
                                uint32_t curr_ts = furi_hal_rtc_datetime_to_timestamp(&curr_dt);
                                plugin_state->timer_start_timestamp = curr_ts;
                                plugin_state->timer_stopped_seconds = 0;
                                plugin_state->timerSecs = 0;
                            } else {
                                if(plugin_state->faceType <= 6) {
                                    plugin_state->faceType = plugin_state->faceType + 1;
                                } else {
                                    plugin_state->faceType = 0;
                                }
                            }
                        }
                        break;
                    case InputKeyOk:
                        if(plugin_state->codeSequence == 9) {
                            plugin_state->codeSequence++;
                        } else {
                            plugin_state->codeSequence = 0;
                            if(!plugin_state->desktop_settings->is_dumbmode) {
                                if(plugin_state->songSelect == 1 ||
                                   plugin_state->songSelect == 2 ||
                                   plugin_state->songSelect == 3) {
                                    notification_message(notification, &clock_alert_startStop);
                                }
                                // START/STOP TIMER
                                FuriHalRtcDateTime curr_dt;
                                furi_hal_rtc_get_datetime(&curr_dt);
                                uint32_t curr_ts = furi_hal_rtc_datetime_to_timestamp(&curr_dt);
                                if(plugin_state->timer_running) {
                                    // Update stopped seconds
                                    plugin_state->timer_stopped_seconds =
                                        curr_ts - plugin_state->timer_start_timestamp;
                                } else {
                                    if(plugin_state->timer_start_timestamp == 0) {
                                        // Set starting timestamp if this is first time
                                        plugin_state->timer_start_timestamp = curr_ts;
                                    } else {
                                        // Timer was already running, need to slightly readjust so we don't
                                        // count the intervening time
                                        plugin_state->timer_start_timestamp =
                                            curr_ts - plugin_state->timer_stopped_seconds;
                                    }
                                }
                                plugin_state->timer_running = !plugin_state->timer_running;
                            }
                        }
                        break;
                    case InputKeyBack:
                        if(plugin_state->codeSequence == 8) {
                            plugin_state->codeSequence++;
                        } else {
                            plugin_state->w_test = false;
                            // Don't Exit the plugin
                            plugin_state->codeSequence--;
                            if(plugin_state->codeSequence < (uint32_t)-1) processing = false;
                        }
                        break;
                    default:
                        break;
                    }
                    if(plugin_state->codeSequence == 10) {
                        plugin_state->codeSequence = 0;
                        plugin_state->desktop_settings->is_dumbmode =
                            true; // MAKE SURE IT'S ON SO IT GETS TURNED OFF
                        desktop_view_main_dumbmode_changed(plugin_state->desktop_settings);
                        if(plugin_state->songSelect == 1 || plugin_state->songSelect == 2 ||
                           plugin_state->songSelect == 3) {
                            notification_message(notification, &sequence_success);
                            notification_message(notification, &sequence_rainbow);
                            notification_message(notification, &sequence_rainbow);
                        }
                        dolphin_deed(getRandomDeed());
                    }
                } else if(event.input.type == InputTypeLong) {
                    if(event.input.key == InputKeyLeft) {
                        plugin_state->codeSequence = 0;
                        if(plugin_state->timer_start_timestamp != 0) {
                            // Reset seconds
                            plugin_state->timer_running = false;
                            plugin_state->timer_start_timestamp = 0;
                            plugin_state->timer_stopped_seconds = 0;
                            plugin_state->timerSecs = 0;
                        }
                    } else if(event.input.key == InputKeyBack) {
                        // Exit the plugin
                        processing = false;
                    }
                }
            } else if(event.type == EventTypeTick) {
                // Do nothing, just need to update viewport
                if(plugin_state->timer_running) {
                    plugin_state->timerSecs = plugin_state->timerSecs + 1;
                    if(plugin_state->timerSecs != 0 && plugin_state->timerSecs % 3 == 0) {
                        plugin_state->curEmotiveFace = plugin_state->curEmotiveFace + 1;
                        if(plugin_state->curEmotiveFace == 25) plugin_state->curEmotiveFace = 0;
                    }
                    if(plugin_state->timerSecs % 60 == 0 && plugin_state->timerSecs != 0 &&
                       plugin_state->songSelect != 0 &&
                       (plugin_state->timerSecs < plugin_state->alert_time ||
                        plugin_state->songSelect == 3)) {
                        notification_message(notification, &clock_alert_perMin);
                    }
                    if(plugin_state->songSelect == 1) {
                        if(plugin_state->timerSecs == plugin_state->alert_time) {
                            FuriHalRtcDateTime curr_dt;
                            furi_hal_rtc_get_datetime(&curr_dt);
                            uint32_t curr_ts = furi_hal_rtc_datetime_to_timestamp(&curr_dt);
                            if(plugin_state->lastexp_timestamp + 10 <= curr_ts &&
                               plugin_state->w_test) {
                                plugin_state->lastexp_timestamp = curr_ts;
                                dolphin_deed(getRandomDeed());
                            }
                            notification_message(notification, &clock_alert_pr1);
                        }
                        if(plugin_state->timerSecs == plugin_state->alert_time + 1) {
                            notification_message(notification, &clock_alert_pr2);
                        }
                        if(plugin_state->timerSecs == plugin_state->alert_time + 2) {
                            notification_message(notification, &clock_alert_pr3);
                            notification_message(notification, &sequence_rainbow);
                            notification_message(notification, &sequence_rainbow);
                        }
                    } else if(plugin_state->songSelect == 2) {
                        if(plugin_state->timerSecs == plugin_state->alert_time) {
                            FuriHalRtcDateTime curr_dt;
                            furi_hal_rtc_get_datetime(&curr_dt);
                            uint32_t curr_ts = furi_hal_rtc_datetime_to_timestamp(&curr_dt);
                            if(plugin_state->lastexp_timestamp + 10 <= curr_ts &&
                               plugin_state->w_test) {
                                plugin_state->lastexp_timestamp = curr_ts;
                                dolphin_deed(getRandomDeed());
                            }
                            notification_message(notification, &clock_alert_mario1);
                        }
                        if(plugin_state->timerSecs == plugin_state->alert_time + 1) {
                            notification_message(notification, &clock_alert_mario2);
                        }
                        if(plugin_state->timerSecs == plugin_state->alert_time + 2) {
                            notification_message(notification, &clock_alert_mario3);
                            notification_message(notification, &sequence_rainbow);
                            notification_message(notification, &sequence_rainbow);
                        }
                    } else {
                        if(plugin_state->timerSecs == plugin_state->alert_time) {
                            FuriHalRtcDateTime curr_dt;
                            furi_hal_rtc_get_datetime(&curr_dt);
                            uint32_t curr_ts = furi_hal_rtc_datetime_to_timestamp(&curr_dt);
                            if(plugin_state->lastexp_timestamp + 10 <= curr_ts &&
                               plugin_state->w_test) {
                                plugin_state->lastexp_timestamp = curr_ts;
                                dolphin_deed(getRandomDeed());
                            }
                            notification_message(notification, &clock_alert_silent);
                            notification_message(notification, &sequence_rainbow);
                            notification_message(notification, &sequence_rainbow);
                        }
                    }
                }
            }
        }
        view_port_update(view_port);
        furi_mutex_release(plugin_state->mutex);
    }
    furi_timer_free(timer);
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_GUI);
    view_port_free(view_port);
    furi_message_queue_free(plugin_state->event_queue);
    furi_mutex_free(plugin_state->mutex);
    free(plugin_state->desktop_settings);
    free(plugin_state);
    return 0;
}