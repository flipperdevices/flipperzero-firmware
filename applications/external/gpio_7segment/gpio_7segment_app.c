/*
@CodeAllNight
https://github.com/jamisonderek/flipper-zero-tutorials

This app controlls a 7-segment display connected to the Flipper Zero GPIO pins.

WARNING: You should connect 220 ohm resistors to each of your output pins!  The
Flipper Zero only has a 51ohm internal resistor and has a MAX rating of 20mA per
pin.  Without a resistor you could potentially be at 65mA per pin (even with a 
1.5v drop across the LED you would still be at 35mA!)

When you run the app, it will display which pins should control which segments.

Be sure to connect your common cathode pin to GND (or common anode pin to 3.3volts.)
WARNING: You should connect 220 ohm resistors to each of your output pins.

All pins will initially have 3.3volts (so you can test that all segments are
connected properly).  Pressing the UP arrow inverts the output (so instead of
3.3volts for making LED glow, it will send GND signal) this is needed if you are
using common anode instead of common cathode 7-segment LEDs.

Press the OK button to have the Flipper randomly guess a number between 1-6
(I used this app to replace a 6-sided die.)
*/

#include <furi.h>
#include <furi_hal.h>

#include <furi_hal_gpio.h>
#include <furi_hal_resources.h>

#include <gui/gui.h>
#include <locale/locale.h>

/*
Our 7-segment display is connected in the following order...
segment 0 = PA7, segment 1 = PA6, segment 2 = PA4, segment 3 = PB3
segment 4 = PB2, segment 5 = PC3, segment 6 = PC1

Common cathode connection goes to GND pin.
We output a 3.3V to turn LED on and a GND to turn LED off.

seg:
55555
4   6
4   6
00000
3   1
3   1
22222

//seg:0  ,   1  ,   2  ,   3  ,   4  ,   5  ,   6
0 = false,  true,  true,  true,  true,  true,  true
1 = false,  true, false, false, false, false,  true
2 =  true, false,  true,  true, false,  true,  true
3 =  true,  true,  true, false, false,  true,  true
4 =  true,  true, false, false,  true, false,  true
5 =  true,  true,  true, false,  true,  true, false
6 =  true,  true,  true,  true,  true,  true, false
7 = false,  true, false, false, false,  true,  true
8 =  true,  true,  true,  true,  true,  true,  true
9 =  true,  true,  true, false,  true,  true,  true
*/

#define TAG "gpio_7segment_app"

bool digits[70] = {
    /* 0 */ false, true,  true,  true,  true,  true,  true,
    /* 1 */ false, true,  false, false, false, false, true,
    /* 2 */ true,  false, true,  true,  false, true,  true,
    /* 3 */ true,  true,  true,  false, false, true,  true,
    /* 4 */ true,  true,  false, false, true,  false, true,
    /* 5 */ true,  true,  true,  false, true,  true,  false,
    /* 6 */ true,  true,  true,  true,  true,  true,  false,
    /* 7 */ false, true,  false, false, false, true,  true,
    /* 8 */ true,  true,  true,  true,  true,  true,  true,
    /* 9 */ true,  true,  true,  false, true,  true,  true,
};

typedef enum {
    GpioEventTypeKey,
} GpioEventType;

typedef struct {
    GpioEventType type; // The reason for this event.
    InputEvent input; // This data is specific to keypress data.
} GpioEvent;

typedef struct {
    FuriString* buffer;
    int digit;
    bool invert;
} GpioData;

typedef struct {
    FuriMessageQueue* queue; // Message queue (GpioEvent items to process).
    FuriMutex* mutex; // Used to provide thread safe access to data.
    GpioData* data; // Data accessed by multiple threads (acquire the mutex before accessing!)
} GpioContext;

// Invoked when input (button press) is detected.  Queues message and returns to caller.
// @param input_event the input event that caused the callback to be invoked.
// @param queue the message queue for queueing key event.
static void gpio_7segment_input_callback(InputEvent* input_event, FuriMessageQueue* queue) {
    furi_assert(queue);
    GpioEvent event = {.type = GpioEventTypeKey, .input = *input_event};
    furi_message_queue_put(queue, &event, FuriWaitForever);
}

