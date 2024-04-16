#pragma once

#include <gui/scene_manager.h>

// For each scene, implement handler callbacks
void scene_about_on_enter(void* context);
bool scene_about_on_event(void* context, SceneManagerEvent event);
void scene_about_on_exit(void* context);
