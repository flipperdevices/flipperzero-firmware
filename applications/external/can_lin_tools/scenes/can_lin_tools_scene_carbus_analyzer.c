#include "../can_lin_tools_app_i.h"
#include "../views/can_lin_tools_view_can_hacker2.h"

enum {
    CanLinToolsSceneCarBusAnalyzerStart,
    CanLinToolsSceneCarBusAnalyzerRun,
};

void can_lin_tools_scene_carbus_analyzer_callback(CanLinToolsCustomEvent event, void* context) {
    furi_assert(context);
    CanLinToolsApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

// static void can_lin_tools_scene_carbus_analyzer_update(void* context) {
//     furi_assert(context);
//     CanLinToolsApp* app = context;
//     UNUSED(app);
// }

void can_lin_tools_scene_carbus_analyzer_on_enter(void* context) {
    furi_assert(context);
    CanLinToolsApp* app = context;

    scene_manager_set_scene_state(
        app->scene_manager, CanLinToolsSceneCarBusAnalyzer, CanLinToolsSceneCarBusAnalyzerStart);

    can_lin_tools_view_carbus_analyzer_set_callback(
        app->can_lin_tools_carbus_analyzer_view, can_lin_tools_scene_carbus_analyzer_callback, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, CanLinToolsViewCarBusAnalyzer);
}

bool can_lin_tools_scene_carbus_analyzer_on_event(void* context, SceneManagerEvent event) {
    furi_assert(context);
    CanLinToolsApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case CanLinToolsCustomEventModuleDisconnected:

            consumed = true;
            break;
        case CanLinToolsCustomEventProcDisconnected:

            if(scene_manager_get_scene_state(app->scene_manager, CanLinToolsSceneCarBusAnalyzer) ==
               CanLinToolsSceneCarBusAnalyzerRun) {
                notification_message(app->notifications, &sequence_error);
            } else {
                scene_manager_set_scene_state(
                    app->scene_manager, CanLinToolsSceneCarBusAnalyzer, CanLinToolsSceneCarBusAnalyzerRun);
            }

            notification_message(app->notifications, &sequence_solid_yellow);
            consumed = true;
            break;
        case CanLinToolsCustomEventProcConnected:
            notification_message(app->notifications, &sequence_success);
            notification_message(app->notifications, &sequence_set_green_255);
            consumed = true;
            break;
        default:
            break;
        }
    }

    // if(event.type == SceneManagerEventTypeTick) {
    //     can_lin_tools_scene_carbus_analyzer_update(app);
    // }

    return consumed;
}

void can_lin_tools_scene_carbus_analyzer_on_exit(void* context) {
    furi_assert(context);
    CanLinToolsApp* app = context;
    notification_message(app->notifications, &sequence_reset_rgb);
    UNUSED(app);
}
