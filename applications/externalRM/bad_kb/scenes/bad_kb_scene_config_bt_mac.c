#include "../bad_kb_app.h"

void bad_kb_scene_config_bt_mac_byte_input_callback(void* context) {
    BadKbApp* bad_kb = context;

    view_dispatcher_send_custom_event(bad_kb->view_dispatcher, BadKbAppCustomEventByteInputDone);
}

void bad_kb_scene_config_bt_mac_on_enter(void* context) {
    BadKbApp* bad_kb = context;
    ByteInput* byte_input = bad_kb->byte_input;

    memmove(bad_kb->bt_mac_buf, bad_kb->config.bt_mac, BAD_KB_MAC_LEN);
    furi_hal_bt_reverse_mac_addr(bad_kb->bt_mac_buf);
    byte_input_set_header_text(byte_input, "Set BT MAC address");

    byte_input_set_result_callback(
        byte_input,
        bad_kb_scene_config_bt_mac_byte_input_callback,
        NULL,
        bad_kb,
        bad_kb->bt_mac_buf,
        BAD_KB_MAC_LEN);

    view_dispatcher_switch_to_view(bad_kb->view_dispatcher, BadKbAppViewByteInput);
}

bool bad_kb_scene_config_bt_mac_on_event(void* context, SceneManagerEvent event) {
    BadKbApp* bad_kb = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        if(event.event == BadKbAppCustomEventByteInputDone) {
            furi_hal_bt_reverse_mac_addr(bad_kb->bt_mac_buf);
            memmove(bad_kb->config.bt_mac, bad_kb->bt_mac_buf, BAD_KB_MAC_LEN);
            bad_kb_config_refresh(bad_kb);
        }
        scene_manager_previous_scene(bad_kb->scene_manager);
    }
    return consumed;
}

void bad_kb_scene_config_bt_mac_on_exit(void* context) {
    BadKbApp* bad_kb = context;
    ByteInput* byte_input = bad_kb->byte_input;

    byte_input_set_result_callback(byte_input, NULL, NULL, NULL, NULL, 0);
    byte_input_set_header_text(byte_input, "");
}
