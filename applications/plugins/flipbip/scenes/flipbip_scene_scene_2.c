// #include "../flipbip.h"
// #include "../helpers/flipbip_custom_event.h"
// #include "../helpers/flipbip_haptic.h"
// #include "../helpers/flipbip_led.h"
// #include "../views/flipbip_scene_2.h"

// void flipbip_scene_2_callback(FlipBipCustomEvent event, void* context) {
//     furi_assert(context);
//     FlipBip* app = context;
//     view_dispatcher_send_custom_event(app->view_dispatcher, event);
// }

// void flipbip_scene_scene_2_on_enter(void* context) {
//     furi_assert(context);
//     FlipBip* app = context;
//     flipbip_scene_2_set_callback(app->flipbip_scene_2, flipbip_scene_2_callback, app);
//     view_dispatcher_switch_to_view(app->view_dispatcher, FlipBipViewIdScene2);
// }

// bool flipbip_scene_scene_2_on_event(void* context, SceneManagerEvent event) {
//     FlipBip* app = context;
//     bool consumed = false;

//     if(event.type == SceneManagerEventTypeCustom) {
//         switch(event.event) {
//             case FlipBipCustomEventScene2Left:
//             case FlipBipCustomEventScene2Right:
//                 break;
//             case FlipBipCustomEventScene2Up:
//             case FlipBipCustomEventScene2Down:
//                 break;
//             case FlipBipCustomEventScene2Back:
//                 notification_message(app->notification, &sequence_reset_red);
//                 notification_message(app->notification, &sequence_reset_green);
//                 notification_message(app->notification, &sequence_reset_blue);
//                 if(!scene_manager_search_and_switch_to_previous_scene(
//                     app->scene_manager, FlipBipSceneMenu)) {
//                         scene_manager_stop(app->scene_manager);
//                         view_dispatcher_stop(app->view_dispatcher);
//                     }
//                 consumed = true;
//                 break;
//         }
//     }

//     return consumed;
// }

// void flipbip_scene_scene_2_on_exit(void* context) {
//     FlipBip* app = context;
//     UNUSED(app);
// }
