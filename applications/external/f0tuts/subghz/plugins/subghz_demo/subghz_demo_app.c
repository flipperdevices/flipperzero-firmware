/*
@CodeAllNight
https://github.com/jamisonderek/flipper-zero-tutorials

This is a demonstration of sending radio signals using the subghz_tx_rx worker library.

Features:
Press OK on Flipper Zero to log and send the current count to another Flipper Zero.
Short press UP button on Flipper Zero to log and send 440Hz tone to another Flipper Zero.
Long press UP button on Flipper Zero to log and send 880Hz tone to another Flipper Zero.

*/

#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_resources.h>
#include <gui/gui.h>
#include <locale/locale.h>

#include <lib/subghz/subghz_tx_rx_worker.h>
#include <lib/subghz/devices/cc1101_int/cc1101_int_interconnect.h>

// This is sent at the beginning of all RF messages.
// You should make the SUBGHZ_GAME_NAME short but unique.
// NOTE: It must end with the ':' character.
#define SUBGHZ_GAME_NAME "SGDEMO:"
#define TAG "subghz_demo_app"

// The message max length should be no larger than a value around 60 to 64.
#define MESSAGE_MAX_LEN 60

// The major version must be a single character (it can be anything - like '1' or 'A' or 'a').
#define MAJOR_VERSION 'A'

// When an RF message is sent, it includes a purpose so the receiving application
// can decide if it should process the message.
typedef enum {
    DemoRfPurposeCounter = 'C', // The message is about the counter.
    DemoRfPurposeTone = 'T', // The message is about a note to play.
} DemoRfPurpose;

// Messages in our event queue are one of the following types.
typedef enum {
    DemoEventTypeTick,
    DemoEventTypeKey,
    DemoEventDataDetected,

    // You can add additional events here.
    DemoEventSendCounter,
    DemoEventReceivedCounter,
    DemoEventSendTone,
    DemoEventReceivedTone,
} DemoEventType;

// An item in the event queue has both the type and its associated data.
// Some data fields are only set for particular types.
typedef struct {
    DemoEventType type; // The reason for this event.

    // You can add additional data that is helpful for your events.
    // Some parameters are only used by some events.
    InputEvent input;
    unsigned int number;
    FuriString* senderName;
} DemoEvent;

// This is the data for our application.  You might have a game board,
// current player, etc.  For this demo we have two counters & a general purpose buffer.
typedef struct {
    FuriString* buffer;

    // You can add additional application state here.
    unsigned int localCounter;
    unsigned int remoteCounter;
} DemoData;

// This is our application context.
typedef struct {
    FuriMessageQueue* queue; // Message queue (DemoEvent items to process).
    FuriMutex* mutex; // Used to provide thread safe access to data.
    DemoData* data; // Data accessed by multiple threads (acquire the mutex before accessing!)

    // Used for subghz communication
    SubGhzTxRxWorker* subghz_txrx;
} DemoContext;

// We register this callback to get invoked whenever new subghz data is received.
// We queue a DemoEventDataDetected message and then return to the caller.
static void subghz_demo_worker_update_rx_event_callback(void* ctx) {
    furi_assert(ctx);
    DemoContext* demo_context = ctx;
    DemoEvent event = {.type = DemoEventDataDetected};
    furi_message_queue_put(demo_context->queue, &event, FuriWaitForever);
}

