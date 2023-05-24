#include "dolphinsauce.h"
#include <cc1101.h>

void draw_callback(Canvas* canvas, void* ctx) {
    furi_assert(ctx);
    AppFSM* app = ctx;
    furi_mutex_acquire(app->mutex, FuriWaitForever);
    canvas_draw_frame(canvas, 0, 0, 128, 64);
    canvas_set_font(canvas, FontPrimary);
    if(app->active_msg == &sms_msg)
        canvas_draw_str(canvas, 27, 10, "Message: SMS");
    else
        canvas_draw_str(canvas, 27, 10, "Message: SOS");

    /*
    char buffer[64];
    snprintf(buffer, 64, "tx %x cw %x", app->txing, app->cw_on);
    canvas_draw_frame(canvas, 0, 0, 128, 64);
    canvas_set_font(canvas, FontKeyboard);
    canvas_draw_str(canvas, 50, 30, buffer);
    */

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 25, "Press up to start - 434.0MHz");
    canvas_draw_str(canvas, 2, 35, "LRLRLR OK for SOS msg");
    canvas_draw_str(canvas, 2, 50, "Unwarranted maydays may");
    canvas_draw_str(canvas, 2, 60, "cause legal dismay");
    furi_mutex_release(app->mutex);
}

void input_callback(InputEvent* input, void* ctx) {
    AppFSM* app = ctx;
    furi_message_queue_put(app->event_queue, input, FuriWaitForever);
}

AppFSM* app_init() {
#define MORSE_WPM 3
#define MORSE_MS 1.0f / (60.0f / (50.0f * MORSE_WPM))

    AppFSM* app = malloc(sizeof(AppFSM));
    app->active_msg = &sms_msg;
    app->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if(!app->mutex) {
        FURI_LOG_E("SOS", "cannot create mutex\r\n");
        app_exit(app);
    }
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->view_port = view_port_alloc();
    view_port_draw_callback_set(app->view_port, draw_callback, app);
    view_port_input_callback_set(app->view_port, input_callback, app);
    app->gui = furi_record_open("gui");
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);
    app->cw_on = false;
    FuriTimer* timer =
        furi_timer_alloc(update_timer_callback, FuriTimerTypePeriodic, app->event_queue);
    // furi_timer_start(timer, furi_kernel_get_tick_frequency() / (1000000.0/((50*MORSE_WPM)*60)) );
    furi_timer_start(timer, furi_kernel_get_tick_frequency() / 6);
    app->timer = timer;

    app_global = app;

    return app;
}

