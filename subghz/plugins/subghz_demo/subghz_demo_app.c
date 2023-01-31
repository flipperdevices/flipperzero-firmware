/*
@CodeAllNight
https://github.com/jamisonderek/flipper-zero-tutorials

This is a demonstration of sending radio signals using the subghz_tx_rx worker library.

Features:
Press OK on Flipper Zero to log the current count.
Short press UP button on Flipper Zero to log 440Hz tone.
Long press UP button on Flipper Zero to log 880Hz tone. 

*/

#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_resources.h>
#include <gui/gui.h>
#include <locale/locale.h>


#define TAG "subghz_demo_app"




// Messages in our event queue are one of the following types.
typedef enum {
    DemoEventTypeTick,
    DemoEventTypeKey,

    // You can add additional events here.
    DemoEventSendCounter,
    DemoEventSendTone,
} DemoEventType;

// An item in the event queue has both the type and its associated data.
// Some data fields are only set for particular types.
typedef struct {
    DemoEventType type; // The reason for this event.
    
    // You can add additional data that is helpful for your events.
    InputEvent input;   // This data is specific to DemoEventTypeKey.
    unsigned int number; // This data is specific to DemoEventSendCounter/DemoEventSendTone.
} DemoEvent;

// This is the data for our application.  You might have a game board, 
// current player, etc.  For this demo we have two counters & a general purpose buffer.
typedef struct {
    FuriString* buffer;

    // You can add additional application state here.
    unsigned int localCounter;
} DemoData;

// This is our application context.
typedef struct {
    FuriMessageQueue* queue; // Message queue (DemoEvent items to process).
    FuriMutex* mutex; // Used to provide thread safe access to data.
    DemoData* data; // Data accessed by multiple threads (acquire the mutex before accessing!)
} DemoContext;


// This gets invoked when input (button press) is detected.  
// We queue a DemoEventTypeKey message with the input event data.
static void subghz_demo_input_callback(InputEvent* input_event, void* ctx_q) {
    furi_assert(ctx_q);
    FuriMessageQueue* queue = ctx_q;
    DemoEvent event = {.type = DemoEventTypeKey, .input = *input_event};
    furi_message_queue_put(queue, &event, FuriWaitForever);
}

// We register this callback to get invoked by the timer on every tick.  
// We queue a DemoEventTypeTick message and then return to the caller.
static void subghz_demo_tick_callback(void* ctx_q) {
    furi_assert(ctx_q);
    FuriMessageQueue* queue = ctx_q;
    DemoEvent event = {.type = DemoEventTypeTick};
    // We don't pass a wait value for 3rd parameter -- this event is not critical (and will happen again soon).
    furi_message_queue_put(queue, &event, 0);
}

// Our DemoEventTypeKey handler invokes this method when user clicks OK button.
// We queue a DemoEventSendCounter message with the counter data.
static void subghz_demo_send_count(void* ctx) {
    furi_assert(ctx);
    DemoContext* demo_context = ctx;
    FuriMessageQueue* queue = demo_context->queue;
    DemoData* data = demo_context->data;
    unsigned int counter = data->localCounter;
    DemoEvent event = {.type = DemoEventSendCounter, .number = counter};
    furi_message_queue_put(queue, &event, FuriWaitForever);
}

// Our DemoEventTypeKey handler invokes this method when user clicks UP button.
// We queue a DemoEventSendTone message with the frequency data.
static void subghz_demo_send_tone(void* ctx, unsigned int frequency) {
    furi_assert(ctx);
    DemoContext* demo_context = ctx;
    DemoData* data = demo_context->data;
    FuriMessageQueue* queue = demo_context->queue;
    DemoEvent event = {.type = DemoEventSendTone, .number = frequency};
    furi_message_queue_put(queue, &event, FuriWaitForever);
}

// We register this callback to get invoked whenever we need to render the screen. 
// We render the UI on this callback thread.
static void subghz_demo_render_callback(Canvas* canvas, void* ctx) {
    furi_assert(ctx);
    DemoContext* demo_context = ctx;

    // Attempt to aquire context, so we can read the data.
    if(furi_mutex_acquire(demo_context->mutex, 200) != FuriStatusOk) {
        return;
    }

    DemoData* data = demo_context->data;
    unsigned int localCounter = data->localCounter;
    // The counter is supposed to be a 4 digit number.
    furi_assert(localCounter < 10000U);

    // Other fonts are FontPrimary, FontSecondary, FontKeyboard, FontBigNumbers,
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 5, 8, AlignLeft, AlignCenter, "Press OK to send count.");
    canvas_draw_str_aligned(canvas, 5, 20, AlignLeft, AlignCenter, "Press UP to send tone.");

    furi_string_printf(data->buffer, "%04u", localCounter);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 64, 42, AlignCenter, AlignTop, furi_string_get_cstr(data->buffer));


    furi_mutex_release(demo_context->mutex);
}