// This gets invoked when we process a DemoEventDataDetected event.
// We read the message using subghz_tx_rx_worker_read.
// We determine if the message is in the valid format.
// If valid, we queue a DemoEventReceivedCounter/Tone message with the counter/frequency.
// IMPORTANT: The code processing our event needs to furi_string_free the senderName!
static void subghz_demo_receive_data(DemoContext* instance) {
    uint8_t message[MESSAGE_MAX_LEN] = {0};
    memset(message, 0x00, MESSAGE_MAX_LEN);
    size_t len = subghz_tx_rx_worker_read(instance->subghz_txrx, message, MESSAGE_MAX_LEN);
    size_t game_name_len = strlen(SUBGHZ_GAME_NAME);
    if(len < (game_name_len + 2)) {
        FURI_LOG_D(TAG, "Message not long enough. >%s<", message);

        // Message wasn't big enough to have our game name + the reason code + version; so it must not be for us.
        return;
    }

    // The message for a counter (C) (like 42) using version (A) should be "SGDEMO:" + "C" + "A" + "0042" + ":" + "YourFlip" + "\r\n"
    if(strcmp(SUBGHZ_GAME_NAME, (const char*)message)) {
        FURI_LOG_D(TAG, "Got message >%s<", message);
        // The purpose immediately follows the game name.
        DemoRfPurpose purpose = message[game_name_len];
        uint8_t version = message[game_name_len + 1];
        FURI_LOG_T(TAG, "Purpose is %c and Version is %c", purpose, version);

        // Right now we don't care much about the version of the application, but in the future we might need to
        // respond differently based on the version of the application running on the other Flipper Zero.
        // Important: Don't always trust what is sent, some people with Flipper Zero might send an
        // invalid version to trick your code into interpreting the payload in a special way.

        // Null terminate the buffer at the end of message so we don't accidently overrun our buffer.
        message[MESSAGE_MAX_LEN - 1] = 0;

        unsigned int number;
        char senderName[9];
        switch(purpose) {
        case DemoRfPurposeCounter:
            // We expect this mesage to contain both the count and the sender name.
            if(sscanf((const char*)message + game_name_len + 2, "%04u:%8s", &number, senderName) ==
               2) {
                // IMPORTANT: The code processing the event needs to furi_string_free the senderName!
                FuriString* name = furi_string_alloc();
                furi_string_set(name, senderName);
                // The counter is supposed to be a 4 digit number.
                if(number >= 10000U) {
                    FURI_LOG_W(TAG, "Number was >= 10000U.  >%s<", message);
                    number %= 10000U;
                }
                DemoEvent event = {
                    .type = DemoEventReceivedCounter, .number = number, .senderName = name};
                furi_message_queue_put(instance->queue, &event, FuriWaitForever);
            } else {
                FURI_LOG_W(TAG, "Failed to parse counter message. >%s<", message);
            }
            break;

        case DemoRfPurposeTone:
            // We expect this message to contain both the frequency and the sender name.
            if(sscanf((const char*)message + game_name_len + 2, "%u:%8s", &number, senderName) ==
               2) {
                // IMPORTANT: The code processing the event needs to furi_string_free the senderName!
                FuriString* name = furi_string_alloc();
                furi_string_set(name, senderName);
                DemoEvent event = {
                    .type = DemoEventReceivedTone, .number = number, .senderName = name};
                furi_message_queue_put(instance->queue, &event, FuriWaitForever);
            } else {
                FURI_LOG_W(TAG, "Failed to parse tone message. >%s<", message);
            }
            break;

            // Add parsing for other messages here.

        default:
            if(version <= MAJOR_VERSION) {
                // The version is same or less than ours, so we should know about the message purpose.
                FURI_LOG_W(TAG, "Message purpose not handled for known version. >%s<", message);
            } else {
                // The version is newer, so it's not surprising we don't know about the purpose.
                FURI_LOG_T(TAG, "Message purpose not handled. >%s<", message);
            }
            break;
        }
    } else {
        FURI_LOG_D(TAG, "Message not for our application. >%s<", message);
    }
}

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
    unsigned int remoteCounter = data->remoteCounter;
    // The counter is supposed to be a 4 digit number.
    furi_assert(localCounter < 10000U);
    furi_assert(remoteCounter <= 10000U); // 10000 means don't display.

    // Other fonts are FontPrimary, FontSecondary, FontKeyboard, FontBigNumbers,
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 5, 8, AlignLeft, AlignCenter, "Press OK to send count.");
    canvas_draw_str_aligned(canvas, 5, 20, AlignLeft, AlignCenter, "Press UP to send tone.");

    furi_string_printf(data->buffer, "%04u", localCounter);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(
        canvas, 64, 42, AlignCenter, AlignTop, furi_string_get_cstr(data->buffer));

    if(remoteCounter < 10000U) {
        furi_string_printf(data->buffer, "Received %04u", remoteCounter);
        canvas_draw_str_aligned(
            canvas, 64, 52, AlignCenter, AlignTop, furi_string_get_cstr(data->buffer));
    }

    furi_mutex_release(demo_context->mutex);
}

// Our DemoEventTypeTick handler invokes this method.
// We increment our counter (wrapping back to 0 if it exceeds a 4 digit number.)
static void subghz_demo_update_local_counter(DemoContext* demo_context) {
    DemoData* data = demo_context->data;

    // Increment the counter (which is supposed to be a 4 digit number for this app.)
    data->localCounter++;
    if(data->localCounter >= 10000U) {
        data->localCounter = 0;
    }

    FURI_LOG_T(TAG, "Local counter %04u", data->localCounter);
}

