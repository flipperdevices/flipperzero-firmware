#include "bad_usb_app_i.h"
#include <furi.h>
#include <furi_hal.h>
#include <storage/storage.h>
#include <lib/toolbox/path.h>
#include <flipper_format/flipper_format.h>

#define BAD_USB_SETTINGS_PATH BAD_USB_APP_BASE_FOLDER "/.badusb.settings"
#define BAD_USB_SETTINGS_FILE_TYPE "Flipper BadUSB Settings File"
#define BAD_USB_SETTINGS_VERSION 1
#define BAD_USB_SETTINGS_DEFAULT_LAYOUT BAD_USB_APP_PATH_LAYOUT_FOLDER "/en-US.kl"

// Forward declarations
static void bad_usb_load_settings(BadUsbApp* app);
static void bad_usb_save_settings(BadUsbApp* app);
static bool bad_usb_app_custom_event_callback(void* context, uint32_t event);
static bool bad_usb_app_back_event_callback(void* context);
static void bad_usb_app_tick_event_callback(void* context);

BadUsbApp* bad_usb_app_alloc(char* arg) {
    BadUsbApp* app = malloc(sizeof(BadUsbApp));
    if (!app) {
        // Handle error: memory allocation failed
        return NULL;
    }

    app->bad_usb_script = NULL;
    app->file_path = furi_string_alloc();
    app->keyboard_layout = furi_string_alloc();
    if (arg && strlen(arg)) {
        furi_string_set(app->file_path, arg);
    }

    bad_usb_load_settings(app);

    app->gui = furi_record_open(RECORD_GUI);
    app->notifications = furi_record_open(RECORD_NOTIFICATION);
    app->dialogs = furi_record_open(RECORD_DIALOGS);

    app->view_dispatcher = view_dispatcher_alloc();
    if (!app->view_dispatcher) {
        // Handle error: failed to allocate view dispatcher
        free(app);
        return NULL;
    }
    view_dispatcher_enable_queue(app->view_dispatcher);

    app->scene_manager = scene_manager_alloc(&bad_usb_scene_handlers, app);
    if (!app->scene_manager) {
        // Handle error: failed to allocate scene manager
        view_dispatcher_free(app->view_dispatcher);
        free(app);
        return NULL;
    }

    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_tick_event_callback(app->view_dispatcher, bad_usb_app_tick_event_callback, 500);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, bad_usb_app_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(app->view_dispatcher, bad_usb_app_back_event_callback);

    // Custom Widget
    app->widget = widget_alloc();
    if (!app->widget) {
        // Handle error: failed to allocate widget
        scene_manager_free(app->scene_manager);
        view_dispatcher_free(app->view_dispatcher);
        free(app);
        return NULL;
    }
    view_dispatcher_add_view(app->view_dispatcher, BadUsbAppViewError, widget_get_view(app->widget));

    // Config menu
    app->var_item_list = variable_item_list_alloc();
    if (!app->var_item_list) {
        // Handle error: failed to allocate variable item list
        widget_free(app->widget);
        scene_manager_free(app->scene_manager);
        view_dispatcher_free(app->view_dispatcher);
        free(app);
        return NULL;
    }
    view_dispatcher_add_view(app->view_dispatcher, BadUsbAppViewConfig, variable_item_list_get_view(app->var_item_list));

    // Bad USB View
    app->bad_usb_view = bad_usb_view_alloc();
    if (!app->bad_usb_view) {
        // Handle error: failed to allocate bad USB view
        variable_item_list_free(app->var_item_list);
        widget_free(app->widget);
        scene_manager_free(app->scene_manager);
        view_dispatcher_free(app->view_dispatcher);
        free(app);
        return NULL;
    }
    view_dispatcher_add_view(app->view_dispatcher, BadUsbAppViewWork, bad_usb_view_get_view(app->bad_usb_view));
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    if (furi_hal_usb_is_locked()) {
        app->error = BadUsbAppErrorCloseRpc;
        app->usb_if_prev = NULL;
        scene_manager_next_scene(app->scene_manager, BadUsbSceneError);
    } else {
        app->usb_if_prev = furi_hal_usb_get_config();
        furi_check(furi_hal_usb_set_config(NULL, NULL));

        if (!furi_string_empty(app->file_path)) {
            scene_manager_next_scene(app->scene_manager, BadUsbSceneWork);
        } else {
            furi_string_set(app->file_path, BAD_USB_APP_BASE_FOLDER);
            scene_manager_next_scene(app->scene_manager, BadUsbSceneFileSelect);
        }
    }

    return app;
}

