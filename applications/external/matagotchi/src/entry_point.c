#include <gui/gui.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>
#include <gui/view.h>

#include <gui/modules/loading.h>
#include "gui/custom_modules/button_panel.h"
#include <gui/modules/variable_item_list.h>
#include <gui/modules/dialog_ex.h>
#include <gui/modules/text_box.h>
#include <gui/modules/popup.h>

#include <core/thread.h>
#include <core/message_queue.h>

#include <dolphin/dolphin.h>

#include "flipper_structs.h"
#include "game_structs.h"
#include "gui/scenes/scenes.h"
#include "gui/view_dispatcher.h"
#include "threads.h"

/******************** Initialisation & startup *****************************/

static void init_gui(struct ApplicationContext* context) {
    // Init SceneManager
    context->scene_manager = scene_manager_alloc(&scene_handlers, context);

    // Init modules
    context->loading_module = loading_alloc(); // Loading screen while reading persisted status
    context->button_module = button_panel_alloc(); // Home screen
    context->variable_item_list_module = variable_item_list_alloc(); // Settings page
    context->dialog_ex_module = dialog_ex_alloc(); // Reset screen
    context->text_box_module = text_box_alloc(); // About screen
    context->popup_module = popup_alloc(); // Status screen

    // Init ViewDispatcher
    context->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(context->view_dispatcher);
    view_dispatcher_set_event_callback_context(context->view_dispatcher, context);
    view_dispatcher_set_custom_event_callback(
        context->view_dispatcher, view_dispatcher_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        context->view_dispatcher, view_dispatcher_navigation_event_callback);

    // Attach modules to view_dispatcher
    view_dispatcher_add_view(
        context->view_dispatcher, scene_loading, loading_get_view(context->loading_module));
    view_dispatcher_add_view(
        context->view_dispatcher, scene_main, button_panel_get_view(context->button_module));
    view_set_orientation(button_panel_get_view(context->button_module), ViewOrientationHorizontal);
    view_dispatcher_add_view(
        context->view_dispatcher,
        scene_settings,
        variable_item_list_get_view(context->variable_item_list_module));
    view_dispatcher_add_view(
        context->view_dispatcher, scene_reset, dialog_ex_get_view(context->dialog_ex_module));
    view_dispatcher_add_view(
        context->view_dispatcher, scene_about, text_box_get_view(context->text_box_module));
    view_dispatcher_add_view(
        context->view_dispatcher, scene_status, popup_get_view(context->popup_module));
    view_dispatcher_add_view(
        context->view_dispatcher, scene_candy, popup_get_view(context->popup_module));
    view_dispatcher_add_view(
        context->view_dispatcher, scene_pill, popup_get_view(context->popup_module));

    // Init GUI and attach the view_dispatcher to it
    context->gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(
        context->view_dispatcher, context->gui, ViewDispatcherTypeFullscreen);
}

static void free_gui(struct ApplicationContext* context) {
    /* Free the view_dispatcher */
    view_dispatcher_remove_view(context->view_dispatcher, scene_pill);
    view_dispatcher_remove_view(context->view_dispatcher, scene_candy);
    view_dispatcher_remove_view(context->view_dispatcher, scene_status);
    view_dispatcher_remove_view(context->view_dispatcher, scene_about);
    view_dispatcher_remove_view(context->view_dispatcher, scene_reset);
    view_dispatcher_remove_view(context->view_dispatcher, scene_settings);
    view_dispatcher_remove_view(context->view_dispatcher, scene_main);
    view_dispatcher_remove_view(context->view_dispatcher, scene_loading);
    view_dispatcher_free(context->view_dispatcher);

    /* Free the modules */
    popup_free(context->popup_module);
    text_box_free(context->text_box_module);
    dialog_ex_free(context->dialog_ex_module);
    variable_item_list_free(context->variable_item_list_module);
    button_panel_free(context->button_module);
    loading_free(context->loading_module);

    /* Free the scene_manager */
    scene_manager_free(context->scene_manager);

    /* Lastly unregister the GUI */
    furi_record_close(RECORD_GUI);
}

/* Allocate the memory and initialise the variables */
static struct ApplicationContext* context_alloc() {
    struct ApplicationContext* context = malloc(sizeof(struct ApplicationContext));
    furi_assert(context);

    init_gui(context);

    // Allocate memory for queues
    context->threads_message_queue = furi_message_queue_alloc(8, sizeof(struct ThreadsMessage));

    // Create a secondary thread
    context->secondary_thread = furi_thread_alloc();
    furi_thread_set_stack_size(context->secondary_thread, 1024U);
    furi_thread_set_context(context->secondary_thread, context);
    furi_thread_set_callback(context->secondary_thread, secondary_thread);

    // Allocate memory for the game state
    context->game_state = malloc(sizeof(struct GameState));
    furi_assert(context->game_state);

    return context;
}

/* Release the unused resources and deallocate memory */
static void context_free(struct ApplicationContext* context) {
    // Free the game state
    free(context->game_state);

    // Free the secondary thread
    furi_thread_free(context->secondary_thread);

    // Free the queues
    furi_message_queue_free(context->threads_message_queue);

    free_gui(context);

    free(context);
}

/* The application's entry point. Execution starts from here. */
int32_t matagotchi_app(void* p) {
    UNUSED(p);

    /* Allocate all of the necessary structures */
    struct ApplicationContext* context = context_alloc();

    /* Perform required RTOS init sequences */
    furi_hal_random_init(); // Init of the RNG unit

    dolphin_deed(DolphinDeedPluginStart);

    /* Start the applicaton's main loop. It won't return until the application was requested to exit. */
    main_thread(context);

    /* Release all unneeded resources */
    context_free(context);

    return 0;
}
