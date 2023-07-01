#include <furi_hal.h>
#include <furi_hal_pwm.h>
#include <gui/gui.h>

// Pin A7 is FuriHalPwmOutputIdTim1PA7, pin A4 is FuriHalPwmOutputIdLptim2PA4.
const FuriHalPwmOutputId channel_led = FuriHalPwmOutputIdTim1PA7;
const GpioPin* const pin_back = &gpio_button_back;

static void my_draw_callback(Canvas* canvas, void* context) {
    UNUSED(context);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 5, 8, "GPIO - BLINK PWM LED");
    canvas_draw_str(canvas, 5, 22, "Connect long LED to pin");
    canvas_draw_str(canvas, 5, 32, "A7.  Connect short LED");
    canvas_draw_str(canvas, 5, 42, "to 220 ohm resistor.");
    canvas_draw_str(canvas, 5, 52, "Connect other end of");
    canvas_draw_str(canvas, 5, 62, "resistor to pin GND.");
}

int gpio_blink_pwm_app(void* p) {
    UNUSED(p);

    // Show directions to user.
    Gui* gui = furi_record_open(RECORD_GUI);
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, my_draw_callback, NULL);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    // Start blinking LED at 1 Hz, 20% duty cycle
    furi_hal_pwm_start(channel_led, 1, 20);

    // Hold the back button to exit (since we only scan it when restarting loop).
    while(furi_hal_gpio_read(pin_back)) {
        furi_delay_ms(100);
    }

    // Stop blinking LED (floating input)
    furi_hal_pwm_stop(channel_led);

    // Remove the directions from the screen.
    gui_remove_view_port(gui, view_port);
    return 0;
}