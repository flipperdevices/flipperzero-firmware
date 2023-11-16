#include "usb_ccb.h"
#include <furi.h>
#include <furi_hal.h>
#include <notification/notification_messages.h>

#define TAG "UsbCcbApp"

enum UsbDebugSubmenuIndex {
    UsbCcbSubmenuIndexAbout,
    UsbCcbSubmenuIndexHelp,
    UsbCcbSubmenuIndexStart,
};

void usb_ccb_submenu_callback(void* context, uint32_t index) {
    furi_assert(context);
    UsbCcb* app = context;
    if(index == UsbCcbSubmenuIndexAbout) {
        app->view_id = UsbCcbViewAbout;
        view_dispatcher_switch_to_view(app->view_dispatcher, UsbCcbViewAbout);
    } else if(index == UsbCcbSubmenuIndexHelp) {
        app->view_id = UsbCcbViewHelp;
        view_dispatcher_switch_to_view(app->view_dispatcher, UsbCcbViewHelp);
    } else if(index == UsbCcbSubmenuIndexStart) {
        app->view_id = UsbCcbViewStart;
        view_dispatcher_switch_to_view(app->view_dispatcher, UsbCcbViewStart);
    }
}

void usb_ccb_dialog_callback(DialogExResult result, void* context) {
    furi_assert(context);
    UsbCcb* app = context;
    if(result == DialogExResultRight) {
        view_dispatcher_stop(app->view_dispatcher);
    } else if(result == DialogExResultLeft) {
        view_dispatcher_switch_to_view(app->view_dispatcher, app->view_id); // Show last view
    } else if(result == DialogExResultCenter) {
        view_dispatcher_switch_to_view(app->view_dispatcher, UsbCcbViewSubmenu);
    }
}

uint32_t usb_ccb_exit_confirm_view(void* context) {
    UNUSED(context);
    return UsbCcbViewExitConfirm;
}

uint32_t usb_ccb_exit(void* context) {
    UNUSED(context);
    return VIEW_NONE;
}

UsbCcb* usb_ccb_app_alloc() {
    UsbCcb* app = malloc(sizeof(UsbCcb));

    // Gui
    app->gui = furi_record_open(RECORD_GUI);

    // Notifications
    app->notifications = furi_record_open(RECORD_NOTIFICATION);

    // View dispatcher
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    // Submenu view
    app->submenu = submenu_alloc();
    submenu_set_header(app->submenu, "USB Consumer Control");
    submenu_add_item(
        app->submenu, "About", UsbCcbSubmenuIndexAbout, usb_ccb_submenu_callback, app);
    submenu_add_item(app->submenu, "Help", UsbCcbSubmenuIndexHelp, usb_ccb_submenu_callback, app);
    submenu_add_item(
        app->submenu, "Start", UsbCcbSubmenuIndexStart, usb_ccb_submenu_callback, app);
    view_set_previous_callback(submenu_get_view(app->submenu), usb_ccb_exit);
    view_dispatcher_add_view(
        app->view_dispatcher, UsbCcbViewSubmenu, submenu_get_view(app->submenu));

    // Dialog view
    app->dialog = dialog_ex_alloc();
    dialog_ex_set_result_callback(app->dialog, usb_ccb_dialog_callback);
    dialog_ex_set_context(app->dialog, app);
    dialog_ex_set_right_button_text(app->dialog, "Exit");
    dialog_ex_set_left_button_text(app->dialog, "Stay");
    dialog_ex_set_center_button_text(app->dialog, "Menu");
    dialog_ex_set_header(app->dialog, "Exit or return to menu?", 64, 11, AlignCenter, AlignTop);
    view_dispatcher_add_view(
        app->view_dispatcher, UsbCcbViewExitConfirm, dialog_ex_get_view(app->dialog));

    // About view
    app->usb_ccb_about = usb_ccb_about_alloc();
    view_set_previous_callback(
        usb_ccb_about_get_view(app->usb_ccb_about), usb_ccb_exit_confirm_view);
    view_dispatcher_add_view(
        app->view_dispatcher, UsbCcbViewAbout, usb_ccb_about_get_view(app->usb_ccb_about));

    // Help view
    app->usb_ccb_help = usb_ccb_help_alloc();
    view_set_previous_callback(
        usb_ccb_help_get_view(app->usb_ccb_help), usb_ccb_exit_confirm_view);
    view_dispatcher_add_view(
        app->view_dispatcher, UsbCcbViewHelp, usb_ccb_help_get_view(app->usb_ccb_help));

    // Start view
    app->usb_ccb_start = usb_ccb_start_alloc();
    view_set_previous_callback(
        usb_ccb_start_get_view(app->usb_ccb_start), usb_ccb_exit_confirm_view);
    view_dispatcher_add_view(
        app->view_dispatcher, UsbCcbViewStart, usb_ccb_start_get_view(app->usb_ccb_start));

    app->view_id = UsbCcbViewSubmenu;
    view_dispatcher_switch_to_view(app->view_dispatcher, app->view_id);

    return app;
}

void usb_ccb_app_free(UsbCcb* app) {
    furi_assert(app);

    // Reset notification
    notification_internal_message(app->notifications, &sequence_reset_blue);

    // Free views
    view_dispatcher_remove_view(app->view_dispatcher, UsbCcbViewSubmenu);
    submenu_free(app->submenu);
    view_dispatcher_remove_view(app->view_dispatcher, UsbCcbViewExitConfirm);
    dialog_ex_free(app->dialog);
    view_dispatcher_remove_view(app->view_dispatcher, UsbCcbViewAbout);
    usb_ccb_about_free(app->usb_ccb_about);
    view_dispatcher_remove_view(app->view_dispatcher, UsbCcbViewHelp);
    usb_ccb_help_free(app->usb_ccb_help);
    view_dispatcher_remove_view(app->view_dispatcher, UsbCcbViewStart);
    usb_ccb_start_free(app->usb_ccb_start);
    view_dispatcher_free(app->view_dispatcher);
    // Close records
    furi_record_close(RECORD_GUI);
    app->gui = NULL;
    furi_record_close(RECORD_NOTIFICATION);
    app->notifications = NULL;

    // Free rest
    free(app);
}

int32_t usb_ccb_app(void* p) {
    UNUSED(p);
    // Switch profile to Hid
    UsbCcb* app = usb_ccb_app_alloc();

    FuriHalUsbInterface* usb_mode_prev = furi_hal_usb_get_config();
    furi_hal_usb_unlock();
    furi_check(furi_hal_usb_set_config(&usb_hid, NULL) == true);

    view_dispatcher_run(app->view_dispatcher);

    // Change back profile
    furi_hal_usb_set_config(usb_mode_prev, NULL);
    usb_ccb_app_free(app);

    return 0;
}