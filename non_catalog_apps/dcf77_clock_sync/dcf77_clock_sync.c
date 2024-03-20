#include <furi_hal.h>
#include <furi_hal_pwm.h>
#include <gui/gui.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>

#include <datetime/datetime.h>
#include <locale/locale.h>

#include "dcf77.h"

#define SCREEN_SIZE_X 128
#define SCREEN_SIZE_Y 64
#define DCF77_FREQ 77500
#define DCF77_OFFSET 60
#define SYNC_DELAY 50

char* WEEKDAYS[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

typedef struct {
    DateTime dt;
    bool is_dst;
    FuriString* str;
    LocaleTimeFormat tim_fmt;
    LocaleDateFormat dat_fmt;
} AppData;

static void app_draw_callback(Canvas* canvas, void* context) {
    AppData* app = (AppData*)context;
    furi_assert(app->str);

    uint8_t hour = app->dt.hour;
    bool fmt_12h = false;
    if(app->tim_fmt == LocaleTimeFormat12h) {
        hour = hour == 0 ? 12 : hour % 12;
        fmt_12h = true;
    }

    furi_string_printf(app->str, "%2u:%02u:%02u", hour, app->dt.minute, app->dt.second);
    const char* tim_cstr = furi_string_get_cstr(app->str);

    canvas_set_font(canvas, FontBigNumbers);
    canvas_draw_str_aligned(
        canvas, SCREEN_SIZE_X / 2, SCREEN_SIZE_Y / 2, AlignCenter, AlignCenter, tim_cstr);

    if(fmt_12h) {
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(
            canvas,
            0,
            (SCREEN_SIZE_Y / 2) - 7,
            AlignLeft,
            AlignTop,
            (app->dt.hour >= 12 ? "PM" : "AM"));
    }

    FuriString* dat = furi_string_alloc();
    locale_format_date(dat, &app->dt, app->dat_fmt, "-");
    const char* dow_str = WEEKDAYS[(app->dt.weekday - 1) % 7];
    const char* dst_str = app->is_dst ? "CEST" : "CET";
    furi_string_printf(app->str, "%s %s %s", dow_str, furi_string_get_cstr(dat), dst_str);
    furi_string_free(dat);

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(
        canvas, SCREEN_SIZE_X / 2, 0, AlignCenter, AlignTop, furi_string_get_cstr(app->str));

    if(app->dt.second < 59) {
        char* data = get_dcf77_data(app->dt.second);
        canvas_draw_str_aligned(
            canvas, SCREEN_SIZE_X, SCREEN_SIZE_Y, AlignRight, AlignBottom, data);
    }
}

static void app_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

void set_time(AppData* app, int offset) {
    DateTime dcf_dt;
    uint32_t timestamp = datetime_datetime_to_timestamp(&app->dt) + offset;
    datetime_timestamp_to_datetime(timestamp, &dcf_dt);
    set_dcf77_time(&dcf_dt, app->is_dst);
}

int dcf77_clock_sync_app_main(void* p) {
    UNUSED(p);

    AppData* app = malloc(sizeof(AppData));
    furi_hal_rtc_get_datetime(&app->dt);
    app->is_dst = false;
    app->str = furi_string_alloc();
    app->tim_fmt = locale_get_time_format();
    app->dat_fmt = locale_get_date_format();

    set_time(app, DCF77_OFFSET);

    ViewPort* view_port = view_port_alloc();
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    view_port_draw_callback_set(view_port, app_draw_callback, app);
    view_port_input_callback_set(view_port, app_input_callback, event_queue);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
    notification_message_block(notification, &sequence_display_backlight_enforce_on);

    InputEvent event;
    bool running = false;
    bool exit = false;
    int sec = app->dt.second;
    while(!exit) {
        int silence_ms = 0;
        // wait next second
        while(app->dt.second == sec) furi_hal_rtc_get_datetime(&app->dt);

        if(app->dt.second < 59) {
            if(running) {
                notification_message(notification, &sequence_reset_rgb);
                furi_hal_rfid_tim_read_stop();
                furi_hal_pwm_stop(FuriHalPwmOutputIdLptim2PA4);
                furi_hal_gpio_init(
                    &gpio_ext_pa4, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
            }
            silence_ms = get_dcf77_bit(app->dt.second) ? 200 : 100;
            furi_delay_ms(silence_ms);
            furi_hal_rfid_tim_read_start(DCF77_FREQ, 0.5);
            furi_hal_pwm_start(FuriHalPwmOutputIdLptim2PA4, DCF77_FREQ, 50);
            notification_message(notification, &sequence_set_only_blue_255);
            running = true;
        } else
            set_time(app, DCF77_OFFSET + 1);

        sec = app->dt.second;
        int wait_ms = 1000 - silence_ms - SYNC_DELAY;
        uint32_t tick_start = furi_get_tick();
        while(wait_ms > 0) {
            FuriStatus status = furi_message_queue_get(event_queue, &event, wait_ms);
            if((status == FuriStatusOk) && (event.type == InputTypePress)) {
                if(event.key == InputKeyOk)
                    app->is_dst = !app->is_dst;
                else if(event.key == InputKeyBack) {
                    exit = true;
                    break;
                }
            }
            view_port_update(view_port);
            if(status == FuriStatusErrorTimeout) break;
            wait_ms -= furi_get_tick() - tick_start;
        }
    }

    if(running) {
        furi_hal_rfid_tim_read_stop();
        furi_hal_pwm_stop(FuriHalPwmOutputIdLptim2PA4);
    }

    notification_message_block(notification, &sequence_display_backlight_enforce_auto);
    notification_message(notification, &sequence_reset_rgb);

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_GUI);
    furi_message_queue_free(event_queue);
    view_port_free(view_port);

    furi_string_free(app->str);
    free(app);

    return 0;
}