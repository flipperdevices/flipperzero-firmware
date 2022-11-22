#include "hid.h"
#include "views.h"
#include <furi_hal_bt.h>
#include <furi_hal_usb.h>
#include <notification/notification_messages.h>
#include <dolphin/dolphin.h>

#define TAG "HidApp"

enum HidDebugSubmenuIndex {
    HidSubmenuIndexKeynote,
    HidSubmenuIndexKeyboard,
    HidSubmenuIndexMedia,
    BtHidSubmenuIndexTikTok,
    HidSubmenuIndexMouse,
};
typedef enum { ConnTypeSubmenuIndexBluetooth, ConnTypeSubmenuIndexUsb } ConnTypeDebugSubmenuIndex;

void hid_submenu_callback(void* context, uint32_t index) {
    furi_assert(context);
    Hid* app = context;
    if(index == HidSubmenuIndexKeynote) {
        app->view_id = HidViewKeynote;
        view_dispatcher_switch_to_view(app->view_dispatcher, HidViewKeynote);
    } else if(index == HidSubmenuIndexKeyboard) {
        app->view_id = HidViewKeyboard;
        view_dispatcher_switch_to_view(app->view_dispatcher, HidViewKeyboard);
    } else if(index == HidSubmenuIndexMedia) {
        app->view_id = HidViewMedia;
        view_dispatcher_switch_to_view(app->view_dispatcher, HidViewMedia);
    } else if(index == HidSubmenuIndexMouse) {
        app->view_id = HidViewMouse;
        view_dispatcher_switch_to_view(app->view_dispatcher, HidViewMouse);
    } else if(index == BtHidSubmenuIndexTikTok) {
        app->view_id = BtHidViewTikTok;
        view_dispatcher_switch_to_view(app->view_dispatcher, BtHidViewTikTok);
    }
}

void bt_hid_connection_status_changed_callback(BtStatus status, void* context) {
    furi_assert(context);
    Hid* hid = context;
    bool connected = (status == BtStatusConnected);
    if(connected) {
        notification_internal_message(hid->notifications, &sequence_set_blue_255);
    } else {
        notification_internal_message(hid->notifications, &sequence_reset_blue);
    }
    hid_keynote_set_connected_status(hid->hid_keynote, connected);
    hid_keyboard_set_connected_status(hid->hid_keyboard, connected);
    hid_media_set_connected_status(hid->hid_media, connected);
    hid_mouse_set_connected_status(hid->hid_mouse, connected);
    bt_hid_tiktok_set_connected_status(hid->bt_hid_tiktok, connected);
}
void hid_conn_type_submenu_callback(void* context, uint32_t index) {
    furi_assert(context);
    Hid* app = context;
    app->view_id = HidViewSubmenu;
    if(index == ConnTypeSubmenuIndexBluetooth) {
        FURI_LOG_D(TAG, "Selected bluetooth profile");
        app->is_bluetooth = true;
        // Change profile
        if(!bt_set_profile(app->bt, BtProfileHidKeyboard)) {
            FURI_LOG_E(TAG, "Failed to switch profile");
        }
        furi_hal_bt_start_advertising();
        submenu_add_item(
            app->device_type_submenu,
            "TikTok Controller",
            BtHidSubmenuIndexTikTok,
            hid_submenu_callback,
            app);
        app->hid_conn_selected = true;
        // Set is_bluetooth to true for devices
        bt_set_status_changed_callback(app->bt, bt_hid_connection_status_changed_callback, app);
    } else if(index == ConnTypeSubmenuIndexUsb) {
        FURI_LOG_D(TAG, "Selected USB profile");
        app->is_bluetooth = false;
        furi_hal_usb_unlock();
        furi_check(furi_hal_usb_set_config(&usb_hid, NULL) == true);
        app->hid_conn_selected = true;
    } else {
        furi_crash("Neither connection type specified");
    }
    hid_keyboard_set_conn_type(app->hid_keyboard, app->is_bluetooth);
    hid_keynote_set_conn_type(app->hid_keynote, app->is_bluetooth);
    hid_media_set_conn_type(app->hid_media, app->is_bluetooth);
    hid_mouse_set_conn_type(app->hid_mouse, app->is_bluetooth);
    view_dispatcher_switch_to_view(app->view_dispatcher, HidViewSubmenu);
}
void hid_dialog_callback(DialogExResult result, void* context) {
    furi_assert(context);
    Hid* app = context;
    if(result == DialogExResultLeft) {
        view_dispatcher_stop(app->view_dispatcher);
    } else if(result == DialogExResultRight) {
        view_dispatcher_switch_to_view(app->view_dispatcher, app->view_id); // Show last view
    } else if(result == DialogExResultCenter) {
        view_dispatcher_switch_to_view(app->view_dispatcher, HidViewSubmenu);
    }
}

