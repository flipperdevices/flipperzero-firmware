#pragma once

#include "scene_manager.h"
#include <furi.h>
#include <m-array.h>

ARRAY_DEF(SceneManagerArray, AppScene*, M_PTR_OPLIST);

struct SceneManager {
    SceneManagerArray_t scene_array;
    void* context;
};

bool scene_manager_next_scene(SceneManager* scene_manager);
bool scene_manager_previous_scene(SceneManager* scene_manager);
bool scene_manager_search_previous_scene(SceneManager* scene_manager, uint32_t scene_id);
