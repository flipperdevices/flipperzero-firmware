#ifndef SCENE_MAIN_MENU_HEADERS
#define SCENE_MAIN_MENU_HEADERS

#include "flipper.h"

/** main menu options enum */
typedef enum {
    MainMenuSelectionHCSR04,
    //ResistorsMainMenuSelectionR4,
    //ResistorsMainMenuSelectionR5,
    //ResistorsMainMenuSelectionR6,
} ResistorsMainMenuSceneIndex;

/** main menu events */
typedef enum {
    MainMenuSceneSelectionEventHCSR04,
    //ResistorsMainMenuSceneSelectionEventR4,
    //ResistorsMainMenuSceneSelectionEventR5,
    //ResistorsMainMenuSceneSelectionEventR6,
} ResistorsMainMenuEvent;

void fcom_main_menu_scene_on_enter(void* context);
bool fcom_main_menu_scene_on_event(void* context, SceneManagerEvent event);
void fcom_main_menu_scene_on_exit(void* context);

#endif