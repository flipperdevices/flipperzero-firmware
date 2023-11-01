#include <furi.h>
#include <gui/gui.h>

// For list of pins see https://github.com/flipperdevices/flipperzero-firmware/blob/dev/firmware/targets/f7/furi_hal/furi_hal_resources.c
const GpioPin* const pin_led = &gpio_ext_pa7;
const GpioPin* const pin_back = &gpio_button_back;

static void my_draw_callback(Canvas* canvas, void* context) {
    UNUSED(context);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 5, 8, "GPIO - BLINK LED DEMO");
    canvas_draw_str(canvas, 5, 22, "Connect long LED to pin");
    canvas_draw_str(canvas, 5, 32, "A7.  Connect short LED");
    canvas_draw_str(canvas, 5, 42, "to 220 ohm resistor.");
    canvas_draw_str(canvas, 5, 52, "Connect other end of");
    canvas_draw_str(canvas, 5, 62, "resistor to pin GND.");
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
    furi_hal_gpio_init_simple(pin_led, GpioModeOutputOpenDrain);
    do {
        furi_hal_gpio_write(pin_led, true);
        furi_delay_ms(500);
        furi_hal_gpio_write(pin_led, false);
        furi_delay_ms(500);

        // Hold the back button to exit (since we only scan it when restarting loop).
    } while(furi_hal_gpio_read(pin_back));

    // Typically when a pin is no longer in use, it is set to analog mode.
    furi_hal_gpio_init_simple(pin_led, GpioModeAnalog);

    // Remove the directions from the screen.
    gui_remove_view_port(gui, view_port);
    return 0;
}