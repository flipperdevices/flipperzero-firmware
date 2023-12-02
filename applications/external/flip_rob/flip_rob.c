/*
    flip_rob - Flipper app to control a Nintendo R.O.B./Family Computer Robot.
    John Riney 2023
*/

#include <furi.h>
#include <gui/gui.h>
#include <flip_rob_icons.h>

/* Pin definitions for the IR LEDs and buttons */
const GpioPin* const IR_PIN = &gpio_infrared_tx;
const GpioPin* const BACK_PIN = &gpio_button_back;
const GpioPin* const UP_PIN = &gpio_button_up;
const GpioPin* const DOWN_PIN = &gpio_button_down;
const GpioPin* const LEFT_PIN = &gpio_button_left;
const GpioPin* const RIGHT_PIN = &gpio_button_right;
const GpioPin* const OK_PIN = &gpio_button_ok;

/*
    Timings and signals for IR pulses - derived from
    https://learn.adafruit.com/controlling-a-classic-nintendo-r-o-b-robot-using-circuit-playground-express/overview
*/
const int32_t ON_US = 1500;
const int32_t OFF_US = 15167;

const int8_t COMMAND_LEN = 13;
const int8_t BLINK_COMMAND[] = {0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1};
const int8_t LEFT_COMMAND[] = {0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0};
const int8_t RIGHT_COMMAND[] = {0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0};
const int8_t UP_COMMAND[] = {0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1};
const int8_t DOWN_COMMAND[] = {0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1};
const int8_t CLOSE_COMMAND[] = {0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0};
const int8_t OPEN_COMMAND[] = {0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0};

/* Draw a really minimal UI with instructions. */
static void my_draw_callback(Canvas* canvas, void* context) {
    UNUSED(context);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 60, 10, "Move R.O.B.");
    canvas_draw_str(canvas, 60, 20, "with arrows.");
    canvas_draw_str(canvas, 60, 30, "Toggle hands");
    canvas_draw_str(canvas, 60, 40, "with OK.");
    canvas_draw_str(canvas, 20, 64, "Hold Back to exit.");

    canvas_draw_icon(canvas, 0, 0, &I_rob_mono);

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 8, 36, "by riney");
}

/* Blink */
void send_bit(int8_t b) {
    if(b == 0) {
        furi_hal_gpio_write(IR_PIN, false);
    }
    else {
        furi_hal_gpio_write(IR_PIN, true);
    }

    furi_delay_us(ON_US);
    furi_hal_gpio_write(IR_PIN, false);
    furi_delay_us(OFF_US);
}

/* Blinkblinkblinkblink */
void send_command(const int8_t cmd[]) {
    for(int32_t i = 0; i < COMMAND_LEN; i++) {
        send_bit(cmd[i]);
    }
}

/* Entry point */
int flip_rob_app(void* p) {
    UNUSED(p);

    /* Show directions to user. */
    Gui* gui = furi_record_open(RECORD_GUI);
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, my_draw_callback, NULL);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    /* Initialize the LED pin as 3.3V output. */
    furi_hal_gpio_init_simple(IR_PIN, GpioModeOutputPushPull);

    int8_t hands_closed = 0;

    /* Main loop to read keys and send commands */
    do {
        if(!furi_hal_gpio_read(LEFT_PIN)) {
            /* L/R mirrored for ease of use */
            send_command(RIGHT_COMMAND);
        }
        else if(!furi_hal_gpio_read(RIGHT_PIN)) {
            send_command(LEFT_COMMAND);
        }
        else if(!furi_hal_gpio_read(UP_PIN)) {
            send_command(UP_COMMAND);
        }
        else if(!furi_hal_gpio_read(DOWN_PIN)) {
            send_command(DOWN_COMMAND);
        }
        else if(furi_hal_gpio_read(OK_PIN)) {
            /* OK isn't inverted for some reason? */
            if(hands_closed == 1) {
                hands_closed = 0;
                send_command(OPEN_COMMAND);
            }
            else {
                hands_closed = 1;
                send_command(CLOSE_COMMAND);
            }
        }

        furi_delay_ms(250);
    }
    while(furi_hal_gpio_read(BACK_PIN));

    /* Reset IR pin */
    furi_hal_gpio_init_simple(IR_PIN, GpioModeAnalog);

    /* Clean up GUI and bail */
    gui_remove_view_port(gui, view_port);

    return 0;
}
