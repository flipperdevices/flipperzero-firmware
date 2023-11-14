/**
MIT License

Copyright (c) 2023 Vojtech Suk (https://github.com/sukvojte)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <furi.h>

#include "wch_swio_flasher.h"
#include "scenes/wch_swio_flasher_scene.h"
#include <wch_swio_flasher_icons.h>
#include <dolphin/dolphin.h>
#include <storage/storage.h>
#include "utils.h"

#define TAG "WCH_SWIO_FL"

#define SYSTEM_DEFAULTS_MAGIC 0xCAFED00D
#define SYSTEM_DEFAULTS_FILENAME "defaults.txt"

typedef struct {
    uint32_t magic;
    uint32_t last_file_len;
    char last_file[];
} system_defaults;

static bool wch_swio_flasher_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    WchSwioFlasherApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool wch_swio_flasher_back_event_callback(void* context) {
    furi_assert(context);
    WchSwioFlasherApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

void wch_swio_flasher_load_defaults(WchSwioFlasherApp* app) {
    Storage* storage = furi_record_open(RECORD_STORAGE);

    // Allocate file
    File* file = storage_file_alloc(storage);

    furi_string_set_str(app->views.file_path, "");

    if(!storage_file_open(
           file, APP_DATA_PATH(SYSTEM_DEFAULTS_FILENAME), FSAM_READ, FSOM_OPEN_EXISTING)) {
        FURI_LOG_E(TAG, "Failed to open config file");
    } else {
        system_defaults head; // = malloc(sizeof(system_defaults) + last_file_len);
        if(!storage_file_read(file, &head, sizeof(head))) {
            FURI_LOG_E(TAG, "Failed to read header from config file");
        } else {
            char* last_file = malloc(head.last_file_len);
            if(head.magic == SYSTEM_DEFAULTS_MAGIC &&
               !storage_file_read(file, last_file, head.last_file_len)) {
                FURI_LOG_E(TAG, "Failed to read from config file");
            } else {
                FURI_LOG_D(TAG, "Loaded default path: %s", last_file);
                furi_string_set_str(app->views.file_path, last_file);
            }
            free(last_file);
        }
    }

    storage_file_close(file);

    // Deallocate file
    storage_file_free(file);

    // Close storage
    furi_record_close(RECORD_STORAGE);
}

void wch_swio_flasher_store_defaults(WchSwioFlasherApp* app) {
    Storage* storage = furi_record_open(RECORD_STORAGE);

    // Allocate file
    File* file = storage_file_alloc(storage);

    if(!storage_file_open(
           file, APP_DATA_PATH(SYSTEM_DEFAULTS_FILENAME), FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
        FURI_LOG_E(TAG, "Failed to open config file");
    }

    const char* last_file = furi_string_get_cstr(app->views.file_path);
    uint32_t last_file_len = strlen(last_file);
    system_defaults* sd = malloc(sizeof(system_defaults) + last_file_len);

    sd->magic = SYSTEM_DEFAULTS_MAGIC;
    sd->last_file_len = last_file_len;
    memcpy(sd->last_file, last_file, last_file_len);

    FURI_LOG_D(TAG, "Store default path: %s", last_file);

    if(!storage_file_write(file, sd, sizeof(system_defaults) + last_file_len)) {
        FURI_LOG_E(TAG, "Failed to write to config file");
    }

    // Close file
    storage_file_close(file);

    // Deallocate file
    storage_file_free(file);

    // Deallocate data
    free(sd);

    // Close storage
    furi_record_close(RECORD_STORAGE);
}

WchSwioFlasherApp* wch_swio_flasher_app_alloc() {
    WchSwioFlasherApp* app = malloc(sizeof(WchSwioFlasherApp));

    app->helpers.swio = WchSwioFlasher_SWIO_create();
    app->helpers.riscv_debug = WchSwioFlasher_RiscVDebug_create(app->helpers.swio);
    app->helpers.flasher = WchSwioFlasher_WchFlasher_create(app->helpers.riscv_debug);

    app->services.mini_debugger =
        WchSwioFlasher_MinichlinkDebugger_create(app->helpers.swio, app->helpers.riscv_debug);
    app->services.emulator = WchSwioFlasher_NhcLink042Emu_create(app->services.mini_debugger);
    app->services.flasher =
        WchSwioFlasher_Ch32vFlipperFlasher_create(app->helpers.riscv_debug, app->helpers.flasher);

    app->gui = furi_record_open(RECORD_GUI);
    app->notification = furi_record_open(RECORD_NOTIFICATION);

    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager = scene_manager_alloc(&wch_swio_flasher_scene_handlers, app);
    view_dispatcher_enable_queue(app->view_dispatcher);

    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);

    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, wch_swio_flasher_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, wch_swio_flasher_back_event_callback);

    app->views.debuger_emulator = debugger_emulator_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        WchSwioFlasherViewDebuggerEmulator,
        debugger_emulator_get_view(app->views.debuger_emulator));

    // Widget
    app->views.widget = widget_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, WchSwioFlasherViewWidget, widget_get_view(app->views.widget));

    // SubMenu
    app->views.submenu = submenu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, WchSwioFlasherViewSubmenu, submenu_get_view(app->views.submenu));

    // FileBrowser
    app->views.file_path = furi_string_alloc();
    app->views.file_browser = file_browser_alloc(app->views.file_path);
    file_browser_configure(app->views.file_browser, "*", NULL, true, false, &I_bin10x10, true);
    view_dispatcher_add_view(
        app->view_dispatcher,
        WchSwioFlasherViewFileBrowser,
        file_browser_get_view(app->views.file_browser));

    // Flasher
    app->views.flasher = view_flasher_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        WchSwioFlasherViewFlasher,
        view_flasher_get_view(app->views.flasher));

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    wch_swio_flasher_load_defaults(app);

    scene_manager_next_scene(app->scene_manager, WchSwioFlasherSceneMain);
    return app;
}

void wch_swio_flasher_app_free(WchSwioFlasherApp* app) {
    furi_assert(app);

    view_dispatcher_stop(app->view_dispatcher);

    view_dispatcher_remove_view(app->view_dispatcher, WchSwioFlasherViewWidget);
    view_dispatcher_remove_view(app->view_dispatcher, WchSwioFlasherViewSubmenu);
    view_dispatcher_remove_view(app->view_dispatcher, WchSwioFlasherViewDebuggerEmulator);
    view_dispatcher_remove_view(app->view_dispatcher, WchSwioFlasherViewFileBrowser);
    view_dispatcher_remove_view(app->view_dispatcher, WchSwioFlasherViewFlasher);

    widget_free(app->views.widget);
    submenu_free(app->views.submenu);
    file_browser_free(app->views.file_browser);
    furi_string_free(app->views.file_path);

    debugger_emulator_free(app->views.debuger_emulator);
    view_flasher_free(app->views.flasher);
    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);

    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);

    WchSwioFlasher_NhcLink042Emu_destroy(app->services.emulator);
    WchSwioFlasher_MinichlinkDebugger_destroy(app->services.mini_debugger);
    WchSwioFlasher_Ch32vFlipperFlasher_destroy(app->services.flasher);

    WchSwioFlasher_WchFlasher_destroy(app->helpers.flasher);
    WchSwioFlasher_RiscVDebug_destroy(app->helpers.riscv_debug);
    WchSwioFlasher_SWIO_destroy(app->helpers.swio);

    free(app);
}

int32_t wch_swio_flasher_app(void* p) {
    UNUSED(p);

    WchSwioFlasherApp* app = wch_swio_flasher_app_alloc();

    dolphin_deed(DolphinDeedPluginStart);

    view_dispatcher_run(app->view_dispatcher);

    wch_swio_flasher_app_free(app);

    return 0;
}
