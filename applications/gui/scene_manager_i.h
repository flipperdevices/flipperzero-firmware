#pragma once

#include "scene_manager.h"
#include <m-array.h>

ARRAY_DEF(SceneManagerIdArray, uint32_t, M_DEFAULT_OPLIST);

typedef struct {
    uint32_t id;
    uint32_t state;
} AppScene;

struct SceneManager {
    SceneManagerIdArray_t scene_id_array;
    const SceneManagerHandlers* scene_handlers;
    AppScene* scene;
    void* context;
};
