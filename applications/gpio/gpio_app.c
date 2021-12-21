#include "gpio_app_i.h"

#include <furi.h>
#include <furi-hal.h>

GpioApp* gpio_app_alloc() {
    GpioApp* app = furi_alloc(sizeof(GpioApp));

    app->gui = furi_record_open("gui");

    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_allocate_scene_manager(app->view_dispatcher, &gpio_scene_handlers, app);
    view_dispatcher_set_start_scene(app->view_dispatcher, GpioSceneStart);
    app->scene_manager = view_dispatcher_get_scene_manager(app->view_dispatcher);
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_tick_event_period(app->view_dispatcher, 100);

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    app->notifications = furi_record_open("notification");

    app->var_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        GpioAppViewVarItemList,
        variable_item_list_get_view(app->var_item_list));
    app->gpio_test = gpio_test_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, GpioAppViewGpioTest, gpio_test_get_view(app->gpio_test));

    app->gpio_usb_uart = gpio_usb_uart_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, GpioAppViewUsbUart, gpio_usb_uart_get_view(app->gpio_usb_uart));

    view_dispatcher_add_view(
        app->view_dispatcher,
        GpioAppViewUsbUartCfg,
        variable_item_list_get_view(app->var_item_list));

    return app;
}

void gpio_app_free(GpioApp* app) {
    furi_assert(app);

    // Views
    view_dispatcher_remove_view(app->view_dispatcher, GpioAppViewVarItemList);
    view_dispatcher_remove_view(app->view_dispatcher, GpioAppViewGpioTest);
    view_dispatcher_remove_view(app->view_dispatcher, GpioAppViewUsbUart);
    view_dispatcher_remove_view(app->view_dispatcher, GpioAppViewUsbUartCfg);
    variable_item_list_free(app->var_item_list);
    gpio_test_free(app->gpio_test);
    gpio_usb_uart_free(app->gpio_usb_uart);

    // View dispatcher
    view_dispatcher_free(app->view_dispatcher);

    // Close records
    furi_record_close("gui");
    furi_record_close("notification");

    free(app);
}

int32_t gpio_app(void* p) {
    GpioApp* gpio_app = gpio_app_alloc();

    view_dispatcher_run(gpio_app->view_dispatcher);

    gpio_app_free(gpio_app);

    return 0;
}