// Our DemoEventReceivedCounter handler invokes this method.
// We update our remote counter.
static void subghz_demo_update_remote_counter(DemoContext* demo_context, DemoEvent* event) {
    // The queueing code should have made sure the value was valid.
    furi_assert(event->number < 10000U);
    DemoData* data = demo_context->data;

    data->remoteCounter = event->number;
    FURI_LOG_I(TAG, "Remote counter %04u", data->remoteCounter);
}

// Our DemoEventReceivedTone handler invokes this method.
// We play a quick (100ms) tone of the desired frequency.
static void subghz_demo_play_tone(DemoContext* demo_context, DemoEvent* event) {
    UNUSED(demo_context);
    unsigned int frequency = event->number;
    FURI_LOG_I(TAG, "Playing frequency %04u", frequency);

    // Make tones if the speaker is available.
    if(furi_hal_speaker_acquire(1000)) { // We wait up to a second for now, is that too long?
        float freq = (float)frequency;
        float volume = 1.0f; // 100% volume.
        furi_hal_speaker_start(freq, volume);
        furi_delay_ms(100);
        furi_hal_speaker_stop();
        furi_hal_speaker_release();
    }
}

// This is a helper method that broadcasts a buffer.
// If the message is too large, the message will get truncated.
static void subghz_demo_broadcast(DemoContext* demo_context, FuriString* buffer) {
    uint8_t* message = (uint8_t*)furi_string_get_cstr(buffer);

    // Make sure our message will fit into a packet; if not truncate it.
    size_t length = strlen((char*)message);
    if(length > MESSAGE_MAX_LEN) {
        FURI_LOG_E(
            TAG, "Outgoing message bigger than %d bytes! >%s<", MESSAGE_MAX_LEN, (char*)message);

        // Add \r\n(null) to the end of the 0-indexed string.
        message[MESSAGE_MAX_LEN - 1] = 0;
        message[MESSAGE_MAX_LEN - 2] = '\n';
        message[MESSAGE_MAX_LEN - 3] = '\r';
        length = MESSAGE_MAX_LEN;
    }

    while(!subghz_tx_rx_worker_write(demo_context->subghz_txrx, message, length)) {
        // Wait a few milliseconds on failure before trying to send again.
        furi_delay_ms(20);
    }
}

// Our DemoEventSendCounter handler invokes this method.
// We broadcast - "game name + purpose (Counter) + Version (A) + 4 digit counter value + : + Flipper name + \r\n"
static void subghz_demo_broadcast_counter(DemoContext* demo_context, unsigned int counterToSend) {
    // The counter is supposed to be a 4 digit number.
    furi_assert(counterToSend < 10000U);
    DemoData* data = demo_context->data;

    FURI_LOG_I(TAG, "Sending counter %04u", counterToSend);

    // The message for a counter with value 42 should look like...  "SGDEMO:CA0042:YourFlip\r\n"
    furi_string_printf(
        data->buffer,
        "%s%c%c%04u:%s\r\n",
        SUBGHZ_GAME_NAME,
        DemoRfPurposeCounter,
        MAJOR_VERSION,
        counterToSend,
        furi_hal_version_get_name_ptr());

    subghz_demo_broadcast(demo_context, data->buffer);
}

// Our DemoEventSendTone handler invokes this method.
// We broadcast - "game name + purpose (Tone) + Version (A) + frequency + : + Flipper name + \r\n"
static void subghz_demo_broadcast_tone(DemoContext* demo_context, unsigned int frequency) {
    DemoData* data = demo_context->data;

    FURI_LOG_I(TAG, "Sending frequency %04u", frequency);

    // The message for a frequency of 440 should look like...  "SGDEMO:TA440:YourFlip\r\n"
    furi_string_printf(
        data->buffer,
        "%s%c%c%u:%s\r\n",
        SUBGHZ_GAME_NAME,
        DemoRfPurposeTone,
        MAJOR_VERSION,
        frequency,
        furi_hal_version_get_name_ptr());

    subghz_demo_broadcast(demo_context, data->buffer);
}

