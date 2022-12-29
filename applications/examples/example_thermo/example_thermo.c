#include <gui/gui.h>
#include <gui/view_port.h>

#include <core/thread.h>
#include <core/kernel.h>

#include <locale/locale.h>

#include <one_wire/one_wire_host.h>

#define UPDATE_PERIOD_MS 1000UL
#define TEXT_STORE_SIZE 64U

#define DS18B20_CMD_CONVERT 0x44U
#define DS18B20_CMD_READ_SCRATCHPAD 0xbeU

#define DS18B20_CFG_RESOLUTION_POS 5U
#define DS18B20_CFG_RESOLUTION_MASK 0x03U
#define DS18B20_DECIMAL_PART_MASK 0x0fU

#define DS18B20_SIGN_MASK 0xf0U

typedef enum {
    ReaderThreadFlagExit = 1,
} ReaderThreadFlag;

typedef union {
    struct {
        uint8_t temp_lsb;
        uint8_t temp_msb;
        uint8_t user_alarm_high;
        uint8_t user_alarm_low;
        uint8_t config;
        const uint8_t reserved[3];
        uint8_t crc;
    } fields;
    uint8_t bytes[9];
} DS18B20Scratchpad;

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriThread* reader_thread;
    FuriMessageQueue* event_queue;
    OneWireHost* onewire;
    char text_store[TEXT_STORE_SIZE];
    float temp_celsius;
    bool is_connected;
} ExampleThermoContext;

static void example_thermo_request_temperature(ExampleThermoContext* context) {
    OneWireHost* onewire = context->onewire;

    FURI_CRITICAL_ENTER();

    bool success = false;
    do {
        if(!onewire_host_reset(onewire)) break;
        onewire_host_skip(onewire);
        onewire_host_write(onewire, DS18B20_CMD_CONVERT);
        success = true;
    } while(false);

    FURI_CRITICAL_EXIT();

    context->is_connected = success;
}

static void example_thermo_read_temperature(ExampleThermoContext* context) {
    OneWireHost* onewire = context->onewire;

    FURI_CRITICAL_ENTER();

    bool success = false;
    do {
        if(!onewire_host_reset(onewire)) break;
        onewire_host_skip(onewire);

        DS18B20Scratchpad buf;
        onewire_host_write(onewire, DS18B20_CMD_READ_SCRATCHPAD);
        onewire_host_read_bytes(onewire, buf.bytes, sizeof(buf.bytes));

        const uint8_t resolution_mode = (buf.fields.config >> DS18B20_CFG_RESOLUTION_POS) &
                                        DS18B20_CFG_RESOLUTION_MASK;
        const uint8_t decimal_mask =
            (DS18B20_DECIMAL_PART_MASK << (DS18B20_CFG_RESOLUTION_MASK - resolution_mode)) &
            DS18B20_DECIMAL_PART_MASK;

        const uint8_t integer_part = (buf.fields.temp_msb << 4U) | (buf.fields.temp_lsb >> 4U);
        const uint8_t decimal_part = buf.fields.temp_lsb & decimal_mask;

        const bool is_negative = (buf.fields.temp_msb & DS18B20_SIGN_MASK) != 0;
        const float temp_celsius_abs = integer_part + decimal_part / 16.f;

        context->temp_celsius = is_negative ? -temp_celsius_abs : temp_celsius_abs;

        success = true;
    } while(false);

    FURI_CRITICAL_EXIT();

    context->is_connected = success;
}

static int32_t example_thermo_reader_thread_callback(void* ctx) {
    ExampleThermoContext* context = ctx;

    for(;;) {
        /* Tell the termometer to start measuring the temperature. The process may take up to 750ms. */
        example_thermo_request_temperature(context);

        /* Wait for the measurement to finish. At the same time wait for an exit signal. */
        const uint32_t flags = furi_thread_flags_wait(ReaderThreadFlagExit, FuriFlagWaitAny, UPDATE_PERIOD_MS);

        /* If an exit signal was received, return from this thread. */
        if(flags != FuriFlagErrorTimeout) break;

        /* The measurement is now ready, read it from the termometer. */
        example_thermo_read_temperature(context);
    }

    return 0;
}

