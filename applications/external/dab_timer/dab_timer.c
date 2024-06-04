#include "dab_timer.h"
#include "dab_timer_icons.h"

#define TAG "DabTimer"

static Vector2 angle_to_vector2(float angle_in_degrees, uint8_t distance, Vector2 center) {
    float radians = (angle_in_degrees - 90) * (PI / 180);

    Vector2 vec = {
        .x = center.x + cos(radians) * distance,
        .y = center.y + sin(radians) * distance,
    };

    return vec;
}

#if __has_include(<cfw/cfw.h>)
static void dab_timer_dumbmode_changed(DesktopSettings* settings) {
    settings->is_dumbmode = !settings->is_dumbmode;
    DESKTOP_SETTINGS_SAVE(settings);
}
#endif

void dab_timer_render_binary_face(
    Canvas* const canvas,
    int32_t value,
    int32_t height,
    uint32_t timer_start_timestamp) {
    if(timer_start_timestamp != 0) {
        // FuriString* str = furi_string_alloc();
        // furi_string_printf(str, "%li", value);
        // canvas_draw_str_aligned(
        // canvas, 10, height+4, AlignCenter, AlignCenter, furi_string_get_cstr(str)); // DRAW TIME
        // furi_string_free(str);
    }
    int32_t h_loc_start = 24;
    if(value >= 32) {
        canvas_draw_icon(canvas, h_loc_start, height, &I_GameModeIcon_11x8);
        value = value - 32;
    } else {
        canvas_draw_icon(canvas, h_loc_start, height, &I_InvertGameMode_11x8);
    }
    if(value >= 16) {
        canvas_draw_icon(canvas, h_loc_start + (14 * 1), height, &I_GameModeIcon_11x8);
        value = value - 16;
    } else {
        canvas_draw_icon(canvas, h_loc_start + (14 * 1), height, &I_InvertGameMode_11x8);
    }
    if(value >= 8) {
        canvas_draw_icon(canvas, h_loc_start + (14 * 2), height, &I_GameModeIcon_11x8);
        value = value - 8;
    } else {
        canvas_draw_icon(canvas, h_loc_start + (14 * 2), height, &I_InvertGameMode_11x8);
    }
    if(value >= 4) {
        canvas_draw_icon(canvas, h_loc_start + (14 * 3), height, &I_GameModeIcon_11x8);
        value = value - 4;
    } else {
        canvas_draw_icon(canvas, h_loc_start + (14 * 3), height, &I_InvertGameMode_11x8);
    }
    if(value >= 2) {
        canvas_draw_icon(canvas, h_loc_start + (14 * 4), height, &I_GameModeIcon_11x8);
        value = value - 2;
    } else {
        canvas_draw_icon(canvas, h_loc_start + (14 * 4), height, &I_InvertGameMode_11x8);
    }
    if(value >= 1) {
        canvas_draw_icon(canvas, h_loc_start + (14 * 5), height, &I_GameModeIcon_11x8);
        value = value - 1;
    } else {
        canvas_draw_icon(canvas, h_loc_start + (14 * 5), height, &I_InvertGameMode_11x8);
    }
}

