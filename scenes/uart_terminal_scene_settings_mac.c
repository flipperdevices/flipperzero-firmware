#include "../uart_terminal_app_i.h"

void uart_terminal_scene_settings_mac_input_callback(void* context) {
    UART_TerminalApp* app = context;

    view_dispatcher_send_custom_event(app->view_dispatcher, UART_TerminalEventStartConsole);
}

void uart_terminal_scene_settings_mac_changed_callback(void *context) {
    UART_TerminalApp *app = context;

    // TODO: do something
}

void uart_terminal_scene_settings_mac_on_enter(void* context) {
    UART_TerminalApp* app = context;

    ByteInput *mac_input = app->settings_mac_bytes;

    byte_input_set_header_text(mac_input, "Enter MAC");
    byte_input_set_result_callback(
        mac_input,
        uart_terminal_scene_settings_mac_input_callback,
        uart_terminal_scene_settings_mac_changed_callback,
        app,
        app->mac_bytes,
        NUM_MAC_BYTES);

    view_dispatcher_switch_to_view(app->view_dispatcher, Gravity_AppViewSettingsMac);
}

bool uart_terminal_scene_settings_mac_on_event(void* context, SceneManagerEvent event) {
    UART_TerminalApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == UART_TerminalEventStartConsole) {
            // Point to custom string to send
//            app->selected_tx_string = app->text_input_store;
            scene_manager_next_scene(app->scene_manager, UART_TerminalAppViewConsoleOutput);
            consumed = true;
        }
    }

    return consumed;
}

void uart_terminal_scene_settings_mac_on_exit(void* context) {
    UART_TerminalApp* app = context;

    byte_input_free(app->settings_mac_bytes);
}
