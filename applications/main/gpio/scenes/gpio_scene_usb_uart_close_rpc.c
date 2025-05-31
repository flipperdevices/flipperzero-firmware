#include "../gpio_app_i.h"
#include "../gpio_custom_event.h"

void gpio_scene_usb_uart_close_rpc_on_enter(void* context) {
    GpioApp* app = context;

    // Pomijamy komunikat o aktywnym połączeniu i od razu wracamy do poprzedniego ekranu
    if(!scene_manager_previous_scene(app->scene_manager)) {
        scene_manager_stop(app->scene_manager);
        view_dispatcher_stop(app->view_dispatcher);
    }
}

bool gpio_scene_usb_uart_close_rpc_on_event(void* context, SceneManagerEvent event) {
    GpioApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GpioCustomEventErrorBack) {
            if(!scene_manager_previous_scene(app->scene_manager)) {
                scene_manager_stop(app->scene_manager);
                view_dispatcher_stop(app->view_dispatcher);
            }
            consumed = true;
        }
    }
    return consumed;
}

void gpio_scene_usb_uart_close_rpc_on_exit(void* context) {
    GpioApp* app = context;
    widget_reset(app->widget);
}
