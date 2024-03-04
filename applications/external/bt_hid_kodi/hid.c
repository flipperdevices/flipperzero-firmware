#include "hid.h"
#include <extra_profiles/hid_profile.h>
#include <profiles/serial_profile.h>
#include "views.h"
#include <notification/notification_messages.h>
#include "hid_icons.h"

#define TAG "HidApp"

bool hid_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    Hid* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

bool hid_back_event_callback(void* context) {
    furi_assert(context);
    //Hid* app = context;
    FURI_LOG_D("HID", "Back event");
    /*app->view_id = HidViewSubmenu;
    view_dispatcher_switch_to_view(app->view_dispatcher, HidViewSubmenu);*/
    return true;
}

static void bt_hid_connection_status_changed_callback(BtStatus status, void* context) {
    furi_assert(context);
    Hid* hid = context;
    bool connected = (status == BtStatusConnected);
#ifdef HID_TRANSPORT_BLE
    if(connected) {
        notification_internal_message(hid->notifications, &sequence_set_blue_255);
    } else {
        notification_internal_message(hid->notifications, &sequence_reset_blue);
    }
#endif
    hid_keynote_set_connected_status(hid->hid_keynote, connected);
}

static uint32_t hid_exit(void* context) {
    UNUSED(context);
    return VIEW_NONE;
}

Hid* hid_alloc() {
    Hid* app = malloc(sizeof(Hid));

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
    view_dispatcher_set_navigation_event_callback(app->view_dispatcher, hid_back_event_callback);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);

    // Scene Manager
    app->scene_manager = scene_manager_alloc(&hid_scene_handlers, app);

    // Keynote view
    app->hid_keynote = hid_keynote_alloc(app);
    view_dispatcher_add_view(
       app->view_dispatcher, HidViewKeynote, hid_keynote_get_view(app->hid_keynote));

    // Open Keynote view
    app->view_id = HidViewKeynote;
    view_set_previous_callback(hid_keynote_get_view(app->hid_keynote), hid_exit);
    hid_keynote_set_orientation(app->hid_keynote, false);
    view_dispatcher_switch_to_view(app->view_dispatcher, HidViewKeynote);

    return app;
}

Hid* hid_app_alloc_view(void* context) {
    furi_assert(context);
    Hid* app = context;

    return app;
}

void hid_free(Hid* app) {
    furi_assert(app);

    // Reset notification
#ifdef HID_TRANSPORT_BLE
    notification_internal_message(app->notifications, &sequence_reset_blue);
#endif
    // Free views
    view_dispatcher_remove_view(app->view_dispatcher, HidViewKeynote);
    hid_keynote_free(app->hid_keynote);
    scene_manager_free(app->scene_manager);
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

int32_t bt_hid_kodi(void* p) {
    UNUSED(p);
    Hid* app = hid_alloc();
    app = hid_app_alloc_view(app);

    FURI_LOG_D("HID", "Starting as BLE app");

    bt_disconnect(app->bt);

    // Wait 2nd core to update nvm storage
    furi_delay_ms(200);

    // Migrate data from old sd-card folder
    Storage* storage = furi_record_open(RECORD_STORAGE);

    storage_common_migrate(
        storage,
        EXT_PATH("apps/Bluetooth/" HID_BT_KEYS_STORAGE_NAME),
        APP_DATA_PATH(HID_BT_KEYS_STORAGE_NAME));

    bt_keys_storage_set_storage_path(app->bt, APP_DATA_PATH(HID_BT_KEYS_STORAGE_NAME));

    furi_record_close(RECORD_STORAGE);

    app->ble_hid_profile = bt_profile_start(app->bt, ble_profile_hid, NULL);

    furi_check(app->ble_hid_profile);

    furi_hal_bt_start_advertising();
    bt_set_status_changed_callback(app->bt, bt_hid_connection_status_changed_callback, app);

    scene_manager_next_scene(app->scene_manager, HidSceneMain);

    view_dispatcher_run(app->view_dispatcher);

    bt_set_status_changed_callback(app->bt, NULL, NULL);

    bt_disconnect(app->bt);

    // Wait 2nd core to update nvm storage
    furi_delay_ms(200);

    bt_keys_storage_set_default_path(app->bt);

    furi_check(bt_profile_restore_default(app->bt));

    hid_free(app);

    return 0;
}
