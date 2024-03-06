#include "../pi_terminal_app_i.h"

void Pi_Terminal_scene_hex_input_callback(void* context) {
    Pi_TerminalApp* app = context;

    view_dispatcher_send_custom_event(app->view_dispatcher, Pi_TerminalEventStartConsole);
}

void Pi_Terminal_scene_hex_input_on_enter(void* context) {
    Pi_TerminalApp* app = context;

    // Setup view
    UART_TextInput* text_input = app->hex_input;
    // Add help message to header
    uart_hex_input_set_header_text(text_input, "Send HEX packet to UART");
    uart_hex_input_set_result_callback(
        text_input,
        Pi_Terminal_scene_hex_input_callback,
        app,
        app->text_input_store,
        Pi_Terminal_TEXT_INPUT_STORE_SIZE,
        false);

    view_dispatcher_switch_to_view(app->view_dispatcher, Pi_TerminalAppViewHexInput);
}

bool Pi_Terminal_scene_hex_input_on_event(void* context, SceneManagerEvent event) {
    Pi_TerminalApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == Pi_TerminalEventStartConsole) {
            // Point to custom string to send
            app->selected_tx_string = app->text_input_store;
            scene_manager_next_scene(app->scene_manager, Pi_TerminalAppViewConsoleOutput);
            consumed = true;
        }
    }

    return consumed;
}

void Pi_Terminal_scene_hex_input_on_exit(void* context) {
    Pi_TerminalApp* app = context;

    uart_hex_input_reset(app->text_input);
}
