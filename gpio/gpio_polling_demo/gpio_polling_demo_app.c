/*
@CodeAllNight
https://github.com/jamisonderek/flipper-zero-tutorials

This is a demonstration of reading GPIO using polling.

*/

#include <furi.h>
#include <furi_hal.h>

#include <furi_hal_gpio.h>
#include <furi_hal_resources.h>

#include <gui/gui.h>
#include <locale/locale.h>

typedef enum {
    DemoEventTypeTick,
    DemoEventTypeKey,
    // You can add additional events here.
} DemoEventType;

typedef struct {
    DemoEventType type; // The reason for this event.
    InputEvent input;   // This data is specific to keypress data.
    // You can add additional data that is helpful for your events.
} DemoEvent;

typedef struct {
    FuriString* buffer;
    // You can add additional state here.
    int counter;
    bool pin_grounded;
} DemoData;

typedef struct {
    FuriMessageQueue* queue; // Message queue (DemoEvent items to process).
    FuriMutex* mutex; // Used to provide thread safe access to data.
    DemoData* data; // Data accessed by multiple threads (acquire the mutex before accessing!)
} DemoContext;

GpioPin demo_message_pin; // This is the pin used for our GPIO demo.

// Invoked when input (button press) is detected.  We queue a message and then return to the caller.
static void gpio_polling_demo_input_callback(InputEvent* input_event, FuriMessageQueue* queue) {
    furi_assert(queue);
    DemoEvent event = {.type = DemoEventTypeKey, .input = *input_event};
    furi_message_queue_put(queue, &event, FuriWaitForever);
}

// Invoked by the timer on every tick.  We queue a message and then return to the caller.
static void gpio_polling_demo_tick(void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* queue = ctx;
    DemoEvent event = {.type = DemoEventTypeTick};
    // It's OK to loose this event if system overloaded (so we don't pass a wait value for 3rd parameter.)
    furi_message_queue_put(queue, &event, 0);
}

// Invoked by the draw callback to render the screen. We render our UI on the callback thread.
static void gpio_polling_demo_render_callback(Canvas* canvas, void* ctx) {
    // Attempt to aquire context, so we can read the data.
    DemoContext* demo_context = ctx;
    if(furi_mutex_acquire(demo_context->mutex, 200) != FuriStatusOk) {
        return;
    }

    DemoData* data = demo_context->data;
    bool show_hello = !data->pin_grounded;
    int counter = data->counter;
    bool even_counter = (counter & 1) == 0; // If the lowest bit is 0, then counter is even.
    
    // Other fonts are FontPrimary, FontSecondary, FontKeyboard, FontBigNumbers,
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 31, AlignLeft, AlignCenter, (show_hello ? "HELLO" : "WORLD"));

    // Put the lowest 4 digits of counter value into the buffer, then concatenate the text "Even" or "Odd". 
    furi_string_printf(data->buffer, "%04u", (counter % 10000));
    furi_string_cat_printf(data->buffer, " %s", (even_counter) ? ("Even") : ("Odd"));

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 64, 42, AlignCenter, AlignTop, furi_string_get_cstr(data->buffer));

    // Release the context, so other threads can update the data.
    furi_mutex_release(demo_context->mutex);

    // Make tones if the speaker is available.
	if (furi_hal_speaker_acquire(1000)) { 
        float freq = 100.0f + (counter*4.0);
        float volume = 1.0f;
        furi_hal_speaker_start(freq, volume);
        furi_delay_ms(100);
        furi_hal_speaker_stop();
        furi_hal_speaker_release();
    }
}

// Our main loop invokes this method after acquiring the mutex, so we can safely access the protected data.
// We increment a counter and update if the GPIO pin is grounded.
static void gpio_polling_demo_update_pin_status(void* ctx) {
    DemoContext* demo_context = ctx;
    DemoData* data = demo_context->data;
    data->counter++;

    // read returns true for VCC and false for ground, so invert answer.
    data->pin_grounded = !furi_hal_gpio_read(&demo_message_pin);
}

int32_t gpio_polling_demo_app(void* p) {
    UNUSED(p);

    // Configure our initial data.
    DemoContext* demo_context = malloc(sizeof(DemoContext));
    demo_context->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    demo_context->data = malloc(sizeof(DemoData));
    demo_context->data->buffer = furi_string_alloc();
    demo_context->data->counter = 0;
    demo_context->data->pin_grounded = false;

    // For this demo we are using pin "gpio_ext_pa4" from \firmware\targets\f7\furi_hal\furi_hal_resources.c
    demo_message_pin = gpio_ext_pa4;
    furi_hal_gpio_init(&demo_message_pin, GpioModeInput, GpioPullUp, GpioSpeedVeryHigh);

    // Queue for events (tick or input)
    demo_context->queue = furi_message_queue_alloc(8, sizeof(DemoEvent));

    // Set ViewPort callbacks
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, gpio_polling_demo_render_callback, demo_context);
    view_port_input_callback_set(view_port, gpio_polling_demo_input_callback, demo_context->queue);

    // Open GUI and register view_port
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    // Update the screen fairly frequently (every 250 milliseconds.)
    FuriTimer* timer = furi_timer_alloc(gpio_polling_demo_tick, FuriTimerTypePeriodic, demo_context->queue);
    furi_timer_start(timer, 250);

    // Main loop
    DemoEvent event;
    bool processing = true;
    do {
        if (furi_message_queue_get(demo_context->queue, &event, FuriWaitForever) == FuriStatusOk) {
            switch (event.type) {
                case DemoEventTypeKey:
                    // Short press of back button exits the program.
                    if(event.input.type == InputTypeShort && event.input.key == InputKeyBack) {
                        processing = false;
                    }
                    break;
                case DemoEventTypeTick:
                    // Every timer tick we update the pin status. 
                    furi_mutex_acquire(demo_context->mutex, FuriWaitForever);
                    gpio_polling_demo_update_pin_status(demo_context);
                    furi_mutex_release(demo_context->mutex);
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
    } while (processing);

    // Make pin float on exit.
    furi_hal_gpio_init(&demo_message_pin, GpioModeInput, GpioPullNo, GpioSpeedVeryHigh);

    // Free resources
    furi_timer_free(timer);
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