#include "../pi_terminal_app_i.h"

void Pi_Terminal_console_output_handle_rx_data_cb(uint8_t* buf, size_t len, void* context) {
    furi_assert(context);
    Pi_TerminalApp* app = context;
    FuriString* new_str = furi_string_alloc();

    if(app->hex_mode) {
        while(len--) {
            uint8_t byte = *(buf++);
            if(byte == '\0') break;
            furi_string_cat_printf(new_str, "%02X ", byte);
        }
    } else {
        buf[len] = '\0';
        furi_string_cat_printf(new_str, "%s", buf);
    }

    // If text box store gets too big, then truncate it
    app->text_box_store_strlen += furi_string_size(new_str);
    ;
    while(app->text_box_store_strlen >= Pi_Terminal_TEXT_BOX_STORE_SIZE - 1) {
        furi_string_right(app->text_box_store, app->text_box_store_strlen / 2);
        app->text_box_store_strlen = furi_string_size(app->text_box_store) + len;
    }

    furi_string_cat(app->text_box_store, new_str);
    furi_string_free(new_str);

    view_dispatcher_send_custom_event(
        app->view_dispatcher, Pi_TerminalEventRefreshConsoleOutput);
}

static uint8_t hex_char_to_byte(const char c) {
    if(c >= '0' && c <= '9') {
        return c - '0';
    }
    if(c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    if(c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    return 0;
}

void Pi_Terminal_scene_console_output_on_enter(void* context) {
    Pi_TerminalApp* app = context;

    TextBox* text_box = app->text_box;
    text_box_reset(app->text_box);
    text_box_set_font(text_box, TextBoxFontText);
    text_box_set_focus(text_box, TextBoxFocusEnd);

    bool need_reinit = false;

    //Change baudrate
    if(app->BAUDRATE != app->NEW_BAUDRATE && app->NEW_BAUDRATE) {
        need_reinit = true;
    }

    //Change UART port
    if(app->uart_ch != app->new_uart_ch) {
        need_reinit = true;
    }

    if(need_reinit) {
        Pi_Terminal_uart_free(app->uart);
        app->BAUDRATE = app->NEW_BAUDRATE;
        app->uart_ch = app->new_uart_ch;
        app->uart = Pi_Terminal_uart_init(app);
    }

    if(app->is_command) {
        furi_string_reset(app->text_box_store);
        app->text_box_store_strlen = 0;
    }

    // Set starting text - for "View Log", this will just be what was already in the text box store
    text_box_set_text(app->text_box, furi_string_get_cstr(app->text_box_store));

    scene_manager_set_scene_state(app->scene_manager, Pi_TerminalSceneConsoleOutput, 0);
    view_dispatcher_switch_to_view(app->view_dispatcher, Pi_TerminalAppViewConsoleOutput);

    // Register callback to receive data
    Pi_Terminal_uart_set_handle_rx_data_cb(
        app->uart, Pi_Terminal_console_output_handle_rx_data_cb); // setup callback for rx thread

    if(app->hex_mode) {
        // Send binary packet
        if(app->selected_tx_string) {
            const char* str = app->selected_tx_string;
            uint8_t digit_num = 0;
            uint8_t byte = 0;
            while(*str) {
                byte |= (hex_char_to_byte(*str) << ((1 - digit_num) * 4));

                if(++digit_num == 2) {
                    Pi_Terminal_uart_tx(app->uart, &byte, 1);
                    digit_num = 0;
                    byte = 0;
                }
                str++;
            }

            if(digit_num) {
                Pi_Terminal_uart_tx(app->uart, &byte, 1);
            }
        }
    } else {
        // Send command with CR+LF or newline '\n'
        if(app->is_command && app->selected_tx_string) {
            if(app->TERMINAL_MODE == 1) {
                Pi_Terminal_uart_tx(
                    app->uart,
                    (uint8_t*)(app->selected_tx_string),
                    strlen(app->selected_tx_string));
                Pi_Terminal_uart_tx(app->uart, (uint8_t*)("\r\n"), 2);
            } else {
                Pi_Terminal_uart_tx(
                    app->uart,
                    (uint8_t*)(app->selected_tx_string),
                    strlen(app->selected_tx_string));
                Pi_Terminal_uart_tx(app->uart, (uint8_t*)("\n"), 1);
            }
        }
    }
}

bool Pi_Terminal_scene_console_output_on_event(void* context, SceneManagerEvent event) {
    Pi_TerminalApp* app = context;

    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        text_box_set_text(app->text_box, furi_string_get_cstr(app->text_box_store));
        consumed = true;
    } else if(event.type == SceneManagerEventTypeTick) {
        consumed = true;
    }

    return consumed;
}

void Pi_Terminal_scene_console_output_on_exit(void* context) {
    Pi_TerminalApp* app = context;

    // Unregister rx callback
    Pi_Terminal_uart_set_handle_rx_data_cb(app->uart, NULL);
}