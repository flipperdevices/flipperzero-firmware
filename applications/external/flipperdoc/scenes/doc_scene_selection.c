#include "doc_scene_common.h"
#include "doc_i.h"

typedef void (*DocSelectionCommonOnEnter)(Doc* app);
typedef bool (*DocSelectionCommonOnEvent)(Doc* app, SceneManagerEvent event);
typedef void (*DocSelectionCommonOnExit)(Doc* app);

typedef struct {
    DocSelectionCommonOnEnter on_enter;
    DocSelectionCommonOnEvent on_event;
    DocSelectionCommonOnExit on_exit;
} DocSelectionCommonSceneBase;

static const DocSelectionCommonSceneBase doc_selection_scenes[];

void doc_scene_selection_on_enter(DocSelectionScene scene, void* ctx) {
    furi_assert(scene < SelectionSceneCount);
    furi_assert(ctx);

    Doc* app = ctx;
    app->in_selection = true;
    doc_selection_scenes[scene].on_enter(app);
}

bool doc_scene_selection_on_event(DocSelectionScene scene, void* ctx, SceneManagerEvent event) {
    furi_assert(scene < SelectionSceneCount);
    furi_assert(ctx);

    Doc* app = ctx;
    return doc_selection_scenes[scene].on_event(app, event);
}

void doc_scene_selection_on_exit(DocSelectionScene scene, void* ctx) {
    furi_assert(scene < SelectionSceneCount);
    furi_assert(ctx);

    Doc* app = ctx;
    doc_selection_scenes[scene].on_enter(app);
    app->in_selection = false;
}

static const DocSelectionCommonSceneBase doc_selection_scenes[SelectionSceneCount] = {
    [SelectionSceneXP] = {
        .on_enter = doc_scene_selection_xp_on_enter,
        .on_event = doc_scene_selection_xp_on_event,
        .on_exit = doc_scene_selection_xp_on_exit,
    },
    [SelectionSceneButthurt] = {
        .on_enter = doc_scene_selection_butthurt_on_enter,
        .on_event = doc_scene_selection_butthurt_on_event,
        .on_exit = doc_scene_selection_butthurt_on_exit,
    },
    [SelectionSceneMenu] = {
        .on_enter = doc_scene_selection_menu_on_enter,
        .on_event = doc_scene_selection_menu_on_event,
        .on_exit = doc_scene_selection_menu_on_exit,
    },
    [SelectionSceneBackup] = {
        .on_enter = doc_scene_selection_backup_on_enter,
        .on_event = doc_scene_selection_backup_on_event,
        .on_exit = doc_scene_selection_backup_on_exit,
    },
};