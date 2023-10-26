#include <furi.h>

#include "threads.h"
#include "constants.h"
#include "game_structs.h"
#include "gui.h"
#include "state_management.h"

void main_thread(struct ApplicationContext *context) {
    furi_thread_start(context->secondary_thread);

    bool running = true;
    InputEvent input_event;
    struct ThreadsMessage threads_message;

    FURI_LOG_D(LOG_TAG, "Main thread started");

    while(running) {
        FuriStatus status = furi_message_queue_get(context->user_input_queue, &input_event, USER_INPUT_IDLE_TICKS);
        if(status == FuriStatusOk) {
            // Received something
            if(input_event.type == InputTypeShort) {
                switch(input_event.key) {
                    case InputKeyLeft:
                    case InputKeyRight:
                    case InputKeyUp:
                    case InputKeyDown:
                    case InputKeyOk:
                    case InputKeyBack:
                        threads_message.type = BUTTON_PRESSED;
                        threads_message.key = input_event.key;
                        FURI_LOG_T(LOG_TAG, "User input received: %u", input_event.key);
                        furi_message_queue_put(context->threads_message_queue, &threads_message, FuriWaitForever);
                        break;
                    default:
                        furi_crash("Unexpected input value");
                }
            } else if (input_event.type == InputTypeLong) {
                switch(input_event.key) {
                    case InputKeyBack:
                        running = false;
                        break;
                    default:
                        // Not handled
                        break;
                }
            }
        } else if(status == FuriStatusErrorTimeout) {
            // No user input, perform background operations
            threads_message.type = IDLE_TIMEOUT;
            FURI_LOG_T(LOG_TAG, "Timeout waiting for user input");
            furi_message_queue_put(context->threads_message_queue, &threads_message, FuriWaitForever);
        } else {
            furi_crash("Unexpected status in message queue");
        }
    }

    /* Signal the secondary thread to cease operation and exit */
    FURI_LOG_D(LOG_TAG, "Time to stop");
    threads_message.type = SAVE_AND_EXIT;
    furi_message_queue_put(context->threads_message_queue, &threads_message, FuriWaitForever);

    /* Wait for the secondary thread to finish */
    furi_thread_join(context->secondary_thread);
    FURI_LOG_D(LOG_TAG, "Ciao!");
}

int32_t secondary_thread(void *ctx)
{
    struct ApplicationContext *context = (struct ApplicationContext *)ctx;

    FURI_LOG_D(LOG_TAG, "Secondary thread started");

    // Start by initializing everything
    init_state(context->game_state);
    refresh_gui(context->view_port);

    // Process actions the main thread requests to perform
    struct ThreadsMessage message;
    while(true) {
        // Block until a message is received
        FuriStatus status = furi_message_queue_get(context->threads_message_queue, &message, FuriWaitForever);
        if(status == FuriStatusOk) {
          // Received something
            switch(message.type) {
                case IDLE_TIMEOUT:
                    FURI_LOG_T(LOG_TAG, "Received timeout message");
                    struct GameEvents events = { 0 };
                    generate_new_random_events(context->game_state, &events);
                    if (process_events(context->game_state, events)) {
                        // There was an update in the game state,
                        // reset the animation
                        context->game_state->next_animation_index = 0;
                    } else {
                        // Continue with the background animation
                        context->game_state->next_animation_index++;
                    }
                    refresh_gui(context->view_port);
                    break;
                case SAVE_AND_EXIT:
                    FURI_LOG_T(LOG_TAG, "Received termination message");
                    persist_state(context->game_state);
                    return 0;
                case BUTTON_PRESSED:
                    FURI_LOG_T(LOG_TAG, "Received input message");
                    // Nothing yet
                    break;
                default:
                    furi_crash("Unexpected game event type");
            }
        } else {
            furi_crash("Unexpected status in game event queue");
        }
    }
    return 0;
}

/***** Draw and Input callbacks run on dedicated threads *****/

// Screen is 128x64 px
void main_draw_callback(Canvas *canvas, void *ctx)
{
    struct ApplicationContext *context = (struct ApplicationContext *)ctx;

    canvas_clear(canvas);
    decode_and_draw_image(canvas, context->game_state);
}

void main_input_callback(InputEvent *event, void *ctx)
{
    furi_assert(ctx);

    struct ApplicationContext *context = (struct ApplicationContext *)ctx;
    furi_message_queue_put(context->user_input_queue, event, FuriWaitForever);
}
