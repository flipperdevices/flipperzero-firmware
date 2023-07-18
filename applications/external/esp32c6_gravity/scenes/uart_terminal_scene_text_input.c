#include "../uart_terminal_app_i.h"

/* GRAVITY: Import usage strings */
#include "../esp_flip_const.h"

void uart_terminal_scene_text_input_callback(void* context) {
    UART_TerminalApp* app = context;

    view_dispatcher_send_custom_event(app->view_dispatcher, UART_TerminalEventStartConsole);
}

void uart_terminal_scene_text_input_on_enter(void* context) {
    UART_TerminalApp* app = context;

    if(false == app->is_custom_tx_string) {
        // Fill text input with selected string so that user can add to it
        size_t length = strlen(app->selected_tx_string);
        furi_assert(length < UART_TERMINAL_TEXT_INPUT_STORE_SIZE);
        bzero(app->text_input_store, UART_TERMINAL_TEXT_INPUT_STORE_SIZE);
        strncpy(app->text_input_store, app->selected_tx_string, length);

        // Add space - because flipper keyboard currently doesn't have a space
        //app->text_input_store[length] = ' ';
        app->text_input_store[length + 1] = '\0';
        app->is_custom_tx_string = true;
    }

    // Setup view
    UART_TextInput* text_input = app->text_input;
    // Add help message to header
    char* helpStr = NULL;
    switch(app->gravityCommand) {
    case GRAVITY_BEACON:
        helpStr = (char*)SHORT_BEACON;
        break;
    case GRAVITY_TARGET_SSIDS:
        helpStr = (char*)SHORT_TARGET_SSIDS;
        break;
    case GRAVITY_PROBE:
        helpStr = (char*)SHORT_PROBE;
        break;
    case GRAVITY_FUZZ:
        helpStr = (char*)SHORT_FUZZ;
        break;
    case GRAVITY_SNIFF:
        helpStr = (char*)SHORT_SNIFF;
        break;
    case GRAVITY_DEAUTH:
        helpStr = (char*)SHORT_DEAUTH;
        break;
    case GRAVITY_MANA:
        helpStr = (char*)SHORT_MANA;
        break;
    case GRAVITY_STALK:
        helpStr = (char*)SHORT_STALK;
        break;
    case GRAVITY_AP_DOS:
        helpStr = (char*)SHORT_AP_DOS;
        break;
    case GRAVITY_AP_CLONE:
        helpStr = (char*)SHORT_AP_CLONE;
        break;
    case GRAVITY_SCAN:
        helpStr = (char*)SHORT_SCAN;
        break;
    case GRAVITY_HOP:
        helpStr = (char*)SHORT_HOP;
        break;
    case GRAVITY_SET:
        helpStr = (char*)SHORT_SET;
        break;
    case GRAVITY_GET:
        helpStr = (char*)SHORT_GET;
        break;
    case GRAVITY_VIEW:
        helpStr = (char*)SHORT_VIEW;
        break;
    case GRAVITY_SELECT:
        helpStr = (char*)SHORT_SELECT;
        break;
    case GRAVITY_SELECTED:
        helpStr = (char*)SHORT_SELECTED;
        break;
    case GRAVITY_CLEAR:
        helpStr = (char*)SHORT_CLEAR;
        break;
    case GRAVITY_HANDSHAKE:
        helpStr = (char*)SHORT_HANDSHAKE;
        break;
    case GRAVITY_COMMANDS:
        helpStr = (char*)SHORT_COMMANDS;
        break;
    case GRAVITY_INFO:
        helpStr = (char*)SHORT_INFO;
        break;
    default:
        helpStr = "Send command to UART";
        break;
    }

    uart_text_input_set_header_text(text_input, helpStr);
    uart_text_input_set_result_callback(
        text_input,
        uart_terminal_scene_text_input_callback,
        app,
        app->text_input_store,
        UART_TERMINAL_TEXT_INPUT_STORE_SIZE,
        false);

    view_dispatcher_switch_to_view(app->view_dispatcher, UART_TerminalAppViewTextInput);
}

bool uart_terminal_scene_text_input_on_event(void* context, SceneManagerEvent event) {
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

void uart_terminal_scene_text_input_on_exit(void* context) {
    UART_TerminalApp* app = context;

    uart_text_input_reset(app->text_input);
}
