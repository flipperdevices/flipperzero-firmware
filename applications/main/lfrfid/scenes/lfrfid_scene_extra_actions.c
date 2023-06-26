#include "../lfrfid_i.h"
#include <dolphin/dolphin.h>
#include <dialogs/dialogs.h>

typedef enum {
    SubmenuIndexASK,
    SubmenuIndexPSK,
    SubmenuIndexClearT5577,
    SubmenuIndexSetPwT5577,
    //SubmenuIndexReadBlockT5577,   //FIX
    SubmenuIndexWriteBlockT5577,
    SubmenuIndexWipeT5577,
    // SubmenuIndexHitag,
    SubmenuIndexRAW,
    SubmenuIndexRAWEmulate,
} SubmenuIndex;

static void lfrfid_scene_extra_actions_submenu_callback(void* context, uint32_t index) {
    LfRfid* app = context;

    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void lfrfid_scene_extra_actions_on_enter(void* context) {
    LfRfid* app = context;
    Submenu* submenu = app->submenu;

    submenu_add_item(
        submenu,
        "Read ASK (FDX,Regular)",
        SubmenuIndexASK,
        lfrfid_scene_extra_actions_submenu_callback,
        app);
    submenu_add_item(
        submenu,
        "Read PSK (Indala)",
        SubmenuIndexPSK,
        lfrfid_scene_extra_actions_submenu_callback,
        app);
    submenu_add_item(
        submenu,
        "Clear T5577 Password",
        SubmenuIndexClearT5577,
        lfrfid_scene_extra_actions_submenu_callback,
        app);
    submenu_add_item(
        submenu,
        "Set/clear T5577 Cust.Pw",
        SubmenuIndexSetPwT5577,
        lfrfid_scene_extra_actions_submenu_callback,
        app);
    submenu_add_item(
        submenu,
        "Write T5577 Block",
        SubmenuIndexWriteBlockT5577,
        lfrfid_scene_extra_actions_submenu_callback,
        app);
    // submenu_add_item(
    //	submenu,
    //	"Read T5577 Block",
    //	SubmenuIndexReadBlockT5577,
    //	lfrfid_scene_extra_actions_submenu_callback,
    //	app);
    submenu_add_item(
        submenu,
        "Wipe T5577",
        SubmenuIndexWipeT5577,
        lfrfid_scene_extra_actions_submenu_callback,
        app);
    // submenu_add_item(
    // submenu,
    // "Read RTF (Reader Talks First)",
    // SubmenuIndexHitag,
    // lfrfid_scene_extra_actions_submenu_callback,
    // app);
    submenu_add_item(
        submenu,
        "Read RAW RFID data",
        SubmenuIndexRAW,
        lfrfid_scene_extra_actions_submenu_callback,
        app);
    submenu_add_item(
        submenu,
        "Emulate RAW RFID data",
        SubmenuIndexRAWEmulate,
        lfrfid_scene_extra_actions_submenu_callback,
        app);

    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(app->scene_manager, LfRfidSceneExtraActions));

    // clear key
    furi_string_reset(app->file_name);
    app->protocol_id = PROTOCOL_NO;
    app->read_type = LFRFIDWorkerReadTypeAuto;

    view_dispatcher_switch_to_view(app->view_dispatcher, LfRfidViewSubmenu);
}

