#pragma once

#include <gui/scene_manager.h>

// For each scene, implement handler callbacks
void scene_action_rename_on_enter(void* context);
bool scene_action_rename_on_event(void* context, SceneManagerEvent event);
void scene_action_rename_on_exit(void* context);
