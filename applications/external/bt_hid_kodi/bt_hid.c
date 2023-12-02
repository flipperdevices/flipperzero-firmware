#include "bt_hid.h"
#include "bt_hid_kodi_icons.h"
#include <furi_hal_bt.h>
#include <notification/notification_messages.h>

#define TAG "BtHidApp"

void bt_hid_dialog_callback(DialogExResult result, void* context) {
    furi_assert(context);
    BtHid* app = context;
    if(result == DialogExResultLeft) {
        view_dispatcher_stop(app->view_dispatcher);
    } else if(result == DialogExResultRight) {
        view_dispatcher_switch_to_view(app->view_dispatcher, app->view_id); // Show last view
    }
}

uint32_t bt_hid_exit_confirm_view(void* context) {
    UNUSED(context);
    return BtHidViewExitConfirm;
}

uint32_t bt_hid_exit(void* context) {
    UNUSED(context);
    return VIEW_NONE;
}

void bt_hid_connection_status_changed_callback(BtStatus status, void* context) {
    furi_assert(context);
    BtHid* bt_hid = context;
    bool connected = (status == BtStatusConnected);
    if(connected) {
        notification_internal_message(bt_hid->notifications, &sequence_set_blue_255);
    } else {
        notification_internal_message(bt_hid->notifications, &sequence_reset_blue);
    }
    bt_hid_keynote_set_connected_status(bt_hid->bt_hid_keynote, connected);
}

BtHid* bt_hid_app_alloc() {
    BtHid* app = malloc(sizeof(BtHid));

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

    // Dialog view
    app->dialog = dialog_ex_alloc();
    dialog_ex_set_result_callback(app->dialog, bt_hid_dialog_callback);
    dialog_ex_set_context(app->dialog, app);
    dialog_ex_set_left_button_text(app->dialog, "Exit");
    dialog_ex_set_right_button_text(app->dialog, "Stay");
    dialog_ex_set_header(app->dialog, "Close Current App?", 16, 12, AlignLeft, AlignTop);
    view_dispatcher_add_view(
        app->view_dispatcher, BtHidViewExitConfirm, dialog_ex_get_view(app->dialog));

    // Keynote view
    app->bt_hid_keynote = bt_hid_keynote_alloc();
    view_set_previous_callback(
        bt_hid_keynote_get_view(app->bt_hid_keynote), bt_hid_exit_confirm_view);
    view_dispatcher_add_view(
        app->view_dispatcher, BtHidViewKeynote, bt_hid_keynote_get_view(app->bt_hid_keynote));

    app->view_id = BtHidViewKeynote;
    view_dispatcher_switch_to_view(app->view_dispatcher, app->view_id);

    return app;
}

void bt_hid_app_free(BtHid* app) {
    furi_assert(app);

    // Reset notification
    notification_internal_message(app->notifications, &sequence_reset_blue);

    // Free views
    view_dispatcher_remove_view(app->view_dispatcher, BtHidViewExitConfirm);
    dialog_ex_free(app->dialog);
    view_dispatcher_remove_view(app->view_dispatcher, BtHidViewKeynote);
    bt_hid_keynote_free(app->bt_hid_keynote);

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

int32_t bt_hid_kodi(void* p) {
    UNUSED(p);
    // Switch profile to Hid
    BtHid* app = bt_hid_app_alloc();
    bt_set_status_changed_callback(app->bt, bt_hid_connection_status_changed_callback, app);
    // Change profile
    if(!bt_set_profile(app->bt, BtProfileHidKeyboard)) {
        FURI_LOG_E(TAG, "Failed to switch profile");
        bt_hid_app_free(app);
        return -1;
    }
    furi_hal_bt_start_advertising();

    view_dispatcher_run(app->view_dispatcher);

    bt_set_status_changed_callback(app->bt, NULL, NULL);
    // Change back profile to Serial
    bt_set_profile(app->bt, BtProfileSerial);

    bt_hid_app_free(app);

    return 0;
}