static void example_thermo_draw_callback(Canvas* canvas, void* ctx) {
    ExampleThermoContext* context = ctx;

    const size_t middle_x = canvas_width(canvas) / 2U;

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, middle_x, 12, AlignCenter, AlignBottom, "Thermometer Demo");
    canvas_draw_line(canvas, 0, 16, 128, 16);

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(
        canvas, middle_x, 30, AlignCenter, AlignBottom, "Connnect thermometer");
    canvas_draw_str_aligned(canvas, middle_x, 42, AlignCenter, AlignBottom, "to GPIO pin 17");

    canvas_set_font(canvas, FontKeyboard);

    if(context->is_connected) {
        float temp;
        char temp_units;

        switch(locale_get_measurement_unit()) {
        case LocaleMeasurementUnitsMetric:
            temp = context->temp_celsius;
            temp_units = 'C';
            break;
        case LocaleMeasurementUnitsImperial:
            temp = locale_celsius_to_fahrenheit(context->temp_celsius);
            temp_units = 'F';
            break;
        default:
            furi_crash("Illegal measurement units");
        }
        snprintf(
            context->text_store, TEXT_STORE_SIZE, "Temperature: %+.1f%c", (double)temp, temp_units);
    } else {
        strncpy(context->text_store, "-- No data --", TEXT_STORE_SIZE);
    }

    canvas_draw_str_aligned(canvas, middle_x, 58, AlignCenter, AlignBottom, context->text_store);
}

static void example_thermo_input_callback(InputEvent* event, void* ctx) {
    ExampleThermoContext* context = ctx;
    furi_message_queue_put(context->event_queue, event, FuriWaitForever);
}


static void example_thermo_run(ExampleThermoContext* context) {
    furi_thread_start(context->reader_thread);

    for(bool is_running = true; is_running;) {
        InputEvent event;
        const FuriStatus status =
            furi_message_queue_get(context->event_queue, &event, FuriWaitForever);

        if((status != FuriStatusOk) || (event.type != InputTypeShort)) {
            continue;
        }

        if(event.key == InputKeyBack) {
            is_running = false;
        }
    }

    furi_thread_flags_set(furi_thread_get_id(context->reader_thread), ReaderThreadFlagExit);
    furi_thread_join(context->reader_thread);
}

static void example_thermo_setup(ExampleThermoContext* context) {
    OneWireHost* onewire = context->onewire;
    onewire_host_start(onewire);
}

static void example_thermo_wrapup(ExampleThermoContext* context) {
    OneWireHost* onewire = context->onewire;
    onewire_host_stop(onewire);
}

static ExampleThermoContext* example_thermo_context_alloc() {
    ExampleThermoContext* context = malloc(sizeof(ExampleThermoContext));

    context->view_port = view_port_alloc();
    view_port_draw_callback_set(context->view_port, example_thermo_draw_callback, context);
    view_port_input_callback_set(context->view_port, example_thermo_input_callback, context);

    context->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    context->reader_thread = furi_thread_alloc();
    furi_thread_set_stack_size(context->reader_thread, 1024U);
    furi_thread_set_context(context->reader_thread, context);
    furi_thread_set_callback(context->reader_thread, example_thermo_reader_thread_callback);

    context->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(context->gui, context->view_port, GuiLayerFullscreen);

    context->onewire = onewire_host_alloc();

    return context;
}

static void example_thermo_context_free(ExampleThermoContext* context) {
    view_port_enabled_set(context->view_port, false);
    gui_remove_view_port(context->gui, context->view_port);

    onewire_host_free(context->onewire);
    furi_thread_free(context->reader_thread);
    furi_message_queue_free(context->event_queue);
    view_port_free(context->view_port);

    furi_record_close(RECORD_GUI);
}

int32_t example_thermo_main(void* p) {
    UNUSED(p);

    ExampleThermoContext* context = example_thermo_context_alloc();
    example_thermo_setup(context);

    example_thermo_run(context);

    example_thermo_wrapup(context);
    example_thermo_context_free(context);

    return 0;
}
