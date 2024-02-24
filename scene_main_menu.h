/*
Main Menu
*/
#ifndef SCENE_MAIN_MENU_HEADERS
#define SCENE_MAIN_MENU_HEADERS

#include "flipper.h"

/** main menu options enum */
typedef enum {
    MainMenuSelectionListen,
    MainMenuSelectionSaved,
    MainMenuSelectionAdd,
    MainMenuSelectionSerial,
} FcomMainMenuSceneIndex;

/** main menu events */
typedef enum {
    MainMenuSceneSelectionEventListen,
    MainMenuSceneSelectionEventSaved,
    MainMenuSceneSelectionEventAdd,
    MainMenuSceneSelectionEventSerial,
} FcomMainMenuEvent;

void fcom_main_menu_scene_on_enter(void* context);
bool fcom_main_menu_scene_on_event(void* context, SceneManagerEvent event);
void fcom_main_menu_scene_on_exit(void* context);

#endif