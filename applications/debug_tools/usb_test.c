#include <furi.h>
#include <furi-hal.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/gui.h>
#include "furi-hal-usb-hid.h"
#include <storage/storage.h>
#include <cmsis_os.h>

typedef struct {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    osThreadAttr_t thread_attr;
    osThreadId_t thread;
} UsbTestApp;

typedef enum {
    UsbTestSubmenuIndexEnable,
    UsbTestSubmenuIndexDisable,
    UsbTestSubmenuIndexVcpSingle,
    UsbTestSubmenuIndexVcpDual,
    UsbTestSubmenuIndexHid,
    UsbTestSubmenuIndexHidU2F,

    UsbTestSubmenuIndexHidBadUsb,
    UsbTestSubmenuIndexHidMouse,
} SubmenuIndex;

static void keyboard_print_task(void* context);

void usb_test_submenu_callback(void* context, uint32_t index) {
    furi_assert(context);
    UsbTestApp* app = context;
    if(index == UsbTestSubmenuIndexEnable) {
        furi_hal_usb_enable();
    } else if(index == UsbTestSubmenuIndexDisable) {
        furi_hal_usb_disable();
    } else if(index == UsbTestSubmenuIndexVcpSingle) {
        furi_hal_usb_set_config(UsbModeVcpSingle);
    } else if(index == UsbTestSubmenuIndexVcpDual) {
        furi_hal_usb_set_config(UsbModeVcpDual);
    } else if(index == UsbTestSubmenuIndexHid) {
        furi_hal_usb_set_config(UsbModeHid);
    } else if(index == UsbTestSubmenuIndexHidU2F) {
        //furi_hal_usb_set_config(UsbModeU2F);
    } else if(index == UsbTestSubmenuIndexHidBadUsb) {
        if(app->thread == NULL)
            app->thread = osThreadNew(keyboard_print_task, (void*)app, &app->thread_attr);
        else if(eTaskGetState(app->thread) == eDeleted)
            app->thread = osThreadNew(keyboard_print_task, (void*)app, &app->thread_attr);
    } else if(index == UsbTestSubmenuIndexHidMouse) {
    }
}

static void keyboard_print_task(void* context) {
    //UsbTestApp* app = context;
    File* script_file = storage_file_alloc(furi_record_open("storage"));

    if(storage_file_open(script_file, "/ext/badusb.txt", FSAM_READ, FSOM_OPEN_EXISTING)) {
        char buffer[16];
        uint16_t ret;
        do {
            ret = storage_file_read(script_file, buffer, 16);
            for(uint16_t i = 0; i < ret; i++) {
                furi_hal_hid_kb_press(HID_ASCII_TO_KEY(buffer[i]));
                furi_hal_hid_kb_release(HID_ASCII_TO_KEY(buffer[i]));
            }
        } while(ret > 0);
    } else {
        FURI_LOG_E("BadUSB", "Script file open error");
    }
    furi_hal_hid_kb_release_all();
    storage_file_close(script_file);
    storage_file_free(script_file);

    osThreadExit();
}

uint32_t usb_test_exit(void* context) {
    return VIEW_NONE;
}

UsbTestApp* usb_test_app_alloc() {
    UsbTestApp* app = furi_alloc(sizeof(UsbTestApp));
    app->thread_attr.name = "hid_keyboard";
    app->thread_attr.stack_size = 1024;

    // Gui
    app->gui = furi_record_open("gui");

    // View dispatcher
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    // Views
    app->submenu = submenu_alloc();
    submenu_add_item(
        app->submenu, "Enable", UsbTestSubmenuIndexEnable, usb_test_submenu_callback, app);
    submenu_add_item(
        app->submenu, "Disable", UsbTestSubmenuIndexDisable, usb_test_submenu_callback, app);
    submenu_add_item(
        app->submenu, "Single VCP", UsbTestSubmenuIndexVcpSingle, usb_test_submenu_callback, app);
    submenu_add_item(
        app->submenu, "Dual VCP", UsbTestSubmenuIndexVcpDual, usb_test_submenu_callback, app);
    submenu_add_item(
        app->submenu, "HID KB+Mouse", UsbTestSubmenuIndexHid, usb_test_submenu_callback, app);
    submenu_add_item(
        app->submenu, "TODO: HID U2F", UsbTestSubmenuIndexHidU2F, usb_test_submenu_callback, app);
    submenu_add_item(
        app->submenu,
        "[HID] Send script",
        UsbTestSubmenuIndexHidBadUsb,
        usb_test_submenu_callback,
        app);
    submenu_add_item(
        app->submenu,
        "[HID] Mouse demo",
        UsbTestSubmenuIndexHidMouse,
        usb_test_submenu_callback,
        app);
    view_set_previous_callback(submenu_get_view(app->submenu), usb_test_exit);
    view_dispatcher_add_view(app->view_dispatcher, 0, submenu_get_view(app->submenu));

    // Switch to menu
    view_dispatcher_switch_to_view(app->view_dispatcher, 0);

    return app;
}

void usb_test_app_free(UsbTestApp* app) {
    furi_assert(app);

    // Free views
    view_dispatcher_remove_view(app->view_dispatcher, 0);
    submenu_free(app->submenu);
    view_dispatcher_free(app->view_dispatcher);

    // Close gui record
    furi_record_close("gui");
    app->gui = NULL;

    // Free rest
    free(app);
}

int32_t usb_test_app(void* p) {
    UsbTestApp* app = usb_test_app_alloc();

    view_dispatcher_run(app->view_dispatcher);

    usb_test_app_free(app);
    return 0;
}