void rf_dumb() {
    uint32_t frequency = 438000000;
    frequency = 434000000;
    furi_hal_subghz_load_preset(FuriHalSubGhzPresetOok650Async);
    frequency = furi_hal_subghz_set_frequency_and_path(frequency);

    /*
    furi_hal_subghz_set_path(FuriHalSubGhzPath433);
    furi_hal_spi_acquire(&furi_hal_spi_bus_handle_subghz);
    uint32_t real_frequency = cc1101_set_frequency(&furi_hal_spi_bus_handle_subghz, frequency);
    UNUSED(real_frequency);
    cc1101_calibrate(&furi_hal_spi_bus_handle_subghz);
    while(true) {
        CC1101Status status = cc1101_get_status(&furi_hal_spi_bus_handle_subghz);
        if(status.STATE == CC1101StateIDLE) break;
    }
    furi_hal_spi_release(&furi_hal_spi_bus_handle_subghz);
    */

    furi_hal_gpio_init(&gpio_cc1101_g0, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_write(&gpio_cc1101_g0, true);
    furi_hal_power_suppress_charge_enter();

    furi_hal_speaker_start(MARKER_HZ, 1);
    furi_hal_subghz_tx();
    furi_delay_ms(250);
    furi_hal_speaker_stop();

    furi_hal_subghz_set_path(FuriHalSubGhzPathIsolate);
    furi_hal_subghz_sleep();
    furi_hal_power_suppress_charge_exit();
}

void rf_start_cw() {
    /*if (app_global->active_msg == &sos_msg)
    {*/
    furi_hal_light_set(LightBacklight, 0xFF);
    // furi_hal_speaker_start(MARKER_HZ, 1);
    //}
    furi_hal_light_set(LightRed | LightGreen | LightBlue, 0xFF);
    uint32_t frequency = 432100000;
    frequency = 434000000;
    furi_hal_subghz_load_preset(FuriHalSubGhzPresetOok650Async);
    frequency = furi_hal_subghz_set_frequency_and_path(frequency);

    furi_hal_gpio_init(&gpio_cc1101_g0, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_write(&gpio_cc1101_g0, true);
    furi_hal_power_suppress_charge_enter();

    furi_hal_subghz_tx();
}

void rf_stop_cw() {
    furi_hal_subghz_set_path(FuriHalSubGhzPathIsolate);
    furi_hal_subghz_sleep();
    furi_hal_power_suppress_charge_exit();

    /*
    if (app_global->active_msg == &sos_msg)
    {*/
    furi_hal_light_set(LightBacklight, 0);
    // furi_hal_speaker_stop();
    //}
    furi_hal_light_set(LightRed | LightGreen | LightBlue, 0);
}

static void update_timer_callback(FuriMessageQueue* event_queue) {
    furi_assert(event_queue);

    AppFSM* app = app_global;
    uint8_t bit = (*(app_global->active_msg) & (1 << (31 - test_msg_pos))) != 0;
    if(app->cw_on) {
        if(bit & !app->txing) {
            rf_start_cw();
            app->txing = true;
        } else if(!bit) {
            rf_stop_cw();
            app->txing = false;
        }
        test_msg_pos = (test_msg_pos + 1) % 31; // 24 data + 7 silence
    }

    // TODO
    // furi_message_queue_put(event_queue, &event, 0);
}

void app_exit(AppFSM* app) {
    rf_stop_cw();
    app->cw_on = false;
    furi_timer_free(app->timer);
    view_port_enabled_set(app->view_port, false);
    gui_remove_view_port(app->gui, app->view_port);
    furi_record_close("gui");
    view_port_free(app->view_port);
    furi_message_queue_free(app->event_queue);
    furi_mutex_free(app->mutex);
    furi_hal_speaker_stop();
    free(app);
}

int32_t sos_app(void* p) {
    UNUSED(p);
    AppFSM* app = app_init();
    InputEvent event;
    for(bool processing = true; processing;) {
        FuriStatus status = furi_message_queue_get(app->event_queue, &event, 100);
        furi_mutex_acquire(app->mutex, FuriWaitForever);
        // float volume = 1.0f;
        if(status == FuriStatusOk) {
            if(event.type == InputTypePress) {
                switch(event.key) {
                case InputKeyUp:
                    app->sos_mode_code = 0;
                    test_msg_pos = 24;
                    app->cw_on = true;
                    break;
                case InputKeyDown:
                    app->sos_mode_code = 0;
                    app->cw_on = false;
                    rf_stop_cw();
                    break;
                case InputKeyLeft:

                    if(app->sos_mode_code < 6 && app->sos_mode_code % 2 == 0) {
                        app->sos_mode_code++;
                    } else {
                        app->sos_mode_code = 0;
                        app->cw_on = false;
                        rf_stop_cw();
                    }
                    break;
                case InputKeyRight:
                    app->cw_on = false;
                    if(app->sos_mode_code < 6 && app->sos_mode_code % 2 == 1) {
                        app->sos_mode_code++;
                    } else {
                        app->sos_mode_code = 0;
                    }
                    // rf_dumb();
                    break;
                case InputKeyOk:

                    if(app->sos_mode_code == 6) {
                        if(app->active_msg == &sms_msg) {
                            app->active_msg = &sos_msg;
                        } else {
                            app->active_msg = &sms_msg;
                        }
                    } else {
                        app->cw_on = false;
                        rf_stop_cw();
                    }
                    app->sos_mode_code = 0;
                    break;
                case InputKeyBack:
                    processing = false;
                    break;
                default:
                    break;
                }
            } else if(event.type == InputTypeRelease) {
                furi_hal_speaker_stop();
            }
        }
        view_port_update(app->view_port);
        furi_mutex_release(app->mutex);
    }
    app_exit(app);
    return 0;
}
