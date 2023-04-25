#ifndef SCENE_HEADERS
#define SCENE_HEADERS

#include "flipper.h"
#include "scene_main_menu.h"
#include "scene_edit.h"

/** The current scene */
typedef enum { ResistorsMainMenuScene, ResistorsEditScene, ResistorsSceneCount } ResistorsScene;

/** The current view */
typedef enum { ResistorsSubmenuView, ResistorsEditView } ResistorsView;

extern void (*const resistors_scene_on_enter_handlers[])(void*);
extern bool (*const resistors_scene_on_event_handlers[])(void*, SceneManagerEvent);
extern void (*const resistors_scene_on_exit_handlers[])(void*);
extern const SceneManagerHandlers resistors_scene_manager_handlers;

bool resistors_custom_callback(void* context, uint32_t custom_event);
bool resistors_back_event_callback(void* context);

#endif