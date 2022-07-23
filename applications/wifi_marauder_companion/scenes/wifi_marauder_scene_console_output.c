#include "../wifi_marauder_app_i.h"

//void wifi_marauder_scene_console_output_callback(WifiMarauderCustomEvent event, void* context) {
    
//}

void wifi_marauder_console_output_handle_rx_data_cb(uint8_t *buf, size_t len, void* context) {
    furi_assert(context);
    WifiMarauderApp* app = context;

    // If the text box store will reach 75% capacity, then truncate half from the beginning
    const size_t text_box_store_size = string_size(app->text_box_store);
    if ((text_box_store_size + len) >= (3*string_capacity(app->text_box_store)/4)) {
        string_right(app->text_box_store, text_box_store_size/2);
    }

    // TODO: optimize
    for (size_t i = 0; i < len; ++i) {
        string_cat_printf(app->text_box_store, "%c", buf[i]);
    }

    view_dispatcher_send_custom_event(app->view_dispatcher, WifiMarauderEventRefreshConsoleOutput);
}

void wifi_marauder_scene_console_output_on_enter(void* context) {
    WifiMarauderApp* app = context;

    TextBox* text_box = app->text_box;
    text_box_set_font(text_box, TextBoxFontText);
    text_box_set_focus(text_box, TextBoxFocusEnd);
    string_reset(app->text_box_store);

    //wifi_marauder_console_output_set_callback(app->detect, wifi_marauder_scene_console_output_callback, app);
    scene_manager_set_scene_state(app->scene_manager, WifiMarauderSceneConsoleOutput, 0);
    view_dispatcher_switch_to_view(app->view_dispatcher, WifiMarauderAppViewConsoleOutput);

    // TODO: start the thread here instead?
    wifi_marauder_uart_set_handle_rx_data_cb(app->uart, wifi_marauder_console_output_handle_rx_data_cb); // setup callback for rx thread

    if (app->selected_tx_string) {
        wifi_marauder_uart_tx((uint8_t*)(app->selected_tx_string), strlen(app->selected_tx_string));
    }
}

bool wifi_marauder_scene_console_output_on_event(void* context, SceneManagerEvent event) {
    WifiMarauderApp* app = context;

    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        text_box_set_text(app->text_box, string_get_cstr(app->text_box_store));
        consumed = true;
    } else if(event.type == SceneManagerEventTypeTick) {
        consumed = true;
    }

    return consumed;
}

void wifi_marauder_scene_console_output_on_exit(void* context) {
    WifiMarauderApp* app = context;

    // Unregister rx callback
    wifi_marauder_uart_set_handle_rx_data_cb(app->uart, NULL);

    // Automatically stop the scan when exiting view
    wifi_marauder_uart_tx((uint8_t*)("stopscan\n"), strlen("stopscan\n"));

    text_box_reset(app->text_box);
    string_reset(app->text_box_store);
}