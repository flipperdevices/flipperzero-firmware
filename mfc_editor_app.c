#include "mfc_editor_app_i.h"

MfcEditorApp* mfc_editor_app_alloc() {
    MfcEditorApp* instance = malloc(sizeof(MfcEditorApp));

    instance->view_dispatcher = view_dispatcher_alloc();
    instance->scene_manager = scene_manager_alloc(&mfc_editor_scene_handlers, instance);
    view_dispatcher_enable_queue(instance->view_dispatcher);
    view_dispatcher_set_event_callback_context(instance->view_dispatcher, instance);

    instance->gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(
        instance->view_dispatcher, instance->gui, ViewDispatcherTypeFullscreen);

    instance->nfc_device = nfc_device_alloc();

    return instance;
}

void mfc_editor_app_free(MfcEditorApp* instance) {
    furi_assert(instance);

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
