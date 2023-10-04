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
#include "utils.h"

#define TAG "WCH_SWIO_FL"

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

WchSwioFlasherApp* wch_swio_flasher_app_alloc() {
    WchSwioFlasherApp* app = malloc(sizeof(WchSwioFlasherApp));

    app->swio = WchSwioFlasher_SWIO_create();
    app->riscv_debug = WchSwioFlasher_RiscVDebug_create(app->swio);
    app->flasher = WchSwioFlasher_WchFlasher_create(app->riscv_debug);

    app->mini_debugger = WchSwioFlasher_MinichlinkDebugger_create(app->swio, app->riscv_debug);
    app->emulator = WchSwioFlasher_NhcLink042Emu_create(app->mini_debugger);

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

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

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

    widget_free(app->views.widget);
    submenu_free(app->views.submenu);
    file_browser_free(app->views.file_browser);
    furi_string_free(app->views.file_path);

    debugger_emulator_free(app->views.debuger_emulator);
    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);

    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);

    WchSwioFlasher_NhcLink042Emu_destroy(app->emulator);
    WchSwioFlasher_MinichlinkDebugger_destroy(app->mini_debugger);

    WchSwioFlasher_WchFlasher_destroy(app->flasher);
    WchSwioFlasher_RiscVDebug_destroy(app->riscv_debug);
    WchSwioFlasher_SWIO_destroy(app->swio);

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
