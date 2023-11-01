#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/modules/submenu.h>
#include <gui/view_dispatcher.h>
#include <gui/canvas.h>
#include <input/input.h>

#include "uart_helper.h"

#define DEVICE_BAUDRATE 115200
#define LINE_DELIMITER '\n'
#define INCLUDE_LINE_DELIMITER false

typedef struct {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    uint32_t index;
    UartHelper* uart_helper;
    FuriString* message2;
} UartDemoApp;

typedef enum {
    UartDemoSubMenuViewId = 1,
} UartDemoViewIds;

/**
 * This callback function is called when a submenu item is clicked.
 * 
 * @param context The context passed to the submenu.
 * @param index   The index of the submenu item that was clicked.
*/
static void uart_demo_submenu_item_callback(void* context, uint32_t index);

/**
 * Adds the default submenu entries.
 * 
 * @param submenu The submenu.
 * @param context The context to pass to the submenu item callback function.
*/
static void uart_demo_submenu_add_default_entries(Submenu* submenu, void* context) {
    UartDemoApp* app = context;
    submenu_reset(submenu);
    submenu_add_item(submenu, "Clear", 0, uart_demo_submenu_item_callback, context);
    submenu_add_item(submenu, "Send Msg 1", 1, uart_demo_submenu_item_callback, context);
    submenu_add_item(submenu, "Send Msg 2", 2, uart_demo_submenu_item_callback, context);

    app->index = 3;
}

static void uart_demo_submenu_item_callback(void* context, uint32_t index) {
    UartDemoApp* app = context;

    if(index == 0) {
        // Clear the submenu and add the default entries.
        uart_demo_submenu_add_default_entries(app->submenu, app);
    } else if(index == 1) {
        // Send a "Hello World!" message over the UART.
        uart_helper_send(app->uart_helper, "Hello World!\n", 13);
    } else if(index == 2) {
        furi_string_printf(app->message2, "Index is %ld.\n", app->index);
        uart_helper_send_string(app->uart_helper, app->message2);
    } else {
        // The item was received data.
    }
}

static void uart_demo_process_line(FuriString* line, void* context) {
    UartDemoApp* app = context;
    submenu_add_item(
        app->submenu,
        furi_string_get_cstr(line),
        app->index++,
        uart_demo_submenu_item_callback,
        app);
}

static bool uart_demo_navigation_callback(void* context) {
    UNUSED(context);
    // We don't want to handle any navigation events, the back button should exit the app.
    return true;
}

static uint32_t uart_demo_exit(void* context) {
    UNUSED(context);
    // Exit the app.
    return VIEW_NONE;
}

static UartDemoApp* uart_demo_app_alloc() {
    UartDemoApp* app = malloc(sizeof(UartDemoApp));

    // Initialize the GUI. Create a view dispatcher and attach it to the GUI.
    // Create a submenu, add default entries and add the submenu to the view
    // dispatcher. Set the submenu as the current view.
    app->gui = furi_record_open(RECORD_GUI);
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);
    app->submenu = submenu_alloc();
    uart_demo_submenu_add_default_entries(app->submenu, app);
    view_dispatcher_add_view(
        app->view_dispatcher, UartDemoSubMenuViewId, submenu_get_view(app->submenu));
    view_set_previous_callback(submenu_get_view(app->submenu), uart_demo_exit);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, uart_demo_navigation_callback);
    view_dispatcher_switch_to_view(app->view_dispatcher, UartDemoSubMenuViewId);

    // Allocate a string to store the second message.
    app->message2 = furi_string_alloc();

    // Initialize the UART helper.
    app->uart_helper = uart_helper_alloc();
    uart_helper_set_baud_rate(app->uart_helper, DEVICE_BAUDRATE);
    uart_helper_set_delimiter(app->uart_helper, LINE_DELIMITER, INCLUDE_LINE_DELIMITER);
    uart_helper_set_callback(app->uart_helper, uart_demo_process_line, app);

    return app;
}

static void uart_demo_app_free(UartDemoApp* app) {
    uart_helper_free(app->uart_helper);

    furi_string_free(app->message2);

    view_dispatcher_remove_view(app->view_dispatcher, UartDemoSubMenuViewId);
    view_dispatcher_free(app->view_dispatcher);
    submenu_free(app->submenu);
    furi_record_close(RECORD_GUI);

    free(app);
}

int32_t uart_demo_main(void* p) {
    UNUSED(p);

    UartDemoApp* app = uart_demo_app_alloc();
    view_dispatcher_run(app->view_dispatcher);
    uart_demo_app_free(app);

    return 0;
}