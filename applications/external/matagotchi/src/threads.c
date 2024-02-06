#include <furi.h>

#include "threads.h"
#include "constants.h"
#include "game_structs.h"
#include "settings_management.h"
#include "state_management.h"
#include "gui/utils.h"

void main_thread(struct ApplicationContext* context) {
    furi_thread_start(context->secondary_thread);

    FURI_LOG_D(LOG_TAG, "Main thread started");

    start_gui_and_block(context); // Blocking call until the GUI is terminated

    /* At this point the GUI is stopped */

    /* Signal the secondary thread to cease operation and exit */
    FURI_LOG_D(LOG_TAG, "Time to stop");
    struct ThreadsMessage threads_message = {.type = SAVE_AND_EXIT};
    furi_message_queue_put(context->threads_message_queue, &threads_message, FuriWaitForever);

    /* Wait for the secondary thread to finish */
    furi_thread_join(context->secondary_thread);
    FURI_LOG_D(LOG_TAG, "Ciao!");
}

int32_t secondary_thread(void* ctx) {
    struct ApplicationContext* context = (struct ApplicationContext*)ctx;

    FURI_LOG_D(LOG_TAG, "Secondary thread started");

    // Start by initializing everything
    init_settings(context->game_state);
    init_state(context->game_state);

    // Now we are ready to open the main scene
    switch_to_main_scene(context);

    // Process actions the main thread requests to perform
    struct ThreadsMessage message;
    while(true) {
        // Block until a message is received
        FuriStatus status = furi_message_queue_get(
            context->threads_message_queue, &message, BACKGROUND_ACTIVITY_TICKS);
        if(status == FuriStatusOk) {
            // Received something
            struct GameEvents events = {0};
            switch(message.type) {
            case SAVE_AND_EXIT:
                FURI_LOG_T(LOG_TAG, "Received termination message");
                persist_settings(context->game_state);
                persist_state(context->game_state);
                return 0;
            case RESET_STATE:
                FURI_LOG_T(LOG_TAG, "Received reset state request");
                reset_state(context->game_state);
                context->game_state->next_animation_index = 0;
                go_back_to_main_scene(context);
                break;
            case PROCESS_CANDY:
                FURI_LOG_T(LOG_TAG, "Received candy request");
                give_candy(context->game_state, &events);
                if(process_events(context->game_state, events)) {
                    context->game_state->next_animation_index = 0;
                    play_action(context->game_state);
                    vibrate_short(context->game_state);
                    go_to_candy_animation(context);
                }
                break;
            case PROCESS_PILL:
                FURI_LOG_T(LOG_TAG, "Received pill request");
                give_pill(context->game_state, &events);
                if(process_events(context->game_state, events)) {
                    context->game_state->next_animation_index = 0;
                    play_action(context->game_state);
                    vibrate_short(context->game_state);
                    go_to_pill_animation(context);
                }
                break;
            default:
                furi_crash("Unexpected game event type");
            }
        } else if(status == FuriStatusErrorTimeout) {
            FURI_LOG_T(
                LOG_TAG,
                "Timeout in waiting for actions from main_thread, performing background activity");
            struct GameEvents events = {0};
            generate_new_random_events(context->game_state, &events);
            if(process_events(context->game_state, events)) {
                // There was an update in the game state,
                // reset the animation
                context->game_state->next_animation_index = 0;
            } else {
                // Continue with the background animation
                context->game_state->next_animation_index++;
            }
            send_tick_to_scene(context);
        } else {
            furi_crash("Unexpected status in game event queue");
        }
    }
    return 0;
}
