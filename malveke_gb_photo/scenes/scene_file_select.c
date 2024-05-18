#include "../malveke_gb_photo.h"
#include <storage/storage.h>

static bool file_select(Boilerplate* app) {
    furi_assert(app);

    furi_string_set(app->file_path, MALVEKE_APP_FOLDER_RAMS);
    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(
        &browser_options, MALVEKE_APP_RAM_EXTENSION, &I_icon_10px);
    browser_options.base_path = MALVEKE_APP_FOLDER_RAMS;
    browser_options.skip_assets = true;

    // Input events and views are managed by file_browser
    bool res =
        dialog_file_browser_show(app->dialogs, app->file_path, app->file_path, &browser_options);
    return res;
}

void boilerplate_scene_fileselect_on_enter(void* context) {
    Boilerplate* app = context;

    // if(app->script) {
    //     script_close(app->script);
    //     app->script = NULL;
    // }
    // if(app->camera_ram_sav && storage_file_is_open(app->camera_ram_sav)) {
    //     storage_file_close(app->camera_ram_sav);
    // }
    // app->camera_ram_sav = storage_file_alloc(app->storage);
    if(file_select(app)) {
        if(storage_file_open(
               app->camera_ram_sav,
               furi_string_get_cstr(app->file_path),
               FSAM_READ,
               FSOM_OPEN_EXISTING)) {
            scene_manager_next_scene(app->scene_manager, BoilerplateSceneMenu);
        }
    } else {
        view_dispatcher_stop(app->view_dispatcher);
    }
}

bool boilerplate_scene_fileselect_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    // Boilerplate* app = context;
    return false;
}

void boilerplate_scene_fileselect_on_exit(void* context) {
    UNUSED(context);
    // Boilerplate* app = context;
}