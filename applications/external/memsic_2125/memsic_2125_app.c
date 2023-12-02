/*
@CodeAllNight
https://github.com/jamisonderek/flipper-zero-tutorials

This is a demostration of using GPIO interrupts to interpret data from the 
Memsic 2125 (Mx2125) Dual-Axis Accelerometer.

Memsic - Name - Purpose                                    - Flipper
======   ====   ==========================================   =============
Pin 1  - Tout - Temperature Out                            - not connected
Pin 2  - Yout - Y-axis PWM Out (100Hz, duty cycle = value) - C0
Pin 3  - GND  - Ground                                     - GND 
Pin 4  - GND  - Ground                                     - GND
Pin 5  - Xout - X-axis PWM Out (100Hz, duty cycle = value) - C1
Pin 6  - Vdd  - Drain voltage (3.3V to 5V DC)              - 3v3 

*/

#include <furi.h>
#include <furi_hal.h>

#include <furi_hal_gpio.h>
#include <furi_hal_resources.h>

#include <gui/gui.h>
#include <locale/locale.h>

#include <notification/notification.h>
#include <notification/notification_messages.h>

#define TAG "memsic_2125_app"

typedef enum {
    DemoEventTypeKey,
} DemoEventType;

typedef struct {
    DemoEventType type; // The reason for this event.
    InputEvent input; // This data is specific to keypress data.
} DemoEvent;

typedef struct {
    const GpioPin* pin; // pin being monitored.
    uint32_t high; // timestamp of when pin went high.
    uint32_t low; // timestamp of when pin when low.
    float value; // duty cycle (0.0 to 100.0)
    bool reading; // when true, then value will not be updated.
} AxisData;

typedef struct {
    FuriString* buffer;
    AxisData* xData;
    AxisData* yData;
} DemoData;

typedef struct {
    FuriMessageQueue* queue; // Message queue
    DemoData* data;
} DemoContext;

// Invoked when input (button press) is detected.
// We queue a message and then return to the caller.
// @input_event the button that triggered the callback.
// @queue our message queue.
static void input_callback(InputEvent* input_event, FuriMessageQueue* queue) {
    furi_assert(queue);
    DemoEvent event = {.type = DemoEventTypeKey, .input = *input_event};
    furi_message_queue_put(queue, &event, FuriWaitForever);
}

// Invoked whenever the monitored pin changes state.
// @data pointer to an AxisData.
void pulse_callback(void* data) {
    // Get the current time from high-resolution timer.
    FuriHalCortexTimer timer = furi_hal_cortex_timer_get(0);
    uint32_t now = timer.start;

    // Our parameter is a pointer to the axis data.
    furi_assert(data);
    AxisData* d = (AxisData*)data;

    // Get the current state of the pin (true = 3.3volts, false = GND)
    bool state = furi_hal_gpio_read(d->pin);

    if(state) {
        // state=true, so we are in GND->3v3 transition.

        // See if we have timings for both the high & low transitions.
        if((d->high != 0) && (d->low != 0) && (d->high < d->low) && !d->reading) {
            uint32_t durationCycle = now - d->high;
            uint32_t durationLow = d->low - d->high;

            // Update the value of the axis to reflect the duty cycle.
            d->value = (100.0f * durationLow) / durationCycle;
        }

        // Store the current time that the rise transition happened.
        d->high = timer.start;
    } else {
        // Store the current time that the fall transition happened.
        d->low = timer.start;
    }
}

// Invoked by the draw callback to render the screen.
// We render our UI on the callback thread.
// @canvas the surface to render our UI
// @ctx a pointer to a DemoContext object.
static void render_callback(Canvas* canvas, void* ctx) {
    // Attempt to aquire context, so we can read the data.
    DemoContext* demo_context = ctx;
    DemoData* data = demo_context->data;

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 1, 1, AlignLeft, AlignTop, "Memsic 2125  C1:X C0:Y");

    data->xData->reading = true;
    data->yData->reading = true;
    furi_string_printf(
        data->buffer, "%0.3f  %0.3f", (double)data->xData->value, (double)data->yData->value);
    data->xData->reading = false;
    data->yData->reading = false;
    canvas_draw_str_aligned(
        canvas, 30, 55, AlignLeft, AlignTop, furi_string_get_cstr(data->buffer));

    // Draw a circle based on the xData & yData information.
    data->xData->reading = true;
    uint8_t x = (0.5f + ((50.0f - data->xData->value) / 12.0f)) * 128;
    data->xData->reading = false;
    x = MAX(0, MIN(x, 127));
    data->yData->reading = true;
    uint8_t y = (0.5f + (((float)data->yData->value - 50.0f) / 12.0f)) * 64;
    data->yData->reading = false;
    y = MAX(0, MIN(y, 63));
    canvas_draw_circle(canvas, x, y, 2);
}

// Program entry point
int32_t memsic_2125_app(void* p) {
    UNUSED(p);

    // Configure our initial data.
    DemoContext* demo_context = malloc(sizeof(DemoContext));
    demo_context->data = malloc(sizeof(DemoData));
    demo_context->data->buffer = furi_string_alloc();

    AxisData* xData = malloc(sizeof(AxisData));
    xData->pin = &gpio_ext_pc1;
    xData->high = 0;
    xData->low = 0;
    xData->reading = false;
    xData->value = 0;
    demo_context->data->xData = xData;

    AxisData* yData = malloc(sizeof(AxisData));
    yData->pin = &gpio_ext_pc0;
    yData->high = 0;
    yData->low = 0;
    yData->reading = false;
    yData->value = 0;
    demo_context->data->yData = yData;

    // Queue for events (tick or input)
    demo_context->queue = furi_message_queue_alloc(8, sizeof(DemoEvent));

    // x-axis is a 100Hz pulse, with variable duty-cycle (which we need to measure).
    // Invoke the pulse_callback method passing the xData structure whenever the pin
    // transitions (Rise GND->3v3 or Fall 3v3->GND).
    furi_hal_gpio_init(xData->pin, GpioModeInterruptRiseFall, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_add_int_callback(xData->pin, pulse_callback, xData);

    // y-axis is a 100Hz pulse, with variable duty-cycle (which we need to measure).
    // Invoke the pulse_callback method passing the xData structure whenever the pin
    // transitions (Rise GND->3v3 or Fall 3v3->GND).
    furi_hal_gpio_init(yData->pin, GpioModeInterruptRiseFall, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_add_int_callback(yData->pin, pulse_callback, yData);

    // Set ViewPort callbacks
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, demo_context);
    view_port_input_callback_set(view_port, input_callback, demo_context->queue);

    // Open GUI and register view_port
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    // Main loop
    DemoEvent event;
    bool processing = true;
    do {
        if(furi_message_queue_get(demo_context->queue, &event, FuriWaitForever) == FuriStatusOk) {
            FURI_LOG_T(TAG, "Got event type: %d", event.type);
            switch(event.type) {
            case DemoEventTypeKey:
                // Short press of back button exits the program.
                if(event.input.type == InputTypeShort && event.input.key == InputKeyBack) {
                    FURI_LOG_I(TAG, "Short-Back pressed. Exiting program.");
                    processing = false;
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
    } while(processing);

    // Free resources
    furi_hal_gpio_remove_int_callback(yData->pin);
    furi_hal_gpio_remove_int_callback(xData->pin);
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);
    furi_message_queue_free(demo_context->queue);
    furi_string_free(demo_context->data->buffer);
    free(demo_context->data);
    free(demo_context);

    return 0;
}