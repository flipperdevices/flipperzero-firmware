#pragma once

#include "scene_manager.h"
#include <m-array.h>

ARRAY_DEF(SceneManagerArray, AppScene*, M_PTR_OPLIST);

struct SceneManager {
    SceneManagerArray_t scene_array;
    AppScene* scene;
    uint32_t scene_num;
    void* context;
};
