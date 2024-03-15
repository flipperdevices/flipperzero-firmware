#include "../bad_kb_app_i.h"

static void bad_kb_scene_config_usb_name_text_input_callback(void* context) {
    BadKbApp* bad_kb = context;

    view_dispatcher_send_custom_event(bad_kb->view_dispatcher, BadKbAppCustomEventTextInputDone);
}

void bad_kb_scene_config_usb_name_on_enter(void* context) {
    BadKbApp* bad_kb = context;
    TextInput* text_input = bad_kb->text_input;

    if(scene_manager_get_scene_state(bad_kb->scene_manager, BadKbSceneConfigUsbName)) {
        strlcpy(
            bad_kb->usb_name_buf,
            bad_kb->set_usb_id ? bad_kb->id_config.usb.manuf : bad_kb->config.usb.manuf,
            sizeof(bad_kb->usb_name_buf));
        text_input_set_header_text(text_input, "Set USB manufacturer name");
    } else {
        strlcpy(
            bad_kb->usb_name_buf,
            bad_kb->set_usb_id ? bad_kb->id_config.usb.product : bad_kb->config.usb.product,
            sizeof(bad_kb->usb_name_buf));
        text_input_set_header_text(text_input, "Set USB product name");
    }

    text_input_set_result_callback(
        text_input,
        bad_kb_scene_config_usb_name_text_input_callback,
        bad_kb,
        bad_kb->usb_name_buf,
        sizeof(bad_kb->usb_name_buf),
        true);

    view_dispatcher_switch_to_view(bad_kb->view_dispatcher, BadKbAppViewTextInput);
}

bool bad_kb_scene_config_usb_name_on_event(void* context, SceneManagerEvent event) {
    BadKbApp* bad_kb = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        if(event.event == BadKbAppCustomEventTextInputDone) {
            if(scene_manager_get_scene_state(bad_kb->scene_manager, BadKbSceneConfigUsbName)) {
                // Set user config and remember
                strlcpy(
                    bad_kb->config.usb.manuf,
                    bad_kb->usb_name_buf,
                    sizeof(bad_kb->config.usb.manuf));
                // Apply to ID config so its temporarily overridden
                if(bad_kb->set_usb_id) {
                    strlcpy(
                        bad_kb->id_config.usb.manuf,
                        bad_kb->usb_name_buf,
                        sizeof(bad_kb->id_config.usb.manuf));
                }
            } else {
                // Set user config and remember
                strlcpy(
                    bad_kb->config.usb.product,
                    bad_kb->usb_name_buf,
                    sizeof(bad_kb->config.usb.product));
                // Apply to ID config so its temporarily overridden
                if(bad_kb->set_usb_id) {
                    strlcpy(
                        bad_kb->id_config.usb.product,
                        bad_kb->usb_name_buf,
                        sizeof(bad_kb->id_config.usb.product));
                }
            }
            bad_kb_config_refresh(bad_kb);
        }
        scene_manager_previous_scene(bad_kb->scene_manager);
    }
    return consumed;
}

void bad_kb_scene_config_usb_name_on_exit(void* context) {
    BadKbApp* bad_kb = context;
    TextInput* text_input = bad_kb->text_input;

    text_input_reset(text_input);
}
