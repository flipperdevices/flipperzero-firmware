#include "../gb_cartridge_app.h"
#include "../helpers/gb_cartridge_custom_event.h"
#include "../views/gb_cartridge_scene_1.h"

void gb_cartridge_scene_1_callback(GBCartridgeCustomEvent event, void* context) {
    furi_assert(context);
    GBCartridge* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

void gb_cartridge_scene_scene_1_on_enter(void* context) {
    furi_assert(context);
    GBCartridge* app = context;
    gb_cartridge_scene_1_set_callback(
        app->gb_cartridge_scene_1, gb_cartridge_scene_1_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, GBCartridgeViewIdScene1);
}

bool gb_cartridge_scene_scene_1_on_event(void* context, SceneManagerEvent event) {
    GBCartridge* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case GBCartridgeCustomEventScene1Left:
        case GBCartridgeCustomEventScene1Right:
            break;
        case GBCartridgeCustomEventScene1Up:
        case GBCartridgeCustomEventScene1Down:
            break;
        case GBCartridgeCustomEventScene1Back:
            notification_message(app->notification, &sequence_reset_red);
            notification_message(app->notification, &sequence_reset_green);
            notification_message(app->notification, &sequence_reset_blue);

            if(!scene_manager_search_and_switch_to_previous_scene(
                   app->scene_manager, GBCartridgeSceneMenu)) {
                scene_manager_stop(app->scene_manager);
                view_dispatcher_stop(app->view_dispatcher);
            }
            consumed = true;
            break;
        }
    }

    return consumed;
}

void gb_cartridge_scene_scene_1_on_exit(void* context) {
    GBCartridge* app = context;
    // UNUSED(app);
    // Unregister rx callback
    uart_set_handle_rx_data_cb(app->uart, NULL);
    uart_set_handle_rx_data_cb(app->lp_uart, NULL);
}