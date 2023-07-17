#include <furi.h>

#include "battery_checker.h"
#include "adc/adc.h"

#include <battery_checker_icons.h>

#define TAG "BatteryChecker"

static void app_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

static void app_draw_callback(Canvas* canvas, void* ctx) {
    furi_assert(ctx);
    BatteryCheckerApp* app = ctx;

    canvas_clear(canvas);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 25, 15, "Connect battery");
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 9, 30, "to GND (-) and PC3 (+)");

    FuriString* tmp_string = furi_string_alloc();
    float voltage = app->voltage;
    furi_string_printf(tmp_string, "Current Voltage: %.2f V", (double)voltage);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 3, 50, furi_string_get_cstr(tmp_string));
}

BatteryCheckerApp* app_alloc() {
    BatteryCheckerApp* app = malloc(sizeof(BatteryCheckerApp));
    app->test = false;
    app->voltage = 0.00;

    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    view_port_draw_callback_set(app->view_port, app_draw_callback, app);
    view_port_input_callback_set(app->view_port, app_input_callback, app->event_queue);

    app->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);
    return app;
}

void app_free(BatteryCheckerApp* app) {
    furi_assert(app);

    view_port_enabled_set(app->view_port, false);
    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);

    furi_message_queue_free(app->event_queue);

    furi_record_close(RECORD_GUI);
}

int32_t battery_checker_app(void* p) {
    UNUSED(p);

    BatteryCheckerApp* app = app_alloc();
    InputEvent event;

    // Setup Pin For Read
    furi_hal_gpio_init(&gpio_ext_pc3, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    FURI_LOG_I(TAG, "Gpio Set OK");

    adc_init();
    FURI_LOG_I(TAG, "ADC Full Init OK");

    while(1) {
        if(furi_message_queue_get(app->event_queue, &event, 100) == FuriStatusOk) {
            if(event.key == InputKeyBack) {
                if(event.type == InputTypePress) break;
            } else {
                if(event.key == InputKeyOk) {
                    if(event.type == InputTypePress) {
                        bool current = app->test;
                        app->test = !current;
                    }
                }
            }
        } else {
            uint32_t adc_value = adc_read();
            float adc_voltage = 2.5f * (float)adc_value / 4095.0f;
            FURI_LOG_I(TAG, "ADC: %ld, %f V", adc_value, (double)adc_voltage);
            app->voltage = adc_voltage;
            furi_delay_ms(500);
        }
    }

    adc_deinit();
    FURI_LOG_I(TAG, "ADC Full DeInit OK");

    app_free(app);
    return 0;
}