#include "bt_hid.h"
#include <furi-hal-bt.h>

enum BtDebugSubmenuIndex {
    BtHidSubmenuIndexKeyboard,
    BtHidSubmenuIndexMediaController,
};

void bt_hid_submenu_callback(void* context, uint32_t index) {
    furi_assert(context);
    BtHid* app = context;
    if(index == BtHidSubmenuIndexKeyboard) {
        view_dispatcher_switch_to_view(app->view_dispatcher, BtHidViewKeyboard);
    } else if(index == BtHidSubmenuIndexMediaController) {
        view_dispatcher_switch_to_view(app->view_dispatcher, BtHidViewMediaController);
    }
}

uint32_t bt_hid_exit(void* context) {
    return VIEW_NONE;
}

uint32_t bt_hid_start_view(void* context) {
    return BtHidViewSubmenu;
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
        app->submenu,
        "Media controller",
        BtHidSubmenuIndexMediaController,
        bt_hid_submenu_callback,
        app);
    view_set_previous_callback(submenu_get_view(app->submenu), bt_hid_exit);
    view_dispatcher_add_view(
        app->view_dispatcher, BtHidViewSubmenu, submenu_get_view(app->submenu));
    app->bt_hid_keyboard = bt_hid_keyboard_alloc();
    view_set_previous_callback(bt_hid_keyboard_get_view(app->bt_hid_keyboard), bt_hid_start_view);
    view_dispatcher_add_view(
        app->view_dispatcher, BtHidViewKeyboard, bt_hid_keyboard_get_view(app->bt_hid_keyboard));
    // app->bt_packet_test = bt_packet_test_alloc();
    // view_set_previous_callback(bt_packet_test_get_view(app->bt_packet_test), bt_hid_start_view);
    // view_dispatcher_add_view(
    //     app->view_dispatcher,
    //     BtHidViewPacketTest,
    //     bt_packet_test_get_view(app->bt_packet_test));

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
    // view_dispatcher_remove_view(app->view_dispatcher, BtHidViewPacketTest);
    // bt_packet_test_free(app->bt_packet_test);
    view_dispatcher_free(app->view_dispatcher);

    // Close gui record
    furi_record_close("gui");
    app->gui = NULL;

    // Free rest
    free(app);
}

int32_t bt_hid_app(void* p) {
    BtHid* app = bt_hid_app_alloc();

    view_dispatcher_run(app->view_dispatcher);

    bt_hid_app_free(app);

    return 0;
}
