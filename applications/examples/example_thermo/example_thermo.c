#include <furi.h>
#include <furi_hal.h>
#include <core/log.h>

#include <gui/gui.h>
#include <gui/view_port.h>

#include <locale/locale.h>

#define UPDATE_PERIOD_MS 500UL
#define TEXT_STORE_SIZE 64U

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    char text_store[TEXT_STORE_SIZE];
    float temp_celsius;
    bool is_connected;
    bool is_running;
} ExampleThermoContext;

static void example_thermo_draw_callback(Canvas* canvas, void* ctx) {
    ExampleThermoContext* context = ctx;

    const size_t middle_x = canvas_width(canvas) / 2U;

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, middle_x, 12, AlignCenter, AlignBottom, "Thermometer Demo");
    canvas_draw_line(canvas, 0, 16, 128, 16);

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, middle_x, 30, AlignCenter, AlignBottom, "Connnect thermometer");
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
        snprintf(context->text_store, TEXT_STORE_SIZE, "Temperature: %+.1f%c", (double)temp, temp_units);
    } else {
        strncpy(context->text_store, "-- No data --", TEXT_STORE_SIZE);
    }


    canvas_draw_str_aligned(canvas, middle_x, 58, AlignCenter, AlignBottom, context->text_store);
}

static void example_thermo_input_callback(InputEvent* event, void* ctx) {
    ExampleThermoContext* context = ctx;
    furi_message_queue_put(context->event_queue, event, FuriWaitForever);
}

static void example_thermo_handle_input(ExampleThermoContext* context) {
    InputEvent event;
    const FuriStatus status = furi_message_queue_get(context->event_queue, &event, UPDATE_PERIOD_MS);

    if((status != FuriStatusOk) || (event.type != InputTypeShort)) {
        return;
    }

    if(event.key == InputKeyBack) {
        context->is_running = false;
    }
}

static void example_thermo_read_temperature(ExampleThermoContext* context) {
    context->is_connected = true;
    context->temp_celsius += 0.1f;
}

static void example_thermo_run(ExampleThermoContext* context) {
    context->is_running = true;
    context->temp_celsius = -20.0f;

    while(context->is_running) {
        example_thermo_read_temperature(context);
        example_thermo_handle_input(context);
    }
}

static ExampleThermoContext* example_thermo_context_alloc() {
    ExampleThermoContext* context = malloc(sizeof(ExampleThermoContext));

    context->view_port = view_port_alloc();
    view_port_draw_callback_set(context->view_port, example_thermo_draw_callback, context);
    view_port_input_callback_set(context->view_port, example_thermo_input_callback, context);

    context->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    context->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(context->gui, context->view_port, GuiLayerFullscreen);

    return context;
}

static void example_thermo_context_free(ExampleThermoContext* context) {
    view_port_enabled_set(context->view_port, false);
    gui_remove_view_port(context->gui, context->view_port);

    furi_message_queue_free(context->event_queue);
    view_port_free(context->view_port);

    furi_record_close(RECORD_GUI);
}

int32_t example_thermo_main(void* p) {
    UNUSED(p);

    ExampleThermoContext* context = example_thermo_context_alloc();
    example_thermo_run(context);
    example_thermo_context_free(context);

    return 0;
}