bool lfrfid_scene_extra_actions_on_event(void* context, SceneManagerEvent event) {
    LfRfid* app = context;
    SceneManager* scene_manager = app->scene_manager;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexASK) {
            app->read_type = LFRFIDWorkerReadTypeASKOnly;
            scene_manager_next_scene(app->scene_manager, LfRfidSceneRead);
            dolphin_deed(DolphinDeedRfidRead);
            consumed = true;
        } else if(event.event == SubmenuIndexPSK) {
            app->read_type = LFRFIDWorkerReadTypePSKOnly;
            scene_manager_next_scene(app->scene_manager, LfRfidSceneRead);
            dolphin_deed(DolphinDeedRfidRead);
            consumed = true;
        } else if(event.event == SubmenuIndexClearT5577) {
            scene_manager_next_scene(app->scene_manager, LfRfidSceneClearT5577Confirm);
            consumed = true;

        } else if(event.event == SubmenuIndexSetPwT5577) {
            app->extra_options = 0;
            DialogMessage* message = dialog_message_alloc();
            dialog_message_set_header(message, "T5577 access", 0, 0, AlignLeft, AlignTop);
            dialog_message_set_buttons(message, "Clear", NULL, "Set");
            dialog_message_set_text(
                message,
                "Clear or set password?",
                SCREEN_WIDTH_CENTER,
                SCREEN_HEIGHT_CENTER,
                AlignCenter,
                AlignCenter);
            DialogMessageButton dialog_result = dialog_message_show(app->dialogs, message);
            dialog_message_free(message);
            if(dialog_result == DialogMessageButtonLeft) { //clear pw
                memset(app->new_password, 0, 4);
                app->extra_options |= LfRfidUsePassword;
                app->extra_options |= LfRfidDisablePasswordMode;
                app->setting_hex = LfRfidSettingCurrentPassword;
                scene_manager_next_scene(scene_manager, LfRfidSceneEnterHex);
            } else { // set password
                memset(app->password, 0, 4);
                app->setting_hex = LfRfidSettingNewPassword;
                scene_manager_next_scene(scene_manager, LfRfidSceneEnterHex);
            }
            consumed = true;
        } else if(event.event == SubmenuIndexWriteBlockT5577) {
            app->extra_options = 0;
            app->extra_options |= LfRfidWriteBlockMode;
            DialogMessage* message = dialog_message_alloc();
            dialog_message_set_header(message, "T5577 access", 0, 0, AlignLeft, AlignTop);
            dialog_message_set_buttons(message, "No", NULL, "Yes");
            dialog_message_set_text(
                message,
                "Is tag password protected?",
                SCREEN_WIDTH_CENTER,
                SCREEN_HEIGHT_CENTER,
                AlignCenter,
                AlignCenter);
            DialogMessageButton dialog_result = dialog_message_show(app->dialogs, message);
            dialog_message_free(message);
            if(dialog_result == DialogMessageButtonRight) {
                app->extra_options |= LfRfidUsePassword;
                app->setting_hex = LfRfidSettingCurrentPassword;
                scene_manager_next_scene(scene_manager, LfRfidSceneEnterHex);
            } else {
                memset(app->password, 0, 4);
                app->setting_hex = LfRfidSettingHexGeneric;
                scene_manager_next_scene(scene_manager, LfRfidSceneEnterHex);
            }
            consumed = true;
            //} else if(event.event == SubmenuIndexReadBlockT5577) {
            // FIX, doesn't work, needs at lest new generic protocol
            //    app->extra_options = 0;
            //    app->extra_options |= LfRfidReadBlockMode;
            //    DialogMessage* message = dialog_message_alloc();
            //    dialog_message_set_header(message, "T5577 access", 0, 0, AlignLeft,
            //	AlignTop);
            //    dialog_message_set_buttons(message, "No", NULL, "Yes");
            //    dialog_message_set_text(message, "Is tag password protected?",
            //	SCREEN_WIDTH_CENTER, SCREEN_HEIGHT_CENTER, AlignCenter, AlignCenter);
            //    DialogMessageButton dialog_result = dialog_message_show(app->dialogs, message);
            //    dialog_message_free(message);
            //    if (dialog_result == DialogMessageButtonRight) {
            //	app->extra_options |= LfRfidUsePassword;
            //	app->setting_hex = LfRfidSettingCurrentPassword;
            //	scene_manager_next_scene(scene_manager, LfRfidSceneEnterHex);
            //    } else {
            //	scene_manager_next_scene(app->scene_manager, LfRfidSceneReadBlockT5577Confirm);
            //    }
            //    consumed = true;
        } else if(event.event == SubmenuIndexWipeT5577) {
            app->extra_options = 0;
            //app->extra_options |= LfRfidWipeMode;
            DialogMessage* message = dialog_message_alloc();
            dialog_message_set_header(message, "T5577 reset", 0, 0, AlignLeft, AlignTop);
            dialog_message_set_buttons(message, "No", NULL, "Yes");
            dialog_message_set_text(
                message,
                " This overwrites T5577 user data blocks. Password must be cleared before",
                SCREEN_WIDTH_CENTER,
                SCREEN_HEIGHT_CENTER,
                AlignCenter,
                AlignCenter);
            DialogMessageButton dialog_result = dialog_message_show(app->dialogs, message);
            dialog_message_free(message);
            if(dialog_result == DialogMessageButtonRight) {
                scene_manager_next_scene(scene_manager, LfRfidSceneWipeT5577Confirm);
            }
            consumed = true;
            // } else if(event.event == SubmenuIndexHitag) {
            // app->read_type = LFRFIDWorkerReadTypeRTFOnly;
            // scene_manager_next_scene(app->scene_manager, LfRfidSceneRead);
            // dolphin_deed(DolphinDeedRfidRead);
            // consumed = true;
        } else if(event.event == SubmenuIndexRAW) {
            scene_manager_next_scene(app->scene_manager, LfRfidSceneRawName);
            consumed = true;
        } else if(event.event == SubmenuIndexRAWEmulate) {
            scene_manager_next_scene(app->scene_manager, LfRfidSceneSelectRawKey);
            consumed = true;
        }
        scene_manager_set_scene_state(app->scene_manager, LfRfidSceneExtraActions, event.event);
    }

    return consumed;
}

void lfrfid_scene_extra_actions_on_exit(void* context) {
    LfRfid* app = context;

    submenu_reset(app->submenu);
}
