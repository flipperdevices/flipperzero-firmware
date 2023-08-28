#include <furi.h>
#include <furi_hal.h>

#include <furi_hal_gpio.h>
#include <furi_hal_resources.h>

#include <gui/gui.h>
#include <locale/locale.h>
#include <TEA5767.h>

/// The band that will be tuned by this sketch is FM.
#define FIX_BAND RADIO_BAND_FM

/// The station that will be tuned by this sketch is 95.30 MHz.
#define FIX_STATION 9530

typedef enum {
    RadioStateNotFound,
    RadioStateFound,
    RadioStateWriteSuccess,
    RadioStateReadSuccess,
    RadioStateWriteReadSuccess,
} RadioState;

typedef enum {
    DemoEventTypeTick,
    DemoEventTypeKey,
    // You can add additional events here.
} DemoEventType;

typedef struct {
    DemoEventType type; // The reason for this event.
    InputEvent input; // This data is specific to keypress data.
    // You can add additional data that is helpful for your events.
} DemoEvent;

typedef struct {
    FuriString* buffer;
    // You can add additional state here.
    int address;
    RadioState state;
    int value;
    uint8_t registers[5];
} DemoData;

typedef struct {
    FuriMessageQueue* queue; // Message queue (DemoEvent items to process).
    FuriMutex* mutex; // Used to provide thread safe access to data.
    DemoData* data; // Data accessed by multiple threads (acquire the mutex before accessing!)
} DemoContext;

// Invoked when input (button press) is detected.  We queue a message and then return to the caller.
static void input_callback(InputEvent* input_event, FuriMessageQueue* queue) {
    furi_assert(queue);
    DemoEvent event = {.type = DemoEventTypeKey, .input = *input_event};
    furi_message_queue_put(queue, &event, FuriWaitForever);
}

// Invoked by the timer on every tick.  We queue a message and then return to the caller.
static void tick_callback(void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* queue = ctx;
    DemoEvent event = {.type = DemoEventTypeTick};
    // It's OK to loose this event if system overloaded (so we don't pass a wait value for 3rd parameter.)
    furi_message_queue_put(queue, &event, 0);
}

// Invoked by the draw callback to render the screen. We render our UI on the callback thread.
static void render_callback(Canvas* canvas, void* ctx) {
    // Attempt to aquire context, so we can read the data.
    DemoContext* demo_context = ctx;
    if(furi_mutex_acquire(demo_context->mutex, 200) != FuriStatusOk) {
        return;
    }

    DemoData* data = demo_context->data;

    canvas_set_font(canvas, FontPrimary);
    if(data->address == TEA5767_ADR) {
        canvas_draw_str_aligned(canvas, 64, 20, AlignCenter, AlignCenter, "FOUND I2C DEVICE");
        furi_string_printf(data->buffer, "Address 0x%02x", (data->address));
        canvas_draw_str_aligned(
            canvas, 64, 30, AlignCenter, AlignCenter, furi_string_get_cstr(data->buffer));
        if(data->state == RadioStateFound) {
            canvas_draw_str_aligned(canvas, 64, 40, AlignCenter, AlignCenter, "FOUND DEVICE");
        } else if(data->state == RadioStateWriteSuccess) {
            canvas_draw_str_aligned(canvas, 64, 40, AlignCenter, AlignCenter, "WRITE SUCCESS");
        } else if(data->state == RadioStateReadSuccess) {
            canvas_draw_str_aligned(canvas, 64, 40, AlignCenter, AlignCenter, "READ SUCCESS");

        } else if(data->state == RadioStateWriteReadSuccess) {
            canvas_draw_str_aligned(
                canvas, 64, 40, AlignCenter, AlignCenter, "WRITE/READ SUCCESS");
        }
        // furi_string_printf(data->buffer, "value %d", (data->value));
        furi_string_printf(
            data->buffer,
            "registers: %02X%02X%02X%02X%02X",
            data->registers[0],
            data->registers[1],
            data->registers[2],
            data->registers[3],
            data->registers[4]);
        canvas_draw_str_aligned(
            canvas, 64, 50, AlignCenter, AlignCenter, furi_string_get_cstr(data->buffer));
    } else {
        canvas_draw_str_aligned(canvas, 64, 20, AlignCenter, AlignCenter, "I2C NOT FOUND");
        canvas_draw_str_aligned(canvas, 64, 30, AlignCenter, AlignCenter, "pin15=SDA. pin16=SCL");
        canvas_draw_str_aligned(canvas, 64, 40, AlignCenter, AlignCenter, "pin9=VCC. pin18=GND");
    }

    // Release the context, so other threads can update the data.
    furi_mutex_release(demo_context->mutex);
}

// Our main loop invokes this method after acquiring the mutex, so we can safely access the protected data.
static void update_i2c_status(void* ctx) {
    DemoContext* demo_context = ctx;
    DemoData* data = demo_context->data;

    data->address = 0;
    data->state = RadioStateNotFound;
    if(tea5767_is_device_ready()) {
        data->address = TEA5767_ADR;
        data->state = RadioStateFound;

        if(tea5767_init(data->registers)) {
            data->state = RadioStateWriteSuccess;

            int frequency = FIX_STATION;

            if(tea5767_set_frequency(data->registers, frequency)) {
                data->state = RadioStateWriteSuccess;

                data->value = 0;
                if(tea5767_get_frequency(data->registers, &data->value)) {
                    data->state = RadioStateReadSuccess;
                }
            }
        }
    }
}

int32_t radio_app(void* p) {
    UNUSED(p);

    // Configure our initial data.
    DemoContext* demo_context = malloc(sizeof(DemoContext));
    demo_context->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    demo_context->data = malloc(sizeof(DemoData));
    demo_context->data->buffer = furi_string_alloc();
    demo_context->data->address = 0;
    demo_context->data->state = RadioStateNotFound;
    demo_context->data->value = 0;
    demo_context->data->registers[0] = 0x00;
    demo_context->data->registers[1] = 0x00;
    demo_context->data->registers[2] = 0xB0;
    demo_context->data->registers[3] = REG_4_XTAL | REG_4_SMUTE;
    demo_context->data->registers[4] = 0x00;

    // Queue for events (tick or input)
    demo_context->queue = furi_message_queue_alloc(8, sizeof(DemoEvent));

    // Set ViewPort callbacks
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, demo_context);
    view_port_input_callback_set(view_port, input_callback, demo_context->queue);

    // Open GUI and register view_port
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    // Update the screen fairly frequently (every 1000 milliseconds = 1 second.)
    FuriTimer* timer = furi_timer_alloc(tick_callback, FuriTimerTypePeriodic, demo_context->queue);
    furi_timer_start(timer, 1000);

    // Main loop
    DemoEvent event;
    bool processing = true;
    do {
        if(furi_message_queue_get(demo_context->queue, &event, FuriWaitForever) == FuriStatusOk) {
            switch(event.type) {
            case DemoEventTypeKey:
                // Short press of back button exits the program.
                if(event.input.type == InputTypeShort && event.input.key == InputKeyBack) {
                    processing = false;
                }
                break;
            case DemoEventTypeTick:
                // Every timer tick we update the i2c status.
                furi_mutex_acquire(demo_context->mutex, FuriWaitForever);
                update_i2c_status(demo_context);
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
    } while(processing);

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