// Our DemoEventTypeTick handler invokes this method.
// We increment our counter (wrapping back to 0 if it exceeds a 4 digit number.)
static void subghz_demo_update_local_counter(DemoContext* demo_context) {
    DemoData* data = demo_context->data;
    
    // Increment the counter (which is supposed to be a 4 digit number for this app.)
    data->localCounter++;
    if (data->localCounter >= 10000U) {
        data->localCounter = 0;
    }

    FURI_LOG_T(TAG, "Local counter %04u", data->localCounter);
}


// Our DemoEventSendCounter handler invokes this method.
static void subghz_demo_broadcast_counter(DemoContext* demo_context, unsigned int counterToSend) {
    // The counter is supposed to be a 4 digit number.
    furi_assert(counterToSend < 10000U);
    DemoData* data = demo_context->data;

    FURI_LOG_I(TAG, "Sending counter %04u", counterToSend);
}

// Our DemoEventSendTone handler invokes this method.
static void subghz_demo_broadcast_tone(DemoContext* demo_context, unsigned int frequency) {
    DemoData* data = demo_context->data;

    FURI_LOG_I(TAG, "Sending frequency %04u", frequency);
}

// This is the entry point for our application, which should match the application.fam file.
int32_t subghz_demo_app(void* p) {
    UNUSED(p);



    // Configure our initial data.
    DemoContext* demo_context = malloc(sizeof(DemoContext));
    demo_context->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    demo_context->data = malloc(sizeof(DemoData));
    demo_context->data->buffer = furi_string_alloc();
    demo_context->data->localCounter = 0;

    // Queue for events
    demo_context->queue = furi_message_queue_alloc(8, sizeof(DemoEvent));


    // Set ViewPort callbacks
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, subghz_demo_render_callback, demo_context);
    view_port_input_callback_set(view_port, subghz_demo_input_callback, demo_context->queue);

    // Open GUI and register view_port
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    // Update the screen fairly frequently (every 250 milliseconds.)
    FuriTimer* timer = furi_timer_alloc(subghz_demo_tick_callback, FuriTimerTypePeriodic, demo_context->queue);
    furi_timer_start(timer, 250);

    // Main loop
    DemoEvent event;
    bool processing = true;
    do {
        if (furi_message_queue_get(demo_context->queue, &event, FuriWaitForever) == FuriStatusOk) {
            switch (event.type) {
                case DemoEventTypeKey:
                    // Short press of OK button, queue DemoEventSendCounter event with the current count.
                    if(event.input.type == InputTypeShort && event.input.key == InputKeyOk) {
                        furi_mutex_acquire(demo_context->mutex, FuriWaitForever);
                        subghz_demo_send_count(demo_context);
                        furi_mutex_release(demo_context->mutex);
                    }
                    // Short press of UP button, queue DemoEventSendTone event.
                    else if(event.input.type == InputTypeShort && event.input.key == InputKeyUp) {
                        furi_mutex_acquire(demo_context->mutex, FuriWaitForever);
                        subghz_demo_send_tone(demo_context, 440U);
                        furi_mutex_release(demo_context->mutex);
                    } 
                    // Long press of UP button, queue DemoEventSendTone event.
                    else if (event.input.type == InputTypeLong && event.input.key == InputKeyUp) {
                        furi_mutex_acquire(demo_context->mutex, FuriWaitForever);
                        subghz_demo_send_tone(demo_context, 880U);
                        furi_mutex_release(demo_context->mutex);
                    }
                    // Short press of back button exits the program.
                    else if(event.input.type == InputTypeShort && event.input.key == InputKeyBack) {
                        processing = false;
                    }
                    break;
                case DemoEventTypeTick:
                    // Every timer tick we update the counter. 
                    furi_mutex_acquire(demo_context->mutex, FuriWaitForever);
                    subghz_demo_update_local_counter(demo_context);
                    furi_mutex_release(demo_context->mutex);
                    break;
                case DemoEventSendCounter:
                    // Actually send the counter value to the other Flipper Zero.
                    furi_mutex_acquire(demo_context->mutex, FuriWaitForever);   
                    subghz_demo_broadcast_counter(demo_context, event.number);
                    furi_mutex_release(demo_context->mutex);
                    break;
                case DemoEventSendTone:
                    // Actually send the frequency value to the other Flipper Zero.
                    furi_mutex_acquire(demo_context->mutex, FuriWaitForever);   
                    subghz_demo_broadcast_tone(demo_context, event.number);
                    furi_mutex_release(demo_context->mutex);
                    break;
                default:
                    FURI_LOG_E(TAG, "Queue had unknown message type: %u", event.type);
                    break;
            }

            // Send signal to update the screen (callback will get invoked at some point later.)
            view_port_update(view_port);
        } else {
            // We had an issue getting message from the queue, so exit application.
            FURI_LOG_E(TAG, "Issue encountered reading from queue.  Exiting application.");
            processing = false;
        }
    } while (processing);

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