static void dab_timer_input_callback(InputEvent* input_event, void* ctx) {
    FuriMessageQueue* event_queue = ctx;
    furi_assert(event_queue);
    PluginEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void dab_timer_render_callback(Canvas* const canvas, void* ctx) {
    DabTimerState* plugin_state = ctx;
    if(furi_mutex_acquire(plugin_state->mutex, 200) != FuriStatusOk) {
        PluginEvent event = {.type = EventTypeTick};
        furi_message_queue_put(plugin_state->event_queue, &event, 0);
        return;
    }
    DateTime curr_dt;
    furi_hal_rtc_get_datetime(&curr_dt);
    uint32_t curr_ts = furi_hal_rtc_get_timestamp();
    char time_string[TIME_LEN];
    char date_string[DATE_LEN];
    char meridian_string[MERIDIAN_LEN];
    char timer_string[20];
    if(plugin_state->time_format == LocaleTimeFormat24h) {
        snprintf(
            time_string,
            TIME_LEN,
            DAB_TIMER_TIME_FORMAT,
            curr_dt.hour,
            curr_dt.minute,
            curr_dt.second);
    } else {
        bool pm = curr_dt.hour > 12;
        bool pm12 = curr_dt.hour >= 12;
        bool am12 = curr_dt.hour == 0;
        snprintf(
            time_string,
            TIME_LEN,
            DAB_TIMER_TIME_FORMAT,
            pm ? curr_dt.hour - 12 : (am12 ? 12 : curr_dt.hour),
            curr_dt.minute,
            curr_dt.second);

        snprintf(
            meridian_string,
            MERIDIAN_LEN,
            MERIDIAN_FORMAT,
            pm12 ? MERIDIAN_STRING_PM : MERIDIAN_STRING_AM);
    }
    if(plugin_state->date_format == LocaleDateFormatYMD) {
        snprintf(
            date_string,
            DATE_LEN,
            DAB_TIMER_ISO_DATE_FORMAT,
            curr_dt.year,
            curr_dt.month,
            curr_dt.day);
    } else if(plugin_state->date_format == LocaleDateFormatMDY) {
        snprintf(
            date_string,
            DATE_LEN,
            DAB_TIMER_RFC_DATE_FORMAT,
            curr_dt.month,
            curr_dt.day,
            curr_dt.year);
    } else {
        snprintf(
            date_string,
            DATE_LEN,
            DAB_TIMER_RFC_DATE_FORMAT,
            curr_dt.day,
            curr_dt.month,
            curr_dt.year);
    }
    bool timer_running = plugin_state->timer_running;
    uint8_t songSelect = plugin_state->songSelect;
    uint32_t alert_time = plugin_state->alert_time;
    uint32_t timer_start_timestamp = plugin_state->timer_start_timestamp;
    uint32_t timer_stopped_seconds = plugin_state->timer_stopped_seconds;
    char alertTime[4];
    snprintf(alertTime, sizeof(alertTime), "%ld", alert_time);
    furi_mutex_release(plugin_state->mutex);
    if(plugin_state->faceType == FaceStylePwn || plugin_state->faceType == FaceStylePwnInverted) {
        if(plugin_state->faceType == FaceStylePwnInverted) {
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
                elements_button_left(canvas, "F1");
            }
            canvas_set_color(canvas, ColorBlack);
        }
        canvas_set_font(canvas, FontBigNumbers);
        uint32_t elapsed_secs = timer_running ? (curr_ts - timer_start_timestamp) :
                                                timer_stopped_seconds;
        snprintf(timer_string, 20, "%.2ld:%.2ld", elapsed_secs / 60, elapsed_secs % 60);
        canvas_draw_str_aligned(canvas, 64, 8, AlignCenter, AlignCenter, time_string); // DRAW TIME
        if(plugin_state->w_test && timer_start_timestamp != 0) {
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
            canvas_draw_icon(canvas, 1, 32, flip_face[plugin_state->curEmotiveFace]);
        } else {
            canvas_draw_icon(canvas, 1, 32, &I_cool_flipagotchi);
        }
#ifdef CANVAS_HAS_FONT_BATTERYPERCENT
        canvas_set_font(canvas, FontBatteryPercent);
        canvas_draw_str_aligned(canvas, 117, 11, AlignCenter, AlignCenter, alertTime);
        if(plugin_state->time_format == LocaleTimeFormat12h)
            canvas_draw_str_aligned(canvas, 117, 4, AlignCenter, AlignCenter, meridian_string);
        canvas_draw_str_aligned(canvas, 96, 20, AlignCenter, AlignTop, date_string); // DRAW DATE
#else
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(canvas, 118, 12, AlignCenter, AlignCenter, alertTime);
        if(plugin_state->time_format == LocaleTimeFormat12h)
            canvas_draw_str_aligned(canvas, 118, 4, AlignCenter, AlignCenter, meridian_string);
        canvas_draw_str_aligned(canvas, 64, 20, AlignCenter, AlignTop, date_string); // DRAW DATE
#endif
    } else if(
        plugin_state->faceType == FaceStyleOriginal ||
        plugin_state->faceType == FaceStyleOriginalInverted) {
        canvas_set_font(canvas, FontSecondary);
        if(plugin_state->faceType == FaceStyleOriginalInverted) {
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
                elements_button_left(canvas, "F2");
            }
            canvas_set_color(canvas, ColorBlack);
        }
        canvas_set_font(canvas, FontBigNumbers);
        if(timer_start_timestamp != 0 && !plugin_state->w_test) {
            uint32_t elapsed_secs = timer_running ? (curr_ts - timer_start_timestamp) :
                                                    timer_stopped_seconds;
            snprintf(timer_string, 20, "%.2ld:%.2ld", elapsed_secs / 60, elapsed_secs % 60);
            canvas_draw_str_aligned(
                canvas, 64, 8, AlignCenter, AlignCenter, time_string); // DRAW TIME
            canvas_draw_str_aligned(
                canvas, 64, 32, AlignCenter, AlignTop, timer_string); // DRAW TIMER
#ifdef CANVAS_HAS_FONT_BATTERYPERCENT
            canvas_set_font(canvas, FontBatteryPercent);
            if(plugin_state->time_format == LocaleTimeFormat12h)
                canvas_draw_str_aligned(canvas, 117, 4, AlignCenter, AlignCenter, meridian_string);
            canvas_draw_str_aligned(canvas, 117, 11, AlignCenter, AlignCenter, alertTime);
#else
            canvas_set_font(canvas, FontSecondary);
            if(plugin_state->time_format == LocaleTimeFormat12h)
                canvas_draw_str_aligned(canvas, 118, 4, AlignCenter, AlignCenter, meridian_string);
            canvas_draw_str_aligned(canvas, 118, 12, AlignCenter, AlignCenter, alertTime);
#endif
            canvas_draw_str_aligned(
                canvas, 64, 20, AlignCenter, AlignTop, date_string); // DRAW DATE
#ifdef CANVAS_HAS_FONT_BATTERYPERCENT
            canvas_set_font(canvas, FontSecondary);
#endif
        } else {
#ifdef CANVAS_HAS_FONT_BATTERYPERCENT
            if(plugin_state->w_test) canvas_set_font(canvas, FontBatteryPercent);
#else
            if(plugin_state->w_test) canvas_set_font(canvas, FontSecondary);
#endif
            if(plugin_state->w_test && timer_start_timestamp != 0) {
                uint32_t elapsed_secs = timer_running ? (curr_ts - timer_start_timestamp) :
                                                        timer_stopped_seconds;
                snprintf(timer_string, 20, "%.2ld:%.2ld", elapsed_secs / 60, elapsed_secs % 60);
                uint32_t elapsed_secs_img = (elapsed_secs % 60) % 5;
                uint32_t elapsed_secs_img2 = (elapsed_secs % 60) % 4;
                static const Icon* const count_anim[5] = {
                    &I_HappyFlipper_128x64, &I_G0ku, &I_g0ku_1, &I_g0ku_2, &I_g0ku_3};
                static const Icon* const count_anim2[4] = {
                    &I_EviWaiting1_18x21,
                    &I_EviWaiting2_18x21,
                    &I_EviSmile1_18x21,
                    &I_EviSmile2_18x21};
                static const Icon* const count_anim3[4] = {
                    &I_frame_01, &I_frame_02, &I_frame_03, &I_frame_02};
#ifdef CANVAS_HAS_FONT_BATTERYPERCENT
                canvas_draw_icon(canvas, -5, 15, count_anim[elapsed_secs_img]);
                canvas_draw_icon(canvas, 90, 0, count_anim2[elapsed_secs_img2]);
                canvas_draw_icon(canvas, 110, 5, count_anim3[elapsed_secs_img2]);
                canvas_draw_str_aligned(
                    canvas, 64, 32, AlignCenter, AlignTop, timer_string); // DRAW TIMER
            }
            canvas_draw_str_aligned(
                canvas, 64, 26, AlignCenter, AlignCenter, time_string); // DRAW TIME
            canvas_set_font(canvas, FontBatteryPercent);
            if(plugin_state->time_format == LocaleTimeFormat12h)
                canvas_draw_str_aligned(canvas, 69, 15, AlignCenter, AlignCenter, meridian_string);
#else
                canvas_draw_icon(canvas, -6, 14, count_anim[elapsed_secs_img]);
                canvas_draw_icon(canvas, 55, -2, count_anim2[elapsed_secs_img2]);
                canvas_draw_icon(canvas, 40, 4, count_anim3[elapsed_secs_img2]);
                canvas_draw_str_aligned(
                    canvas, 97, 38, AlignCenter, AlignCenter, timer_string); // DRAW TIMER
                canvas_draw_str_aligned(
                    canvas, 97, 26, AlignCenter, AlignCenter, time_string); // DRAW TIME
                if(plugin_state->time_format == LocaleTimeFormat12h)
                    canvas_draw_str_aligned(
                        canvas, 97, 14, AlignCenter, AlignCenter, meridian_string);
            } else {
                canvas_draw_str_aligned(
                    canvas, 64, 26, AlignCenter, AlignCenter, time_string); // DRAW TIME
                canvas_set_font(canvas, FontSecondary);
                if(plugin_state->time_format == LocaleTimeFormat12h)
                    canvas_draw_str_aligned(
                        canvas, 100, 14, AlignCenter, AlignCenter, meridian_string);
            }
#endif
            if(!plugin_state->w_test)
                canvas_draw_str_aligned(
                    canvas, 64, 38, AlignCenter, AlignTop, date_string); // DRAW DATE
#ifdef CANVAS_HAS_FONT_BATTERYPERCENT
            canvas_set_font(canvas, FontSecondary);
#endif
        }
    } else if(
        plugin_state->faceType == FaceStyleOriginalSmall ||
        plugin_state->faceType == FaceStyleOriginalSmallInverted) {
        if(plugin_state->faceType == FaceStyleOriginalSmallInverted) {
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
                elements_button_left(canvas, "F3");
            }
            canvas_set_color(canvas, ColorBlack);
        }
