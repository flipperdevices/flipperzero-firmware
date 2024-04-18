#include "bad_kb_app_i.h"
#include <furi.h>
#include <furi_hal.h>
#include <storage/storage.h>
#include <lib/toolbox/path.h>
#include <lib/flipper_format/flipper_format.h>
#include <bt/bt_service/bt_i.h>
#include "helpers/ducky_script_i.h"

// Adjusts to serial MAC +2 in app init
uint8_t BAD_KB_BOUND_MAC[GAP_MAC_ADDR_SIZE] = {0};

static bool bad_kb_app_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    BadKbApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool bad_kb_app_back_event_callback(void* context) {
    furi_assert(context);
    BadKbApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static void bad_kb_app_tick_event_callback(void* context) {
    furi_assert(context);
    BadKbApp* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

void bad_kb_load_settings(BadKbApp* app) {
    furi_string_reset(app->keyboard_layout);
    BadKbConfig* cfg = &app->config;

    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* file = flipper_format_file_alloc(storage);
    if(flipper_format_file_open_existing(file, BAD_KB_SETTINGS_PATH)) {
        FuriString* tmp_str = furi_string_alloc();
        uint32_t tmp_uint = 0;

        if(!flipper_format_read_string(file, "Keyboard_Layout", app->keyboard_layout)) {
            furi_string_reset(app->keyboard_layout);
            flipper_format_rewind(file);
        }

        if(!flipper_format_read_bool(file, "Is_Bt", &app->is_bt, 1)) {
            app->is_bt = false;
            flipper_format_rewind(file);
        }

        if(!flipper_format_read_bool(file, "Bt_Remember", &cfg->ble.bonding, 1)) {
            cfg->ble.bonding = false;
            flipper_format_rewind(file);
        }

        if(!flipper_format_read_uint32(file, "Bt_Pairing", &tmp_uint, 1)) {
            tmp_uint = GapPairingNone;
            flipper_format_rewind(file);
        }
        cfg->ble.pairing = tmp_uint;

        if(flipper_format_read_string(file, "Bt_Name", tmp_str)) {
            strlcpy(cfg->ble.name, furi_string_get_cstr(tmp_str), sizeof(cfg->ble.name));
        } else {
            cfg->ble.name[0] = '\0';
            flipper_format_rewind(file);
        }

        if(!flipper_format_read_hex(
               file, "Bt_Mac", (uint8_t*)&cfg->ble.mac, sizeof(cfg->ble.mac))) {
            memset(cfg->ble.mac, 0, sizeof(cfg->ble.mac));
            flipper_format_rewind(file);
        }

        if(flipper_format_read_string(file, "Usb_Manuf", tmp_str)) {
            strlcpy(cfg->usb.manuf, furi_string_get_cstr(tmp_str), sizeof(cfg->usb.manuf));
        } else {
            cfg->usb.manuf[0] = '\0';
            flipper_format_rewind(file);
        }

        if(flipper_format_read_string(file, "Usb_Product", tmp_str)) {
            strlcpy(cfg->usb.product, furi_string_get_cstr(tmp_str), sizeof(cfg->usb.product));
        } else {
            cfg->usb.product[0] = '\0';
            flipper_format_rewind(file);
        }

        if(!flipper_format_read_uint32(file, "Usb_Vid", &cfg->usb.vid, 1)) {
            cfg->usb.vid = 0;
            flipper_format_rewind(file);
        }

        if(!flipper_format_read_uint32(file, "Usb_Pid", &cfg->usb.pid, 1)) {
            cfg->usb.pid = 0;
            flipper_format_rewind(file);
        }

        furi_string_free(tmp_str);
        flipper_format_file_close(file);
    }
    flipper_format_free(file);

    if(!furi_string_empty(app->keyboard_layout)) {
        FileInfo layout_file_info;
        FS_Error file_check_err = storage_common_stat(
            storage, furi_string_get_cstr(app->keyboard_layout), &layout_file_info);
        if(file_check_err != FSE_OK) {
            furi_string_reset(app->keyboard_layout);
            return;
        }
        if(layout_file_info.size != 256) {
            furi_string_reset(app->keyboard_layout);
        }
    }

    furi_record_close(RECORD_STORAGE);
}

static void bad_kb_save_settings(BadKbApp* app) {
    BadKbConfig* cfg = &app->config;
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* file = flipper_format_file_alloc(storage);
    if(flipper_format_file_open_always(file, BAD_KB_SETTINGS_PATH)) {
        uint32_t tmp_uint = 0;
        flipper_format_write_string(file, "Keyboard_Layout", app->keyboard_layout);
        flipper_format_write_bool(file, "Is_Bt", &app->is_bt, 1);
        flipper_format_write_bool(file, "Bt_Remember", &cfg->ble.bonding, 1);
        tmp_uint = cfg->ble.pairing;
        flipper_format_write_uint32(file, "Bt_Pairing", &tmp_uint, 1);
        flipper_format_write_string_cstr(file, "Bt_Name", cfg->ble.name);
        flipper_format_write_hex(file, "Bt_Mac", (uint8_t*)&cfg->ble.mac, sizeof(cfg->ble.mac));
        flipper_format_write_string_cstr(file, "Usb_Manuf", cfg->usb.manuf);
        flipper_format_write_string_cstr(file, "Usb_Product", cfg->usb.product);
        flipper_format_write_uint32(file, "Usb_Vid", &cfg->usb.vid, 1);
        flipper_format_write_uint32(file, "Usb_Pid", &cfg->usb.pid, 1);
        flipper_format_file_close(file);
    }
    flipper_format_free(file);
    furi_record_close(RECORD_STORAGE);
}

void bad_kb_app_show_loading_popup(BadKbApp* app, bool show) {
    if(show) {
        // Raise timer priority so that animations can play
        furi_timer_set_thread_priority(FuriTimerThreadPriorityElevated);
        view_dispatcher_switch_to_view(app->view_dispatcher, BadKbAppViewLoading);
    } else {
        // Restore default timer priority
        furi_timer_set_thread_priority(FuriTimerThreadPriorityNormal);
    }
}

int32_t bad_kb_conn_apply(BadKbApp* app) {
    if(app->is_bt) {
        // Setup profile config
        BadKbConfig* cfg = app->set_bt_id ? &app->id_config : &app->config;
        memcpy(&app->cur_ble_cfg, &cfg->ble, sizeof(cfg->ble));
        if(app->cur_ble_cfg.bonding) {
            // Hardcode mac for remember mode
            // Change in config copy to preserve user choice for non-remember mode
            memcpy(app->cur_ble_cfg.mac, BAD_KB_BOUND_MAC, sizeof(BAD_KB_BOUND_MAC));
        }

        // Prepare for new profile
        bt_timeout = bt_hid_delays[LevelRssi39_0];
        bt_disconnect(app->bt);
        furi_delay_ms(200);
        bt_keys_storage_set_storage_path(app->bt, BAD_KB_KEYS_PATH);

        // Set profile
        app->ble_hid = bt_profile_start(app->bt, ble_profile_hid, &app->cur_ble_cfg);
        furi_check(app->ble_hid);

        // Advertise even if BT is off in settings
        furi_hal_bt_start_advertising();

        app->conn_mode = BadKbConnModeBt;

    } else {
        // Unlock RPC connections
        furi_hal_usb_unlock();

        // Context will apply with set_config only if pointer address is different, so we use a copy
        FuriHalUsbHidConfig* cur_usb_cfg = malloc(sizeof(FuriHalUsbHidConfig));

        // Setup new config
        BadKbConfig* cfg = app->set_usb_id ? &app->id_config : &app->config;
        memcpy(cur_usb_cfg, &cfg->usb, sizeof(cfg->usb));

        // Set profile
        furi_check(furi_hal_usb_set_config(&usb_hid, cur_usb_cfg));
        if(app->cur_usb_cfg) free(app->cur_usb_cfg);
        app->cur_usb_cfg = cur_usb_cfg;

        app->conn_mode = BadKbConnModeUsb;
    }

    return 0;
}

void bad_kb_conn_reset(BadKbApp* app) {
    if(app->conn_mode == BadKbConnModeBt) {
        bt_disconnect(app->bt);
        furi_delay_ms(200);
        bt_keys_storage_set_default_path(app->bt);
        furi_check(bt_profile_restore_default(app->bt));
    } else if(app->conn_mode == BadKbConnModeUsb) {
        // TODO: maybe also restore USB context?
        furi_check(furi_hal_usb_set_config(app->prev_usb_mode, NULL));
    }

    app->conn_mode = BadKbConnModeNone;
}

void bad_kb_config_adjust(BadKbConfig* cfg) {
    // Avoid empty name
    if(cfg->ble.name[0] == '\0') {
        snprintf(
            cfg->ble.name, sizeof(cfg->ble.name), "Control %s", furi_hal_version_get_name_ptr());
    }

    const uint8_t* normal_mac = furi_hal_version_get_ble_mac();
    uint8_t empty_mac[sizeof(cfg->ble.mac)] = {0};
    uint8_t default_mac[sizeof(cfg->ble.mac)] = {0x6c, 0x7a, 0xd8, 0xac, 0x57, 0x72}; //furi_hal_bt
    if(memcmp(cfg->ble.mac, empty_mac, sizeof(cfg->ble.mac)) == 0 ||
       memcmp(cfg->ble.mac, normal_mac, sizeof(cfg->ble.mac)) == 0 ||
       memcmp(cfg->ble.mac, default_mac, sizeof(cfg->ble.mac)) == 0) {
        memcpy(cfg->ble.mac, normal_mac, sizeof(cfg->ble.mac));
        cfg->ble.mac[2]++;
    }

    // Use defaults if vid or pid are unset
    if(cfg->usb.vid == 0) cfg->usb.vid = 0x046D;
    if(cfg->usb.pid == 0) cfg->usb.pid = 0xC529;
}

void bad_kb_config_refresh(BadKbApp* app) {
    bt_set_status_changed_callback(app->bt, NULL, NULL);
    furi_hal_hid_set_state_callback(NULL, NULL);
    if(app->bad_kb_script) {
        furi_thread_flags_set(furi_thread_get_id(app->bad_kb_script->thread), WorkerEvtDisconnect);
    }
    if(app->conn_init_thread) {
        furi_thread_join(app->conn_init_thread);
    }

    bool apply = false;
    if(app->is_bt) {
        BadKbConfig* cfg = app->set_bt_id ? &app->id_config : &app->config;
        bad_kb_config_adjust(cfg);

        if(app->conn_mode != BadKbConnModeBt) {
            apply = true;
            bad_kb_conn_reset(app);
        } else {
            BleProfileHidParams* cur = &app->cur_ble_cfg;
            apply = apply || cfg->ble.bonding != cur->bonding;
            apply = apply || cfg->ble.pairing != cur->pairing;
            apply = apply || strncmp(cfg->ble.name, cur->name, sizeof(cfg->ble.name));
            apply = apply || memcmp(cfg->ble.mac, cur->mac, sizeof(cfg->ble.mac));
        }
    } else {
        BadKbConfig* cfg = app->set_usb_id ? &app->id_config : &app->config;
        bad_kb_config_adjust(cfg);

        if(app->conn_mode != BadKbConnModeUsb) {
            apply = true;
            bad_kb_conn_reset(app);
        } else {
            FuriHalUsbHidConfig* cur = app->cur_usb_cfg;
            apply = apply || cfg->usb.vid != cur->vid;
            apply = apply || cfg->usb.pid != cur->pid;
            apply = apply || strncmp(cfg->usb.manuf, cur->manuf, sizeof(cur->manuf));
            apply = apply || strncmp(cfg->usb.product, cur->product, sizeof(cur->product));
        }
    }

    if(apply) {
        bad_kb_conn_apply(app);
    }

    if(app->bad_kb_script) {
        BadKbScript* script = app->bad_kb_script;
        script->st.is_bt = app->is_bt;
        script->bt = app->is_bt ? app->bt : NULL;
        bool connected;
        if(app->is_bt) {
            bt_set_status_changed_callback(app->bt, bad_kb_bt_hid_state_callback, script);
            connected = furi_hal_bt_is_connected();
        } else {
            furi_hal_hid_set_state_callback(bad_kb_usb_hid_state_callback, script);
            connected = furi_hal_hid_is_connected();
        }
        if(connected) {
            furi_thread_flags_set(furi_thread_get_id(script->thread), WorkerEvtConnect);
        }
    }

    // Reload config page
    scene_manager_next_scene(app->scene_manager, BadKbSceneConfig);
    scene_manager_previous_scene(app->scene_manager);
}

BadKbApp* bad_kb_app_alloc(char* arg) {
    BadKbApp* app = malloc(sizeof(BadKbApp));

    app->bad_kb_script = NULL;

    app->file_path = furi_string_alloc();
    app->keyboard_layout = furi_string_alloc();
    if(arg && strlen(arg)) {
        furi_string_set(app->file_path, arg);
    }

    bad_kb_load_settings(app);

    app->gui = furi_record_open(RECORD_GUI);
    app->notifications = furi_record_open(RECORD_NOTIFICATION);
    app->dialogs = furi_record_open(RECORD_DIALOGS);

    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);

    app->scene_manager = scene_manager_alloc(&bad_kb_scene_handlers, app);

    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, bad_kb_app_tick_event_callback, 250);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, bad_kb_app_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, bad_kb_app_back_event_callback);

    Bt* bt = furi_record_open(RECORD_BT);
    app->bt = bt;
    app->bt->suppress_pin_screen = true;
    bad_kb_config_adjust(&app->config);

    // Save prev config
    app->prev_usb_mode = furi_hal_usb_get_config();

    // Adjust BT remember MAC to be serial MAC +2
    memcpy(BAD_KB_BOUND_MAC, furi_hal_version_get_ble_mac(), sizeof(BAD_KB_BOUND_MAC));
    BAD_KB_BOUND_MAC[2] += 2;

    // Custom Widget
    app->widget = widget_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, BadKbAppViewWidget, widget_get_view(app->widget));

    app->var_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        BadKbAppViewVarItemList,
        variable_item_list_get_view(app->var_item_list));

    app->bad_kb_view = bad_kb_view_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, BadKbAppViewWork, bad_kb_view_get_view(app->bad_kb_view));

    app->text_input = text_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, BadKbAppViewTextInput, text_input_get_view(app->text_input));

    app->byte_input = byte_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, BadKbAppViewByteInput, byte_input_get_view(app->byte_input));

    app->loading = loading_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, BadKbAppViewLoading, loading_get_view(app->loading));

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    app->conn_mode = BadKbConnModeNone;
    app->conn_init_thread =
        furi_thread_alloc_ex("BadKbConnInit", 1024, (FuriThreadCallback)bad_kb_conn_apply, app);
    furi_thread_start(app->conn_init_thread);
    if(!furi_string_empty(app->file_path)) {
        app->bad_kb_script = bad_kb_script_open(app->file_path, app->is_bt ? app->bt : NULL, app);
        bad_kb_script_set_keyboard_layout(app->bad_kb_script, app->keyboard_layout);
        scene_manager_next_scene(app->scene_manager, BadKbSceneWork);
    } else {
        furi_string_set(app->file_path, BAD_USB_APP_BASE_FOLDER);
        scene_manager_next_scene(app->scene_manager, BadKbSceneFileSelect);
    }

    return app;
}

