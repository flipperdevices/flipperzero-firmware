#include <furi.h>
#include <gui/scene_manager.h>

#include "threads.h"
#include "constants.h"
#include "game_structs.h"
#include "gui/scenes.h"
#include "state_management.h"

void main_thread(struct ApplicationContext *context) {
    furi_thread_start(context->secondary_thread);

    FURI_LOG_D(LOG_TAG, "Main thread started");

    // Select the first scene to launch
    scene_manager_next_scene(context->scene_manager, scene_loading);
    FURI_LOG_D(LOG_TAG, "Starting dispatcher...");

    /* Blocking call until the back button is pressed from the main view */
    view_dispatcher_run(context->view_dispatcher);

    /* At this point the GUI is stopped */

    /* Signal the secondary thread to cease operation and exit */
    FURI_LOG_D(LOG_TAG, "Time to stop");
    struct ThreadsMessage threads_message = {.type = SAVE_AND_EXIT};
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

    // Now we are ready to open the main scene
    scene_manager_next_scene(context->scene_manager, scene_main);

    // Process actions the main thread requests to perform
    struct ThreadsMessage message;
    while(true) {
        // Block until a message is received
        FuriStatus status = furi_message_queue_get(context->threads_message_queue, &message, BACKGROUND_ACTIVITY_TICKS);
        if(status == FuriStatusOk) {
          // Received something
            switch(message.type) {
                case SAVE_AND_EXIT:
                    FURI_LOG_T(LOG_TAG, "Received termination message");
                    persist_state(context->game_state);
                    return 0;
                default:
                    furi_crash("Unexpected game event type");
            }
        } else if(status == FuriStatusErrorTimeout) {
            FURI_LOG_T(LOG_TAG, "Timeout in waiting for actions from main_thread, performing background activity");
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
            scene_manager_handle_tick_event(context->scene_manager);
        } else {
            furi_crash("Unexpected status in game event queue");
        }
    }
    return 0;
}
