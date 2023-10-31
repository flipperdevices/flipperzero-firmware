#include <furi.h>
#include "evil_portal_app_i.h"
#include "helpers/evil_portal_storage.h"

static bool evil_portal_app_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    Evil_PortalApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool evil_portal_app_back_event_callback(void* context) {
    furi_assert(context);
    Evil_PortalApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static void evil_portal_app_tick_event_callback(void* context) {
    furi_assert(context);
    Evil_PortalApp* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

Evil_PortalApp* evil_portal_app_alloc() {
    Evil_PortalApp* app = malloc(sizeof(Evil_PortalApp));

    app->sent_html = false;
    app->sent_ap = false;
    app->sent_reset = false;
    app->has_command_queue = false;
    app->command_index = 0;
    app->portal_logs = furi_string_alloc();

    app->dialogs = furi_record_open(RECORD_DIALOGS);
    app->file_path = furi_string_alloc();

    app->gui = furi_record_open(RECORD_GUI);
    app->storage = furi_record_open(RECORD_STORAGE);

    app->view_dispatcher = view_dispatcher_alloc();

    app->loading = loading_alloc();

    app->scene_manager = scene_manager_alloc(&evil_portal_scene_handlers, app);
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);

    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, evil_portal_app_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, evil_portal_app_back_event_callback);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, evil_portal_app_tick_event_callback, 100);

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    app->view_stack = view_stack_alloc();

    app->var_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        Evil_PortalAppViewVarItemList,
        variable_item_list_get_view(app->var_item_list));

    app->text_input = text_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, Evil_PortalAppViewTextInput, text_input_get_view(app->text_input));

    for(int i = 0; i < NUM_MENU_ITEMS; ++i) {
        app->selected_option_index[i] = 0;
    }

    app->text_box = text_box_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, Evil_PortalAppViewConsoleOutput, text_box_get_view(app->text_box));
    app->text_box_store = furi_string_alloc();
    furi_string_reserve(app->text_box_store, EVIL_PORTAL_TEXT_BOX_STORE_SIZE);

    scene_manager_next_scene(app->scene_manager, Evil_PortalSceneStart);

    return app;
}

void evil_portal_app_free(Evil_PortalApp* app) {
    // save latest logs
    if(furi_string_utf8_length(app->portal_logs) > 0) {
        write_logs(app->storage, app->portal_logs);
        furi_string_free(app->portal_logs);
    }

    // Send reset event to dev board
    evil_portal_uart_tx((uint8_t*)(RESET_CMD), strlen(RESET_CMD));
    evil_portal_uart_tx((uint8_t*)("\n"), 1);

    furi_assert(app);

    // Views
    view_dispatcher_remove_view(app->view_dispatcher, Evil_PortalAppViewVarItemList);
    view_dispatcher_remove_view(app->view_dispatcher, Evil_PortalAppViewConsoleOutput);

    text_box_free(app->text_box);
    furi_string_free(app->text_box_store);
    text_input_free(app->text_input);

    view_stack_free(app->view_stack);
    loading_free(app->loading);

    // View dispatcher
    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);

    evil_portal_uart_free(app->uart);

    // Close records
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_STORAGE);

    furi_record_close(RECORD_DIALOGS);
    furi_string_free(app->file_path);

    free(app);
}

int32_t evil_portal_app(void* p) {
    UNUSED(p);
    Evil_PortalApp* evil_portal_app = evil_portal_app_alloc();

    evil_portal_app->uart = evil_portal_uart_init(evil_portal_app);

    view_dispatcher_run(evil_portal_app->view_dispatcher);

    evil_portal_app_free(evil_portal_app);

    return 0;
}
