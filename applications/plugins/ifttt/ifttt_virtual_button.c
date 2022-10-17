#include "ifttt_virtual_button.h"

static bool ifttt_virtual_button_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    VirtualButtonApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool ifttt_virtual_button_back_event_callback(void* context) {
    furi_assert(context);
    VirtualButtonApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static void ifttt_virtual_button_tick_event_callback(void* context) {
    furi_assert(context);
    VirtualButtonApp* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

VirtualButtonApp* ifttt_virtual_button_app_alloc(uint32_t first_scene) {
    VirtualButtonApp* app = malloc(sizeof(VirtualButtonApp));

    // Records
    app->gui = furi_record_open(RECORD_GUI);
    app->power = furi_record_open(RECORD_POWER);

    // View dispatcher
    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager = scene_manager_alloc(&virtual_button_scene_handlers, app);
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, ifttt_virtual_button_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, ifttt_virtual_button_back_event_callback);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, ifttt_virtual_button_tick_event_callback, 2000);
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    // Views
    app->sen_view = send_view_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, VirtualButtonAppViewSendView, send_view_get_view(app->sen_view));

    app->modul_view = module_view_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        VirtualButtonAppViewModuleView,
        module_view_get_view(app->modul_view));

    app->reboo_view = reboot_view_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        VirtualButtonAppViewRebootView,
        reboot_view_get_view(app->reboo_view));

    app->rese_view = reset_view_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, VirtualButtonAppViewResetView, reset_view_get_view(app->rese_view));

    app->submenu = submenu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, VirtualButtonAppViewSubmenu, submenu_get_view(app->submenu));
    app->dialog = dialog_ex_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, VirtualButtonAppViewDialog, dialog_ex_get_view(app->dialog));

    // Set first scene
    scene_manager_next_scene(app->scene_manager, first_scene);
    return app;
}

void ifttt_virtual_button_app_free(VirtualButtonApp* app) {
    furi_assert(app);
    // Views
    view_dispatcher_remove_view(app->view_dispatcher, VirtualButtonAppViewSendView);
    send_view_free(app->sen_view);
    view_dispatcher_remove_view(app->view_dispatcher, VirtualButtonAppViewModuleView);
    module_view_free(app->modul_view);
    view_dispatcher_remove_view(app->view_dispatcher, VirtualButtonAppViewRebootView);
    reboot_view_free(app->reboo_view);
    view_dispatcher_remove_view(app->view_dispatcher, VirtualButtonAppViewResetView);
    reset_view_free(app->rese_view);
    view_dispatcher_remove_view(app->view_dispatcher, VirtualButtonAppViewSubmenu);
    submenu_free(app->submenu);
    view_dispatcher_remove_view(app->view_dispatcher, VirtualButtonAppViewDialog);
    dialog_ex_free(app->dialog);
    // View dispatcher
    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);
    // Records
    furi_record_close(RECORD_POWER);
    furi_record_close(RECORD_GUI);
    free(app);
}

int32_t ifttt_virtual_button_app(void* p) {
    UNUSED(p);
    uint32_t first_scene = VirtualButtonAppSceneStart;
    VirtualButtonApp* app = ifttt_virtual_button_app_alloc(first_scene);
    view_dispatcher_run(app->view_dispatcher);
    ifttt_virtual_button_app_free(app);
    return 0;
}
