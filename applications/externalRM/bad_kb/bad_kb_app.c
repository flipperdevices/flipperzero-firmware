#include "bad_kb_app.h"
#include <furi.h>
#include <furi_hal.h>
#include <storage/storage.h>
#include <lib/toolbox/path.h>
#include <lib/flipper_format/flipper_format.h>

#include <bt/bt_service/bt_i.h>
#include <bt/bt_service/bt.h>

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

static void bad_kb_load_settings(BadKbApp* app) {
    furi_string_reset(app->keyboard_layout);
    BadKbConfig* cfg = &app->config;
    strcpy(cfg->bt_name, "");
    memcpy(cfg->bt_mac, BAD_KB_EMPTY_MAC, BAD_KB_MAC_LEN);
    strcpy(cfg->usb_cfg.manuf, "");
    strcpy(cfg->usb_cfg.product, "");
    cfg->usb_cfg.vid = 0;
    cfg->usb_cfg.pid = 0;

    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* file = flipper_format_file_alloc(storage);
    if(flipper_format_file_open_existing(file, BAD_KB_SETTINGS_PATH)) {
        FuriString* tmp_str = furi_string_alloc();
        if(!flipper_format_read_string(file, "Keyboard_Layout", app->keyboard_layout)) {
            furi_string_reset(app->keyboard_layout);
        }
        if(!flipper_format_read_bool(file, "Is_BT", &(app->is_bt), 1)) {
            app->is_bt = false;
        }
        if(!flipper_format_read_bool(file, "BT_Remember", &(app->bt_remember), 1)) {
            app->bt_remember = false;
        }
        if(flipper_format_read_string(file, "Bt_Name", tmp_str) && !furi_string_empty(tmp_str)) {
            strlcpy(cfg->bt_name, furi_string_get_cstr(tmp_str), BAD_KB_NAME_LEN);
        } else {
            strcpy(cfg->bt_name, "");
        }
        if(!flipper_format_read_hex(file, "Bt_Mac", (uint8_t*)&cfg->bt_mac, BAD_KB_MAC_LEN)) {
            memcpy(cfg->bt_mac, BAD_KB_EMPTY_MAC, BAD_KB_MAC_LEN);
        }
        if(flipper_format_read_string(file, "Usb_Manuf", tmp_str) && !furi_string_empty(tmp_str)) {
            strlcpy(cfg->usb_cfg.manuf, furi_string_get_cstr(tmp_str), BAD_KB_USB_LEN);
        } else {
            strcpy(cfg->usb_cfg.manuf, "");
        }
        if(flipper_format_read_string(file, "Usb_Product", tmp_str) &&
           !furi_string_empty(tmp_str)) {
            strlcpy(cfg->usb_cfg.product, furi_string_get_cstr(tmp_str), BAD_KB_USB_LEN);
        } else {
            strcpy(cfg->usb_cfg.product, "");
        }
        if(!flipper_format_read_uint32(file, "Usb_Vid", &cfg->usb_cfg.vid, 1)) {
            cfg->usb_cfg.vid = 0;
        }
        if(!flipper_format_read_uint32(file, "Usb_Pid", &cfg->usb_cfg.pid, 1)) {
            cfg->usb_cfg.pid = 0;
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
        flipper_format_write_string(file, "Keyboard_Layout", app->keyboard_layout);
        flipper_format_write_bool(file, "Is_BT", &(app->is_bt), 1);
        flipper_format_write_bool(file, "BT_Remember", &(app->bt_remember), 1);
        flipper_format_write_string_cstr(file, "Bt_Name", cfg->bt_name);
        flipper_format_write_hex(file, "Bt_Mac", (uint8_t*)&cfg->bt_mac, BAD_KB_MAC_LEN);
        flipper_format_write_string_cstr(file, "Usb_Manuf", cfg->usb_cfg.manuf);
        flipper_format_write_string_cstr(file, "Usb_Product", cfg->usb_cfg.product);
        flipper_format_write_uint32(file, "Usb_Vid", &cfg->usb_cfg.vid, 1);
        flipper_format_write_uint32(file, "Usb_Pid", &cfg->usb_cfg.pid, 1);
        flipper_format_file_close(file);
    }
    flipper_format_free(file);
    furi_record_close(RECORD_STORAGE);
}

BadKbApp* bad_kb_app_alloc(char* arg) {
    BadKbApp* app = malloc(sizeof(BadKbApp));

    app->bad_kb_script = NULL;

    app->file_path = furi_string_alloc();
    app->keyboard_layout = furi_string_alloc();
    if(arg && strlen(arg)) {
        furi_string_set(app->file_path, arg);
    }

    Storage* storage = furi_record_open(RECORD_STORAGE);
    storage_simply_mkdir(storage, BAD_KB_APP_BASE_FOLDER);
    storage_simply_mkdir(storage, BAD_KB_APP_BASE_CONFIG_FOLDER);
    furi_record_close(RECORD_STORAGE);

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
    FuriHalBtProfile kbd = FuriHalBtProfileHidKeyboard;
    app->prev_bt_mode = furi_hal_bt_get_profile_pairing_method(kbd);
    memcpy(app->prev_bt_mac, furi_hal_bt_get_profile_mac_addr(kbd), BAD_KB_MAC_LEN);
    strlcpy(app->prev_bt_name, furi_hal_bt_get_profile_adv_name(kbd), BAD_KB_NAME_LEN);

    // Adjust BT remember MAC to be serial MAC +2
    memcpy(BAD_KB_BOUND_MAC, furi_hal_version_get_ble_mac(), BAD_KB_MAC_LEN);
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

    app->bad_kb_view = bad_kb_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, BadKbAppViewWork, bad_kb_get_view(app->bad_kb_view));

    app->text_input = text_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, BadKbAppViewTextInput, text_input_get_view(app->text_input));

    app->byte_input = byte_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, BadKbAppViewByteInput, byte_input_get_view(app->byte_input));

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
        furi_string_set(app->file_path, BAD_KB_APP_BASE_FOLDER);
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
    bad_kb_free(app->bad_kb_view);

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
    if(app->hid_cfg) free(app->hid_cfg);

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

int32_t bad_kb_app(char* p) {
    BadKbApp* bad_kb_app = bad_kb_app_alloc(p);

    view_dispatcher_run(bad_kb_app->view_dispatcher);

    bad_kb_app_free(bad_kb_app);
    return 0;
}
