#include "tpms_app_i.h"

#include <furi.h>
#include <furi_hal.h>
#include "protocols/protocol_items.h"

static bool tpms_app_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    TPMSApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool tpms_app_back_event_callback(void* context) {
    furi_assert(context);
    TPMSApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static void tpms_app_tick_event_callback(void* context) {
    furi_assert(context);
    TPMSApp* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

TPMSApp* tpms_app_alloc() {
    TPMSApp* app = malloc(sizeof(TPMSApp));

    // GUI
    app->gui = furi_record_open(RECORD_GUI);

    // View Dispatcher
    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager = scene_manager_alloc(&tpms_scene_handlers, app);
    view_dispatcher_enable_queue(app->view_dispatcher);

    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, tpms_app_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, tpms_app_back_event_callback);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, tpms_app_tick_event_callback, 100);

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    // Open Notification record
    app->notifications = furi_record_open(RECORD_NOTIFICATION);

    // Variable Item List
    app->variable_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        TPMSViewVariableItemList,
        variable_item_list_get_view(app->variable_item_list));

    // SubMenu
    app->submenu = submenu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, TPMSViewSubmenu, submenu_get_view(app->submenu));

    // Widget
    app->widget = widget_alloc();
    view_dispatcher_add_view(app->view_dispatcher, TPMSViewWidget, widget_get_view(app->widget));

    // Receiver
    app->tpms_receiver = tpms_view_receiver_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, TPMSViewReceiver, tpms_view_receiver_get_view(app->tpms_receiver));

    // Receiver Info
    app->tpms_receiver_info = tpms_view_receiver_info_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        TPMSViewReceiverInfo,
        tpms_view_receiver_info_get_view(app->tpms_receiver_info));

    //init setting
    app->setting = subghz_setting_alloc();

    //ToDo FIX  file name setting
    subghz_setting_load(app->setting, EXT_PATH("subghz/assets/setting_user"));

    //init Worker & Protocol & History
    app->lock = TPMSLockOff;
    app->txrx = malloc(sizeof(TPMSTxRx));
    app->txrx->preset = malloc(sizeof(SubGhzRadioPreset));
    app->txrx->preset->name = furi_string_alloc();
    tpms_preset_init(app, "AM650", subghz_setting_get_default_frequency(app->setting), NULL, 0);

    app->txrx->hopper_state = TPMSHopperStateOFF;
    app->txrx->history = tpms_history_alloc();
    app->txrx->worker = subghz_worker_alloc();
    app->txrx->environment = subghz_environment_alloc();
    subghz_environment_set_protocol_registry(
        app->txrx->environment, (void*)&tpms_protocol_registry);
    app->txrx->receiver = subghz_receiver_alloc_init(app->txrx->environment);

    subghz_devices_init();

    app->txrx->radio_device =
        radio_device_loader_set(app->txrx->radio_device, SubGhzRadioDeviceTypeExternalCC1101);

    subghz_devices_reset(app->txrx->radio_device);
    subghz_devices_idle(app->txrx->radio_device);

    subghz_receiver_set_filter(app->txrx->receiver, SubGhzProtocolFlag_Decodable);
    subghz_worker_set_overrun_callback(
        app->txrx->worker, (SubGhzWorkerOverrunCallback)subghz_receiver_reset);
    subghz_worker_set_pair_callback(
        app->txrx->worker, (SubGhzWorkerPairCallback)subghz_receiver_decode);
    subghz_worker_set_context(app->txrx->worker, app->txrx->receiver);

    furi_hal_power_suppress_charge_enter();

    scene_manager_next_scene(app->scene_manager, TPMSSceneReceiver);

    return app;
}

void tpms_app_free(TPMSApp* app) {
    furi_assert(app);

    subghz_devices_sleep(app->txrx->radio_device);
    radio_device_loader_end(app->txrx->radio_device);

    subghz_devices_deinit();

    // Submenu
    view_dispatcher_remove_view(app->view_dispatcher, TPMSViewSubmenu);
    submenu_free(app->submenu);

    // Variable Item List
    view_dispatcher_remove_view(app->view_dispatcher, TPMSViewVariableItemList);
    variable_item_list_free(app->variable_item_list);

    //  Widget
    view_dispatcher_remove_view(app->view_dispatcher, TPMSViewWidget);
    widget_free(app->widget);

    // Receiver
    view_dispatcher_remove_view(app->view_dispatcher, TPMSViewReceiver);
    tpms_view_receiver_free(app->tpms_receiver);

    // Receiver Info
    view_dispatcher_remove_view(app->view_dispatcher, TPMSViewReceiverInfo);
    tpms_view_receiver_info_free(app->tpms_receiver_info);

    //setting
    subghz_setting_free(app->setting);

    //Worker & Protocol & History
    subghz_receiver_free(app->txrx->receiver);
    subghz_environment_free(app->txrx->environment);
    tpms_history_free(app->txrx->history);
    subghz_worker_free(app->txrx->worker);
    furi_string_free(app->txrx->preset->name);
    free(app->txrx->preset);
    free(app->txrx);

    // View dispatcher
    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);

    // Notifications
    furi_record_close(RECORD_NOTIFICATION);
    app->notifications = NULL;

    // Close records
    furi_record_close(RECORD_GUI);

    furi_hal_power_suppress_charge_exit();

    free(app);
}

int32_t tpms_app(void* p) {
    UNUSED(p);
    TPMSApp* tpms_app = tpms_app_alloc();

    view_dispatcher_run(tpms_app->view_dispatcher);

    tpms_app_free(tpms_app);

    return 0;
}
