#include "../gba_cartridge_app.h"
#include "../helpers/gba_cartridge_custom_event.h"
#include "../views/gba_cartridge_startscreen.h"

void gba_cartridge_scene_startscreen_callback(GBACartridgeCustomEvent event, void* context) {
    furi_assert(context);
    GBACartridge* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

void gba_cartridge_scene_startscreen_on_enter(void* context) {
    furi_assert(context);
    GBACartridge* app = context;
    gba_cartridge_startscreen_set_callback(
        app->gba_cartridge_startscreen, gba_cartridge_scene_startscreen_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, GBACartridgeViewIdStartscreen);
}

bool gba_cartridge_scene_startscreen_on_event(void* context, SceneManagerEvent event) {
    GBACartridge* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case GBACartridgeCustomEventStartscreenLeft:
        case GBACartridgeCustomEventStartscreenRight:
            break;
        case GBACartridgeCustomEventStartscreenUp:
        case GBACartridgeCustomEventStartscreenDown:
            break;
        case GBACartridgeCustomEventStartscreenOk:
            scene_manager_next_scene(app->scene_manager, GBACartridgeSceneMenu);
            consumed = true;
            break;
        case GBACartridgeCustomEventStartscreenBack:
            notification_message(app->notification, &sequence_reset_red);
            notification_message(app->notification, &sequence_reset_green);
            notification_message(app->notification, &sequence_reset_blue);
            if(!scene_manager_search_and_switch_to_previous_scene(
                   app->scene_manager, GBACartridgeSceneStartscreen)) {
                scene_manager_stop(app->scene_manager);
                view_dispatcher_stop(app->view_dispatcher);
            }
            consumed = true;
            break;
        }
    }

    return consumed;
}

void gba_cartridge_scene_startscreen_on_exit(void* context) {
    GBACartridge* app = context;
    UNUSED(app);
}