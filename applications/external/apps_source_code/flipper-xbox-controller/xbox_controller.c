#include "xbox_controller.h"

#define TAG "XboxControllerApp"

enum XboxControllerSubmenuIndex {
    XboxControllerSubmenuIndexXboxOne,
    XboxControllerSubmenuIndexPower
};

uint32_t usb_hid_exit_confirm_view(void* context) {
    UNUSED(context);
    return UsbHidViewSubmenu;
}

uint32_t usb_hid_exit(void* context) {
    UNUSED(context);
    return VIEW_NONE;
}

void usb_hid_submenu_callback(void* context, uint32_t index) {
    furi_assert(context);
    XboxController* app = context;
    if(index == XboxControllerSubmenuIndexXboxOne) {
        app->view_id = UsbHidViewXboxController;
        view_dispatcher_switch_to_view(app->view_dispatcher, UsbHidViewXboxController);
    } else if(index == XboxControllerSubmenuIndexPower) {
        InfraredMessage* message = malloc(sizeof(InfraredMessage));
        message->protocol = InfraredProtocolNECext;
        message->address = 0xD880;
        message->command = 0xD02F;
        message->repeat = false;
        infrared_send(message, 2);
        free(message);
    }
}

void usb_hid_dialog_callback(DialogExResult result, void* context) {
    furi_assert(context);
    XboxController* app = context;
    if(result == DialogExResultLeft) {
        view_dispatcher_stop(app->view_dispatcher);
    } else if(result == DialogExResultRight) {
        view_dispatcher_switch_to_view(app->view_dispatcher, app->view_id); // Show last view
    } else if(result == DialogExResultCenter) {
        view_dispatcher_switch_to_view(app->view_dispatcher, UsbHidViewSubmenu);
    }
}

XboxController* xbox_controller_app_alloc() {
    XboxController* app = malloc(sizeof(XboxController));

    app->gui = furi_record_open(RECORD_GUI);
    app->notifications = furi_record_open(RECORD_NOTIFICATION);
    app->view_dispatcher = view_dispatcher_alloc();

    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    // Submenu view
    app->submenu = submenu_alloc();
    view_set_orientation(submenu_get_view(app->submenu), ViewOrientationVertical);

    submenu_add_item(
        app->submenu, "Xbox One", XboxControllerSubmenuIndexXboxOne, usb_hid_submenu_callback, app);
    submenu_add_item(
        app->submenu, "Power", XboxControllerSubmenuIndexPower, usb_hid_submenu_callback, app);
    view_set_previous_callback(submenu_get_view(app->submenu), usb_hid_exit);
    view_dispatcher_add_view(
        app->view_dispatcher, UsbHidViewSubmenu, submenu_get_view(app->submenu));

    // Xbox Controller View
    app->xbox_controller_view = xbox_controller_view_alloc(app->notifications);
    view_set_previous_callback(
        xbox_controller_view_get_view(app->xbox_controller_view), usb_hid_exit_confirm_view);
    view_dispatcher_add_view(
        app->view_dispatcher,
        UsbHidViewXboxController,
        xbox_controller_view_get_view(app->xbox_controller_view));

    // Switch to Xbox Controller by default
    app->view_id = UsbHidViewSubmenu;
    view_dispatcher_switch_to_view(app->view_dispatcher, app->view_id);

    return app;
}

void xbox_controller_app_free(XboxController* app) {
    furi_assert(app);

    // Reset notification
    notification_internal_message(app->notifications, &sequence_reset_blue);

    // Free views
    view_dispatcher_remove_view(app->view_dispatcher, UsbHidViewSubmenu);
    submenu_free(app->submenu);
    view_dispatcher_remove_view(app->view_dispatcher, UsbHidViewXboxController);
    xbox_controller_view_free(app->xbox_controller_view);
    view_dispatcher_free(app->view_dispatcher);

    // Close records
    furi_record_close(RECORD_GUI);
    app->gui = NULL;
    furi_record_close(RECORD_NOTIFICATION);
    app->notifications = NULL;

    // Free rest
    free(app);
}

int32_t xbox_controller_app(void* p) {
    UNUSED(p);

    XboxController* app = xbox_controller_app_alloc();
    view_dispatcher_run(app->view_dispatcher);

    xbox_controller_app_free(app);

    return 0;
}
