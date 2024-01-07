#ifndef SCENE_HEADERS
#define SCENE_HEADERS

#include "flipper.h"
#include "scene_main_menu.h"
#include "scene_hcsr04.h"

/** The current scene */
typedef enum { FcomMainMenuScene, FcomHCSR04Scene, FcomSceneCount } FcomScene;

/** The current view */
typedef enum { FcomSubmenuView, FcomHCSR04View } FcomView;

extern void (*const fcom_scene_on_enter_handlers[])(void*);
extern bool (*const fcom_scene_on_event_handlers[])(void*, SceneManagerEvent);
extern void (*const fcom_scene_on_exit_handlers[])(void*);
extern const SceneManagerHandlers fcom_scene_manager_handlers;

bool fcom_custom_callback(void* context, uint32_t custom_event);
bool fcom_back_event_callback(void* context);

#endif