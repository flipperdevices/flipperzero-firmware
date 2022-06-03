#include "infrared_i.h"

static bool infrared_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    Infrared* infrared = context;
    return scene_manager_handle_custom_event(infrared->scene_manager, event);
}

static bool infrared_back_event_callback(void* context) {
    furi_assert(context);
    Infrared* infrared = context;
    return scene_manager_handle_back_event(infrared->scene_manager);
}

static void infrared_tick_event_callback(void* context) {
    furi_assert(context);
    Infrared* infrared = context;
    scene_manager_handle_tick_event(infrared->scene_manager);
}

static Infrared* infrared_alloc() {
    Infrared* infrared = malloc(sizeof(Infrared));

    infrared->scene_manager = scene_manager_alloc(&infrared_scene_handlers, infrared);
    infrared->view_dispatcher = view_dispatcher_alloc();

    infrared->gui = furi_record_open("gui");

    view_dispatcher_attach_to_gui(
        infrared->view_dispatcher, infrared->gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_enable_queue(infrared->view_dispatcher);
    view_dispatcher_set_event_callback_context(infrared->view_dispatcher, infrared);
    view_dispatcher_set_custom_event_callback(
        infrared->view_dispatcher, infrared_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        infrared->view_dispatcher, infrared_back_event_callback);
    view_dispatcher_set_tick_event_callback(
        infrared->view_dispatcher, infrared_tick_event_callback, 100);

    infrared->storage = furi_record_open("storage");
    infrared->dialogs = furi_record_open("dialogs");
    infrared->notifications = furi_record_open("notification");

    infrared->submenu = submenu_alloc();
    view_dispatcher_add_view(
        infrared->view_dispatcher, InfraredViewSubmenu, submenu_get_view(infrared->submenu));

    return infrared;
}

static void infrared_free(Infrared* infrared) {
    furi_assert(infrared);

    view_dispatcher_remove_view(infrared->view_dispatcher, InfraredViewSubmenu);
    submenu_free(infrared->submenu);

    view_dispatcher_free(infrared->view_dispatcher);
    scene_manager_free(infrared->scene_manager);

    furi_record_close("gui");
    infrared->gui = NULL;

    furi_record_close("notification");
    infrared->notifications = NULL;

    furi_record_close("dialogs");
    infrared->dialogs = NULL;

    furi_record_close("gui");
    infrared->gui = NULL;

    free(infrared);
}

int32_t infrared_app(void* p) {
    UNUSED(p);

    Infrared* infrared = infrared_alloc();

    scene_manager_next_scene(infrared->scene_manager, InfraredSceneStart);
    view_dispatcher_run(infrared->view_dispatcher);

    infrared_free(infrared);
    return 0;
}
