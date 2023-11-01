/*
@CodeAllNight
https://github.com/jamisonderek/flipper-zero-tutorials

This is a GPIO interrupt demo for the Flipper Zero.  The goal of this project is 
to show how interrupt GPIO input can be used in your own application.

GPIO pin C3 is our interrupt pin (with internal pull-up resistor).  I connect pin 
to a 220 ohm resistor and then the other side of the resistor to one side of a
momentary switch.  Other side of the switch connects to our GND pin.

GPIO A7, A6 and A4 connect to 220ohm resistors then the + or - pins of LED... 
LED1: +A7  -A6
LED2: +A6  -A7
LED3: +A7  -A4
LED4: +A4  -A7
LED5: +A6  -A4
LED6: +A4  -A6

LEDs blink in order.  When switch is pressed, pin C3 transitions from VCC to GND and the
interrupt callback method gets invoked.  This copies the current LED number into the
selected LED (which stops LEDs from blinking).  Pressing down button clears selected LED,
so LEDs start again.  
*/

#include <furi.h>
#include <furi_hal.h>

#include <furi_hal_gpio.h>
#include <furi_hal_resources.h>

#include <gui/gui.h>
#include <locale/locale.h>

#define TAG "gpio_interrupt_demo_app"

typedef enum {
    DemoEventTypeKey,
    // You can add additional events here.
    DemoEventTypeTick,
} DemoEventType;

typedef struct {
    DemoEventType type; // The reason for this event.
    InputEvent input; // This data is specific to keypress data.
    // You can add additional data that is helpful for your events.
} DemoEvent;

typedef struct {
    FuriString* buffer;
    // You can add additional state here.
} DemoData;

typedef struct {
    FuriMessageQueue* queue; // Message queue (DemoEvent items to process).
    FuriMutex* mutex; // Used to provide thread safe access to data.
    DemoData* data; // Data accessed by multiple threads (acquire the mutex before accessing!)
} DemoContext;

static uint8_t currentLed; // The current LED to light (1-6)
static uint8_t selectedLed; // The LED at the time of GPIO interrupt (or 0 if none).

// Invoked when input (button press) is detected.
// We queue a message and then return to the caller.
// @input_event the button that triggered the callback.
// @queue our message queue.
static void demo_input_callback(InputEvent* input_event, FuriMessageQueue* queue) {
    furi_assert(queue);
    DemoEvent event = {.type = DemoEventTypeKey, .input = *input_event};
    furi_message_queue_put(queue, &event, FuriWaitForever);
}

// Invoked from our timer.  We queue a message and then return to the caller.
// @ctx our message queue.
static void demo_tick(void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* queue = ctx;
    DemoEvent event = {.type = DemoEventTypeTick};
    furi_message_queue_put(queue, &event, 0);
}

// Invoked when our GPIO pin transitions from VCC to GND.
// @ctx unused for this method.
static void demo_gpio_fall_callback(void* ctx) {
    UNUSED(ctx);
    selectedLed = currentLed;
}

// Invoked by the draw callback to render the screen.
// We render our UI on the callback thread.
// @canvas the surface to render our UI
// @ctx a pointer to a DemoContext object.
static void demo_render_callback(Canvas* canvas, void* ctx) {
    // Attempt to aquire context, so we can read the data.
    DemoContext* demo_context = ctx;
    if(furi_mutex_acquire(demo_context->mutex, 200) != FuriStatusOk) {
        return;
    }

    DemoData* data = demo_context->data;
    if(selectedLed) {
        furi_string_printf(data->buffer, "Stopped on LED %d.", selectedLed);
    } else {
        furi_string_printf(data->buffer, "GND pin C3 to stop.");
    }

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(
        canvas, 10, 20, AlignLeft, AlignTop, furi_string_get_cstr(data->buffer));

    // Release the context, so other threads can update the data.
    furi_mutex_release(demo_context->mutex);
}

