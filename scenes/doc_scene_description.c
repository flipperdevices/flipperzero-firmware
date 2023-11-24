#include "doc_scene_common.h"
#include "doc_i.h"

typedef void (*DocDescriptionCommonOnEnter)(Doc* app);
typedef bool (*DocDescriptionCommonOnEvent)(Doc* app, SceneManagerEvent event);
typedef void (*DocDescriptionCommonOnExit)(Doc* app);

typedef struct {
    DocDescriptionCommonOnEnter on_enter;
    DocDescriptionCommonOnEvent on_event;
    DocDescriptionCommonOnExit on_exit;
} DocDescriptionCommonSceneBase;

static const DocDescriptionCommonSceneBase doc_description_scenes[];

void doc_scene_description_on_enter(DocDescriptionScene scene, void* ctx) {
    furi_assert(scene < DescriptionSceneCount);
    furi_assert(ctx);

    Doc* app = ctx;
    app->in_description = true;
    doc_description_scenes[scene].on_enter(app);
}

bool doc_scene_description_on_event(DocDescriptionScene scene, void* ctx, SceneManagerEvent event) {
    furi_assert(scene < DescriptionSceneCount);
    furi_assert(ctx);

    Doc* app = ctx;
    return doc_description_scenes[scene].on_event(app, event);
}

void doc_scene_description_on_exit(DocDescriptionScene scene, void* ctx) {
    furi_assert(scene < DescriptionSceneCount);
    furi_assert(ctx);

    Doc* app = ctx;
    doc_description_scenes[scene].on_enter(app);
    app->in_description = false;
}

static const DocDescriptionCommonSceneBase doc_description_scenes[DescriptionSceneCount] = {
    [DescriptionSceneCurrent] = {
        .on_enter = doc_scene_description_current_on_enter,
        .on_event = doc_scene_description_current_on_event,
        .on_exit = doc_scene_description_current_on_exit,
    },
    [DescriptionSceneXP] = {
        .on_enter = doc_scene_description_xp_on_enter,
        .on_event = doc_scene_description_xp_on_event,
        .on_exit = doc_scene_description_xp_on_exit,
    },
    [DescriptionSceneButthurt] = {
        .on_enter = doc_scene_description_butthurt_on_enter,
        .on_event = doc_scene_description_butthurt_on_event,
        .on_exit = doc_scene_description_butthurt_on_exit,
    },
};