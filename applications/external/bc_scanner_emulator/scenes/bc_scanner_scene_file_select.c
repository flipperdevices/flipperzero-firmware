#include "../bc_scanner_app_i.h"
#include <storage/storage.h>

static bool bc_scanner_file_select(BarCodeApp* bc_app) {
    furi_assert(bc_app);

    Storage* storage = furi_record_open(RECORD_STORAGE);
    if(!storage_file_exists(storage, BC_SCANNER_APP_PATH_FOLDER)) {
        storage_common_mkdir(storage, BC_SCANNER_APP_PATH_FOLDER); //Make Folder If dir not exist
    }
    furi_record_close(RECORD_STORAGE);

    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(&browser_options, BC_SCANNER_APP_EXTENSION, &I_bc_10px);
    browser_options.base_path = BC_SCANNER_APP_PATH_FOLDER;

    // Input events and views are managed by file_browser
    bool res = dialog_file_browser_show(
        bc_app->dialogs, bc_app->file_path, bc_app->file_path, &browser_options);

    return res;
}

void bc_scanner_scene_file_select_on_enter(void* context) {
    BarCodeApp* bc_app = context;

    if(bc_scanner_file_select(bc_app)) {
        scene_manager_next_scene(bc_app->scene_manager, BarCodeAppViewWork);
    } else {
        //scene_manager_previous_scene(bc_app->scene_manager);
        view_dispatcher_stop(bc_app->view_dispatcher);
    }
}

bool bc_scanner_scene_file_select_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    // BadUsbApp* bad_usb = context;
    return false;
}

void bc_scanner_scene_file_select_on_exit(void* context) {
    UNUSED(context);
    // BadUsbApp* bad_usb = context;
}
