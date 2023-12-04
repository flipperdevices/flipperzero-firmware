#include "../gba_cartridge_app.h"
#include "../helpers/gba_cartridge_custom_event.h"
#include "../views/gba_cartridge_scene_1.h"

void gba_cartridge_scene_1_callback(GBACartridgeCustomEvent event, void* context) {
    furi_assert(context);
    GBACartridge* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

void gba_cartridge_scene_scene_1_on_enter(void* context) {
    furi_assert(context);
    GBACartridge* app = context;
    gba_cartridge_scene_1_set_callback(
        app->gba_cartridge_scene_1, gba_cartridge_scene_1_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, GBACartridgeViewIdScene1);
}

bool gba_cartridge_scene_scene_1_on_event(void* context, SceneManagerEvent event) {
    GBACartridge* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case GBACartridgeCustomEventScene1Left:
        case GBACartridgeCustomEventScene1Right:
            break;
        case GBACartridgeCustomEventScene1Up:
        case GBACartridgeCustomEventScene1Down:
            break;
        case GBACartridgeCustomEventScene1Back:
            notification_message(app->notification, &sequence_reset_red);
            notification_message(app->notification, &sequence_reset_green);
            notification_message(app->notification, &sequence_reset_blue);

            if(!scene_manager_search_and_switch_to_previous_scene(
                   app->scene_manager, GBACartridgeSceneMenu)) {
                scene_manager_stop(app->scene_manager);
                view_dispatcher_stop(app->view_dispatcher);
            }
            consumed = true;
            break;
        }
    }

    return consumed;
}

void gba_cartridge_scene_scene_1_on_exit(void* context) {
    GBACartridge* app = context;
    // UNUSED(app);
    // Unregister rx callback
    uart_set_handle_rx_data_cb(app->uart, NULL);
    uart_set_handle_rx_data_cb(app->lp_uart, NULL);
}