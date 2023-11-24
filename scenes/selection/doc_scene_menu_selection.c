#include <scenes/doc_scene.h>
#include <scenes/doc_scene_common.h>

void doc_scene_menu_on_enter(void* ctx) {
    doc_scene_selection_on_enter(SelectionSceneMenu, ctx);
}

bool doc_scene_menu_on_event(void* ctx, SceneManagerEvent event) {
    return doc_scene_selection_on_event(SelectionSceneMenu, ctx, event);
}

void doc_scene_menu_on_exit(void* ctx) {
    doc_scene_selection_on_exit(SelectionSceneMenu, ctx);
}

//! ------------------ Main ------------------

enum {
    CurrentMenu,
    XPMenu,
    ButthurtMenu,
    BackupMenu,

    MaxCountMenu,
};

const char* menu_items[MaxCountMenu] = {
    "General\t\n",
    "Daily XP\t\n",
    "Daily Butthurt\t\n",
    "Backup\t\n"
};

void doc_scene_selection_menu_change_text(Doc* app) {
    FuriString* text = doc_selection_get_string(app->selection);
    furi_string_reset(text);

    for(int i = 0; i < MaxCountMenu; ++i) {
        furi_string_cat_printf(text, menu_items[i]);
    }

    doc_selection_set_size(app->selection, MaxCountMenu);
}

void doc_scene_selection_menu_on_enter(Doc* app) {
    doc_selection_set_callback(app->selection, doc_scene_callback, app);

    doc_selection_set_title(app->selection, "Flipper Doc");
    doc_scene_selection_menu_change_text(app);
    doc_selection_set_index(app->selection, scene_manager_get_scene_state(app->scene_manager, DocSceneMenu));

    view_dispatcher_switch_to_view(app->view_dispatcher, DocSelectionView);
}

bool doc_scene_selection_menu_on_event(Doc* app, SceneManagerEvent event) {
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(app->scene_manager, DocSceneMenu, event.event);
        switch(event.event) {
        case CurrentMenu:
            scene_manager_next_scene(app->scene_manager, DocSceneCurrent);
            break;
        case XPMenu:
            scene_manager_next_scene(app->scene_manager, DocSceneXP);
            break;
        case ButthurtMenu:
            scene_manager_next_scene(app->scene_manager, DocSceneButthurt);
            break;
        case BackupMenu:
            scene_manager_next_scene(app->scene_manager, DocSceneBackup);
            break;
        default:
            break;
        }
        consumed = true;
    }

    return consumed;
}

void doc_scene_selection_menu_on_exit(Doc* app) {
    UNUSED(app);
}