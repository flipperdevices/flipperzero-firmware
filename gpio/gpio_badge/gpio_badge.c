#include <furi.h>
#include <gui/gui.h>

const GpioPin* const pin_leds[] = {
    &gpio_ext_pa7,
    &gpio_ext_pa6,
    &gpio_ext_pa4,
    &gpio_ext_pb3,
    &gpio_ext_pb2,
    &gpio_ext_pc3,
    &gpio_swclk,
    &gpio_swdio,
    &gpio_usart_tx,
    &gpio_usart_rx,
    &gpio_ext_pc1,
    &gpio_ext_pc0,
    &gpio_ibutton,
};

uint32_t speed = 80;

const GpioPin* const pin_back = &gpio_button_back;

static void my_draw_callback(Canvas* canvas, void* context) {
    UNUSED(context);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 5, 8, "GPIO BADGE");
}

static void my_input_callback(InputEvent* event, void* context) {
    UNUSED(context);
    if(event->type == InputTypePress) {
        uint32_t last_speed = speed;
        if(event->key == InputKeyLeft) {
            speed *= 0.8;
            if(speed < 1) {
                speed = 1;
            }
        } else if(event->key == InputKeyRight) {
            speed *= 1.2;
            if(speed < last_speed + 5) {
                speed += 5;
            }
        }
    }
}

int gpio_diag_lights_app(void* p) {
    UNUSED(p);

    Gui* gui = furi_record_open(RECORD_GUI);
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, my_draw_callback, NULL);
    view_port_input_callback_set(view_port, my_input_callback, NULL);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    for(size_t i = 0; i < COUNT_OF(pin_leds); i++) {
        furi_hal_gpio_init_simple(pin_leds[i], GpioModeOutputPushPull);
    }

    int i = 0;
    int d = 1;
    do {
        furi_hal_gpio_write(pin_leds[i], true);
        furi_delay_ms(speed);
        furi_hal_gpio_write(pin_leds[i], false);
        i += d;
        if(i == COUNT_OF(pin_leds) || i < 0) {
            if(i < 1)
                i++;
            else
                i--;
            d = d * -1;
        }

        // Hold the back button to exit (since we only scan it when restarting loop).
    } while(furi_hal_gpio_read(pin_back));

    // Typically when a pin is no longer in use, it is set to analog mode.
    // but we need to use open drain for the 1Wire pin.
    for(size_t i = 0; i < COUNT_OF(pin_leds); i++) {
        furi_hal_gpio_init_simple(pin_leds[i], GpioModeAnalog);
    }
    furi_hal_gpio_init_simple(&gpio_ibutton, GpioModeOutputOpenDrain);

    // Remove the directions from the screen.
    gui_remove_view_port(gui, view_port);
    return 0;
}