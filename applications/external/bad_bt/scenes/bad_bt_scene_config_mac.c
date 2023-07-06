#include "../bad_bt_app.h"

void bad_bt_scene_config_mac_byte_input_callback(void* context) {
    BadBtApp* bad_bt = context;

    view_dispatcher_send_custom_event(bad_bt->view_dispatcher, BadBtAppCustomEventByteInputDone);
}

void bad_bt_scene_config_mac_on_enter(void* context) {
    BadBtApp* bad_bt = context;
    ByteInput* byte_input = bad_bt->byte_input;

    memmove(bad_bt->bt_mac_buf, bad_bt->config.bt_mac, BAD_BT_MAC_LEN);
    byte_input_set_header_text(byte_input, "Set BT MAC address");

    byte_input_set_result_callback(
        byte_input,
        bad_bt_scene_config_mac_byte_input_callback,
        NULL,
        bad_bt,
        bad_bt->bt_mac_buf,
        BAD_BT_MAC_LEN);

    view_dispatcher_switch_to_view(bad_bt->view_dispatcher, BadBtAppViewByteInput);
}

bool bad_bt_scene_config_mac_on_event(void* context, SceneManagerEvent event) {
    BadBtApp* bad_bt = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        if(event.event == BadBtAppCustomEventByteInputDone) {
            memmove(bad_bt->config.bt_mac, bad_bt->bt_mac_buf, BAD_BT_MAC_LEN);
            bad_bt_config_refresh(bad_bt);
        }
        scene_manager_previous_scene(bad_bt->scene_manager);
    }
    return consumed;
}

void bad_bt_scene_config_mac_on_exit(void* context) {
    BadBtApp* bad_bt = context;
    ByteInput* byte_input = bad_bt->byte_input;

    byte_input_set_result_callback(byte_input, NULL, NULL, NULL, NULL, 0);
    byte_input_set_header_text(byte_input, "");
}
