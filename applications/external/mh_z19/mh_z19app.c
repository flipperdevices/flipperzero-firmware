
#include "mh_z19_icons.h"

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include <input/input.h>
#include <notification/notification_messages.h>

typedef enum {
    GreenStatus,
    YellowStatus,
    RedStatus,
} StatusPPM;

typedef enum { RANGE_2000 = 2000, RANGE_5000 = 5000 } SensorRange;

struct MHZ19App {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    FuriMutex* mutex;
    NotificationApp* notifications;

    bool have_5v;
    int32_t current_page;

    StatusPPM status_ppm;

    SensorRange sensor_range;

    const GpioPin* input_pin;

    int32_t co2_ppm;
};

typedef struct MHZ19App MHZ19App;

const NotificationSequence green_led_sequence = {
    &message_green_255,
    &message_do_not_reset,
    NULL,
};

const NotificationSequence yellow_led_sequence = {
    &message_green_255,
    &message_red_255,
    &message_do_not_reset,
    &message_vibro_on,
    &message_note_c5,
    &message_delay_50,
    &message_vibro_off,
    &message_sound_off,
    NULL,
};

const NotificationSequence red_led_sequence = {
    &message_red_255,
    &message_do_not_reset,
    &message_vibro_on,
    &message_note_c5,
    &message_delay_50,
    &message_vibro_off,
    &message_sound_off,
    NULL,
};

void mh_z19app_draw_callback(Canvas* canvas, void* ctx) {
    furi_assert(ctx);

    MHZ19App* app = ctx;
    canvas_clear(canvas);

    if(!app->have_5v) {
        canvas_set_font(canvas, FontPrimary);
        elements_multiline_text_aligned(
            canvas,
            4,
            28,
            AlignLeft,
            AlignTop,
            "5V on GPIO must be\nenabled, or USB must\nbe connected.");
        return;
    } else if(app->current_page == 0) {
        canvas_set_font(canvas, FontPrimary);
        elements_multiline_text_aligned(
            canvas,
            4,
            1,
            AlignLeft,
            AlignTop,
            "Connect sensor MH-Z19 to pins:\n5V -> 1\nGND -> 8\nPWM -> 3\nPress center button...");
        return;
    } else if(app->current_page == 1) {
        canvas_set_font(canvas, FontPrimary);
        elements_multiline_text_aligned(
            canvas,
            4,
            1,
            AlignLeft,
            AlignTop,
            "Select sensor measuring range by arrows.\nAvailable:\n\t- 0-2000ppm\n\t- 0-5000ppm\nPress center button...");
        return;
    }

    FuriString* strbuf = furi_string_alloc();

    const Icon* icon;
    FuriString* status_text = furi_string_alloc();

    switch(app->status_ppm) {
    case GreenStatus:
        icon = &I_passport_okay1_46x49;
        furi_string_set_str(status_text, "it's OK!");
        break;
    case YellowStatus:
        icon = &I_passport_bad1_46x49;
        furi_string_set_str(status_text, "Not good!");
        break;
    case RedStatus:
        icon = &I_passport_bad3_46x49;
        furi_string_set_str(status_text, "Very bad!");
        break;
    default:
        icon = &I_passport_okay1_46x49;
        furi_string_set_str(status_text, "It's OK!");
        break;
    }

    const Icon* co2_icon = &I_co2;

    canvas_draw_icon(canvas, 9, 7, icon);

    canvas_draw_icon(canvas, 59, 8, co2_icon);

    furi_string_printf(strbuf, "%ld", app->co2_ppm);
    canvas_set_font(canvas, FontBigNumbers);
    canvas_draw_str(canvas, 60, 40, furi_string_get_cstr(strbuf));

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 60, 55, furi_string_get_cstr(status_text));

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 110, 40, "ppm");

    furi_string_printf(strbuf, "%d", app->sensor_range);
    canvas_set_font(canvas, FontKeyboard);
    canvas_draw_str(canvas, 104, 8, furi_string_get_cstr(strbuf));

    furi_string_free(strbuf);
    furi_string_free(status_text);
}

void mh_z19app_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

