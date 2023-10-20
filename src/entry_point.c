#include <gui/gui.h>
#include <gui/view_port.h>
#include <core/thread.h>
#include <core/message_queue.h>

#include <dolphin/dolphin.h>

#include "structs.h"
#include "main_callbacks.h"

/* Starts the reader thread and handles the input */
static void main_runner(struct ApplicationContext *context) {
    /* Start the reader thread. It will talk to the thermometer in the background. */
    furi_thread_start(context->secondary_thread);

    /* Signal the reader thread to cease operation and exit */
    furi_thread_flags_set(furi_thread_get_id(context->secondary_thread), 0xfe);

    /* Wait for the reader thread to finish */
    furi_thread_join(context->secondary_thread);
}

/******************** Initialisation & startup *****************************/

/* Allocate the memory and initialise the variables */
static struct ApplicationContext * context_alloc() {
    struct ApplicationContext *context = malloc(sizeof(struct ApplicationContext));

    context->view_port = view_port_alloc();
    view_port_draw_callback_set(context->view_port, main_draw_callback, context);
    view_port_input_callback_set(context->view_port, main_input_callback, context);

    context->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    context->secondary_thread = furi_thread_alloc();
    furi_thread_set_stack_size(context->secondary_thread, 1024U);
    furi_thread_set_context(context->secondary_thread, context);
    furi_thread_set_callback(context->secondary_thread, main_thread_callback);

    context->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(context->gui, context->view_port, GuiLayerFullscreen);

    return context;
}

/* Release the unused resources and deallocate memory */
static void context_free(struct ApplicationContext *context) {
    view_port_enabled_set(context->view_port, false);
    gui_remove_view_port(context->gui, context->view_port);

    furi_thread_free(context->secondary_thread);
    furi_message_queue_free(context->event_queue);
    view_port_free(context->view_port);

    furi_record_close(RECORD_GUI);
}

/* The application's entry point. Execution starts from here. */
int32_t matagotchi_app(void* p) {
    UNUSED(p);

    /* Allocate all of the necessary structures */
    struct ApplicationContext *context = context_alloc();

		dolphin_deed(DolphinDeedPluginStart);

    /* Start the applicaton's main loop. It won't return until the application was requested to exit. */
    main_runner(context);

    /* Release all unneeded resources */
    context_free(context);

    return 0;
}
