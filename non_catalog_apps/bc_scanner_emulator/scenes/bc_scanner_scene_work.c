#include "../bc_scanner_script.h"
#include "../bc_scanner_app_i.h"
#include "../views/bc_scanner_view.h"
#include "furi_hal.h"
#include "toolbox/path.h"

#define TAG "BcScanner"
#define WORKER_TAG TAG "WorkScene"

void bc_scanner_scene_work_ok_callback(InputType type, void* context) {
    furi_assert(context);
    BarCodeApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, type);
}

bool bc_scanner_scene_work_on_event(void* context, SceneManagerEvent event) {
    BarCodeApp* app = context;
    UNUSED(app);
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        bc_scanner_script_toggle(app->bc_scanner_script);
        consumed = true;
    } else if(event.type == SceneManagerEventTypeTick) {
        bc_scanner_set_state(
            app->bc_scanner_view, bc_scanner_script_get_state(app->bc_scanner_script));
    }
    return consumed;
}

void bc_scanner_scene_work_on_enter(void* context) {
    FURI_LOG_I(WORKER_TAG, "bc_scanner_scene_work_on_enter");
    BarCodeApp* app = context;

    FuriString* file_name;
    file_name = furi_string_alloc();

    path_extract_filename(app->file_path, file_name, true);
    bc_scanner_set_file_name(app->bc_scanner_view, furi_string_get_cstr(file_name));
    app->bc_scanner_script = bc_scanner_script_open(app->file_path);

    furi_string_free(file_name);

    bc_scanner_set_state(
        app->bc_scanner_view, bc_scanner_script_get_state(app->bc_scanner_script));

    bc_scanner_set_ok_callback(app->bc_scanner_view, bc_scanner_scene_work_ok_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, BarCodeAppViewWork);
}

void bc_scanner_scene_work_on_exit(void* context) {
    BarCodeApp* app = context;
    UNUSED(app);
    bc_scanner_script_close(app->bc_scanner_script);
}
