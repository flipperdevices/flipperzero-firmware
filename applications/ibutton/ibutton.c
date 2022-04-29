#include "ibutton.h"
#include "ibutton_i.h"
#include "ibutton/scenes/ibutton_scene.h"

bool ibutton_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    iButton* ibutton = context;
    return scene_manager_handle_custom_event(ibutton->scene_manager, event);
}

bool ibutton_back_event_callback(void* context) {
    furi_assert(context);
    iButton* ibutton = context;
    return scene_manager_handle_back_event(ibutton->scene_manager);
}

void ibutton_tick_event_callback(void* context) {
    furi_assert(context);
    iButton* ibutton = context;
    scene_manager_handle_tick_event(ibutton->scene_manager);
}

iButton *ibutton_alloc() {
    iButton* ibutton = malloc(sizeof(iButton));

    ibutton->scene_manager = scene_manager_alloc(&ibutton_scene_handlers, ibutton);

    ibutton->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(ibutton->view_dispatcher);
    view_dispatcher_set_event_callback_context(ibutton->view_dispatcher, ibutton);
    view_dispatcher_set_custom_event_callback(ibutton->view_dispatcher, ibutton_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(ibutton->view_dispatcher, ibutton_back_event_callback);
    view_dispatcher_set_tick_event_callback(ibutton->view_dispatcher, ibutton_tick_event_callback, 100);

    ibutton->gui = furi_record_open("gui");
    view_dispatcher_attach_to_gui(ibutton->view_dispatcher, ibutton->gui, ViewDispatcherTypeFullscreen);

    // Submenu
    ibutton->submenu = submenu_alloc();
    view_dispatcher_add_view(ibutton->view_dispatcher, iButtonViewSubmenu, submenu_get_view(ibutton->submenu));

    return ibutton;
}

void ibutton_free(iButton* ibutton) {
    furi_assert(ibutton);

    view_dispatcher_remove_view(ibutton->view_dispatcher, iButtonViewSubmenu);
    submenu_free(ibutton->submenu);

    view_dispatcher_free(ibutton->view_dispatcher);
    scene_manager_free(ibutton->scene_manager);

    furi_record_close("gui");
    ibutton->gui = NULL;

    free(ibutton);
}

int32_t ibutton_app(void *p) {
    iButton *ibutton = ibutton_alloc();

    scene_manager_next_scene(ibutton->scene_manager, iButtonSceneStart);
    view_dispatcher_run(ibutton->view_dispatcher);

    ibutton_free(ibutton);
    return 0;
}