// Invoked by the draw callback to render the screen.
// @param canvas surface to draw on.
// @param ctx a pointer to the application GpioContext.
static void gpio_7segment_render_callback(Canvas* canvas, void* ctx) {
    // Attempt to aquire context, so we can read the data.
    GpioContext* context = ctx;
    if(furi_mutex_acquire(context->mutex, 200) != FuriStatusOk) {
        return;
    }

    GpioData* data = context->data;

    // Title
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 5, 0, AlignLeft, AlignTop, "GPIO 7-Segment LED");

    // Draw two boxes (7-segment LEDs)
    canvas_draw_frame(canvas, 45, 22, 20, 16);
    canvas_draw_frame(canvas, 45, 37, 20, 16);

    // Draw the labels (use CAPS if LED should be glowing.)
    int index = 7 * data->digit;
    canvas_draw_str_aligned(canvas, 50, 39, AlignLeft, AlignTop, digits[index++] ? "A7" : "a7");
    canvas_draw_str_aligned(canvas, 66, 39, AlignLeft, AlignTop, digits[index++] ? "A6" : "a6");
    canvas_draw_str_aligned(canvas, 50, 54, AlignLeft, AlignTop, digits[index++] ? "A4" : "a4");
    canvas_draw_str_aligned(canvas, 32, 39, AlignLeft, AlignTop, digits[index++] ? "B3" : "b3");
    canvas_draw_str_aligned(canvas, 32, 26, AlignLeft, AlignTop, digits[index++] ? "B2" : "b2");
    canvas_draw_str_aligned(canvas, 50, 13, AlignLeft, AlignTop, digits[index++] ? "C3" : "c3");
    canvas_draw_str_aligned(canvas, 66, 26, AlignLeft, AlignTop, digits[index++] ? "C1" : "c1");

    // Tell user if GPIO pins are GND or 3.3v to glow.
    canvas_draw_str_aligned(canvas, 90, 40, AlignLeft, AlignTop, data->invert ? "GND" : "3.3v");

    // Display the current number.
    furi_string_printf(data->buffer, "Digit: %d", data->digit);
    canvas_draw_str_aligned(
        canvas, 90, 50, AlignLeft, AlignTop, furi_string_get_cstr(data->buffer));

    // Release the context, so other threads can update the data.
    furi_mutex_release(context->mutex);
}

// Sets the GPIO pin output to display a number.
// @param digit a value between 0-9 to display.
// @param invert use true if your 7-segment LED display is common anode
// (and all the output pins go to cathode side of LEDs).  use false if
// your display is common cathode.
void gpio_7segment_show(int digit, bool invert) {
    // There are 7 segments per digit.
    int index = 7 * digit;

    furi_hal_gpio_write(&gpio_ext_pa7, digits[index++] ^ invert);
    furi_hal_gpio_write(&gpio_ext_pa6, digits[index++] ^ invert);
    furi_hal_gpio_write(&gpio_ext_pa4, digits[index++] ^ invert);
    furi_hal_gpio_write(&gpio_ext_pb3, digits[index++] ^ invert);
    furi_hal_gpio_write(&gpio_ext_pb2, digits[index++] ^ invert);
    furi_hal_gpio_write(&gpio_ext_pc3, digits[index++] ^ invert);
    furi_hal_gpio_write(&gpio_ext_pc1, digits[index++] ^ invert);
}

