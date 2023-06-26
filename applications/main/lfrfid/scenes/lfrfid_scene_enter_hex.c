#include "../lfrfid_i.h"

void lfrfid_scene_enter_hex_on_enter(void* context) {
    LfRfid* app = context;
    ByteInput* byte_input = app->byte_input;

    size_t size = sizeof(uint32_t);

    if(app->setting_hex == LfRfidSettingCurrentPassword)
        byte_input_set_header_text(byte_input, "Enter the current pw in hex");
    else if(app->setting_hex == LfRfidSettingNewPassword)
        byte_input_set_header_text(byte_input, "Enter the new password in hex");
    else // LfRfidSettingHexGeneric
        byte_input_set_header_text(byte_input, "Enter the block data in hex");

    uint8_t* bytes;
    if(app->setting_hex == LfRfidSettingCurrentPassword)
        bytes = app->password;
    else if(app->setting_hex == LfRfidSettingNewPassword)
        bytes = app->new_password;
    else // LfRfidSettingHexGeneric
        bytes = app->new_key_data;
    byte_input_set_result_callback(byte_input, lfrfid_text_input_callback, NULL, app, bytes, size);

    view_dispatcher_switch_to_view(app->view_dispatcher, LfRfidViewByteInput);
}

bool lfrfid_scene_enter_hex_on_event(void* context, SceneManagerEvent event) {
    LfRfid* app = context;
    SceneManager* scene_manager = app->scene_manager;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == LfRfidEventNext) {
            consumed = true;
            /* if (app->extra_options & LfRfidReadBlockMode) {
		scene_manager_next_scene(scene_manager, LfRfidSceneReadBlockT5577Confirm);
	    } else */
            if(app->extra_options & LfRfidWriteBlockMode) {
                if(app->setting_hex == LfRfidSettingCurrentPassword) {
                    app->setting_hex = LfRfidSettingHexGeneric;
                    scene_manager_next_scene(scene_manager, LfRfidSceneEnterHex);
                } else
                    scene_manager_next_scene(scene_manager, LfRfidSceneWriteBlockT5577Confirm);
            } else {
                if((app->setting_hex == LfRfidSettingCurrentPassword) &&
                   !(app->extra_options & LfRfidDisablePasswordMode)) {
                    app->setting_hex = LfRfidSettingNewPassword;
                    scene_manager_next_scene(scene_manager, LfRfidSceneEnterHex);
                } else
                    scene_manager_next_scene(scene_manager, LfRfidSceneSetPwT5577Confirm);
            }
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_next_scene(scene_manager, LfRfidSceneExtraActions);
    }

    return consumed;
}

void lfrfid_scene_enter_hex_on_exit(void* context) {
    UNUSED(context);
}