// This is the entry point for our application, which should match the application.fam file.
int32_t subghz_demo_app(void* p) {
    UNUSED(p);

    // For this demo we hardcode to 433.92MHz.
    uint32_t frequency = 433920000;

    // TODO: Have an ordered list of frequencies we try, instead of just 1 frequency.

    // Configure our initial data.
    DemoContext* demo_context = malloc(sizeof(DemoContext));
    demo_context->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    demo_context->data = malloc(sizeof(DemoData));
    demo_context->data->buffer = furi_string_alloc();
    demo_context->data->localCounter = 0;
    demo_context->data->remoteCounter = 10000U; // Won't display, since larger than 4-digit number.

    // Queue for events
    demo_context->queue = furi_message_queue_alloc(8, sizeof(DemoEvent));

    // Subghz worker.
    demo_context->subghz_txrx = subghz_tx_rx_worker_alloc();
    subghz_devices_init();

    // Try to start the TX/RX on the frequency and configure our callback
    // whenever new data is received.
    const SubGhzDevice* device =
        subghz_devices_get_by_name(SUBGHZ_DEVICE_CC1101_INT_NAME); // Use internal CC1101 radio
    furi_assert(device);
    if(subghz_tx_rx_worker_start(demo_context->subghz_txrx, device, frequency)) {
        subghz_tx_rx_worker_set_callback_have_read(
            demo_context->subghz_txrx, subghz_demo_worker_update_rx_event_callback, demo_context);
    } else {
        FURI_LOG_E(TAG, "Failed to start subghz_tx_rx_worker.");

        // For this demo we don't show a friendly error about not being
        // allowed to broadcast on this frequency.  Instead the application
        // just exits.
        if(subghz_tx_rx_worker_is_running(demo_context->subghz_txrx)) {
            subghz_tx_rx_worker_stop(demo_context->subghz_txrx);
        }
        subghz_tx_rx_worker_free(demo_context->subghz_txrx);
        subghz_devices_deinit();
        furi_message_queue_free(demo_context->queue);
        furi_mutex_free(demo_context->mutex);
        furi_string_free(demo_context->data->buffer);
        free(demo_context->data);
        free(demo_context);
        return 2;
    }

    // All the subghz CLI apps disable charging; so our demo does it too.
    furi_hal_power_suppress_charge_enter();

    // Set ViewPort callbacks
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, subghz_demo_render_callback, demo_context);
    view_port_input_callback_set(view_port, subghz_demo_input_callback, demo_context->queue);

    // Open GUI and register view_port
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    // Update the screen fairly frequently (every 250 milliseconds.)
    FuriTimer* timer =
        furi_timer_alloc(subghz_demo_tick_callback, FuriTimerTypePeriodic, demo_context->queue);
    furi_timer_start(timer, 250);

    // Main loop
    DemoEvent event;
    bool processing = true;
    do {
        if(furi_message_queue_get(demo_context->queue, &event, FuriWaitForever) == FuriStatusOk) {
            switch(event.type) {
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
                else if(event.input.type == InputTypeLong && event.input.key == InputKeyUp) {
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
            case DemoEventDataDetected:
                // Another Flipper sent us data!  Process it, potentially queuing an event.
                subghz_demo_receive_data(demo_context);
                break;
            case DemoEventReceivedCounter:
                // Process the counter sent by the other Flipper Zero.
                furi_mutex_acquire(demo_context->mutex, FuriWaitForever);
                subghz_demo_update_remote_counter(demo_context, &event);
                furi_mutex_release(demo_context->mutex);
                break;
            case DemoEventReceivedTone:
                // Process the tone sent by the other Flipper Zero.
                furi_mutex_acquire(demo_context->mutex, FuriWaitForever);
                subghz_demo_play_tone(demo_context, &event);
                furi_mutex_release(demo_context->mutex);
                break;
            default:
                FURI_LOG_E(TAG, "Queue had unknown message type: %u", event.type);
                break;
            }

            // If message contains a sender name furi_string, free it.
            if(event.senderName) {
                furi_string_free(event.senderName);
            }

            // Send signal to update the screen (callback will get invoked at some point later.)
            view_port_update(view_port);
        } else {
            // We had an issue getting message from the queue, so exit application.
            FURI_LOG_E(TAG, "Issue encountered reading from queue.  Exiting application.");
            processing = false;
        }
    } while(processing);

    // Free resources
    furi_timer_free(timer);
    if(subghz_tx_rx_worker_is_running(demo_context->subghz_txrx)) {
        subghz_tx_rx_worker_stop(demo_context->subghz_txrx);
    }
    subghz_tx_rx_worker_free(demo_context->subghz_txrx);
    subghz_devices_deinit();
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);
    furi_message_queue_free(demo_context->queue);
    furi_mutex_free(demo_context->mutex);
    furi_string_free(demo_context->data->buffer);
    free(demo_context->data);
    free(demo_context);

    // Reenable charging.
    furi_hal_power_suppress_charge_exit();

    return 0;
}