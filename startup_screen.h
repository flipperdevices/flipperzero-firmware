#ifndef MINESWEEPER_STARTUP_SCREEN_H
#define MINESWEEPER_STARTUP_SCREEN_H

#include <gui/scene_manager.h>


void minesweeper_scenes_startup_scene_on_enter(void* context);
bool minesweeper_scenes_startup_scene_on_event(void* context, SceneManagerEvent event);
void minesweeper_scenes_startup_scene_on_exit(void* context);

#endif