#ifdef CANVAS_HAS_FONT_BATTERYPERCENT
        canvas_set_font(canvas, FontBatteryPercent);
#else
        canvas_set_font(canvas, FontSecondary);
#endif
        if(timer_start_timestamp != 0) {
            int32_t elapsed_secs = timer_running ? (curr_ts - timer_start_timestamp) :
                                                   timer_stopped_seconds;
            snprintf(timer_string, 20, "%.2ld:%.2ld", elapsed_secs / 60, elapsed_secs % 60);
            canvas_draw_str_aligned(
                canvas, 64, 31, AlignCenter, AlignTop, timer_string); // DRAW TIMER
        }
        canvas_draw_str_aligned(
#ifdef CANVAS_HAS_FONT_BATTERYPERCENT
            canvas, 64, 26, AlignCenter, AlignCenter, time_string); // DRAW TIME
        canvas_set_font(canvas, FontBatteryPercent);
        if(plugin_state->time_format == LocaleTimeFormat12h)
            canvas_draw_str_aligned(canvas, 69, 15, AlignCenter, AlignCenter, meridian_string);
        if(!plugin_state->w_test)
            canvas_draw_str_aligned(
                canvas, 64, 38, AlignCenter, AlignTop, date_string); // DRAW DATE
        canvas_set_font(canvas, FontSecondary);
