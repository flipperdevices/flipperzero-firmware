#include <furi.h>
#include <furi-hal.h>
#include "flipper_world_i.h"

bool flipper_world_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    FlipperWorld* flipper_world = (FlipperWorld*)context;
    return scene_manager_handle_custom_event(flipper_world->scene_manager, event);
}

bool flipper_world_back_event_callback(void* context) {
    furi_assert(context);
    FlipperWorld* flipper_world = (FlipperWorld*)context;
    return scene_manager_handle_back_event(flipper_world->scene_manager);
}

FlipperWorld* flipper_world_app_alloc() {
    FlipperWorld* flipper_world = furi_alloc(sizeof(FlipperWorld));

    flipper_world->gui = furi_record_open("gui");
    flipper_world->view_dispatcher = view_dispatcher_alloc();
    flipper_world->scene_manager =
        scene_manager_alloc(&flipper_world_scene_handlers, flipper_world);

    view_dispatcher_enable_queue(flipper_world->view_dispatcher);
    view_dispatcher_attach_to_gui(
        flipper_world->view_dispatcher, flipper_world->gui, ViewDispatcherTypeFullscreen);

    view_dispatcher_set_event_callback_context(flipper_world->view_dispatcher, flipper_world);
    view_dispatcher_set_custom_event_callback(
        flipper_world->view_dispatcher, flipper_world_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        flipper_world->view_dispatcher, flipper_world_back_event_callback);

    flipper_world->main_view = flipper_world_main_alloc();
    flipper_world->food_game = flipper_foodgame_alloc();
    flipper_world->passport = flipper_world_passport_alloc();

    view_dispatcher_add_view(
        flipper_world->view_dispatcher,
        FlipperWorldViewMain,
        flipper_main_get_view(flipper_world->main_view));
    view_dispatcher_add_view(
        flipper_world->view_dispatcher,
        FlipperWorldFoodGame,
        flipper_foodgame_get_view(flipper_world->food_game));
    view_dispatcher_add_view(
        flipper_world->view_dispatcher,
        FlipperWorldPassport,
        flipper_world_passport_get_view(flipper_world->passport));

    return flipper_world;
}

void flipper_world_free(FlipperWorld* flipper_world) {
    furi_assert(flipper_world);

    view_dispatcher_remove_view(flipper_world->view_dispatcher, FlipperWorldViewMain);
    view_dispatcher_remove_view(flipper_world->view_dispatcher, FlipperWorldPassport);
    view_dispatcher_remove_view(flipper_world->view_dispatcher, FlipperWorldFoodGame);

    view_dispatcher_free(flipper_world->view_dispatcher);
    scene_manager_free(flipper_world->scene_manager);

    flipper_world_main_free(flipper_world->main_view);
    flipper_world_passport_free(flipper_world->passport);
    flipper_foodgame_free(flipper_world->food_game);

    furi_record_close("gui");
    flipper_world->gui = NULL;

    free(flipper_world);
}

int32_t flipper_world_app(void* p) {
    FlipperWorld* flipper_world = flipper_world_app_alloc();

    scene_manager_next_scene(flipper_world->scene_manager, FlipperWorldSceneMain);
    view_dispatcher_run(flipper_world->view_dispatcher);
    flipper_world_free(flipper_world);

    return 0;
}