uint32_t hid_exit_confirm_view(void* context) {
    UNUSED(context);
    return HidViewExitConfirm;
}

uint32_t hid_exit(void* context) {
    UNUSED(context);
    return VIEW_NONE;
}

Hid* hid_app_alloc_submenu() {
    Hid* app = malloc(sizeof(Hid));
    app->hid_conn_selected = false;

    // Gui
    app->gui = furi_record_open(RECORD_GUI);

    // Bt
    app->bt = furi_record_open(RECORD_BT);

    // Notifications
    app->notifications = furi_record_open(RECORD_NOTIFICATION);

    // View dispatcher
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);
    // Connection Type Submenu view
    app->conn_type_submenu = submenu_alloc();
    submenu_add_item(
        app->conn_type_submenu,
        "Bluetooth",
        ConnTypeSubmenuIndexBluetooth,
        hid_conn_type_submenu_callback,
        app);
    submenu_add_item(
        app->conn_type_submenu,
        "USB",
        ConnTypeSubmenuIndexUsb,
        hid_conn_type_submenu_callback,
        app);
    view_set_previous_callback(submenu_get_view(app->conn_type_submenu), hid_exit);
    view_dispatcher_add_view(
        app->view_dispatcher, HidViewConnTypeSubMenu, submenu_get_view(app->conn_type_submenu));
    // Device Type Submenu view
    app->device_type_submenu = submenu_alloc();
    submenu_add_item(
        app->device_type_submenu, "Keynote", HidSubmenuIndexKeynote, hid_submenu_callback, app);
    submenu_add_item(
        app->device_type_submenu, "Keyboard", HidSubmenuIndexKeyboard, hid_submenu_callback, app);
    submenu_add_item(
        app->device_type_submenu, "Media", HidSubmenuIndexMedia, hid_submenu_callback, app);
    submenu_add_item(
        app->device_type_submenu, "Mouse", HidSubmenuIndexMouse, hid_submenu_callback, app);
    view_set_previous_callback(submenu_get_view(app->device_type_submenu), hid_exit);
    view_dispatcher_add_view(
        app->view_dispatcher, HidViewSubmenu, submenu_get_view(app->device_type_submenu));
    app->view_id = HidViewConnTypeSubMenu;
    view_dispatcher_switch_to_view(app->view_dispatcher, app->view_id);
    return app;
}
Hid* hid_app_alloc_view(void* context) {
    furi_assert(context);
    Hid* app = context;
    // Dialog view
    app->dialog = dialog_ex_alloc();
    dialog_ex_set_result_callback(app->dialog, hid_dialog_callback);
    dialog_ex_set_context(app->dialog, app);
    dialog_ex_set_left_button_text(app->dialog, "Exit");
    dialog_ex_set_right_button_text(app->dialog, "Stay");
    dialog_ex_set_center_button_text(app->dialog, "Menu");
    dialog_ex_set_header(app->dialog, "Close Current App?", 16, 12, AlignLeft, AlignTop);
    view_dispatcher_add_view(
        app->view_dispatcher, HidViewExitConfirm, dialog_ex_get_view(app->dialog));

    // Keynote view
    app->hid_keynote = hid_keynote_alloc(app);
    view_set_previous_callback(hid_keynote_get_view(app->hid_keynote), hid_exit_confirm_view);
    view_dispatcher_add_view(
        app->view_dispatcher, HidViewKeynote, hid_keynote_get_view(app->hid_keynote));

    // Keyboard view
    app->hid_keyboard = hid_keyboard_alloc(app);
    view_set_previous_callback(hid_keyboard_get_view(app->hid_keyboard), hid_exit_confirm_view);
    view_dispatcher_add_view(
        app->view_dispatcher, HidViewKeyboard, hid_keyboard_get_view(app->hid_keyboard));

    // Media view
    app->hid_media = hid_media_alloc(app);
    view_set_previous_callback(hid_media_get_view(app->hid_media), hid_exit_confirm_view);
    view_dispatcher_add_view(
        app->view_dispatcher, HidViewMedia, hid_media_get_view(app->hid_media));

    // TikTok view
    app->bt_hid_tiktok = bt_hid_tiktok_alloc();
    view_set_previous_callback(bt_hid_tiktok_get_view(app->bt_hid_tiktok), hid_exit_confirm_view);
    view_dispatcher_add_view(
        app->view_dispatcher, BtHidViewTikTok, bt_hid_tiktok_get_view(app->bt_hid_tiktok));

    // Mouse view
    app->hid_mouse = hid_mouse_alloc(app);
    view_set_previous_callback(hid_mouse_get_view(app->hid_mouse), hid_exit_confirm_view);
    view_dispatcher_add_view(
        app->view_dispatcher, HidViewMouse, hid_mouse_get_view(app->hid_mouse));

    return app;
}