#else
            canvas, 64, 24, AlignCenter, AlignCenter, time_string); // DRAW TIME
        if(plugin_state->time_format == LocaleTimeFormat12h)
            canvas_draw_str_aligned(canvas, 94, 24, AlignCenter, AlignCenter, meridian_string);
        if(!plugin_state->w_test)
            canvas_draw_str_aligned(
                canvas, 64, 40, AlignCenter, AlignTop, date_string); // DRAW DATE
#endif
    } else if(
        plugin_state->faceType == FaceStyleCircle ||
        plugin_state->faceType == FaceStyleCircleInverted) {
        if(plugin_state->faceType == FaceStyleCircleInverted) {
            canvas_draw_icon(canvas, 0, 0, &I_black);
            if(timer_start_timestamp != 0) {
                // elements_button_left(canvas, "Reset");
            }
            canvas_set_color(canvas, ColorWhite);
        } else {
            canvas_set_color(canvas, ColorWhite);
            if(timer_start_timestamp != 0) {
                // elements_button_left(canvas, "Reset");
            }
            canvas_set_color(canvas, ColorBlack);
        }
        int8_t width = 124;
        int8_t height = 64;
        Vector2 clock_center = {
            .x = 30 + width / 2,
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
        furi_string_free(str);
    } else {
        if(plugin_state->faceType == FaceStyleBinaryInverted) {
            canvas_draw_icon(canvas, 0, 0, &I_black);
            if(timer_start_timestamp != 0) {
                // elements_button_left(canvas, "Reset");
            }
            canvas_set_color(canvas, ColorWhite);
        } else {
            canvas_set_color(canvas, ColorWhite);
            if(timer_start_timestamp != 0) {
                // elements_button_left(canvas, "Reset");
            }
            canvas_set_color(canvas, ColorBlack);
        }
#ifdef CANVAS_HAS_FONT_BATTERYPERCENT
        canvas_set_font(canvas, FontBatteryPercent);
#endif
        if(timer_start_timestamp != 0) {
            uint32_t elapsed_secs = timer_running ? (curr_ts - timer_start_timestamp) :
                                                    timer_stopped_seconds;
            dab_timer_render_binary_face(canvas, elapsed_secs / 60, 5, timer_start_timestamp);
            dab_timer_render_binary_face(
                canvas, elapsed_secs % 60, 5 + (9 * 1), timer_start_timestamp);
            // snprintf(timer_string, 20, "%.2ld:%.2ld", elapsed_secs / 60, elapsed_secs % 60);
            // canvas_draw_str_aligned(
            // canvas, 12, 1, AlignCenter, AlignTop, timer_string); // DRAW TIMER
            // canvas_draw_str_aligned(
            // canvas, 102, 1, AlignCenter, AlignTop, date_string); // DRAW DATE
            dab_timer_render_binary_face(canvas, curr_dt.hour, 5 + (9 * 3), timer_start_timestamp);
            dab_timer_render_binary_face(
                canvas, curr_dt.minute, 5 + (9 * 4), timer_start_timestamp);
            dab_timer_render_binary_face(
                canvas, curr_dt.second, 5 + (9 * 5), timer_start_timestamp);
        } else {
            dab_timer_render_binary_face(canvas, curr_dt.hour, 18, timer_start_timestamp);
            dab_timer_render_binary_face(canvas, curr_dt.minute, 28, timer_start_timestamp);
            dab_timer_render_binary_face(canvas, curr_dt.second, 38, timer_start_timestamp);
        }
    }
    if(plugin_state->faceType >= FaceStylePwnInverted) {
        canvas_set_color(canvas, ColorBlack);
    }
    if(plugin_state->faceType < FaceStylePwnInverted) {
        canvas_set_color(canvas, ColorWhite);
    }
    if(plugin_state->faceType != FaceStyleCircle && plugin_state->faceType != FaceStyleBinary &&
       plugin_state->faceType != FaceStyleCircleInverted &&
       plugin_state->faceType != FaceStyleBinaryInverted) {
#if __has_include(<cfw/cfw.h>)
        if(!plugin_state->desktop_settings->is_dumbmode && !plugin_state->w_test) {
#else
        if(!plugin_state->w_test) {
#endif
            if(timer_running) {
                elements_button_center(canvas, "Stop");
            } else {
                elements_button_center(canvas, "Start");
            }
        }
        if(timer_running && !plugin_state->w_test) {
            if(songSelect == SoundAlertOff) {
                elements_button_right(canvas, "S:OFF");
            } else if(songSelect == SoundAlertGoGoPoRa) {
                elements_button_right(canvas, "S:PoRa");
            } else if(songSelect == SoundAlertMario) {
                elements_button_right(canvas, "S:Mario");
            } else if(songSelect == SoundAlertByMin) {
                elements_button_right(canvas, "S:ByMin");
            } else if(songSelect == SoundAlertCont) {
                elements_button_right(canvas, "S:Cont");
            }
        }
#if __has_include(<cfw/cfw.h>)
        if(plugin_state->w_test && plugin_state->desktop_settings->is_dumbmode) {
#else
        if(plugin_state->w_test) {
#endif
            canvas_draw_icon(canvas, 0, 0, &I_GameModeIcon_11x8);
        }
    }
}

static void dab_timer_state_init(DabTimerState* const plugin_state) {
    memset(plugin_state, 0, sizeof(DabTimerState));
    plugin_state->alert_time = 80;
    plugin_state->date_format = locale_get_date_format();
#if __has_include(<cfw/cfw.h>)
    plugin_state->desktop_settings = malloc(sizeof(DesktopSettings));
#endif
    plugin_state->curEmotiveFace = 0;
    plugin_state->codeSequence = 0;
    plugin_state->faceType = FaceStylePwn;
    plugin_state->lastexp_timestamp = 0;
    plugin_state->songSelect = SoundAlertMario;
    plugin_state->time_format = locale_get_time_format();
    plugin_state->timer_start_timestamp = 0;
    plugin_state->timer_stopped_seconds = 0;
    plugin_state->timerSecs = 0;
    plugin_state->w_test = false;
}

void dab_timer_free(DabTimerState* plugin_state) {
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);
#if __has_include(<cfw/cfw.h>)
    free(plugin_state->desktop_settings);
#endif
    furi_message_queue_free(plugin_state->event_queue);
    furi_mutex_free(plugin_state->mutex);
    free(plugin_state);
}

static void dab_timer_tick(void* ctx) {
    // Runs every 1000ms by default
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    PluginEvent event = {.type = EventTypeTick};
    // It's OK to lose this event if system overloaded
    furi_message_queue_put(event_queue, &event, 0);
}

int32_t dab_timer_app(void* p) {
    UNUSED(p);
    DabTimerState* plugin_state = malloc(sizeof(DabTimerState));
    dab_timer_state_init(plugin_state);
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
        furi_timer_alloc(dab_timer_tick, FuriTimerTypePeriodic, plugin_state->event_queue);
    if(timer == NULL) {
        FURI_LOG_E(TAG, "cannot create timer\n");
        furi_mutex_free(plugin_state->mutex);
        furi_message_queue_free(plugin_state->event_queue);
        free(plugin_state);
        return 255;
    }
#if __has_include(<cfw/cfw.h>)
    DESKTOP_SETTINGS_LOAD(plugin_state->desktop_settings);
#endif
    // Set system callbacks
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, dab_timer_render_callback, plugin_state);
    view_port_input_callback_set(view_port, dab_timer_input_callback, plugin_state->event_queue);
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
                            if(plugin_state->timer_running && plugin_state->alert_time >= 5)
                                plugin_state->alert_time = plugin_state->alert_time - 5;
                        } else {
                            plugin_state->codeSequence = 0;
                            if(plugin_state->timer_running && plugin_state->alert_time >= 5)
                                plugin_state->alert_time = plugin_state->alert_time - 5;
                        }
                        break;
                    case InputKeyRight:
                        if(plugin_state->codeSequence == 5 || plugin_state->codeSequence == 7) {
                            plugin_state->codeSequence++;
                            if(plugin_state->codeSequence == 8) {
#if __has_include(<cfw/cfw.h>)
                                dab_timer_dumbmode_changed(plugin_state->desktop_settings);
#endif
                                plugin_state->w_test = true; // OH HEY NOW LETS GAIN EXP & MORE FUN
                            }
                        } else {
                            plugin_state->codeSequence = 0;
                            if(plugin_state->songSelect < SoundAlertCount - 1) {
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
                                uint32_t curr_ts = furi_hal_rtc_get_timestamp();
                                plugin_state->timer_start_timestamp = curr_ts;
                                plugin_state->timer_stopped_seconds = 0;
                                plugin_state->timerSecs = 0;
                            } else {
                                if(plugin_state->faceType < FaceStyleCount - 1) {
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
#if __has_include(<cfw/cfw.h>)
                            if(!plugin_state->desktop_settings->is_dumbmode) {
#endif
                                if(plugin_state->songSelect == SoundAlertMario ||
                                   plugin_state->songSelect == SoundAlertGoGoPoRa ||
                                   plugin_state->songSelect == SoundAlertCont ||
                                   plugin_state->songSelect == SoundAlertByMin) {
                                    notification_message(notification, &dab_timer_alert_startStop);
                                }
                                // START/STOP TIMER
                                uint32_t curr_ts = furi_hal_rtc_get_timestamp();
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
#if __has_include(<cfw/cfw.h>)
                            }
#endif
                        }
                        break;
                    case InputKeyBack:
                        if(plugin_state->codeSequence == 8) {
                            plugin_state->codeSequence++;
                        } else {
                            plugin_state->w_test = false;
                            // Don't Exit the plugin
                            plugin_state->codeSequence--;
                            if(plugin_state->codeSequence < (uint8_t)-1) processing = false;
                        }
                        break;
                    default:
                        break;
                    }
                    if(plugin_state->codeSequence == 10) {
                        plugin_state->codeSequence = 0;
#if __has_include(<cfw/cfw.h>)
                        plugin_state->desktop_settings->is_dumbmode =
                            true; // MAKE SURE IT'S ON SO IT GETS TURNED OFF
                        dab_timer_dumbmode_changed(plugin_state->desktop_settings);
#endif
                        if(plugin_state->songSelect == SoundAlertMario ||
                           plugin_state->songSelect == SoundAlertGoGoPoRa ||
                           plugin_state->songSelect == SoundAlertCont ||
                           plugin_state->songSelect == SoundAlertByMin) {
                            notification_message(notification, &sequence_success);
                            notification_message(notification, &sequence_rainbow);
                            notification_message(notification, &sequence_rainbow);
                        }
#if __has_include(<cfw/cfw.h>)
                        dolphin_deed(getRandomDeed());
#else
                        dolphin_deed(DolphinDeedBadUsbPlayScript);
#endif
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
                       plugin_state->songSelect != SoundAlertOff &&
                       (plugin_state->timerSecs < plugin_state->alert_time ||
                        plugin_state->songSelect == SoundAlertByMin)) {
                        notification_message(notification, &dab_timer_alert_perMin);
                    }
                    if(plugin_state->songSelect == SoundAlertGoGoPoRa) {
                        if(plugin_state->timerSecs == plugin_state->alert_time) {
                            uint32_t curr_ts = furi_hal_rtc_get_timestamp();
                            if(plugin_state->lastexp_timestamp + 10 <= curr_ts &&
                               plugin_state->w_test) {
                                plugin_state->lastexp_timestamp = curr_ts;
#if __has_include(<cfw/cfw.h>)
                                dolphin_deed(getRandomDeed());
#else
                                dolphin_deed(DolphinDeedBadUsbPlayScript);
#endif
                            }
                            notification_message(notification, &dab_timer_alert_pr1);
                        }
                        if(plugin_state->timerSecs == plugin_state->alert_time + 1) {
                            notification_message(notification, &dab_timer_alert_pr2);
                        }
                        if(plugin_state->timerSecs == plugin_state->alert_time + 2) {
                            notification_message(notification, &dab_timer_alert_pr3);
                            notification_message(notification, &sequence_rainbow);
                            notification_message(notification, &sequence_rainbow);
                        }
                    } else if(plugin_state->songSelect == SoundAlertCont) {
                        if(plugin_state->timerSecs >= plugin_state->alert_time) {
                            notification_message(notification, &dab_timer_alert_mario1);
                        }
                    } else if(plugin_state->songSelect == SoundAlertMario) {
                        if(plugin_state->timerSecs == plugin_state->alert_time) {
                            uint32_t curr_ts = furi_hal_rtc_get_timestamp();
                            if(plugin_state->lastexp_timestamp + 10 <= curr_ts &&
                               plugin_state->w_test) {
                                plugin_state->lastexp_timestamp = curr_ts;
#if __has_include(<cfw/cfw.h>)
                                dolphin_deed(getRandomDeed());
#else
                                dolphin_deed(DolphinDeedBadUsbPlayScript);
#endif
                            }
                            notification_message(notification, &dab_timer_alert_mario1);
                        }
                        if(plugin_state->timerSecs == plugin_state->alert_time + 1) {
                            notification_message(notification, &dab_timer_alert_mario2);
                        }
                        if(plugin_state->timerSecs == plugin_state->alert_time + 2) {
                            notification_message(notification, &dab_timer_alert_mario3);
                            notification_message(notification, &sequence_rainbow);
                            notification_message(notification, &sequence_rainbow);
                        }
                    } else {
                        if(plugin_state->timerSecs == plugin_state->alert_time) {
                            uint32_t curr_ts = furi_hal_rtc_get_timestamp();
                            if(plugin_state->lastexp_timestamp + 10 <= curr_ts &&
                               plugin_state->w_test) {
                                plugin_state->lastexp_timestamp = curr_ts;
#if __has_include(<cfw/cfw.h>)
                                dolphin_deed(getRandomDeed());
#else
                                dolphin_deed(DolphinDeedBadUsbPlayScript);
#endif
                            }
                            notification_message(notification, &dab_timer_alert_silent);
                            notification_message(notification, &sequence_rainbow);
                            notification_message(notification, &sequence_rainbow);
                        }
                    }
                }
            }
        }
        furi_mutex_release(plugin_state->mutex);
        view_port_update(view_port);
    }
    furi_timer_free(timer);
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    dab_timer_free(plugin_state);
    return 0;
}