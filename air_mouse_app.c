#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_bt_hid.h>
#include <bt/bt_service/bt.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/modules/dialog_ex.h>
#include "imu_mouse.h"
#include "views/air_mouse_view.h"
#include <furi_hal_usb_hid.h>
#include <storage/storage.h>

#define TAG "SensorModule"

#define AIRMOUSE_BT_KEYS_STORAGE_NAME ".airm_bt_hid.keys"

typedef struct {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    Submenu* start_submenu;
    DialogEx* error_dialog;
    AirMouseView* air_mouse_view;
    FuriHalSpiBusHandle* icm42688p_device;
    ICM42688P* icm42688p;
    FuriHalUsbInterface* usb_mode_prev;
    Bt* bt;
} AirMouseApp;

typedef enum {
    AirMouseViewError,
    AirMouseViewStartSubmenu,
    AirMouseViewMain,
} AirMouseViews;

enum StertSubmenuIndex {
    StartSubmenuIndexUsb,
    StartSubmenuIndexBle,
    StartSubmenuIndexBleReset,
};

static const ImuHidApi hid_api_usb = {
    .mouse_move = furi_hal_hid_mouse_move,
    .mouse_key_press = furi_hal_hid_mouse_press,
    .mouse_key_release = furi_hal_hid_mouse_release,
    .mouse_scroll = furi_hal_hid_mouse_scroll,
    .report_rate_max = 200,
};

static const ImuHidApi hid_api_ble = {
    .mouse_move = furi_hal_bt_hid_mouse_move,
    .mouse_key_press = furi_hal_bt_hid_mouse_press,
    .mouse_key_release = furi_hal_bt_hid_mouse_release,
    .mouse_scroll = furi_hal_bt_hid_mouse_scroll,
    .report_rate_max = 30,
};

static void ble_hid_remove_pairing(void) {
    Bt* bt = furi_record_open(RECORD_BT);
    bt_disconnect(bt);

    // Wait 2nd core to update nvm storage
    furi_delay_ms(200);

    bt_keys_storage_set_storage_path(bt, APP_DATA_PATH(AIRMOUSE_BT_KEYS_STORAGE_NAME));
    bt_forget_bonded_devices(bt);

    // Wait 2nd core to update nvm storage
    furi_delay_ms(200);
    bt_keys_storage_set_default_path(bt);

    furi_check(bt_set_profile(bt, BtProfileSerial));
    furi_record_close(RECORD_BT);
}

static void ble_hid_connection_status_callback(BtStatus status, void* context) {
    furi_assert(context);
    AirMouseApp* app = context;
    bool connected = (status == BtStatusConnected);
    air_mouse_view_set_connected_status(app->air_mouse_view, connected);
}

static Bt* ble_hid_init(AirMouseApp* app) {
    Bt* bt = furi_record_open(RECORD_BT);
    bt_disconnect(bt);

    // Wait 2nd core to update nvm storage
    furi_delay_ms(200);

    bt_keys_storage_set_storage_path(bt, APP_DATA_PATH(AIRMOUSE_BT_KEYS_STORAGE_NAME));

    furi_check(bt_set_profile(bt, BtProfileHidKeyboard));

    furi_hal_bt_start_advertising();
    bt_set_status_changed_callback(bt, ble_hid_connection_status_callback, app);
    return bt;
}

static void ble_hid_deinit(Bt* bt) {
    bt_set_status_changed_callback(bt, NULL, NULL);
    bt_disconnect(bt);

    // Wait 2nd core to update nvm storage
    furi_delay_ms(200);
    bt_keys_storage_set_default_path(bt);

    furi_check(bt_set_profile(bt, BtProfileSerial));
    furi_record_close(RECORD_BT);
}

static uint32_t air_mouse_exit(void* context) {
    UNUSED(context);
    return VIEW_NONE;
}

static uint32_t air_mouse_return_to_menu(void* context) {
    UNUSED(context);
    return AirMouseViewStartSubmenu;
}

static void air_mouse_hid_deinit(void* context) {
    furi_assert(context);
    AirMouseApp* app = context;

    if(app->bt) {
        ble_hid_deinit(app->bt);
        app->bt = NULL;
    } else if(app->usb_mode_prev) {
        furi_hal_usb_set_config(app->usb_mode_prev, NULL);
        app->usb_mode_prev = NULL;
    }
}