void hid_app_free(Hid* app) {
    furi_assert(app);

    // Reset notification
    notification_internal_message(app->notifications, &sequence_reset_blue);

    // Free views
    view_dispatcher_remove_view(app->view_dispatcher, HidViewSubmenu);
    submenu_free(app->device_type_submenu);
    submenu_free(app->conn_type_submenu);
    view_dispatcher_remove_view(app->view_dispatcher, HidViewExitConfirm);
    dialog_ex_free(app->dialog);
    view_dispatcher_remove_view(app->view_dispatcher, HidViewKeynote);
    hid_keynote_free(app->hid_keynote);
    view_dispatcher_remove_view(app->view_dispatcher, HidViewKeyboard);
    hid_keyboard_free(app->hid_keyboard);
    view_dispatcher_remove_view(app->view_dispatcher, HidViewMedia);
    hid_media_free(app->hid_media);
    view_dispatcher_remove_view(app->view_dispatcher, HidViewMouse);
    hid_mouse_free(app->hid_mouse);
    view_dispatcher_remove_view(app->view_dispatcher, BtHidViewTikTok);
    bt_hid_tiktok_free(app->bt_hid_tiktok);
    view_dispatcher_free(app->view_dispatcher);

    // Close records
    furi_record_close(RECORD_GUI);
    app->gui = NULL;
    furi_record_close(RECORD_NOTIFICATION);
    app->notifications = NULL;
    furi_record_close(RECORD_BT);
    app->bt = NULL;

    // Free rest
    free(app);
}

int32_t hid_app(void* p) {
    UNUSED(p);
    Hid* app = hid_app_alloc_submenu();
    app = hid_app_alloc_view(app);
    FuriHalUsbInterface* usb_mode_prev = furi_hal_usb_get_config();

    DOLPHIN_DEED(DolphinDeedPluginStart);

    view_dispatcher_run(app->view_dispatcher);
    if(app->is_bluetooth) {
        bt_set_status_changed_callback(app->bt, NULL, NULL);
        bt_set_profile(app->bt, BtProfileSerial);
    } else if(!app->is_bluetooth) {
        furi_hal_usb_set_config(usb_mode_prev, NULL);
    }

    hid_app_free(app);

    return 0;
}
