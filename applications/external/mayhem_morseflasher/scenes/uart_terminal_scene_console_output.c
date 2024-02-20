#include "../uart_terminal_app_i.h"

void uart_terminal_console_output_handle_rx_data_cb(uint8_t* buf, size_t len, void* context) {
    furi_assert(context);
    UART_TerminalApp* app = context;

    // If text box store gets too big, then truncate it
    app->text_box_store_strlen += len;
    if(app->text_box_store_strlen >= UART_TERMINAL_TEXT_BOX_STORE_SIZE - 1) {
        furi_string_right(app->text_box_store, app->text_box_store_strlen / 2);
        app->text_box_store_strlen = furi_string_size(app->text_box_store) + len;
    }

    // Null-terminate buf and append to text box store
    buf[len] = '\0';
    furi_string_cat_printf(app->text_box_store, "%s", buf);

    view_dispatcher_send_custom_event(
        app->view_dispatcher, UART_TerminalEventRefreshConsoleOutput);
}

void uart_terminal_scene_console_output_on_enter(void* context) {
    UART_TerminalApp* app = context;

    TextBox* text_box = app->text_box;
    text_box_reset(app->text_box);
    text_box_set_font(text_box, TextBoxFontText);
    if(app->focus_console_start) {
        text_box_set_focus(text_box, TextBoxFocusStart);
    } else {
        text_box_set_focus(text_box, TextBoxFocusEnd);
    }

    if(app->is_command) {
        furi_string_reset(app->text_box_store);
        app->text_box_store_strlen = 0;

        // app->show_stopscan_tip in the if is just a hack to get the help displayed since there is no commands in this app
        if(app->show_stopscan_tip ||
           0 == strncmp("help", app->selected_tx_string, strlen("help"))) {
            const char* help_msg =
                "Morse Flasher for\nMayhem Fin\n\nBased on UART terminal by\ncool4uma, which is a\nmodified WiFi Marauder\ncompanion by 0xchocolate\n\n";
            furi_string_cat_str(app->text_box_store, help_msg);
            app->text_box_store_strlen += strlen(help_msg);
        }

        if(app->show_stopscan_tip) {
            const char* help_msg = "Press BACK to return\n";
            furi_string_cat_str(app->text_box_store, help_msg);
            app->text_box_store_strlen += strlen(help_msg);
        }
    }

    // Set starting text - for "View Log", this will just be what was already in the text box store
    text_box_set_text(app->text_box, furi_string_get_cstr(app->text_box_store));

    scene_manager_set_scene_state(app->scene_manager, UART_TerminalSceneConsoleOutput, 0);
    view_dispatcher_switch_to_view(app->view_dispatcher, UART_TerminalAppViewConsoleOutput);

    // Register callback to receive data
    uart_terminal_uart_set_handle_rx_data_cb(
        app->uart, uart_terminal_console_output_handle_rx_data_cb); // setup callback for rx thread

    // Send command with newline '\n'
    /*if(!app->is_command && app->selected_tx_string)*/ {
        uart_terminal_uart_tx(
            app->uart, (uint8_t*)(app->selected_tx_string), strlen(app->selected_tx_string));
        uart_terminal_uart_tx(app->uart, (uint8_t*)("\n"), 1);
    }
}

bool uart_terminal_scene_console_output_on_event(void* context, SceneManagerEvent event) {
    UART_TerminalApp* app = context;

    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        text_box_set_text(app->text_box, furi_string_get_cstr(app->text_box_store));
        consumed = true;
    } else if(event.type == SceneManagerEventTypeTick) {
        consumed = true;
    }

    return consumed;
}

void uart_terminal_scene_console_output_on_exit(void* context) {
    UART_TerminalApp* app = context;

    // Unregister rx callback
    uart_terminal_uart_set_handle_rx_data_cb(app->uart, NULL);

    // Automatically logut when exiting view
    //if(app->is_command) {
    //    uart_terminal_uart_tx(app->uart,(uint8_t*)("exit\n"), strlen("exit\n"));
    //}
}
