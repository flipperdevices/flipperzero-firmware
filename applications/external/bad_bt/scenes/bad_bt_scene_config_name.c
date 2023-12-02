#include "../bad_bt_app.h"

static void bad_bt_scene_config_name_text_input_callback(void* context) {
    BadBtApp* bad_bt = context;

    view_dispatcher_send_custom_event(bad_bt->view_dispatcher, BadBtAppCustomEventTextInputDone);
}

void bad_bt_scene_config_name_on_enter(void* context) {
    BadBtApp* bad_bt = context;
    TextInput* text_input = bad_bt->text_input;

    strlcpy(bad_bt->bt_name_buf, bad_bt->config.bt_name, BAD_BT_NAME_LEN);
    text_input_set_header_text(text_input, "Set BT device name");

    text_input_set_result_callback(
        text_input,
        bad_bt_scene_config_name_text_input_callback,
        bad_bt,
        bad_bt->bt_name_buf,
        BAD_BT_NAME_LEN,
        true);

    view_dispatcher_switch_to_view(bad_bt->view_dispatcher, BadBtAppViewTextInput);
}

bool bad_bt_scene_config_name_on_event(void* context, SceneManagerEvent event) {
    BadBtApp* bad_bt = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        if(event.event == BadBtAppCustomEventTextInputDone) {
            strlcpy(bad_bt->config.bt_name, bad_bt->bt_name_buf, BAD_BT_NAME_LEN);
            bad_bt_config_refresh(bad_bt);
        }
        scene_manager_previous_scene(bad_bt->scene_manager);
    }
    return consumed;
}

void bad_bt_scene_config_name_on_exit(void* context) {
    BadBtApp* bad_bt = context;
    TextInput* text_input = bad_bt->text_input;

    text_input_reset(text_input);
}