// Disconnects a GpioPin via OutputOpenDrive, PushPullNo, output true.
// @pin pointer to GpioPin to disconnect.
void gpio_7segment_disconnect_pin(const GpioPin* pin) {
    furi_hal_gpio_init(pin, GpioModeOutputOpenDrain, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_write(pin, true);
}

// This is the entry point to our application.
int32_t gpio_7segment_app(void* p) {
    UNUSED(p);

    // Configure our initial data.
    GpioContext* context = malloc(sizeof(GpioContext));
    context->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    context->data = malloc(sizeof(GpioData));
    context->data->buffer = furi_string_alloc();
    context->data->digit = 8;
    context->data->invert = false;

    // Queue for events (input)
    context->queue = furi_message_queue_alloc(8, sizeof(GpioEvent));

    // Set ViewPort callbacks
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, gpio_7segment_render_callback, context);
    view_port_input_callback_set(view_port, gpio_7segment_input_callback, context->queue);

    // Set our 7 GPIO external pins to output (push-pull).
    // NOTE: For common anode LED, we *could* use GpioModeOutputOpenDrain.
    furi_hal_gpio_init_simple(&gpio_ext_pa7, GpioModeOutputPushPull);
    furi_hal_gpio_init_simple(&gpio_ext_pa6, GpioModeOutputPushPull);
    furi_hal_gpio_init_simple(&gpio_ext_pa4, GpioModeOutputPushPull);
    furi_hal_gpio_init_simple(&gpio_ext_pb3, GpioModeOutputPushPull);
    furi_hal_gpio_init_simple(&gpio_ext_pb2, GpioModeOutputPushPull);
    furi_hal_gpio_init_simple(&gpio_ext_pc3, GpioModeOutputPushPull);
    furi_hal_gpio_init_simple(&gpio_ext_pc1, GpioModeOutputPushPull);

    // Display the number (this should light all 7 of the LED segments)
    gpio_7segment_show(context->data->digit, context->data->invert);

    // Open GUI and register view_port
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    // Main loop
    GpioEvent event;
    bool processing = true;
    do {
        if(furi_message_queue_get(context->queue, &event, FuriWaitForever) == FuriStatusOk) {
            FURI_LOG_T(TAG, "Got event type: %d", event.type);
            switch(event.type) {
            case GpioEventTypeKey:
                // Short press of back button exits the program.
                if(event.input.type == InputTypeShort && event.input.key == InputKeyBack) {
                    FURI_LOG_I(TAG, "Short-Back pressed. Exiting program.");
                    processing = false;
                } else if(event.input.type == InputTypeShort && event.input.key == InputKeyOk) {
                    FURI_LOG_I(TAG, "OK pressed.");
                    if(furi_mutex_acquire(context->mutex, FuriWaitForever) == FuriStatusOk) {
                        // Pick a random number between 1 and 6...
                        if(context->data->digit >= 9) {
                            context->data->digit = 0;
                        } else {
                            context->data->digit += 1;
                        }

                        gpio_7segment_show(context->data->digit, context->data->invert);
                        furi_mutex_release(context->mutex);
                    }
                } else if(event.input.type == InputTypeShort && event.input.key == InputKeyUp) {
                    FURI_LOG_I(TAG, "UP pressed.");
                    if(furi_mutex_acquire(context->mutex, FuriWaitForever) == FuriStatusOk) {
                        // Invert our output (switch between common anode/cathode 7-segment LEDs)
                        context->data->invert = !context->data->invert;
                        gpio_7segment_show(context->data->digit, context->data->invert);
                        furi_mutex_release(context->mutex);
                    }
                }
                break;
            default:
                break;
            }

            // Send signal to update the screen (callback will get invoked at some point later.)
            view_port_update(view_port);
        } else {
            // We had an issue getting message from the queue, so exit application.
            processing = false;
        }
        view_port_update(view_port);
    } while(processing);

    // Disconnect the GPIO pins.
    gpio_7segment_disconnect_pin(&gpio_ext_pa7);
    gpio_7segment_disconnect_pin(&gpio_ext_pa6);
    gpio_7segment_disconnect_pin(&gpio_ext_pa4);
    gpio_7segment_disconnect_pin(&gpio_ext_pb3);
    gpio_7segment_disconnect_pin(&gpio_ext_pb2);
    gpio_7segment_disconnect_pin(&gpio_ext_pc3);
    gpio_7segment_disconnect_pin(&gpio_ext_pc1);

    // Free resources
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);
    furi_message_queue_free(context->queue);
    furi_mutex_free(context->mutex);
    furi_string_free(context->data->buffer);
    free(context->data);
    free(context);

    return 0;
}