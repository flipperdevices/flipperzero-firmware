#pragma once

#include <doc_i.h>

//? -------------- Selection --------------
typedef enum {
    SelectionSceneMenu,
    SelectionSceneXP,
    SelectionSceneButthurt,
    SelectionSceneBackup,

    SelectionSceneCount,
} DocSelectionScene;

void doc_scene_selection_on_enter(DocSelectionScene scene, void* ctx);
bool doc_scene_selection_on_event(DocSelectionScene scene, void* ctx, SceneManagerEvent event);
void doc_scene_selection_on_exit(DocSelectionScene scene, void* ctx);

void doc_scene_selection_xp_on_enter(Doc* app);
bool doc_scene_selection_xp_on_event(Doc* app, SceneManagerEvent event);
void doc_scene_selection_xp_on_exit(Doc* app);

void doc_scene_selection_butthurt_on_enter(Doc* app);
bool doc_scene_selection_butthurt_on_event(Doc* app, SceneManagerEvent event);
void doc_scene_selection_butthurt_on_exit(Doc* app);

void doc_scene_selection_menu_on_enter(Doc* app);
bool doc_scene_selection_menu_on_event(Doc* app, SceneManagerEvent event);
void doc_scene_selection_menu_on_exit(Doc* app);

void doc_scene_selection_backup_on_enter(Doc* app);
bool doc_scene_selection_backup_on_event(Doc* app, SceneManagerEvent event);
void doc_scene_selection_backup_on_exit(Doc* app);

//? -------------- Description --------------
typedef enum {
    DescriptionSceneCurrent,
    DescriptionSceneXP,
    DescriptionSceneButthurt,

    DescriptionSceneCount,
} DocDescriptionScene;

void doc_scene_description_on_enter(DocDescriptionScene scene, void* ctx);
bool doc_scene_description_on_event(DocDescriptionScene scene, void* ctx, SceneManagerEvent event);
void doc_scene_description_on_exit(DocDescriptionScene scene, void* ctx);

void doc_scene_description_current_on_enter(Doc* app);
bool doc_scene_description_current_on_event(Doc* app, SceneManagerEvent event);
void doc_scene_description_current_on_exit(Doc* app);

void doc_scene_description_xp_on_enter(Doc* app);
bool doc_scene_description_xp_on_event(Doc* app, SceneManagerEvent event);
void doc_scene_description_xp_on_exit(Doc* app);

void doc_scene_description_butthurt_on_enter(Doc* app);
bool doc_scene_description_butthurt_on_event(Doc* app, SceneManagerEvent event);
void doc_scene_description_butthurt_on_exit(Doc* app);