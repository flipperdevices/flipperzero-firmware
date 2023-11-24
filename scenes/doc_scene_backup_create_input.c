#include "doc_i.h"

#include <helpers/doc_storage_helper.h>

void doc_backup_text_input_done_callback(void* ctx);

void doc_scene_backup_create_input_on_enter( void* ctx) {
    furi_assert(ctx);
    Doc* app = ctx;

    text_input_set_header_text(app->text_input, "Enter backup name");
    text_input_set_result_callback(app->text_input, doc_backup_text_input_done_callback, app, app->text_input_array, 50, true);

    view_dispatcher_switch_to_view(app->view_dispatcher, DocTextInputView);
}

bool doc_scene_backup_create_input_on_event(void* ctx, SceneManagerEvent event) {
    furi_assert(ctx);
    Doc* app = ctx;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom && event.event == 0) {
        if(!doc_dolphin_state_save_to_path(app->dolphin, DOC_APP_FOLDER, app->text_input_array)) {
            doc_selection_set_footer(app->selection, "Error while saving!");
        }
        scene_manager_previous_scene(app->scene_manager);
        consumed = true;
    }

    return consumed;
}

void doc_scene_backup_create_input_on_exit(void* ctx) {
    UNUSED(ctx);
}

void doc_backup_text_input_done_callback(void* ctx) {
    furi_assert(ctx);
    Doc* app = ctx;
    view_dispatcher_send_custom_event(app->view_dispatcher, 0);
}