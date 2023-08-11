#include "../flipbip.h"
#include "../helpers/flipbip_custom_event.h"
#include "../views/flipbip_scene_1.h"

void flipbip_scene_1_callback(FlipBipCustomEvent event, void* context) {
    furi_assert(context);
    FlipBip* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

void flipbip_scene_scene_1_on_enter(void* context) {
    furi_assert(context);
    FlipBip* app = context;

    if(app->import_from_mnemonic == 1) {
        // handle mnemonic seed import mode with text input, this only
        // uses this scene to have a correct stack of scenes
        app->input_state = FlipBipTextInputMnemonic;
        text_input_set_header_text(app->text_input, "Enter mnemonic phrase");
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipBipViewIdTextInput);
    } else {
        // handle all other modes, these actually use this scene's logic
        flipbip_scene_1_set_callback(app->flipbip_scene_1, flipbip_scene_1_callback, app);
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipBipViewIdScene1);
    }
}

bool flipbip_scene_scene_1_on_event(void* context, SceneManagerEvent event) {
    FlipBip* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        // case FlipBipCustomEventScene1Left:
        // case FlipBipCustomEventScene1Right:
        //     break;
        // case FlipBipCustomEventScene1Up:
        // case FlipBipCustomEventScene1Down:
        //     break;
        case FlipBipCustomEventScene1Back:
            //notification_message(app->notification, &sequence_reset_red);
            //notification_message(app->notification, &sequence_reset_green);
            //notification_message(app->notification, &sequence_reset_blue);
            if(!scene_manager_search_and_switch_to_previous_scene(
                   app->scene_manager, FlipBipSceneMenu)) {
                scene_manager_stop(app->scene_manager);
                view_dispatcher_stop(app->view_dispatcher);
            }
            consumed = true;
            break;
        }
    }

    return consumed;
}

void flipbip_scene_scene_1_on_exit(void* context) {
    FlipBip* app = context;
    UNUSED(app);
}