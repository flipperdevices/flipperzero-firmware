#include "../esp_flasher_app_i.h"

#include "../esp_flasher_worker.h"

void esp_flasher_console_output_handle_rx_data_cb(uint8_t* buf, size_t len, void* context) {
    furi_assert(context);
    EspFlasherApp* app = context;

    // If text box store gets too big, then truncate it
    app->text_box_store_strlen += len;
    if(app->text_box_store_strlen >= ESP_FLASHER_TEXT_BOX_STORE_SIZE - 1) {
        furi_string_right(app->text_box_store, app->text_box_store_strlen / 2);
        app->text_box_store_strlen = furi_string_size(app->text_box_store) + len;
    }

    // Null-terminate buf and append to text box store
    buf[len] = '\0';
    furi_string_cat_printf(app->text_box_store, "%s", buf);
    view_dispatcher_send_custom_event(app->view_dispatcher, EspFlasherEventRefreshConsoleOutput);
}

void esp_flasher_scene_console_output_on_enter(void* context) {
    EspFlasherApp* app = context;

    // Reset text box and set font
    TextBox* text_box = app->text_box;
    text_box_reset(text_box);
    text_box_set_font(text_box, TextBoxFontText);

    // Set focus on end
    text_box_set_focus(text_box, TextBoxFocusEnd);

    // Set starting text
    text_box_set_text(app->text_box, furi_string_get_cstr(app->text_box_store));

    // Set scene state and switch view
    scene_manager_set_scene_state(app->scene_manager, EspFlasherSceneConsoleOutput, 0);
    view_dispatcher_switch_to_view(app->view_dispatcher, EspFlasherAppViewConsoleOutput);

    // Register callbacks to receive data
    // setup callback for general log rx thread
    esp_flasher_uart_set_handle_rx_data_cb(
        app->uart,
        esp_flasher_worker_handle_rx_data_cb); // setup callback for general log rx thread

    // Start flash worker
    esp_flasher_worker_start_thread(app);
}

bool esp_flasher_scene_console_output_on_event(void* context, SceneManagerEvent event) {
    EspFlasherApp* app = context;

    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        text_box_set_text(app->text_box, furi_string_get_cstr(app->text_box_store));
        consumed = true;
    } else if(event.type == SceneManagerEventTypeTick) {
        consumed = true;
    } else {
        if(app->flash_worker_busy) {
            // ignore button presses while flashing
            consumed = true;
        }
    }

    return consumed;
}

void esp_flasher_scene_console_output_on_exit(void* context) {
    EspFlasherApp* app = context;

    esp_flasher_worker_stop_thread(app);

    // Unregister rx callback
    esp_flasher_uart_set_handle_rx_data_cb(app->uart, NULL);
}
