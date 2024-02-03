/*
@CodeAllNight
https://github.com/jamisonderek/flipper-zero-tutorials

This is a basic demonstration of reading/writing I2C protocol.
For this demo, we connect a I2C device to pins:
 - 3V3 (3V3, pin 9)  = VCC
 - GND (GND, pin 18) = GND
 - SCL (C0, pin 16)  = SCL
 - SDA (C1, pin 15)  = SDA
*/

#include <furi.h>
#include <furi_hal.h>

#include <furi_hal_gpio.h>
#include <furi_hal_resources.h>

#include <gui/gui.h>
#include <locale/locale.h>

typedef enum {
    I2cDemoStateNotFound,
    I2cDemoStateFound,
    I2cDemoStateWriteSuccess,
    I2cDemoStateReadSuccess,
    I2cDemoStateWriteReadSuccess,
} I2cDemoState;

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
    I2cDemoState state;
    uint16_t value;
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
    if(data->address) {
        canvas_draw_str_aligned(canvas, 64, 20, AlignCenter, AlignCenter, "FOUND I2C DEVICE");
        furi_string_printf(data->buffer, "Address 0x%02x", (data->address));
        canvas_draw_str_aligned(
            canvas, 64, 30, AlignCenter, AlignCenter, furi_string_get_cstr(data->buffer));

        if(data->state == I2cDemoStateWriteSuccess) {
            canvas_draw_str_aligned(canvas, 64, 40, AlignCenter, AlignCenter, "WRITE SUCCESS");
        } else if(data->state == I2cDemoStateReadSuccess) {
            canvas_draw_str_aligned(canvas, 64, 40, AlignCenter, AlignCenter, "READ SUCCESS");
        } else if(data->state == I2cDemoStateFound) {
            canvas_draw_str_aligned(canvas, 64, 40, AlignCenter, AlignCenter, "FOUND DEVICE");
        } else if(data->state == I2cDemoStateWriteReadSuccess) {
            canvas_draw_str_aligned(
                canvas, 64, 40, AlignCenter, AlignCenter, "WRITE/READ SUCCESS");
        }
        furi_string_printf(data->buffer, "value %d", (data->value));
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

void demo_i2c_call() {
    uint8_t addr = 0x46;
    uint8_t reg = 0x20;
    uint8_t value8 = 0;
    uint16_t value16 = 0;
    uint8_t buffer[3] = {0x20, 0, 0};
    uint32_t timeout = 100;

    furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);

    // Typically you use one of the following methods...
    furi_hal_i2c_is_device_ready(&furi_hal_i2c_handle_external, addr, timeout);
    furi_hal_i2c_tx(&furi_hal_i2c_handle_external, addr, buffer, 1, timeout);
    furi_hal_i2c_rx(&furi_hal_i2c_handle_external, addr, buffer, 1, timeout);
    furi_hal_i2c_trx(&furi_hal_i2c_handle_external, addr, buffer, 1, buffer, 2, timeout);

    // or one of these helper methods...
    furi_hal_i2c_write_reg_8(&furi_hal_i2c_handle_external, addr, reg, value8, timeout);
    furi_hal_i2c_write_reg_16(&furi_hal_i2c_handle_external, addr, reg, value16, timeout);
    furi_hal_i2c_read_reg_8(&furi_hal_i2c_handle_external, addr, reg, &value8, timeout);
    furi_hal_i2c_read_reg_16(&furi_hal_i2c_handle_external, addr, reg, &value16, timeout);

    furi_hal_i2c_release(&furi_hal_i2c_handle_external);
}

uint8_t demo_i2c_find_device() {
    uint8_t addr = 0;

    furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);

    for(uint8_t try_addr = 0; try_addr != 0xff; try_addr++) {
        if(furi_hal_i2c_is_device_ready(&furi_hal_i2c_handle_external, try_addr, 5)) {
            addr = try_addr;
            break;
        }
    }

    furi_hal_i2c_release(&furi_hal_i2c_handle_external);
    return addr;
}

