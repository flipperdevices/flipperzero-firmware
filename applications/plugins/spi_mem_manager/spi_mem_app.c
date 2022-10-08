#include "spi_mem_app.h"

#define TAG "SPIMem"

static bool spi_mem_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    SPIMemApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool spi_mem_back_event_callback(void* context) {
    furi_assert(context);
    SPIMemApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

SPIMemApp* spi_mem_alloc(void) {
    SPIMemApp* instance = malloc(sizeof(SPIMemApp));

    instance->gui = furi_record_open(RECORD_GUI);
    instance->view_dispatcher = view_dispatcher_alloc();
    instance->scene_manager = scene_manager_alloc(&spi_mem_scene_handlers, instance);
    instance->submenu = submenu_alloc();

    view_dispatcher_enable_queue(instance->view_dispatcher);
    view_dispatcher_set_event_callback_context(instance->view_dispatcher, instance);
    view_dispatcher_set_custom_event_callback(
        instance->view_dispatcher, spi_mem_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        instance->view_dispatcher, spi_mem_back_event_callback);
    view_dispatcher_attach_to_gui(
        instance->view_dispatcher, instance->gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_add_view(
        instance->view_dispatcher, SPIMemViewSubmenu, submenu_get_view(instance->submenu));
    scene_manager_next_scene(instance->scene_manager, SPIMemSceneStart);
    return instance;
}

void spi_mem_free(SPIMemApp* instance) {
    view_dispatcher_remove_view(instance->view_dispatcher, SPIMemViewSubmenu);
    submenu_free(instance->submenu);
    view_dispatcher_free(instance->view_dispatcher);
    scene_manager_free(instance->scene_manager);
    furi_record_close(RECORD_GUI);
    free(instance);
}

int32_t spi_mem_app(void* p) {
    UNUSED(p);
    SPIMemApp* instance = spi_mem_alloc();
    view_dispatcher_run(instance->view_dispatcher);
    spi_mem_free(instance);
    return 0;
}
