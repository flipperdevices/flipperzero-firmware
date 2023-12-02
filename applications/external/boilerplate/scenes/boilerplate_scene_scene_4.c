#include "../boilerplate.h"

void boilerplate_scene_scene_4_on_enter(void* context) {
    furi_assert(context);
    Boilerplate* app = context;
    DialogsFileBrowserOptions browser_options;

    // This will filter the browser to only show one file type and also add an icon
    dialog_file_browser_set_basic_options(&browser_options, SUBGHZ_APP_EXTENSION, &I_sub1_10px);

    //Get the Folder you want to browse
    browser_options.base_path = SUBGHZ_APP_FOLDER;
    FuriString* path;
    path = furi_string_alloc();
    furi_string_set(path, SUBGHZ_APP_FOLDER);
    bool success = dialog_file_browser_show(app->dialogs, app->file_path, path, &browser_options);
    furi_string_free(path);

    if(success) {
        // Do something with the result in app->file_path
    }

    if(success) {
        // Load page to do something with result
        //scene_manager_next_scene(app->scene_manager, BoilerplateViewIdMenu);
        scene_manager_previous_scene(app->scene_manager); // temp for showcase
    } else {
        // This is basically if someone quites the browser
        scene_manager_previous_scene(app->scene_manager);
    }
}

bool boilerplate_scene_scene_4_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    bool consumed = true;

    return consumed;
}

void boilerplate_scene_scene_4_on_exit(void* context) {
    UNUSED(context);
}
