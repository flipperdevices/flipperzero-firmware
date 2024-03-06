#include "Pi_Terminal_app_i.h"

#include <furi.h>
#include <furi_hal.h>
#include <expansion/expansion.h>

static bool Pi_Terminal_app_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    Pi_TerminalApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool Pi_Terminal_app_back_event_callback(void* context) {
    furi_assert(context);
    Pi_TerminalApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static void Pi_Terminal_app_tick_event_callback(void* context) {
    furi_assert(context);
    Pi_TerminalApp* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

Pi_TerminalApp* Pi_Terminal_app_alloc() {
    Pi_TerminalApp* app = malloc(sizeof(Pi_TerminalApp));

    app->gui = furi_record_open(RECORD_GUI);

    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager = scene_manager_alloc(&Pi_Terminal_scene_handlers, app);
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);

    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, Pi_Terminal_app_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, Pi_Terminal_app_back_event_callback);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, Pi_Terminal_app_tick_event_callback, 100);

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    app->var_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        Pi_TerminalAppViewVarItemList,
        variable_item_list_get_view(app->var_item_list));

    for(int i = 0; i < START_MENU_ITEMS; ++i) {
        app->selected_option_index[i] = 0;
    }

    app->setup_var_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        Pi_TerminalAppViewSetup,
        variable_item_list_get_view(app->setup_var_item_list));

    for(int i = 0; i < SETUP_MENU_ITEMS; ++i) {
        app->setup_selected_option_index[i] = 0;
    }

    app->widget = widget_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, Pi_TerminalAppViewHelp, widget_get_view(app->widget));

    app->text_box = text_box_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, Pi_TerminalAppViewConsoleOutput, text_box_get_view(app->text_box));
    app->text_box_store = furi_string_alloc();
    furi_string_reserve(app->text_box_store, Pi_Terminal_TEXT_BOX_STORE_SIZE);

    app->text_input = uart_text_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        Pi_TerminalAppViewTextInput,
        uart_text_input_get_view(app->text_input));

    app->hex_input = uart_hex_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        Pi_TerminalAppViewHexInput,
        uart_text_input_get_view(app->hex_input));

    app->setup_selected_option_index[BAUDRATE_ITEM_IDX] = DEFAULT_BAUDRATE_OPT_IDX;

    scene_manager_next_scene(app->scene_manager, Pi_TerminalSceneStart);

    return app;
}

void Pi_Terminal_app_free(Pi_TerminalApp* app) {
    furi_assert(app);

    // Views
    view_dispatcher_remove_view(app->view_dispatcher, Pi_TerminalAppViewVarItemList);
    view_dispatcher_remove_view(app->view_dispatcher, Pi_TerminalAppViewSetup);
    view_dispatcher_remove_view(app->view_dispatcher, Pi_TerminalAppViewConsoleOutput);
    view_dispatcher_remove_view(app->view_dispatcher, Pi_TerminalAppViewTextInput);
    view_dispatcher_remove_view(app->view_dispatcher, Pi_TerminalAppViewHexInput);

    text_box_free(app->text_box);
    furi_string_free(app->text_box_store);
    uart_text_input_free(app->text_input);

    // View dispatcher
    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);

    Pi_Terminal_uart_free(app->uart);

    // Close records
    furi_record_close(RECORD_GUI);

    free(app);
}

int32_t Pi_Terminal_app(void* p) {
    UNUSED(p);
    // Disable expansion protocol to avoid interference with UART Handle
    Expansion* expansion = furi_record_open(RECORD_EXPANSION);
    expansion_disable(expansion);

    Pi_TerminalApp* Pi_Terminal_app = Pi_Terminal_app_alloc();

    Pi_Terminal_app->uart = Pi_Terminal_uart_init(Pi_Terminal_app);

    view_dispatcher_run(Pi_Terminal_app->view_dispatcher);

    Pi_Terminal_app_free(Pi_Terminal_app);

    // Return previous state of expansion
    expansion_enable(expansion);
    furi_record_close(RECORD_EXPANSION);

    return 0;
}
