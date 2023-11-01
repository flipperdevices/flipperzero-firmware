/*
@CodeAllNight
https://github.com/jamisonderek/flipper-zero-tutorials

This is a basic application for the Flipper Zero.  
The goal of this project is to use it as a starting point for other applications.

*/

#include <furi.h>
#include <furi_hal.h>

#include <furi_hal_gpio.h>
#include <furi_hal_resources.h>

#include <gui/gui.h>
#include <locale/locale.h>

#define TAG "basic_demo_app"

typedef enum {
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
} DemoData;

typedef struct {
    FuriMessageQueue* queue; // Message queue (DemoEvent items to process).
    FuriMutex* mutex; // Used to provide thread safe access to data.
    DemoData* data; // Data accessed by multiple threads (acquire the mutex before accessing!)
} DemoContext;

// Invoked when input (button press) is detected.  We queue a message and then return to the caller.
static void basic_demo_input_callback(InputEvent* input_event, FuriMessageQueue* queue) {
    furi_assert(queue);
    DemoEvent event = {.type = DemoEventTypeKey, .input = *input_event};
    furi_message_queue_put(queue, &event, FuriWaitForever);
}

// Invoked by the draw callback to render the screen. We render our UI on the callback thread.
static void basic_demo_render_callback(Canvas* canvas, void* ctx) {
    // Attempt to aquire context, so we can read the data.
    DemoContext* demo_context = ctx;
    if(furi_mutex_acquire(demo_context->mutex, 200) != FuriStatusOk) {
        return;
    }

    DemoData* data = demo_context->data;
    furi_string_printf(data->buffer, "Basic");
    furi_string_cat_printf(data->buffer, " demo");

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 45, 30, AlignLeft, AlignTop, furi_string_get_cstr(data->buffer));

    // Release the context, so other threads can update the data.
    furi_mutex_release(demo_context->mutex);
}

int32_t basic_demo_app(void* p) {
    UNUSED(p);

    // Configure our initial data.
    DemoContext* demo_context = malloc(sizeof(DemoContext));
    demo_context->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    demo_context->data = malloc(sizeof(DemoData));
    demo_context->data->buffer = furi_string_alloc();

    // Queue for events (tick or input)
    demo_context->queue = furi_message_queue_alloc(8, sizeof(DemoEvent));

    // Set ViewPort callbacks
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, basic_demo_render_callback, demo_context);
    view_port_input_callback_set(view_port, basic_demo_input_callback, demo_context->queue);

    // Open GUI and register view_port
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    // Main loop
    DemoEvent event;
    bool processing = true;
    do {
        if (furi_message_queue_get(demo_context->queue, &event, FuriWaitForever) == FuriStatusOk) {
            FURI_LOG_T(TAG, "Got event type: %d", event.type);
            switch (event.type) {
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
    } while (processing);

    // Free resources
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