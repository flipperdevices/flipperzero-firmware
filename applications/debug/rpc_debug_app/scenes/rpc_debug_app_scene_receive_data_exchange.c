#include "../rpc_debug_app.h"

#include <core/log.h>

static void rpc_debug_app_scene_receive_data_exchange_callback(const uint8_t* data, size_t data_size, void* context) {
    RpcDebugApp* app = context;
    memset(app->data_store, 0, DATA_STORE_SIZE);
    memcpy(app->data_store, data, MIN(data_size, DATA_STORE_SIZE));
    view_dispatcher_send_custom_event(app->view_dispatcher, RpcDebugAppCustomEventRpcDataExchange);
}

void rpc_debug_app_scene_receive_data_exchange_on_enter(void* context) {
    RpcDebugApp* app = context;
    strncpy(app->text_store, "Received data will appear here...", TEXT_STORE_SIZE);

    text_box_set_text(app->text_box, app->text_store);
    text_box_set_font(app->text_box, TextBoxFontHex);

    rpc_system_app_set_data_exchange_callback(app->rpc, rpc_debug_app_scene_receive_data_exchange_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, RpcDebugAppViewTextBox);
}

bool rpc_debug_app_scene_receive_data_exchange_on_event(void* context, SceneManagerEvent event) {
    RpcDebugApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == RpcDebugAppCustomEventRpcDataExchange) {
            for(size_t i = 0; i < DATA_STORE_SIZE / 3; ++i) {
                snprintf(app->text_store + (i * 3), 4, "%02X ", app->data_store[i]);
            }
            app->text_store[TEXT_STORE_SIZE - 1] = '\0';
            text_box_set_text(app->text_box, app->text_store);
            consumed = true;
        }
    }

    return consumed;
}

void rpc_debug_app_scene_receive_data_exchange_on_exit(void* context) {
    RpcDebugApp* app = context;
    text_box_reset(app->text_box);
    rpc_system_app_set_data_exchange_callback(app->rpc, NULL, NULL);
}
