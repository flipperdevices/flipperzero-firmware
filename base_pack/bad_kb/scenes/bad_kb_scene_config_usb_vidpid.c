#include "../bad_kb_app_i.h"

void bad_kb_scene_config_usb_vidpid_byte_input_callback(void* context) {
    BadKbApp* bad_kb = context;

    view_dispatcher_send_custom_event(bad_kb->view_dispatcher, BadKbAppCustomEventByteInputDone);
}

void bad_kb_scene_config_usb_vidpid_on_enter(void* context) {
    BadKbApp* bad_kb = context;
    ByteInput* byte_input = bad_kb->byte_input;

    if(bad_kb->set_usb_id) {
        bad_kb->usb_vidpid_buf[0] = __REVSH(bad_kb->id_config.usb.vid);
        bad_kb->usb_vidpid_buf[1] = __REVSH(bad_kb->id_config.usb.pid);
    } else {
        bad_kb->usb_vidpid_buf[0] = __REVSH(bad_kb->config.usb.vid);
        bad_kb->usb_vidpid_buf[1] = __REVSH(bad_kb->config.usb.pid);
    }
    byte_input_set_header_text(byte_input, "Set USB VID:PID");

    byte_input_set_result_callback(
        byte_input,
        bad_kb_scene_config_usb_vidpid_byte_input_callback,
        NULL,
        bad_kb,
        (void*)bad_kb->usb_vidpid_buf,
        sizeof(bad_kb->usb_vidpid_buf));

    view_dispatcher_switch_to_view(bad_kb->view_dispatcher, BadKbAppViewByteInput);
}

bool bad_kb_scene_config_usb_vidpid_on_event(void* context, SceneManagerEvent event) {
    BadKbApp* bad_kb = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        if(event.event == BadKbAppCustomEventByteInputDone) {
            // Set user config and remember
            bad_kb->config.usb.vid = __REVSH(bad_kb->usb_vidpid_buf[0]);
            bad_kb->config.usb.pid = __REVSH(bad_kb->usb_vidpid_buf[1]);
            // Apply to ID config so its temporarily overridden
            if(bad_kb->set_usb_id) {
                bad_kb->id_config.usb.vid = bad_kb->config.usb.vid;
                bad_kb->id_config.usb.pid = bad_kb->config.usb.pid;
            }
            bad_kb_config_refresh(bad_kb);
        }
        scene_manager_previous_scene(bad_kb->scene_manager);
    }
    return consumed;
}

void bad_kb_scene_config_usb_vidpid_on_exit(void* context) {
    BadKbApp* bad_kb = context;
    ByteInput* byte_input = bad_kb->byte_input;

    byte_input_set_result_callback(byte_input, NULL, NULL, NULL, NULL, 0);
    byte_input_set_header_text(byte_input, "");
}