void bad_kb_app_free(BadKbApp* app) {
    furi_assert(app);

    if(app->bad_kb_script) {
        bad_kb_script_close(app->bad_kb_script);
        app->bad_kb_script = NULL;
    }

    // Views
    view_dispatcher_remove_view(app->view_dispatcher, BadKbAppViewWork);
    bad_kb_view_free(app->bad_kb_view);

    // Custom Widget
    view_dispatcher_remove_view(app->view_dispatcher, BadKbAppViewWidget);
    widget_free(app->widget);

    // Variable item list
    view_dispatcher_remove_view(app->view_dispatcher, BadKbAppViewVarItemList);
    variable_item_list_free(app->var_item_list);

    // Text Input
    view_dispatcher_remove_view(app->view_dispatcher, BadKbAppViewTextInput);
    text_input_free(app->text_input);

    // Byte Input
    view_dispatcher_remove_view(app->view_dispatcher, BadKbAppViewByteInput);
    byte_input_free(app->byte_input);

    // Loading
    view_dispatcher_remove_view(app->view_dispatcher, BadKbAppViewLoading);
    loading_free(app->loading);

    // View dispatcher
    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);

    // Restore connection config
    app->bt->suppress_pin_screen = false;
    if(app->conn_init_thread) {
        furi_thread_join(app->conn_init_thread);
        furi_thread_free(app->conn_init_thread);
        app->conn_init_thread = NULL;
    }
    bad_kb_conn_reset(app);
    if(app->cur_usb_cfg) free(app->cur_usb_cfg);

    // Close records
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_DIALOGS);
    furi_record_close(RECORD_BT);

    bad_kb_save_settings(app);

    furi_string_free(app->file_path);
    furi_string_free(app->keyboard_layout);

    free(app);
}

int32_t bad_kb_app(void* p) {
    BadKbApp* bad_kb_app = bad_kb_app_alloc((char*)p);

    view_dispatcher_run(bad_kb_app->view_dispatcher);

    bad_kb_app_free(bad_kb_app);
    return 0;
}
