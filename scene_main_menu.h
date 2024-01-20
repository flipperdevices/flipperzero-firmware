#ifndef SCENE_MAIN_MENU_HEADERS
#define SCENE_MAIN_MENU_HEADERS

#include "flipper.h"

/** main menu options enum */
typedef enum {
    MainMenuSelectionRead,
    MainMenuSelectionSaved,
    MainMenuSelectionAdd,
    MainMenuSelectionSerial,
} ResistorsMainMenuSceneIndex;

/** main menu events */
typedef enum {
    MainMenuSceneSelectionEventRead,
    MainMenuSceneSelectionEventSaved,
    MainMenuSceneSelectionEventAdd,
    MainMenuSceneSelectionEventSerial,
} ResistorsMainMenuEvent;

void fcom_main_menu_scene_on_enter(void* context);
bool fcom_main_menu_scene_on_event(void* context, SceneManagerEvent event);
void fcom_main_menu_scene_on_exit(void* context);

#endif