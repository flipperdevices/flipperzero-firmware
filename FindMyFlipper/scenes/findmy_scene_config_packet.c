#include "../findmy_i.h"

enum ByteInputResult {
    ByteInputResultOk,
};

static void findmy_scene_config_packet_callback(void* context) {
    FindMy* app = context;

    view_dispatcher_send_custom_event(app->view_dispatcher, ByteInputResultOk);
}

void findmy_scene_config_packet_on_enter(void* context) {
    FindMy* app = context;
    ByteInput* byte_input = app->byte_input;

    byte_input_set_header_text(byte_input, "Enter Bluetooth Payload:");

    memcpy(app->packet_buf, app->state.data, sizeof(app->packet_buf));

    byte_input_set_result_callback(
        byte_input,
        findmy_scene_config_packet_callback,
        NULL,
        app,
        app->packet_buf,
        sizeof(app->packet_buf));

    view_dispatcher_switch_to_view(app->view_dispatcher, FindMyViewByteInput);
}

bool findmy_scene_config_packet_on_event(void* context, SceneManagerEvent event) {
    FindMy* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        switch(event.event) {
        case ByteInputResultOk:
            scene_manager_search_and_switch_to_previous_scene(
                app->scene_manager, FindMySceneConfig);
            memcpy(app->state.data, app->packet_buf, sizeof(app->state.data));
            findmy_state_save(&app->state);
            furi_check(
                furi_hal_bt_extra_beacon_set_data(app->state.data, sizeof(app->state.data)));
            findmy_main_update_type(app->findmy_main, findmy_data_get_type(app->state.data));
            break;
        default:
            break;
        }
    }

    return consumed;
}

void findmy_scene_config_packet_on_exit(void* context) {
    FindMy* app = context;

    byte_input_set_result_callback(app->byte_input, NULL, NULL, NULL, NULL, 0);
    byte_input_set_header_text(app->byte_input, "");
}