#include "uart_terminal_app_i.h"

#include <furi.h>
#include <furi_hal.h>

static bool uart_terminal_app_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    UART_TerminalApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool uart_terminal_app_back_event_callback(void* context) {
    furi_assert(context);
    UART_TerminalApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static void uart_terminal_app_tick_event_callback(void* context) {
    furi_assert(context);
    UART_TerminalApp* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

UART_TerminalApp* uart_terminal_app_alloc() {
    UART_TerminalApp* app = malloc(sizeof(UART_TerminalApp));

    app->gui = furi_record_open(RECORD_GUI);

    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager = scene_manager_alloc(&uart_terminal_scene_handlers, app);
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);

    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, uart_terminal_app_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, uart_terminal_app_back_event_callback);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, uart_terminal_app_tick_event_callback, 100);

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    app->main_menu_list = variable_item_list_alloc();
    view_dispatcher_add_view(app->view_dispatcher, Gravity_AppViewMainMenu,
            variable_item_list_get_view(app->main_menu_list));
    app->targets_menu_list = variable_item_list_alloc();
    view_dispatcher_add_view(app->view_dispatcher, Gravity_AppViewTargetsMenu,
            variable_item_list_get_view(app->targets_menu_list));
    app->packets_menu_list = variable_item_list_alloc();
    view_dispatcher_add_view(app->view_dispatcher, Gravity_AppViewPacketsMenu,
            variable_item_list_get_view(app->packets_menu_list));
    app->attacks_menu_list = variable_item_list_alloc();
    view_dispatcher_add_view(app->view_dispatcher, Gravity_AppViewAttacksMenu,
            variable_item_list_get_view(app->attacks_menu_list));
    app->settings_menu_list = variable_item_list_alloc();
    view_dispatcher_add_view(app->view_dispatcher, Gravity_AppViewSettingsMenu,
            variable_item_list_get_view(app->settings_menu_list));
    app->help_menu_list = variable_item_list_alloc();
    view_dispatcher_add_view(app->view_dispatcher, Gravity_AppViewHelpMenu,
            variable_item_list_get_view(app->help_menu_list));
    app->deauth_menu_list = variable_item_list_alloc();
    view_dispatcher_add_view(app->view_dispatcher, Gravity_AppViewDeauthMenu,
            variable_item_list_get_view(app->deauth_menu_list));
    app->fuzz_menu_list = variable_item_list_alloc();
    view_dispatcher_add_view(app->view_dispatcher, Gravity_AppViewFuzzMenu,
            variable_item_list_get_view(app->fuzz_menu_list));

    for(int i = 0; i < MAX_MENU_ITEMS; ++i) {
        app->selected_option_index[i] = 0;
    }

    app->text_box = text_box_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, UART_TerminalAppViewConsoleOutput, text_box_get_view(app->text_box));
    app->text_box_store = furi_string_alloc();
    furi_string_reserve(app->text_box_store, UART_TERMINAL_TEXT_BOX_STORE_SIZE);

    app->text_input = uart_text_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        UART_TerminalAppViewTextInput,
        uart_text_input_get_view(app->text_input));
    app->currentMenu = GRAVITY_MENU_MAIN;

    scene_manager_next_scene(app->scene_manager, UART_TerminalSceneMain);

    return app;
}

void uart_terminal_app_free(UART_TerminalApp* app) {
    furi_assert(app);

    // Views
    view_dispatcher_remove_view(app->view_dispatcher, UART_TerminalAppViewConsoleOutput);
    view_dispatcher_remove_view(app->view_dispatcher, UART_TerminalAppViewTextInput);
    view_dispatcher_remove_view(app->view_dispatcher, Gravity_AppViewMainMenu);
    view_dispatcher_remove_view(app->view_dispatcher, Gravity_AppViewTargetsMenu);
    view_dispatcher_remove_view(app->view_dispatcher, Gravity_AppViewPacketsMenu);
    view_dispatcher_remove_view(app->view_dispatcher, Gravity_AppViewAttacksMenu);
    view_dispatcher_remove_view(app->view_dispatcher, Gravity_AppViewSettingsMenu);
    view_dispatcher_remove_view(app->view_dispatcher, Gravity_AppViewHelpMenu);
    view_dispatcher_remove_view(app->view_dispatcher, Gravity_AppViewDeauthMenu);
    view_dispatcher_remove_view(app->view_dispatcher, Gravity_AppViewFuzzMenu);
    text_box_free(app->text_box);
    furi_string_free(app->text_box_store);
    uart_text_input_free(app->text_input);

    // View dispatcher
    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);

    uart_terminal_uart_free(app->uart);

    // Close records
    furi_record_close(RECORD_GUI);

    free(app);
}

int32_t uart_terminal_app(void* p) {
    UNUSED(p);
    UART_TerminalApp* uart_terminal_app = uart_terminal_app_alloc();

    uart_terminal_app->uart = uart_terminal_uart_init(uart_terminal_app);

    view_dispatcher_run(uart_terminal_app->view_dispatcher);

    uart_terminal_app_free(uart_terminal_app);

    return 0;
}

char *strToken(char *cmdLine, char sep, int tokenNum) {
    int i;
    int tokenCount = 0;
    for (i = 0; i < (int)strlen(cmdLine) && tokenCount != tokenNum; ++i) {
        if (cmdLine[i] == sep) {
            ++tokenCount;
        }
    }
    if (cmdLine[i - 1] == sep || cmdLine[i - 1] == '\0') {
        /* Found the end of the token, now find the beginning */
        int j;
        for (j = (i - 2); j > 0 && cmdLine[j] != sep; --j) { }
        /* Token runs from index j to (i - 2) */
        char *retVal = malloc(sizeof(char) * (i - j));
        if (retVal == NULL) {
            printf("GRAVITY: Failed to malloc token\n");
            return NULL;
        }
        strncpy(retVal, cmdLine, (i - j - 1));
        retVal[i - j - 1] = '\0';
        return retVal;
    } else {
        /* No token */
        if (tokenNum == 1) {
            return cmdLine;
        } else {
            return NULL;
        }
    }
    return NULL;
}