void bad_usb_app_free(BadUsbApp* app) {
    if (!app) {
        return;
    }

    if (app->bad_usb_script) {
        bad_usb_script_close(app->bad_usb_script);
        app->bad_usb_script = NULL;
    }

    // Views
    view_dispatcher_remove_view(app->view_dispatcher, BadUsbAppViewWork);
    bad_usb_view_free(app->bad_usb_view);

    // Custom Widget
    view_dispatcher_remove_view(app->view_dispatcher, BadUsbAppViewError);
    widget_free(app->widget);

    // Config menu
    view_dispatcher_remove_view(app->view_dispatcher, BadUsbAppViewConfig);
    variable_item_list_free(app->var_item_list);

    // View dispatcher
    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);

    // Close records
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_DIALOGS);

    bad_usb_save_settings(app);

    furi_string_free(app->file_path);
    furi_string_free(app->keyboard_layout);

    if (app->usb_if_prev) {
        furi_check(furi_hal_usb_set_config(app->usb_if_prev, NULL));
    }

    free(app);
}

static void bad_usb_load_settings(BadUsbApp* app) {
    if (!app) {
        return;
    }

    Storage* storage = furi_record_open(RECORD_STORAGE);
    if (!storage) {
        // Handle error: failed to open storage
        return;
    }

    FlipperFormat* fff = flipper_format_file_alloc(storage);
    if (!fff) {
        furi_record_close(RECORD_STORAGE);
        // Handle error: failed to allocate flipper format
        return;
    }

    bool state = false;
    FuriString* temp_str = furi_string_alloc();
    uint32_t version = 0;
    uint32_t interface = 0;

    if (flipper_format_file_open_existing(fff, BAD_USB_SETTINGS_PATH)) {
        if (flipper_format_read_header(fff, temp_str, &version) &&
            strcmp(furi_string_get_cstr(temp_str), BAD_USB_SETTINGS_FILE_TYPE) == 0 &&
            version == BAD_USB_SETTINGS_VERSION &&
            flipper_format_read_string(fff, "layout", temp_str) &&
            flipper_format_read_uint32(fff, "interface", &interface, 1) &&
            interface <= BadUsbHidInterfaceBle) {
            state = true;
        }
    }

    if (state) {
        furi_string_set(app->keyboard_layout, temp_str);
        app->interface = interface;

        // Validate layout file or fallback if necessary
        Storage* fs_api = furi_record_open(RECORD_STORAGE);
        if (fs_api) {
            FileInfo layout_file_info;
            FS_Error file_check_err = storage_common_stat(fs_api, furi_string_get_cstr(app->keyboard_layout), &layout_file_info);
            furi_record_close(RECORD_STORAGE);
            if (file_check_err != FSE_OK || layout_file_info.size != 256) {
                furi_string_set(app->keyboard_layout, BAD_USB_SETTINGS_DEFAULT_LAYOUT);
            }
        }
    } else {
        furi_string_set(app->keyboard_layout, BAD_USB_SETTINGS_DEFAULT_LAYOUT);
        app->interface = BadUsbHidInterfaceUsb;
    }

    furi_string_free(temp_str);
    flipper_format_free(fff);
    furi_record_close(RECORD_STORAGE);
}

static void bad_usb_save_settings(BadUsbApp* app) {
    if (!app) {
        return;
    }

    Storage* storage = furi_record_open(RECORD_STORAGE);
    if (!storage) {
        // Handle error: failed to open storage
        return;
    }

    FlipperFormat* fff = flipper_format_file_alloc(storage);
    if (!fff) {
        furi_record_close(RECORD_STORAGE);
        // Handle error: failed to allocate flipper format
        return;
    }

    if (flipper_format_file_open_always(fff, BAD_USB_SETTINGS_PATH)) {
        if (!flipper_format_write_header_cstr(fff, BAD_USB_SETTINGS_FILE_TYPE, BAD_USB_SETTINGS_VERSION) ||
            !flipper_format_write_string(fff, "layout", app->keyboard_layout) ||
            !flipper_format_write_uint32(fff, "interface", (const uint32_t*)&(app->interface), 1)) {
            // Handle error: failed to write settings
        }
    }

    flipper_format_free(fff);
    furi_record_close(RECORD_STORAGE);
}

int32_t bad_usb_app(void* p) {
    if (!p) {
        return -1;
    }

    BadUsbApp* bad_usb_app = bad_usb_app_alloc((char*)p);
    if (!bad_usb_app) {
        // Handle error: failed to allocate BadUsbApp
        return -1;
    }

    view_dispatcher_run(bad_usb_app->view_dispatcher);

    bad_usb_app_free(bad_usb_app);

    return 0;
}

