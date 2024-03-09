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

    memset(app->packet_buf, 0, sizeof(app->packet_buf));
    furi_hal_bt_extra_beacon_get_data(app->packet_buf);

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
            furi_check(
                furi_hal_bt_extra_beacon_set_data(app->packet_buf, sizeof(app->packet_buf)));
            if(app->packet_buf[0] == 0x1E && app->packet_buf[3] == 0x00) {
                app->apple = true; // Checks payload data for Apple identifier
            } else {
                app->apple = false;
            }
            findmy_main_update_apple(app->findmy_main, app->apple);
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