#include <sdq_slave.h>
#include <yuricable_pro_max_structs.c>

#define TAG "YURICABLE_PRO_MAX"
#define SDQ_PIN gpio_ext_pa7 // GPIO 2

bool sdq_started = false;

static void demo_input_callback(InputEvent* input_event, FuriMessageQueue* queue) {
    furi_assert(queue);
    Event event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(queue, &event, FuriWaitForever);
}

static void demo_render_callback(Canvas* canvas, void* ctx) {
    DemoContext* demo_context = ctx;
    if(furi_mutex_acquire(demo_context->mutex, 200) != FuriStatusOk) {
        return;
    }
    DemoData* data = demo_context->data;
    canvas_set_font(canvas, FontPrimary);
    if(sdq_started) {
        furi_string_printf(data->buffer, "SDQ Slave Active");
    } else {
        furi_string_printf(data->buffer, "SDQ Slave Inactive");
    }
    canvas_draw_str_aligned(
        canvas, 10, 20, AlignLeft, AlignTop, furi_string_get_cstr(data->buffer));
    furi_mutex_release(demo_context->mutex);
}

int32_t yuricable_pro_max_app(void* p) {
    UNUSED(p);
    FURI_LOG_I(TAG, "Starting the SDQ Listener on GPIO 2!");
    // Configure our initial data.
    DemoContext* demo_context = malloc(sizeof(DemoContext));
    demo_context->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    demo_context->data = malloc(sizeof(DemoData));
    demo_context->data->buffer = furi_string_alloc();
    // Queue for events (tick or input)
    demo_context->queue = furi_message_queue_alloc(8, sizeof(Event));
    struct SDQSlave* mySDQSlave = sdq_slave_alloc(&SDQ_PIN);
    //  Set ViewPort callbacks
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, demo_render_callback, demo_context);
    view_port_input_callback_set(view_port, demo_input_callback, demo_context->queue);
    // Open GUI and register view_port
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    Event event;
    bool processing = true;
    do {
        if(furi_message_queue_get(demo_context->queue, &event, 1000) == FuriStatusOk) {
            FURI_LOG_T(TAG, "Got event type: %d", event.type);
            switch(event.type) {
            case EventTypeKey:
                // Short press of back button exits the program.
                if(event.input.type == InputTypeShort && event.input.key == InputKeyBack) {
                    FURI_LOG_I(TAG, "Short-Back pressed. Exiting program.");
                    processing = false;
                } else if(event.input.type == InputTypeShort && event.input.key == InputKeyOk) {
                    FURI_LOG_I(TAG, "Pressed Enter Key");
                    if(!sdq_started) {
                        sdq_slave_start(mySDQSlave);
                        sdq_started = true;
                        view_port_update(view_port);
                        FURI_LOG_I(TAG, "SDQ Slave started");
                    } else {
                        sdq_slave_stop(mySDQSlave);
                        sdq_started = false;
                        view_port_update(view_port);
                        FURI_LOG_I(TAG, "SDQ Slave stopped");
                    }
                }
                break;
            default:
                break;
            }
        }
    } while(processing);
    sdq_slave_free(mySDQSlave);
    furi_hal_gpio_init(&SDQ_PIN, GpioModeOutputOpenDrain, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_write(&SDQ_PIN, true);
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);
    return 0;
}