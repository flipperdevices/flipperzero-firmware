#include <furi.h>
#include <gui/gui.h>

// For list of pins see https://github.com/flipperdevices/flipperzero-firmware/blob/dev/firmware/targets/f7/furi_hal/furi_hal_resources.c
const GpioPin* const pin_led_1 = &gpio_ext_pa7;
const GpioPin* const pin_led_2 = &gpio_ext_pa6; 
const GpioPin* const pin_led_3 = &gpio_ext_pa4; 
const GpioPin* const pin_led_4 = &gpio_ext_pb3; 
const GpioPin* const pin_led_5 = &gpio_ext_pb2; 
const GpioPin* const pin_led_6 = &gpio_ext_pc3; 
const GpioPin* const pin_led_7 = &gpio_ext_pc1; 
const GpioPin* const pin_led_8 = &gpio_ext_pc0; 
const GpioPin* const pin_back = &gpio_button_back;
const GpioPin* const pin_enter = &gpio_button_ok;

static void my_draw_callback(Canvas* canvas, void* context) {
    UNUSED(context);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 5, 8, "GPIO - BLINK LED DEMO");
    canvas_draw_str(canvas, 5, 22, "Abgeändertes Programm");
    canvas_draw_str(canvas, 5, 32, "Original von:");
    canvas_draw_str(canvas, 5, 42, "Derek Jamison");
    canvas_draw_str(canvas, 5, 52, "Geändert von:");
    canvas_draw_str(canvas, 5, 62, "Matthias");
}

int gpio_blink_app(void* p) {
    UNUSED(p);

    // Show directions to user.
    Gui* gui = furi_record_open(RECORD_GUI);
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, my_draw_callback, NULL);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    // Initialize the LED pin as output.
    // GpioModeOutputPushPull means true = 3.3 volts, false = 0 volts.
    // GpioModeOutputOpenDrain means true = floating, false = 0 volts.
    furi_hal_gpio_init_simple(pin_led_1, GpioModeOutputPushPull);
    furi_hal_gpio_init_simple(pin_led_2, GpioModeOutputPushPull);
    furi_hal_gpio_init_simple(pin_led_3, GpioModeOutputPushPull);
    furi_hal_gpio_init_simple(pin_led_4, GpioModeOutputPushPull);
    furi_hal_gpio_init_simple(pin_led_5, GpioModeOutputPushPull);
    furi_hal_gpio_init_simple(pin_led_6, GpioModeOutputPushPull);
    furi_hal_gpio_init_simple(pin_led_7, GpioModeOutputPushPull);
    furi_hal_gpio_init_simple(pin_led_8, GpioModeOutputPushPull);

do
{
            furi_hal_gpio_write(pin_led_1, true);
        furi_delay_ms(100);
        furi_hal_gpio_write (pin_led_1, false);
        furi_delay_ms (100);
        furi_hal_gpio_write(pin_led_2, true);
        furi_delay_ms(100);
        furi_hal_gpio_write(pin_led_2, false);
        furi_delay_ms (100);
        furi_hal_gpio_write(pin_led_3, true);
        furi_delay_ms(100);
        furi_hal_gpio_write (pin_led_3, false);
        furi_delay_ms (100);
        furi_hal_gpio_write(pin_led_4, true);
        furi_delay_ms(100);
        furi_hal_gpio_write(pin_led_4, false);
        furi_delay_ms (100);
        furi_hal_gpio_write(pin_led_5, true);
        furi_delay_ms(100);
        furi_hal_gpio_write (pin_led_5, false);
        furi_delay_ms (100);
        furi_hal_gpio_write(pin_led_6, true);
        furi_delay_ms(100);
        furi_hal_gpio_write(pin_led_6, false);
        furi_delay_ms (100);
        furi_hal_gpio_write(pin_led_7, true);
        furi_delay_ms(100);
        furi_hal_gpio_write (pin_led_7, false);
        furi_delay_ms (100);
        furi_hal_gpio_write(pin_led_8, true);
        furi_delay_ms(100);
        furi_hal_gpio_write(pin_led_8, false);
        furi_delay_ms (100);

} while (furi_hal_gpio_read(pin_back));


        // Hold the back button to exit (since we only scan it when restarting loop).
        // while(furi_hal_gpio_read(pin_back));
    
    // Typically when a pin is no longer in use, it is set to analog mode.
    //furi_hal_gpio_init_simple(pin_led, GpioModeAnalog);

    // Remove the directions from the screen.
    gui_remove_view_port(gui, view_port);
    return 0;
}