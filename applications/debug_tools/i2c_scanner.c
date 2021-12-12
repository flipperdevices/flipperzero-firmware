#include <furi.h>
#include <furi-hal.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/gui.h>

static const char header[] = "I2C C0=SCL C1=SDA";

typedef struct {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    bool device_found[128];
    char* submenu_labels[128];
} I2CScannerApp;

static uint32_t i2c_scanner_exit(void* context) {
    return VIEW_NONE;
}

static void submenu_callback(void* context, uint32_t index) {
    return;
}

static void update_list(I2CScannerApp* app) {
    bool empty = true;
    submenu_clean(app->submenu);
    submenu_set_header(app->submenu, header);

    for(uint8_t address = 0; address < 128; address++) {
        if(app->device_found[address]) {
            empty = false;
            app->submenu_labels[address] = furi_alloc(40);
            sprintf(app->submenu_labels[address], "Address 0x%x (%d)", address, address);
            submenu_add_item(
                app->submenu, app->submenu_labels[address], address, submenu_callback, NULL);
        }
    }

    if(empty) {
        submenu_add_item(app->submenu, "No devices found", 0, submenu_callback, NULL);
    }
}

static void scan(I2CScannerApp* app) {
    furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);
    uint8_t test = 0;
    for(uint8_t address = 0; address < 128; address++) {
        app->device_found[address] =
            furi_hal_i2c_rx(&furi_hal_i2c_handle_external, address << 1, &test, 1, 2);
    }
    furi_hal_i2c_release(&furi_hal_i2c_handle_external);
}

static I2CScannerApp* i2c_scanner_app_alloc() {
    I2CScannerApp* app = furi_alloc(sizeof(I2CScannerApp));

    app->gui = furi_record_open("gui");
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    app->submenu = submenu_alloc();
    submenu_set_header(app->submenu, header);
    submenu_add_item(app->submenu, "Scanning...", 0, submenu_callback, NULL);

    view_set_previous_callback(submenu_get_view(app->submenu), i2c_scanner_exit);
    view_dispatcher_add_view(app->view_dispatcher, 0, submenu_get_view(app->submenu));

    view_dispatcher_switch_to_view(app->view_dispatcher, 0);

    // prepare submenu labels array
    memset(app->submenu_labels, 0, sizeof(app->submenu_labels));

    return app;
}

static void i2c_scanner_app_free(I2CScannerApp* app) {
    view_dispatcher_remove_view(app->view_dispatcher, 0);
    submenu_free(app->submenu);
    view_dispatcher_free(app->view_dispatcher);

    furi_record_close("gui");

    // free submenu labels
    for(uint8_t address = 0; address < 128; address++) {
        free(app->submenu_labels[address]);
    }

    free(app);
}

int32_t i2c_scanner_app(void* p) {
    I2CScannerApp* app = i2c_scanner_app_alloc();
    scan(app);
    update_list(app);
    view_dispatcher_run(app->view_dispatcher);
    i2c_scanner_app_free(app);
    return 0;
}
