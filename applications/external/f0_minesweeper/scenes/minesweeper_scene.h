#ifndef MINESWEEPER_SCENE_H
#define MINESWEEPER_SCENE_H

#include <gui/scene_manager.h>

// Generate scene id and total number
#define ADD_SCENE(prefix, name, id) MineSweeperScene##id,
typedef enum {
#include "minesweeper_scene_config.h"
    MineSweeperSceneNum
} MineSweeperScene;
#undef ADD_SCENE

extern const SceneManagerHandlers minesweeper_scene_handlers;

// Generate scene on_enter handlers declaration
#define ADD_SCENE(prefix, name, id) void prefix##_scene_##name##_on_enter(void*);
#include "minesweeper_scene_config.h"
#undef ADD_SCENE

// Generate scene on_enter handlers declaration
#define ADD_SCENE(prefix, name, id) \
    bool prefix##_scene_##name##_on_event(void* context, SceneManagerEvent event);
#include "minesweeper_scene_config.h"
#undef ADD_SCENE

// Generate scene on_enter handlers declaration
#define ADD_SCENE(prefix, name, id) void prefix##_scene_##name##_on_exit(void* context);
#include "minesweeper_scene_config.h"
#undef ADD_SCENE

#endif
