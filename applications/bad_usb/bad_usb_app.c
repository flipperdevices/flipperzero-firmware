#include "bad_usb_app_i.h"
#include <furi.h>
#include <furi-hal.h>

BadUsbApp* bad_usb_app_alloc() {
    BadUsbApp* app = furi_alloc(sizeof(BadUsbApp));

    app->gui = furi_record_open("gui");
    app->notifications = furi_record_open("notification");
    app->dialogs = furi_record_open("dialogs");

    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_allocate_scene_manager(app->view_dispatcher, &bad_usb_scene_handlers, app);
    view_dispatcher_set_start_scene(app->view_dispatcher, BadUsbSceneFileSelect);
    app->scene_manager = view_dispatcher_get_scene_manager(app->view_dispatcher);
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_tick_event_period(app->view_dispatcher, 500);

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    app->bad_usb_view = bad_usb_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, BadUsbAppViewWork, bad_usb_get_view(app->bad_usb_view));

    return app;
}

void bad_usb_app_free(BadUsbApp* app) {
    furi_assert(app);

    // Views
    view_dispatcher_remove_view(app->view_dispatcher, BadUsbAppViewFileSelect);
    view_dispatcher_remove_view(app->view_dispatcher, BadUsbAppViewWork);
    bad_usb_free(app->bad_usb_view);

    // View dispatcher
    view_dispatcher_free(app->view_dispatcher);

    // Close records
    furi_record_close("gui");
    furi_record_close("notification");
    furi_record_close("dialogs");

    free(app);
}

int32_t bad_usb_app(void* p) {
    UsbInterface* usb_mode_prev = furi_hal_usb_get_config();
    furi_hal_usb_set_config(&usb_hid);

    BadUsbApp* bad_usb_app = bad_usb_app_alloc();

    view_dispatcher_run(bad_usb_app->view_dispatcher);

    furi_hal_usb_set_config(usb_mode_prev);
    bad_usb_app_free(bad_usb_app);

    return 0;
}
