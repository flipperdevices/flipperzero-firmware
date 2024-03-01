#include "../evil_portal_app_i.h"
#include "../helpers/evil_portal_storage.h"
#include <m-string.h>

void evil_portal_console_output_handle_rx_data_cb(uint8_t* buf, size_t len, void* context) {
    furi_assert(context);
    Evil_PortalApp* app = context;

    // If text box store gets too big, then truncate it
    app->text_box_store_strlen += len;
    if(app->text_box_store_strlen >= EVIL_PORTAL_TEXT_BOX_STORE_SIZE - 1) {
        furi_string_right(app->text_box_store, app->text_box_store_strlen / 2);
        app->text_box_store_strlen = furi_string_size(app->text_box_store) + len;
    }

    // Null-terminate buf and append to text box store
    buf[len] = '\0';
    furi_string_cat_printf(app->text_box_store, "%s", buf);

    if(app->capture_line) {
        furi_string_cat_printf(app->captured_line, "%s", buf);
    }

    text_box_set_text(app->text_box, furi_string_get_cstr(app->text_box_store));
}

static inline bool captured(Evil_PortalApp* app, const char* str) {
    return furi_string_search_str(app->captured_line, str) != STRING_FAILURE;
}

void evil_portal_scene_console_output_on_enter(void* context) {
    Evil_PortalApp* app = context;

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
        app->sent_reset = false;

        if(0 == strncmp("help", app->selected_tx_string, strlen("help"))) {
            const char* help_msg = "BLUE = Waiting\nGREEN = Good\nRED = Bad\n\nThis project is a "
                                   "WIP.\ngithub.com/bigbrodude6119/flipper-zero-evil-portal\n\n"
                                   "Version 0.0.2\n\n";
            furi_string_cat_str(app->text_box_store, help_msg);
            app->text_box_store_strlen += strlen(help_msg);
            if(app->show_stopscan_tip) {
                const char* msg = "Press BACK to return\n";
                furi_string_cat_str(app->text_box_store, msg);
                app->text_box_store_strlen += strlen(msg);
            }
        }

        if(0 == strncmp("savelogs", app->selected_tx_string, strlen("savelogs"))) {
            const char* help_msg = "Logs saved.\n\n";
            furi_string_cat_str(app->text_box_store, help_msg);
            app->text_box_store_strlen += strlen(help_msg);
            furi_mutex_acquire(app->portal_logs_mutex, FuriWaitForever);
            write_logs(app->portal_logs);
            furi_string_reset(app->portal_logs);
            furi_mutex_release(app->portal_logs_mutex);
            if(app->show_stopscan_tip) {
                const char* msg = "Press BACK to return\n";
                furi_string_cat_str(app->text_box_store, msg);
                app->text_box_store_strlen += strlen(msg);
            }
        }

        if(0 == strncmp("setapname", app->selected_tx_string, strlen("setapname"))) {
            scene_manager_next_scene(app->scene_manager, Evil_PortalSceneRename);
            return;
        }

        if(0 == strncmp("selecthtml", app->selected_tx_string, strlen("selecthtml"))) {
            scene_manager_next_scene(app->scene_manager, Evil_PortalSceneSelectHtml);
            return;
        }

        if(0 == strncmp(SET_HTML_CMD, app->selected_tx_string, strlen(SET_HTML_CMD))) {
            if(app->show_stopscan_tip) {
                const char* msg =
                    "Starting portal\nMarauder takes a few secs to start\nPress BACK to return\n";
                furi_string_cat_str(app->text_box_store, msg);
                app->text_box_store_strlen += strlen(msg);
            }
        }

        if(0 == strncmp(RESET_CMD, app->selected_tx_string, strlen(RESET_CMD))) {
            app->sent_reset = true;
            if(app->show_stopscan_tip) {
                const char* msg = "Reseting portal\nPress BACK to return\n\n\n\n";
                furi_string_cat_str(app->text_box_store, msg);
                app->text_box_store_strlen += strlen(msg);
            }
        }
    }

    text_box_set_text(app->text_box, furi_string_get_cstr(app->text_box_store));

    scene_manager_set_scene_state(app->scene_manager, Evil_PortalSceneConsoleOutput, 0);
    view_dispatcher_switch_to_view(app->view_dispatcher, Evil_PortalAppViewConsoleOutput);

    // Register callback to receive data
    evil_portal_uart_set_handle_rx_data_cb(
        app->uart, evil_portal_console_output_handle_rx_data_cb);

    if(app->is_command && app->selected_tx_string) {
        if(0 == strncmp(SET_HTML_CMD, app->selected_tx_string, strlen(SET_HTML_CMD))) {
            FuriString* data = furi_string_alloc();
            app->capture_line = true;

            evil_portal_read_ap_name(context);
            // Test evil portal syntax and response, marauder ignores it
            furi_string_printf(data, "setap=%s\n", (char*)app->ap_name);
            furi_string_reset(app->captured_line);
            evil_portal_uart_tx(
                app->uart, (uint8_t*)(furi_string_get_cstr(data)), furi_string_size(data));
            // TODO: move timeouts and commands elsewhere, can't block input cycle
            for(uint8_t t = 0; t < 69 && !captured(app, "ap set") && !captured(app, "\n>"); t++)
                furi_delay_ms(100);
            bool icanhazmarauder = !captured(app, "ap set") && // Evil portal didn't respond
                                   captured(app, "\n>"); // Marauder did respond
            // Not evil portal, set up marauder
            if(icanhazmarauder) {
                furi_string_printf(data, "clearlist -a -s -c\nssid -a -n '%s'\n", app->ap_name);
                furi_string_reset(app->captured_line);
                evil_portal_uart_tx(
                    app->uart, (uint8_t*)(furi_string_get_cstr(data)), furi_string_size(data));
                // Marauder echoes the command, maybe still init so wait a while for echo
                for(uint8_t t = 0; t < 10 && !captured(app, (char*)app->ap_name); t++)
                    furi_delay_ms(100);
            }
            free(app->ap_name);

            evil_portal_read_index_html(context);
            if(icanhazmarauder) {
                furi_string_reset(app->captured_line);
                evil_portal_uart_tx(
                    app->uart,
                    (uint8_t*)("evilportal -c sethtmlstr\n"),
                    strlen("evilportal -c sethtmlstr\n"));
                for(uint8_t t = 0; t < 10 && !captured(app, "\n>") &&
                                   !captured(app, "Setting HTML from serial...");
                    t++)
                    furi_delay_ms(100);
                // Check for active attack
                if(!(captured(app, "\n>") && !captured(app, "Setting HTML from serial..."))) {
                    furi_string_reset(app->captured_line);
                    evil_portal_uart_tx(
                        app->uart, app->index_html, strlen((char*)app->index_html));
                    evil_portal_uart_tx(app->uart, (uint8_t*)("\n"), 1);
                    for(uint8_t t = 0; t < 20 && !captured(app, "html set"); t++)
                        furi_delay_ms(100);
                    evil_portal_uart_tx(
                        app->uart,
                        (uint8_t*)("evilportal -c start\n"),
                        strlen("evilportal -c start\n"));
                }
            } else {
                furi_string_set(data, "sethtml=");
                furi_string_cat(data, (char*)app->index_html);
                evil_portal_uart_tx(
                    app->uart,
                    (uint8_t*)(furi_string_get_cstr(data)),
                    strlen(furi_string_get_cstr(data)));
                evil_portal_uart_tx(app->uart, (uint8_t*)("\n"), 1);
            }

            free(app->index_html);
            app->capture_line = false;
            furi_string_reset(app->captured_line);
            furi_string_free(data);
        } else if(0 == strncmp(RESET_CMD, app->selected_tx_string, strlen(RESET_CMD))) {
            evil_portal_uart_tx(
                app->uart, (uint8_t*)(app->selected_tx_string), strlen(app->selected_tx_string));
            evil_portal_uart_tx(app->uart, (uint8_t*)("\nstopscan\n"), strlen("\nstopscan\n"));
        } else if(1 == strncmp("help", app->selected_tx_string, strlen("help"))) {
            evil_portal_uart_tx(
                app->uart, (uint8_t*)(app->selected_tx_string), strlen(app->selected_tx_string));
            evil_portal_uart_tx(app->uart, (uint8_t*)("\n"), 1);
        }
    }
}

bool evil_portal_scene_console_output_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);

    bool consumed = false;

    if(event.type == SceneManagerEventTypeTick) {
        consumed = true;
    }

    return consumed;
}

void evil_portal_scene_console_output_on_exit(void* context) {
    Evil_PortalApp* app = context;

    // Unregister rx callback
    evil_portal_uart_set_handle_rx_data_cb(app->uart, NULL);
}
