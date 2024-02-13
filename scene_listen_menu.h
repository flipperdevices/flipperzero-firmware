#ifndef SCENE_LISTEN_MENU_HEADERS
#define SCENE_LISTEN_MENU_HEADERS

#include "flipper.h"

/** main menu options enum */
typedef enum {
    ListenMenuSelection2Prong,
    ListenMenuSelection3Prong,
    ListenMenuSelectionXrosMini,
} ListenMenuEventSceneIndex;

/** main menu events */
typedef enum {
    ListenMenuSceneSelectionEvent2Prong,
    ListenMenuSceneSelectionEvent3Prong,
    ListenMenuSceneSelectionEventXrosMini,
} ListenMenuEvent;

void fcom_listen_menu_scene_on_enter(void* context);
bool fcom_listen_menu_scene_on_event(void* context, SceneManagerEvent event);
void fcom_listen_menu_scene_on_exit(void* context);

#endif