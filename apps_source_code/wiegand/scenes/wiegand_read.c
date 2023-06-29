#include "../wiegand.h"

void wiegand_isr_d0(void* context) {
    UNUSED(context);
    uint32_t time = DWT->CYCCNT;
    bool rise = furi_hal_gpio_read(pinD0);

    data[bit_count] = 0;

    if(rise) {
        data_rise[bit_count] = time;
        if(bit_count < MAX_BITS) {
            bit_count++;
        }
    } else {
        data_fall[bit_count] = time;
    }
}

void wiegand_isr_d1(void* context) {
    UNUSED(context);
    uint32_t time = DWT->CYCCNT;
    bool rise = furi_hal_gpio_read(pinD1);

    data[bit_count] = 1;

    if(rise) {
        data_rise[bit_count] = time;
        if(bit_count < MAX_BITS) {
            bit_count++;
        }
    } else {
        data_fall[bit_count] = time;
    }
}

void wiegand_start_read(void* context) {
    App* app = context;
    data_saved = false;
    bit_count = 0;
    furi_hal_gpio_init_simple(pinD0, GpioModeInterruptRiseFall);
    furi_hal_gpio_init_simple(pinD1, GpioModeInterruptRiseFall);
    furi_hal_gpio_add_int_callback(pinD0, wiegand_isr_d0, NULL);
    furi_hal_gpio_add_int_callback(pinD1, wiegand_isr_d1, NULL);
    furi_timer_start(app->timer, 100);
}

void wiegand_stop_read(void* context) {
    App* app = context;
    furi_hal_gpio_remove_int_callback(pinD0);
    furi_hal_gpio_remove_int_callback(pinD1);
    furi_hal_gpio_init_simple(pinD0, GpioModeAnalog);
    furi_hal_gpio_init_simple(pinD1, GpioModeAnalog);
    furi_timer_stop(app->timer);
}

void wiegand_timer_callback(void* context) {
    App* app = context;
    uint32_t duration = DWT->CYCCNT;
    const uint32_t one_millisecond = 64000;

    if(bit_count == 0) {
        return;
    }

    duration -= data_fall[bit_count - 1];

    FURI_CRITICAL_ENTER();
    if(duration > 25 * one_millisecond) {
        if(bit_count == 4 || bit_count == 8 || bit_count == 24 || bit_count == 26 ||
           bit_count == 32 || bit_count == 34 || bit_count == 37 || bit_count == 40) {
            wiegand_stop_read(app);
            scene_manager_next_scene(app->scene_manager, WiegandDataScene);
        } else {
            // No data, clear
            bit_count = 0;
        }
    }
    FURI_CRITICAL_EXIT();
}

void wiegand_read_scene_on_enter(void* context) {
    App* app = context;
    widget_reset(app->widget);
    widget_add_string_element(app->widget, 0, 0, AlignLeft, AlignTop, FontPrimary, "Read Wiegand");
    widget_add_string_element(
        app->widget, 0, 25, AlignLeft, AlignTop, FontSecondary, "Waiting for signal...");
    widget_add_string_element(
        app->widget, 0, 45, AlignLeft, AlignTop, FontSecondary, "D0/Green/A4");
    widget_add_string_element(
        app->widget, 0, 53, AlignLeft, AlignTop, FontSecondary, "D1/White/A7");
    wiegand_start_read(app);
    view_dispatcher_switch_to_view(app->view_dispatcher, WiegandWidgetView);
}

void wiegand_read_scene_on_exit(void* context) {
    App* app = context;
    wiegand_stop_read(app);
}