// Turns on one of the LEDs.
// @pin the LED to turn on (value of 1-6)
void update_led(uint8_t led) {
    if(led == 1 || led == 3) {
        // Pin A7 is 3.3 volts for LED 1 & 3.
        furi_hal_gpio_init(&gpio_ext_pa7, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
        furi_hal_gpio_write(&gpio_ext_pa7, true);
    } else {
        furi_hal_gpio_init(&gpio_ext_pa7, GpioModeOutputOpenDrain, GpioPullNo, GpioSpeedLow);
        // Pin A7 is GND (false) for LED 2 & 4.
        furi_hal_gpio_write(&gpio_ext_pa7, !(led == 2 || led == 4));
    }

    if(led == 2 || led == 5) {
        // Pin A6 is 3.3 volts for LED 2 & 5.
        furi_hal_gpio_init(&gpio_ext_pa6, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
        furi_hal_gpio_write(&gpio_ext_pa6, true);
    } else {
        furi_hal_gpio_init(&gpio_ext_pa6, GpioModeOutputOpenDrain, GpioPullNo, GpioSpeedLow);
        // Pin A6 is GND (false) for LED 1 & 6.
        furi_hal_gpio_write(&gpio_ext_pa6, !(led == 1 || led == 6));
    }

    if(led == 4 || led == 6) {
        // Pin A4 is 3.3 volts for LED 4 & 6.
        furi_hal_gpio_init(&gpio_ext_pa4, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
        furi_hal_gpio_write(&gpio_ext_pa4, true);
    } else {
        furi_hal_gpio_init(&gpio_ext_pa4, GpioModeOutputOpenDrain, GpioPullNo, GpioSpeedLow);
        // Pin A4 is GND (false) for LED 3 & 5.
        furi_hal_gpio_write(&gpio_ext_pa4, !(led == 3 || led == 5));
    }
}

// Disconnects a GpioPin via OutputOpenDrive, PushPullNo, output true.
// @pin pointer to GpioPin to disconnect.
void disconnect_pin(const GpioPin* pin) {
    furi_hal_gpio_init(pin, GpioModeOutputOpenDrain, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_write(pin, true);
}

// Program entry point
int32_t gpio_interrupt_demo_app(void* p) {
    UNUSED(p);

    // Configure our initial data.
    DemoContext* demo_context = malloc(sizeof(DemoContext));
    demo_context->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    demo_context->data = malloc(sizeof(DemoData));
    demo_context->data->buffer = furi_string_alloc();

    // Queue for events (tick or input)
    demo_context->queue = furi_message_queue_alloc(8, sizeof(DemoEvent));

    // LEDs connected on GPIO output pins (A7, A6, A4) via 220 ohm resistors.
    currentLed = 1;
    selectedLed = 0;
    update_led(currentLed);

    // GPIO pin (C3) is pull-up to VCC.  Add switch to ground for change in value.
    // I use 220ohm resistor from switch to C3 (but optional if you are SURE
    // the pin is in input/interrupt mode).
    //
    // GpioModeInterruptRiseFall means callback invoked when going from VCC (from our
    // pull-up resistor) to GND.
    //
    // NOTE: You can use GpioModeInterruptRise for invoking on a GND->VCC and
    // GpioModeInterruptRiseFall for invoking on both transitions.
    //
    furi_hal_gpio_init(&gpio_ext_pc3, GpioModeInterruptFall, GpioPullUp, GpioSpeedVeryHigh);

    // NOTE: "add_int_callback" does "enable_int_callback" automatically.
    // For the 3rd parameter, you can pass any object that you want to be passed
    // to your callback method.
    furi_hal_gpio_add_int_callback(&gpio_ext_pc3, demo_gpio_fall_callback, NULL);

    // Timer triggers every 70ms (incrementing our current LED).
    FuriTimer* timer = furi_timer_alloc(demo_tick, FuriTimerTypePeriodic, demo_context->queue);
    furi_timer_start(timer, 70);

    // Set ViewPort callbacks
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, demo_render_callback, demo_context);
    view_port_input_callback_set(view_port, demo_input_callback, demo_context->queue);

    // Open GUI and register view_port
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    // Main loop
    DemoEvent event;
    bool processing = true;
    do {
        if(furi_message_queue_get(demo_context->queue, &event, 1000) == FuriStatusOk) {
            FURI_LOG_T(TAG, "Got event type: %d", event.type);
            switch(event.type) {
            case DemoEventTypeKey:
                // Short press of back button exits the program.
                if(event.input.type == InputTypeShort && event.input.key == InputKeyBack) {
                    FURI_LOG_I(TAG, "Short-Back pressed. Exiting program.");
                    processing = false;
                } else if(event.input.type == InputTypeShort && event.input.key == InputKeyDown) {
                    // Clear our LED stop value.
                    selectedLed = 0;
                }
                break;
            case DemoEventTypeTick:
                if(selectedLed == 0) {
                    if(++currentLed > 6) {
                        currentLed = 1;
                    }
                    update_led(currentLed);
                }
                break;
            default:
                break;
            }

            // Send signal to update the screen (callback will get invoked at some point later.)
            view_port_update(view_port);
        }
    } while(processing);

    // Free resources
    furi_hal_gpio_remove_int_callback(&gpio_ext_pc3);

    furi_timer_free(timer);

    // Pull all pins open.
    disconnect_pin(&gpio_ext_pc3);
    disconnect_pin(&gpio_ext_pa7);
    disconnect_pin(&gpio_ext_pa6);
    disconnect_pin(&gpio_ext_pa4);

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);
    furi_message_queue_free(demo_context->queue);
    furi_mutex_free(demo_context->mutex);
    furi_string_free(demo_context->data->buffer);
    free(demo_context->data);
    free(demo_context);

    return 0;
}