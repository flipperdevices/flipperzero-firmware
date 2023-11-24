#include <scenes/doc_scene.h>
#include <scenes/doc_scene_common.h>
#include <helpers/doc_storage_helper.h>

#include <gui/modules/text_input.h>

void doc_scene_backup_on_enter(void* ctx) {
    doc_scene_selection_on_enter(SelectionSceneBackup, ctx);
}

bool doc_scene_backup_on_event(void* ctx, SceneManagerEvent event) {
    return doc_scene_selection_on_event(SelectionSceneBackup, ctx, event);
}

void doc_scene_backup_on_exit(void* ctx) {
    doc_scene_selection_on_exit(SelectionSceneBackup, ctx);
}

//! ------------------ Main ------------------

void doc_scene_selection_backup_change_text(Doc* app) {
    FuriString* text = doc_selection_get_string(app->selection);
    furi_string_reset(text);

    furi_string_printf(text, "Create Backup\t\nLoad Backup\t\n");

    doc_selection_set_size(app->selection, 2);
}

void doc_scene_selection_backup_on_enter(Doc* app) {
    doc_selection_set_callback(app->selection, doc_scene_callback, app);

    doc_selection_set_title(app->selection, "Backup");
    doc_scene_selection_backup_change_text(app);
    doc_selection_set_index(app->selection, scene_manager_get_scene_state(app->scene_manager, DocSceneBackup));

    view_dispatcher_switch_to_view(app->view_dispatcher, DocSelectionView);
}

bool doc_scene_selection_backup_on_event(Doc* app, SceneManagerEvent event) {
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(app->scene_manager, DocSceneBackup, event.event);
        if(event.event == 0) {
            scene_manager_next_scene(app->scene_manager, DocSceneBackupCreateInput);
        } else if(event.event == 1) {
            scene_manager_next_scene(app->scene_manager, DocSceneBackupLoadFile);
        }
        consumed = true;
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_set_scene_state(app->scene_manager, DocSceneBackup, 0);
    }

    return consumed;
}

void doc_scene_selection_backup_on_exit(Doc* app) {
    doc_selection_set_footer(app->selection, NULL);
    text_input_reset(app->text_input);
}