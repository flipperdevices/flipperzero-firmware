#include "../lfrfid_i.h"
#include <dialogs/dialogs.h>
#include "../helpers/lfrfid_dialog.h"

void lfrfid_scene_setpw_t5577_confirm_on_enter(void* context) {
    LfRfid* app = context;
    //SceneManager* scene_manager = app->scene_manager;

    //char msg[100];
    //snprintf (msg, 100, "%d:%d:%d:%d", app->new_password[0], app->new_password[1],
    //	app->new_password[2], app->new_password[3]);
    //lfrfid_alert_dialog(context, msg);
    //

    //DialogMessage* message = dialog_message_alloc();
    DialogMessageButton dialog_result;

    dialog_result = lfrfid_message_dialog(
        context,
        "T5577 write settings",
        "Set tag configuration block read-only? If unsure answer no.",
        "No",
        NULL,
        "Yes");
    if(dialog_result == DialogMessageButtonRight)
        app->extra_options |= LfRfidSetConfigurationLockBit;
    else
        app->extra_options &= ~LfRfidSetConfigurationLockBit;

    dialog_result = lfrfid_message_dialog(
        context,
        "T5577 write settings",
        " Set password block read-only? If unsure answer no.",
        "No",
        NULL,
        "Yes");
    if(dialog_result == DialogMessageButtonRight)
        app->extra_options |= LfRfidSetPasswordLockBit;
    else
        app->extra_options &= ~LfRfidSetPasswordLockBit;

    dialog_result = lfrfid_message_dialog(
        context,
        "T5577 write settings",
        "Set master key 6? Disables test-mode access. Some t5577 clones go read-only. If unsure answer no.",
        "No",
        NULL,
        "Yes");
    if(dialog_result == DialogMessageButtonRight)
        app->extra_options |= LfRfidSetMasterKeyDisableTestMode;
    else
        app->extra_options &= ~LfRfidSetMasterKeyDisableTestMode;

    Widget* widget = app->widget;

    widget_add_button_element(widget, GuiButtonTypeLeft, "Exit", lfrfid_widget_callback, app);
    widget_add_button_element(widget, GuiButtonTypeRight, "Start", lfrfid_widget_callback, app);
    widget_add_string_multiline_element(
        widget, 64, 22, AlignCenter, AlignBottom, FontPrimary, "Apply tag to\nFlipper's back");
    widget_add_string_multiline_element(
        widget,
        64,
        45,
        AlignCenter,
        AlignBottom,
        FontSecondary,
        "And don't move it\nwhile process is running");

    view_dispatcher_switch_to_view(app->view_dispatcher, LfRfidViewWidget);
}

bool lfrfid_scene_setpw_t5577_confirm_on_event(void* context, SceneManagerEvent event) {
    LfRfid* app = context;
    SceneManager* scene_manager = app->scene_manager;
    bool consumed = false;

    //if (app->setting_hex == LfRfidSettingNewPassword)
    //	scene_manager_next_scene(scene_manager, LfRfidSceneEnterHexPw);

    if(event.type == SceneManagerEventTypeBack) {
        consumed = true; // Ignore Back button presses
    } else if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        if(event.event == GuiButtonTypeLeft) {
            scene_manager_search_and_switch_to_previous_scene(
                scene_manager, LfRfidSceneExtraActions);
        } else if(event.event == GuiButtonTypeRight) {
            scene_manager_next_scene(scene_manager, LfRfidSceneSetPwT5577);
        }
    }

    return consumed;
}

void lfrfid_scene_setpw_t5577_confirm_on_exit(void* context) {
    LfRfid* app = context;
    widget_reset(app->widget);
}
