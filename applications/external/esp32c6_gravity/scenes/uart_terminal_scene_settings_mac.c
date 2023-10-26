#include "../uart_terminal_app_i.h"

#define setMacCmdLen 26

void uart_terminal_scene_settings_mac_input_callback(void* context) {
    UART_TerminalApp* app = context;

    view_dispatcher_send_custom_event(app->view_dispatcher, UART_TerminalEventStartConsole);
}

void uart_terminal_scene_settings_mac_changed_callback(void* context) {
    // Do nothing?
}

void uart_terminal_scene_settings_mac_on_enter(void* context) {
    UART_TerminalApp* app = context;

    ByteInput* mac_input = app->settings_mac_bytes;

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

/* Convert an array of bytesCount uint8_ts into a colon-separated string of bytes
   strBytes must be initialised with sufficient space to hold the output string.
   For a MAC this is 18 bytes. In general it is 3 * bytesCount */
void bytes_to_string(uint8_t* bytes, uint16_t bytesCount, char* strBytes) {
    // memset(strBytes, '\0', bytesCount * 3);
    // for (int i = 0; i < bytesCount; ++i) {
    //     // TODO
    // }
    uint8_t* p_in = bytes;
    const char* hex = "0123456789ABCDEF";
    char* p_out = strBytes;
    for(; p_in < bytes + bytesCount; p_out += 3, ++p_in) {
        p_out[0] = hex[(*p_in >> 4) & 0xF];
        p_out[1] = hex[*p_in & 0xF];
        p_out[2] = ':';
    }
    p_out[-1] = 0;
}

bool uart_terminal_scene_settings_mac_on_event(void* context, SceneManagerEvent event) {
    UART_TerminalApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == UART_TerminalEventStartConsole) {
            /* Convert app->mac_bytes[] to a string */
            char strMac[3 * NUM_MAC_BYTES] = "";
            bytes_to_string(app->mac_bytes, NUM_MAC_BYTES, strMac);
            // Build string set MAC app->mac_bytes[0]:app->mac_bytes[1]:....
            char* tx_command = malloc(sizeof(char) * setMacCmdLen);
            if(tx_command == NULL) {
                // TODO: Panic
            }
            memset(tx_command, '\0', setMacCmdLen);
            strcpy(tx_command, "set mac ");
            strcat(tx_command, strMac);
            app->selected_tx_string = tx_command;
            app->free_command = true;
            scene_manager_next_scene(app->scene_manager, UART_TerminalAppViewConsoleOutput);
            consumed = true;
        }
    }
    return consumed;
}

void uart_terminal_scene_settings_mac_on_exit(void* context) {
    // Do nothing?
}
