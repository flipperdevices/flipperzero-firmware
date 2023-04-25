#ifndef SCENE_MAIN_MENU_HEADERS
#define SCENE_MAIN_MENU_HEADERS

#include "flipper.h"

/** main menu options enum */
typedef enum {
    ResistorsMainMenuScene4BarResistor,
    ResistorsMainMenuScene5BarResistor,
} ResistorsMainMenuSceneIndex;

/** main menu events */
typedef enum {
    ResistorsMainMenuScene4BarSelectionEvent,
    ResistorsMainMenuScene5BarSelectionEvent,
} ResistorsMainMenuEvent;

void resistors_main_menu_scene_on_enter(void* context);
bool resistors_main_menu_scene_on_event(void* context, SceneManagerEvent event);
void resistors_main_menu_scene_on_exit(void* context);

#endif