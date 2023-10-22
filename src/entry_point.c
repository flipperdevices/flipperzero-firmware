#include <gui/gui.h>
#include <gui/view_port.h>
#include <core/thread.h>
#include <core/message_queue.h>

#include <dolphin/dolphin.h>

#include "flipper_structs.h"
#include "game_structs.h"
#include "threads.h"

/******************** Initialisation & startup *****************************/

/* Allocate the memory and initialise the variables */
static struct ApplicationContext * context_alloc() {
    struct ApplicationContext *context = malloc(sizeof(struct ApplicationContext));

    // Configure view_port
    context->view_port = view_port_alloc();
    view_port_draw_callback_set(context->view_port, main_draw_callback, context);
    view_port_input_callback_set(context->view_port, main_input_callback, context);

    // Register view_port in GUI
    context->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(context->gui, context->view_port, GuiLayerFullscreen);

    // Allocate memory for queues
    context->user_input_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    context->threads_message_queue = furi_message_queue_alloc(8, sizeof(struct ThreadsMessage));

    // Create a secondary thread
    context->secondary_thread = furi_thread_alloc();
    furi_thread_set_stack_size(context->secondary_thread, 1024U);
    furi_thread_set_context(context->secondary_thread, context);
    furi_thread_set_callback(context->secondary_thread, secondary_thread);

    // Allocate memory for the game state
    context->game_state = malloc(sizeof(struct GameState));

    return context;
}

/* Release the unused resources and deallocate memory */
static void context_free(struct ApplicationContext *context) {
    // Free the game state
    free(context->game_state);

    // Free the secondary thread
    furi_thread_free(context->secondary_thread);

    // Free the queues
    furi_message_queue_free(context->threads_message_queue);
    furi_message_queue_free(context->user_input_queue);

    // Unregister and free view_port
    view_port_enabled_set(context->view_port, false);
    gui_remove_view_port(context->gui, context->view_port);
    view_port_free(context->view_port);
    furi_record_close(RECORD_GUI);

    free(context);
}

/* The application's entry point. Execution starts from here. */
int32_t matagotchi_app(void* p) {
    UNUSED(p);

    /* Allocate all of the necessary structures */
    struct ApplicationContext *context = context_alloc();

    /* Perform required RTOS init sequences */
    furi_hal_random_init();

    dolphin_deed(DolphinDeedPluginStart);

    /* Start the applicaton's main loop. It won't return until the application was requested to exit. */
    main_thread(context);

    /* Release all unneeded resources */
    context_free(context);

    return 0;
}
