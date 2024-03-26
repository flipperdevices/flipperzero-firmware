#include "doc_i.h"

#include <helpers/doc_storage_helper.h>

void doc_scene_backup_load_file_on_enter( void* ctx) {
    furi_assert(ctx);
    Doc* app = ctx;

    if(doc_get_backup_path(app)) {
        if(doc_dolphin_state_load_from_path(app->dolphin, furi_string_get_cstr(app->file_path))) {
            doc_dolphin_state_save(app->dolphin, true);
        } else {
            doc_dolphin_state_load(app->dolphin);
            doc_selection_set_footer(app->selection, "Error while loading!");
        }
    }

    scene_manager_previous_scene(app->scene_manager);
}

bool doc_scene_backup_load_file_on_event(void* ctx, SceneManagerEvent event) {
    UNUSED(ctx);
    UNUSED(event);
    return false;
}

void doc_scene_backup_load_file_on_exit(void* ctx) {
    UNUSED(ctx);
}