bool demo_i2c_init_bh1750(uint8_t addr) {
    bool result = false;
    uint8_t buffer[1];
    buffer[0] = 0x1; // write a 0x1 to init a BH1750 device.

    furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);
    result = furi_hal_i2c_tx(&furi_hal_i2c_handle_external, addr, buffer, 1, 100);
    furi_hal_i2c_release(&furi_hal_i2c_handle_external);

    return result;
}

bool demo_i2c_write_one_time_h_res_mode_bh1750(uint8_t addr) {
    bool result = false;
    uint8_t buffer[1] = {
        0x20}; // write a 0x20 for "One Time H-Resolution Mode" from BH1750 device.

    furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);

    if(furi_hal_i2c_tx(&furi_hal_i2c_handle_external, addr, buffer, 1, 100)) {
        result = true;
    }

    furi_hal_i2c_release(&furi_hal_i2c_handle_external);
    return result;
}

bool demo_i2c_read_one_time_h_res_mode_bh1750(uint8_t addr, uint16_t* value) {
    bool result = false;
    uint8_t buffer[2];

    furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);

    // Read 2 bytes from BH1750 device.
    if(furi_hal_i2c_rx(&furi_hal_i2c_handle_external, addr, buffer, 2, 100)) {
        *value = (buffer[0] << 8) | buffer[1];
        result = true;
    }

    furi_hal_i2c_release(&furi_hal_i2c_handle_external);
    return result;
}

bool demo_i2c_write_read_bh1750(uint8_t addr, uint16_t* value) {
    bool result = false;
    uint8_t buffer[2] = {
        0x20, 0}; // write a 0x20 for "One Time H-Resolution Mode" from BH1750 device.

    furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);

    if(furi_hal_i2c_trx(&furi_hal_i2c_handle_external, addr, buffer, 1, buffer, 2, 100)) {
        *value = (buffer[0] << 8) | buffer[1];
        result = true;
    }

    furi_hal_i2c_release(&furi_hal_i2c_handle_external);
    return result;
}

bool demo_i2c_read_reg_bh1750(uint8_t addr, uint16_t* value) {
    bool result = false;

    furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);

    if(furi_hal_i2c_read_reg_16(&furi_hal_i2c_handle_external, addr, 0x20, value, 100)) {
        result = true;
    }

    furi_hal_i2c_release(&furi_hal_i2c_handle_external);
    return result;
}

// Our main loop invokes this method after acquiring the mutex, so we can safely access the protected data.
static void update_i2c_status(void* ctx) {
    DemoContext* demo_context = ctx;
    DemoData* data = demo_context->data;

    uint8_t addr = 0;

    addr = demo_i2c_find_device();
    if(addr) {
        data->state = I2cDemoStateFound;

        if(demo_i2c_init_bh1750(addr)) {
            data->state = I2cDemoStateWriteSuccess;

            if(demo_i2c_write_one_time_h_res_mode_bh1750(addr)) {
                data->state = I2cDemoStateWriteSuccess;

                if(demo_i2c_read_one_time_h_res_mode_bh1750(addr, &data->value)) {
                    data->state = I2cDemoStateReadSuccess;

                    if(demo_i2c_write_read_bh1750(addr, &data->value)) {
                        data->state = I2cDemoStateWriteReadSuccess;
                    }
                }
            }
        }
    }

    data->address = addr;
}

int32_t i2c_demo_app(void* p) {
    UNUSED(p);

    // Configure our initial data.
    DemoContext* demo_context = malloc(sizeof(DemoContext));
    demo_context->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    demo_context->data = malloc(sizeof(DemoData));
    demo_context->data->buffer = furi_string_alloc();
    demo_context->data->address = 0;
    demo_context->data->state = I2cDemoStateNotFound;
    demo_context->data->value = 0;

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

    demo_i2c_call();

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