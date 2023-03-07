
#include "rcwl_0516.h"

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include <input/input.h>
#include <notification/notification_messages.h>

static void rcwl_0516_draw_callback(Canvas* canvas, void* ctx) {
    
    furi_assert(ctx);
    Rcwl_0516* app = ctx;

    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    elements_multiline_text_aligned(canvas, 60, 17, AlignCenter, AlignTop, "Motion Status");
    
    canvas_set_font(canvas, FontSecondary);

    if(app->input_value){
        furi_hal_light_set(LightRed, 255);
        furi_hal_light_set(LightBlue, 0);
        furi_hal_light_set(LightGreen, 0);

        elements_multiline_text_aligned(
            canvas, 60, 32, AlignCenter, AlignTop,"Motion Detection");
        notification_message(app->notifications, &sequence_display_backlight_on);

    }else {
        elements_multiline_text_aligned(
        canvas, 60, 32, AlignCenter, AlignTop,"No Motion");
        notification_message(app->notifications, &sequence_display_backlight_off);
        furi_hal_light_set(LightBlue, 255);
        furi_hal_light_set(LightRed, 0);
        furi_hal_light_set(LightGreen, 0);
    }
}

static void rcwl_0516_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

Rcwl_0516* rcwl_0516_alloc() {
    Rcwl_0516* app = malloc(sizeof(Rcwl_0516));

    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    view_port_draw_callback_set(app->view_port, rcwl_0516_draw_callback, app);
    view_port_input_callback_set(app->view_port, rcwl_0516_input_callback, app->event_queue);

    app->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);
    app->notifications = furi_record_open(RECORD_NOTIFICATION);
    app->input_pin = &gpio_ext_pa7;
    app->output_pin = &gpio_ext_pa6;

    furi_hal_gpio_init(app->input_pin, GpioModeInput, GpioPullUp, GpioSpeedLow);
    furi_hal_gpio_init(app->output_pin, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_write(app->output_pin, false);

    return app;
}

void rcwl_0516_free(Rcwl_0516* app) {
    furi_assert(app);

    view_port_enabled_set(app->view_port, false);
    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);

    furi_message_queue_free(app->event_queue);

    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);
    furi_hal_light_set(LightBlue, 0);
    furi_hal_light_set(LightRed, 0);
    furi_hal_light_set(LightGreen, 0);
}

int32_t rcwl_0516(void* p) {
    UNUSED(p);
    Rcwl_0516* app = rcwl_0516_alloc();

    InputEvent event;

    while(1) {
        app->input_value = furi_hal_gpio_read(app->input_pin);
        
        if(furi_message_queue_get(app->event_queue, &event, 100) == FuriStatusOk) {
            if(event.key == InputKeyBack) {
                if(event.type == InputTypePress) break;

            } 
        }
    }

    rcwl_0516_free(app);
    return 0;
}