MHZ19App* mh_z19app_alloc() {
    MHZ19App* app = (MHZ19App*)malloc(sizeof(MHZ19App));

    app->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if(!app->mutex) {
        FURI_LOG_E("MH-Z19", "cannot create mutex\r\n");
        free(app);
        return NULL;
    }
    furi_mutex_acquire(app->mutex, FuriWaitForever);

    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    view_port_draw_callback_set(app->view_port, mh_z19app_draw_callback, app);
    view_port_input_callback_set(app->view_port, mh_z19app_input_callback, app->event_queue);

    app->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    // Enable 5v power, multiple attempts to avoid issues with power chip protection false triggering
    uint8_t attempts = 0;
    while(!furi_hal_power_is_otg_enabled() && attempts++ < 5) {
        furi_hal_power_enable_otg();
        furi_delay_ms(10);
    }

    if(furi_hal_power_is_otg_enabled() || furi_hal_power_is_charging()) {
        app->have_5v = true;
    } else {
        app->have_5v = false;
    }

    app->input_pin = &gpio_ext_pa6;

    furi_hal_gpio_init(app->input_pin, GpioModeInput, GpioPullUp, GpioSpeedVeryHigh);

    app->notifications = furi_record_open(RECORD_NOTIFICATION);
    furi_mutex_release(app->mutex);

    return app;
}

void mh_z19app_free(MHZ19App* app) {
    furi_assert(app);

    if(furi_hal_power_is_otg_enabled()) {
        furi_hal_power_disable_otg();
    }

    view_port_enabled_set(app->view_port, false);
    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);

    furi_message_queue_free(app->event_queue);

    furi_hal_light_set(LightRed | LightGreen | LightBlue, 0x00);

    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_GUI);

    furi_mutex_free(app->mutex);
    free(app);
}

void mh_z19app_init(MHZ19App* app) {
    furi_mutex_acquire(app->mutex, FuriWaitForever);
    app->co2_ppm = 0;
    app->status_ppm = GreenStatus;
    app->current_page = 0;
    app->sensor_range = RANGE_2000;
    app->have_5v = true;
    notification_message(app->notifications, &green_led_sequence);
    furi_mutex_release(app->mutex);
}

int32_t calculate_ppm(
    int32_t* prevVal,
    int32_t val,
    int32_t* th,
    int32_t* tl,
    int32_t* h,
    int32_t* l,
    SensorRange range) {
    int32_t tt = furi_get_tick();
    if(val == 1) {
        if(val != *prevVal) {
            *h = tt;
            *tl = *h - *l;
            *prevVal = val;
        }
    } else {
        if(val != *prevVal) {
            *l = tt;
            *th = *l - *h;
            *prevVal = val;
            return range * (*th - 2) / (*th + *tl - 4);
        }
    }
    return -1;
}

void change_sensor_range(MHZ19App* app) {
    if(app->sensor_range == RANGE_2000) {
        app->sensor_range = RANGE_5000;
    } else {
        app->sensor_range = RANGE_2000;
    }
}

void send_notification_if_needed(MHZ19App* app, int32_t ppm) {
    if(ppm > 0) {
        if(ppm > 1000) {
            if(app->status_ppm != RedStatus) {
                notification_message(app->notifications, &red_led_sequence);
                app->status_ppm = RedStatus;
            }
        } else if(ppm > 800) {
            if(app->status_ppm != YellowStatus) {
                notification_message(app->notifications, &yellow_led_sequence);
                app->status_ppm = YellowStatus;
            }
        } else {
            if(app->status_ppm != GreenStatus) {
                notification_message(app->notifications, &green_led_sequence);
                app->status_ppm = GreenStatus;
            }
        }
    }
}

int32_t mh_z19_app(void* p) {
    UNUSED(p);
    MHZ19App* app = mh_z19app_alloc();
    if(!app) {
        FURI_LOG_E("MH-Z19", "cannot create app\r\n");
        return -1;
    }

    mh_z19app_init(app);

    InputEvent event;

    int32_t prevVal = 0;

    int32_t th, tl, h = 0;
    int32_t l = 0;
    int32_t ppm = 0;

    for(bool processing = true; processing;) {
        furi_mutex_acquire(app->mutex, FuriWaitForever);
        ppm = calculate_ppm(
            &prevVal, furi_hal_gpio_read(app->input_pin), &th, &tl, &h, &l, app->sensor_range);
        if(ppm > 0) {
            app->co2_ppm = ppm;
        }
        send_notification_if_needed(app, app->co2_ppm);
        if(furi_message_queue_get(app->event_queue, &event, 100) == FuriStatusOk) {
            if(event.type == InputTypeShort) {
                switch(event.key) {
                case InputKeyBack:
                    processing = false;
                    break;
                case InputKeyOk:
                    app->current_page++;
                    break;
                case InputKeyLeft:
                case InputKeyRight:
                    change_sensor_range(app);
                    break;
                default:
                    break;
                }
            }
        }
        furi_mutex_release(app->mutex);
        view_port_update(app->view_port);
    }

    mh_z19app_free(app);
    return 0;
}