static void air_mouse_submenu_callback(void* context, uint32_t index) {
    furi_assert(context);
    AirMouseApp* app = context;
    if(index == StartSubmenuIndexUsb) {
        app->usb_mode_prev = furi_hal_usb_get_config();
        furi_hal_usb_unlock();
        furi_hal_usb_set_config(&usb_hid, NULL);

        air_mouse_view_set_hid_api(app->air_mouse_view, &hid_api_usb, false);
        view_dispatcher_switch_to_view(app->view_dispatcher, AirMouseViewMain);
    } else if(index == StartSubmenuIndexBle) {
        app->bt = ble_hid_init(app);

        air_mouse_view_set_hid_api(app->air_mouse_view, &hid_api_ble, true);
        view_dispatcher_switch_to_view(app->view_dispatcher, AirMouseViewMain);
    } else if(index == StartSubmenuIndexBleReset) {
        ble_hid_remove_pairing();
    }
}

static AirMouseApp* air_mouse_alloc(void) {
    AirMouseApp* app = malloc(sizeof(AirMouseApp));

    app->gui = furi_record_open(RECORD_GUI);
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    app->air_mouse_view = air_mouse_view_alloc(air_mouse_hid_deinit, app);
    view_set_previous_callback(
        air_mouse_view_get_view(app->air_mouse_view), air_mouse_return_to_menu);
    view_dispatcher_add_view(
        app->view_dispatcher, AirMouseViewMain, air_mouse_view_get_view(app->air_mouse_view));

    app->start_submenu = submenu_alloc();
    submenu_add_item(
        app->start_submenu, "USB Remote", StartSubmenuIndexUsb, air_mouse_submenu_callback, app);
    submenu_add_item(
        app->start_submenu,
        "Bluetooth Remote",
        StartSubmenuIndexBle,
        air_mouse_submenu_callback,
        app);
    submenu_add_item(
        app->start_submenu,
        "Remove Pairing",
        StartSubmenuIndexBleReset,
        air_mouse_submenu_callback,
        app);
    view_set_previous_callback(submenu_get_view(app->start_submenu), air_mouse_exit);
    view_dispatcher_add_view(
        app->view_dispatcher, AirMouseViewStartSubmenu, submenu_get_view(app->start_submenu));

    app->error_dialog = dialog_ex_alloc();
    dialog_ex_set_header(app->error_dialog, "Sensor Module error", 63, 0, AlignCenter, AlignTop);
    dialog_ex_set_text(app->error_dialog, "Module not conntected", 63, 30, AlignCenter, AlignTop);
    view_set_previous_callback(dialog_ex_get_view(app->error_dialog), air_mouse_exit);
    view_dispatcher_add_view(
        app->view_dispatcher, AirMouseViewError, dialog_ex_get_view(app->error_dialog));

    return app;
}

static void air_mouse_free(AirMouseApp* app) {
    view_dispatcher_remove_view(app->view_dispatcher, AirMouseViewStartSubmenu);
    submenu_free(app->start_submenu);
    view_dispatcher_remove_view(app->view_dispatcher, AirMouseViewError);
    dialog_ex_free(app->error_dialog);
    view_dispatcher_remove_view(app->view_dispatcher, AirMouseViewMain);
    air_mouse_view_free(app->air_mouse_view);

    view_dispatcher_free(app->view_dispatcher);

    furi_record_close(RECORD_GUI);

    free(app);
}

int32_t air_mouse_app(void* arg) {
    UNUSED(arg);
    AirMouseApp* app = air_mouse_alloc();

    app->icm42688p_device = malloc(sizeof(FuriHalSpiBusHandle));
    memcpy(app->icm42688p_device, &furi_hal_spi_bus_handle_external, sizeof(FuriHalSpiBusHandle));
    app->icm42688p_device->cs = &gpio_ext_pc3;
    app->icm42688p = icm42688p_alloc(app->icm42688p_device, &gpio_ext_pb2);
    bool icm42688p_valid = icm42688p_init(app->icm42688p);

    if(icm42688p_valid) {
        air_mouse_view_set_device(app->air_mouse_view, app->icm42688p);
        view_dispatcher_switch_to_view(app->view_dispatcher, AirMouseViewStartSubmenu);
    } else {
        view_dispatcher_switch_to_view(app->view_dispatcher, AirMouseViewError);
    }

    view_dispatcher_run(app->view_dispatcher);

    if(!icm42688p_deinit(app->icm42688p)) {
        FURI_LOG_E(TAG, "Failed to deinitialize ICM42688P");
    }

    icm42688p_free(app->icm42688p);
    free(app->icm42688p_device);

    air_mouse_free(app);
    return 0;
}
