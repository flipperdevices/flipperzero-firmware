#include "bt_hid.h"
#include <furi-hal-bt.h>

#define TAG "BtHidApp"

enum BtDebugSubmenuIndex {
    BtHidSubmenuIndexKeyboard,
    BtHidSubmenuIndexMedia,
};

void bt_hid_submenu_callback(void* context, uint32_t index) {
    furi_assert(context);
    BtHid* app = context;
    if(index == BtHidSubmenuIndexKeyboard) {
        view_dispatcher_switch_to_view(app->view_dispatcher, BtHidViewKeyboard);
    } else if(index == BtHidSubmenuIndexMedia) {
        view_dispatcher_switch_to_view(app->view_dispatcher, BtHidViewMedia);
    }
}

uint32_t bt_hid_exit(void* context) {
    return VIEW_NONE;
}

uint32_t bt_hid_start_view(void* context) {
    return BtHidViewSubmenu;
}

void bt_hid_connection_status_changed_callback(BtStatus status, void* context) {
    furi_assert(context);
    BtHid* bt_hid = context;
    bt_hid_keyboard_set_connected_status(bt_hid->bt_hid_keyboard, status == BtStatusConnected);
}

BtHid* bt_hid_app_alloc() {
    BtHid* app = furi_alloc(sizeof(BtHid));

    // Gui
    app->gui = furi_record_open("gui");

    // View dispatcher
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    // Views
    app->submenu = submenu_alloc();
    submenu_add_item(
        app->submenu, "Clicker", BtHidSubmenuIndexKeyboard, bt_hid_submenu_callback, app);
    submenu_add_item(
        app->submenu, "Media controller", BtHidSubmenuIndexMedia, bt_hid_submenu_callback, app);
    view_set_previous_callback(submenu_get_view(app->submenu), bt_hid_exit);
    view_dispatcher_add_view(
        app->view_dispatcher, BtHidViewSubmenu, submenu_get_view(app->submenu));
    app->bt_hid_keyboard = bt_hid_keyboard_alloc();
    view_set_previous_callback(bt_hid_keyboard_get_view(app->bt_hid_keyboard), bt_hid_start_view);
    view_dispatcher_add_view(
        app->view_dispatcher, BtHidViewKeyboard, bt_hid_keyboard_get_view(app->bt_hid_keyboard));
    app->bt_hid_media = bt_hid_media_alloc();
    view_set_previous_callback(bt_hid_media_get_view(app->bt_hid_media), bt_hid_start_view);
    view_dispatcher_add_view(
        app->view_dispatcher, BtHidViewMedia, bt_hid_media_get_view(app->bt_hid_media));

    // Switch to menu
    view_dispatcher_switch_to_view(app->view_dispatcher, BtHidViewSubmenu);

    return app;
}

void bt_hid_app_free(BtHid* app) {
    furi_assert(app);

    // Free views
    view_dispatcher_remove_view(app->view_dispatcher, BtHidViewSubmenu);
    submenu_free(app->submenu);
    view_dispatcher_remove_view(app->view_dispatcher, BtHidViewKeyboard);
    bt_hid_keyboard_free(app->bt_hid_keyboard);
    view_dispatcher_remove_view(app->view_dispatcher, BtHidViewMedia);
    bt_hid_media_get_view(app->bt_hid_media);
    view_dispatcher_free(app->view_dispatcher);

    // Close gui record
    furi_record_close("gui");
    app->gui = NULL;

    // Free rest
    free(app);
}

int32_t bt_hid_app(void* p) {
    // Switch profile ti Hid
    BtHid* app = bt_hid_app_alloc();
    Bt* bt = furi_record_open("bt");
    bt_set_status_changed_callback(bt, bt_hid_connection_status_changed_callback, app);
    if(!bt_set_profile(bt, BtProfileHidKeyboard)) {
        FURI_LOG_E(TAG, "Failed to switch profile");
        furi_record_close("bt");
        return -1;
    }
    // Save bt status and start advertising
    bool bt_turned_on = furi_hal_bt_is_active();
    if(!bt_turned_on) {
        furi_hal_bt_start_advertising();
    }

    app->bt = bt;

    view_dispatcher_run(app->view_dispatcher);

    // Stop advertising if bt was off
    if(bt_turned_on) {
        furi_hal_bt_stop_advertising();
    }
    // Change back profile to Serial
    bt_set_profile(bt, BtProfileSerial);
    bt_set_status_changed_callback(app->bt, NULL, NULL);
    furi_record_close("bt");

    bt_hid_app_free(app);

    return 0;
}
