#include "../uart_terminal_app_i.h"

void uart_terminal_scene_hex_input_callback(void* context) {
    UART_TerminalApp* app = context;

    view_dispatcher_send_custom_event(app->view_dispatcher, UART_TerminalEventStartConsole);
}

void uart_terminal_scene_hex_input_on_enter(void* context) {
    UART_TerminalApp* app = context;

    // Setup view
    ByteInput* hex_input = app->hex_input;
    // Add help message to header
    byte_input_set_header_text(hex_input, "Send HEX packet to UART");
    byte_input_set_result_callback(
        hex_input,
        uart_terminal_scene_hex_input_callback,
        NULL,
        app,
        app->text_input_store,
        UART_TERMINAL_TEXT_INPUT_STORE_SIZE);

    view_dispatcher_switch_to_view(app->view_dispatcher, UART_TerminalAppViewHexInput);
}

bool uart_terminal_scene_hex_input_on_event(void* context, SceneManagerEvent event) {
    UART_TerminalApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == UART_TerminalEventStartConsole) {
            // Point to custom string to send
            app->selected_tx_string = app->text_input_store;
            scene_manager_next_scene(app->scene_manager, UART_TerminalAppViewConsoleOutput);
            consumed = true;
        }
    }

    return consumed;
}

void uart_terminal_scene_hex_input_on_exit(void* context) {
    UART_TerminalApp* app = context;

    byte_input_set_result_callback(app->hex_input, NULL, NULL, NULL, NULL, 0);
    byte_input_set_header_text(app->hex_input, "");
}
