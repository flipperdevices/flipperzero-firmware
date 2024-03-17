#pragma once

#include <gui/scene_manager.h>

// void scene_settings_item_callback(void* context, int32_t index, InputType type);

// For each scene, implement handler callbacks
void scene_settings_on_enter(void* context);
bool scene_settings_on_event(void* context, SceneManagerEvent event);
void scene_settings_on_exit(void* context);
