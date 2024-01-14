#include "mfc_editor_app_i.h"

bool mfc_editor_app_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    MfcEditorApp* instance = context;
    return scene_manager_handle_custom_event(instance->scene_manager, event);
}

bool mfc_editor_app_back_event_callback(void* context) {
    furi_assert(context);
    MfcEditorApp* instance = context;
    return scene_manager_handle_back_event(instance->scene_manager);
}

void mfc_editor_app_tick_event_callback(void* context) {
    furi_assert(context);
    MfcEditorApp* instance = context;
    scene_manager_handle_tick_event(instance->scene_manager);
}

MfcEditorApp* mfc_editor_app_alloc() {
    MfcEditorApp* instance = malloc(sizeof(MfcEditorApp));

    instance->view_dispatcher = view_dispatcher_alloc();
    instance->scene_manager = scene_manager_alloc(&mfc_editor_scene_handlers, instance);
    view_dispatcher_enable_queue(instance->view_dispatcher);
    view_dispatcher_set_event_callback_context(instance->view_dispatcher, instance);
    view_dispatcher_set_custom_event_callback(
        instance->view_dispatcher, mfc_editor_app_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        instance->view_dispatcher, mfc_editor_app_back_event_callback);
    view_dispatcher_set_tick_event_callback(
        instance->view_dispatcher, mfc_editor_app_tick_event_callback, 100);

    instance->gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(
        instance->view_dispatcher, instance->gui, ViewDispatcherTypeFullscreen);

    instance->nfc_device = nfc_device_alloc();

    instance->submenu = submenu_alloc();
    view_dispatcher_add_view(
        instance->view_dispatcher, MfcEditorAppViewSubmenu, submenu_get_view(instance->submenu));

    return instance;
}

void mfc_editor_app_free(MfcEditorApp* instance) {
    furi_assert(instance);

    view_dispatcher_remove_view(instance->view_dispatcher, MfcEditorAppViewSubmenu);
    submenu_free(instance->submenu);

    view_dispatcher_free(instance->view_dispatcher);
    scene_manager_free(instance->scene_manager);

    furi_record_close(RECORD_GUI);
    instance->gui = NULL;

    nfc_device_free(instance->nfc_device);

    free(instance);
}

int32_t mfc_editor_app(void* p) {
    UNUSED(p);

    MfcEditorApp* instance = mfc_editor_app_alloc();

    scene_manager_next_scene(instance->scene_manager, MfcEditorSceneStart);

    view_dispatcher_run(instance->view_dispatcher);

    mfc_editor_app_free(instance);

    return 0;
}
