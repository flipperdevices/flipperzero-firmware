#include "../gb_cartridge_app.h"
#include "../helpers/gb_cartridge_custom_event.h"
#include "../views/gb_cartridge_startscreen.h"

void gb_cartridge_scene_startscreen_callback(GBCartridgeCustomEvent event, void* context) {
    furi_assert(context);
    GBCartridge* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

void gb_cartridge_scene_startscreen_on_enter(void* context) {
    furi_assert(context);
    GBCartridge* app = context;
    gb_cartridge_startscreen_set_callback(
        app->gb_cartridge_startscreen, gb_cartridge_scene_startscreen_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, GBCartridgeViewIdStartscreen);
}

bool gb_cartridge_scene_startscreen_on_event(void* context, SceneManagerEvent event) {
    GBCartridge* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case GBCartridgeCustomEventStartscreenLeft:
        case GBCartridgeCustomEventStartscreenRight:
            break;
        case GBCartridgeCustomEventStartscreenUp:
        case GBCartridgeCustomEventStartscreenDown:
            break;
        case GBCartridgeCustomEventStartscreenOk:
            scene_manager_next_scene(app->scene_manager, GBCartridgeSceneMenu);
            consumed = true;
            break;
        case GBCartridgeCustomEventStartscreenBack:
            notification_message(app->notification, &sequence_reset_red);
            notification_message(app->notification, &sequence_reset_green);
            notification_message(app->notification, &sequence_reset_blue);
            if(!scene_manager_search_and_switch_to_previous_scene(
                   app->scene_manager, GBCartridgeSceneStartscreen)) {
                scene_manager_stop(app->scene_manager);
                view_dispatcher_stop(app->view_dispatcher);
            }
            consumed = true;
            break;
        }
    }

    return consumed;
}

void gb_cartridge_scene_startscreen_on_exit(void* context) {
    GBCartridge* app = context;
    UNUSED(app);
}