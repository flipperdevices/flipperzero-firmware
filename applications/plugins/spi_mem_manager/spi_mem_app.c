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

static void spi_mem_file_create_folder(SPIMemApp* app) {
    if(!storage_simply_mkdir(app->storage, SPI_MEM_FILE_FOLDER)) {
        dialog_message_show_storage_error(app->dialogs, "Cannot create\napp folder");
    }
}

bool spi_mem_file_select(SPIMemApp* app) {
    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(&browser_options, SPI_MEM_FILE_EXTENSION, &I_Dip8_10px);
    bool success =
        dialog_file_browser_show(app->dialogs, app->file_path, app->file_path, &browser_options);
    if(success) {
        // success = ibutton_load_key_data(ibutton, ibutton->file_path, true);
    }
    return success;
}

SPIMemApp* spi_mem_alloc(void) {
    SPIMemApp* instance = malloc(sizeof(SPIMemApp));

    instance->file_path = furi_string_alloc();
    instance->gui = furi_record_open(RECORD_GUI);
    instance->notifications = furi_record_open(RECORD_NOTIFICATION);
    instance->view_dispatcher = view_dispatcher_alloc();
    instance->scene_manager = scene_manager_alloc(&spi_mem_scene_handlers, instance);
    instance->submenu = submenu_alloc();
    instance->dialog_ex = dialog_ex_alloc();
    instance->popup = popup_alloc();
    instance->worker = spi_mem_worker_alloc();
    instance->dialogs = furi_record_open(RECORD_DIALOGS);
    instance->storage = furi_record_open(RECORD_STORAGE);

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
    view_dispatcher_add_view(
        instance->view_dispatcher, SPIMemViewDialogEx, dialog_ex_get_view(instance->dialog_ex));
    view_dispatcher_add_view(
        instance->view_dispatcher, SPIMemViewPopup, popup_get_view(instance->popup));

    scene_manager_next_scene(instance->scene_manager, SPIMemSceneStart);
    return instance;
}

void spi_mem_free(SPIMemApp* instance) {
    view_dispatcher_remove_view(instance->view_dispatcher, SPIMemViewSubmenu);
    view_dispatcher_remove_view(instance->view_dispatcher, SPIMemViewDialogEx);
    view_dispatcher_remove_view(instance->view_dispatcher, SPIMemViewPopup);
    submenu_free(instance->submenu);
    dialog_ex_free(instance->dialog_ex);
    popup_free(instance->popup);
    view_dispatcher_free(instance->view_dispatcher);
    scene_manager_free(instance->scene_manager);
    spi_mem_worker_free(instance->worker);
    furi_record_close(RECORD_STORAGE);
    furi_record_close(RECORD_DIALOGS);
    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_GUI);
    furi_string_free(instance->file_path);
    free(instance);
}

int32_t spi_mem_app(void* p) {
    UNUSED(p);
    SPIMemApp* instance = spi_mem_alloc();
    spi_mem_file_create_folder(instance);
    view_dispatcher_run(instance->view_dispatcher);
    spi_mem_free(instance);
    return 0;
}
