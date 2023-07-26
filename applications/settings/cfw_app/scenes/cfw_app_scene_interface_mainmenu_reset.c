#include "../cfw_app.h"

static void
    cfw_app_scene_interface_mainmenu_reset_dialog_callback(DialogExResult result, void* context) {
    CfwApp* app = context;

    view_dispatcher_send_custom_event(app->view_dispatcher, result);
}

void cfw_app_scene_interface_mainmenu_reset_on_enter(void* context) {
    CfwApp* app = context;
    DialogEx* dialog_ex = app->dialog_ex;

    dialog_ex_set_header(dialog_ex, "Reset Menu Apps?", 64, 10, AlignCenter, AlignCenter);
    dialog_ex_set_text(dialog_ex, "Your edits will be lost!", 64, 32, AlignCenter, AlignCenter);
    dialog_ex_set_left_button_text(dialog_ex, "Cancel");
    dialog_ex_set_right_button_text(dialog_ex, "Reset");

    dialog_ex_set_context(dialog_ex, app);
    dialog_ex_set_result_callback(
        dialog_ex, cfw_app_scene_interface_mainmenu_reset_dialog_callback);

    view_dispatcher_switch_to_view(app->view_dispatcher, CfwAppViewDialogEx);
}

bool cfw_app_scene_interface_mainmenu_reset_on_event(void* context, SceneManagerEvent event) {
    CfwApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case DialogExResultRight:
            storage_common_remove(furi_record_open(RECORD_STORAGE), CFW_MENU_PATH);
            furi_record_close(RECORD_STORAGE);
            app->save_mainmenu_apps = false;
            app->require_reboot = true;
            cfw_app_apply(app);
            break;
        default:
            break;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = true;
    }

    return consumed;
}

void cfw_app_scene_interface_mainmenu_reset_on_exit(void* context) {
    CfwApp* app = context;
    DialogEx* dialog_ex = app->dialog_ex;

    dialog_ex_reset(dialog_ex);
}