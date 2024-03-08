#include "findmy_i.h"

static bool findmy_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    FindMy* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool findmy_back_event_callback(void* context) {
    furi_assert(context);
    FindMy* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static FindMy* findmy_app_alloc() {
    FindMy* app = malloc(sizeof(FindMy));

    app->gui = furi_record_open(RECORD_GUI);

    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);

    app->scene_manager = scene_manager_alloc(&findmy_scene_handlers, app);

    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, findmy_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, findmy_back_event_callback);

    app->findmy_main = findmy_main_alloc(app);
    view_dispatcher_add_view(
        app->view_dispatcher, FindMyViewMain, findmy_main_get_view(app->findmy_main));

    app->byte_input = byte_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, FindMyViewByteInput, byte_input_get_view(app->byte_input));

    app->var_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        FindMyViewVarItemList,
        variable_item_list_get_view(app->var_item_list));

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    app->beacon_active = false;
    findmy_main_update_active(app->findmy_main, app->beacon_active);
    app->broadcast_interval = 5;
    findmy_main_update_interval(app->findmy_main, app->broadcast_interval);
    app->transmit_power = 6;
    app->apple = true;
    findmy_main_update_apple(app->findmy_main, app->apple);

    return app;
}

static void findmy_app_free(FindMy* app) {
    furi_assert(app);

    view_dispatcher_remove_view(app->view_dispatcher, FindMyViewVarItemList);
    variable_item_list_free(app->var_item_list);

    view_dispatcher_remove_view(app->view_dispatcher, FindMyViewByteInput);
    byte_input_free(app->byte_input);

    view_dispatcher_remove_view(app->view_dispatcher, FindMyViewMain);
    findmy_main_free(app->findmy_main);

    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);

    furi_record_close(RECORD_GUI);

    free(app);
}

static void findmy_start(FindMy* app) {
    furi_hal_bt_extra_beacon_stop(); // Stop any running beacon

    app->config.min_adv_interval_ms = app->broadcast_interval * 1000; // Converting s to ms
    app->config.max_adv_interval_ms = (app->broadcast_interval * 1000) + 150;
    app->config.adv_channel_map = GapAdvChannelMapAll;
    app->config.adv_power_level = GapAdvPowerLevel_0dBm + app->transmit_power;
    app->config.address_type = GapAddressTypePublic;

    uint8_t mac[EXTRA_BEACON_MAC_ADDR_SIZE] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
    furi_hal_bt_reverse_mac_addr(mac);
    memcpy(&app->config.address, mac, sizeof(app->config.address));
    furi_check(furi_hal_bt_extra_beacon_set_config(&app->config));

    uint8_t data[EXTRA_BEACON_MAX_DATA_SIZE];
    uint8_t* it = data;

    // For Apple AirTags
    *it++ = 0x1E, // Length
    *it++ = 0xFF; // Manufacturer Specific Data
    *it++ = 0x4C; // Company ID (Apple, Inc.)
    *it++ = 0x00; // State
    *it++ = 0x12; // Data - Public Key without the MAC address
    *it++ = 0x19;
    *it++ = 0x00;
    *it++ = 0x00;
    *it++ = 0x00;
    *it++ = 0x00;
    *it++ = 0x00;
    *it++ = 0x00;
    *it++ = 0x00;
    *it++ = 0x00;
    *it++ = 0x00;
    *it++ = 0x00;
    *it++ = 0x00;
    *it++ = 0x00;
    *it++ = 0x00;
    *it++ = 0x00;
    *it++ = 0x00;
    *it++ = 0x00;
    *it++ = 0x00;
    *it++ = 0x00;
    *it++ = 0x00;
    *it++ = 0x00;
    *it++ = 0x00;
    *it++ = 0x00;
    *it++ = 0x00;
    *it++ = 0x00; // First 2 bits are the version, the rest is the battery level
    *it++ = 0x00; // Hint (0x00)

    furi_check(furi_hal_bt_extra_beacon_set_data(data, it - data));
}

int32_t findmy_main(void* p) {
    UNUSED(p);
    FindMy* app = findmy_app_alloc();

    findmy_start(app);

    scene_manager_next_scene(app->scene_manager, FindMySceneMain);

    view_dispatcher_run(app->view_dispatcher);

    findmy_app_free(app);
    return 0;
}

void findmy_change_broadcast_interval(FindMy* app, uint8_t value) {
    if(value > 10 || value < 1) {
        return;
    }
    app->broadcast_interval = value;
    findmy_main_update_interval(app->findmy_main, app->broadcast_interval);
    if(app->beacon_active) {
        // Always check if beacon is active before changing config
        furi_check(furi_hal_bt_extra_beacon_stop());
    }
    app->config.min_adv_interval_ms = app->broadcast_interval * 1000;
    app->config.max_adv_interval_ms = app->config.min_adv_interval_ms + 150;
    furi_check(furi_hal_bt_extra_beacon_set_config(&app->config));
    if(app->beacon_active) {
        furi_check(furi_hal_bt_extra_beacon_start());
    }
}

void findmy_change_transmit_power(FindMy* app, uint8_t value) {
    if(value > 6) {
        return;
    }
    app->transmit_power = value;
    if(app->beacon_active) {
        furi_check(furi_hal_bt_extra_beacon_stop());
    }
    app->config.adv_power_level = GapAdvPowerLevel_0dBm + app->transmit_power;
    furi_check(furi_hal_bt_extra_beacon_set_config(&app->config));
    if(app->beacon_active) {
        furi_check(furi_hal_bt_extra_beacon_start());
    }
}

void findmy_toggle_beacon(FindMy* app) {
    app->beacon_active = !app->beacon_active;
    findmy_main_update_active(app->findmy_main, app->beacon_active);
    findmy_main_update_apple(app->findmy_main, app->apple);
    if(app->beacon_active) {
        furi_hal_bt_extra_beacon_start();
    } else {
        furi_hal_bt_extra_beacon_stop();
    }
}