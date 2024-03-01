#include "minesweeper_scene.h"

// Generate scene on_enter handlers array
#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_enter,
void (*const minesweeper_on_enter_handlers[])(void*) = {
#include "minesweeper_scene_config.h"
};
#undef ADD_SCENE

// Generate scene on_event handlers array
#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_event,
bool (*const minesweeper_on_event_handlers[])(void* context, SceneManagerEvent event) = {
#include "minesweeper_scene_config.h"
};
#undef ADD_SCENE

// Generate scene on_enter handlers array
#define ADD_SCENE(prefix, name, id) prefix##_scene_##name##_on_exit,
void (*const minesweeper_on_exit_handlers[])(void* context) = {
#include "minesweeper_scene_config.h"
};
#undef ADD_SCENE

// Initialize scene handlers configuration structure
const SceneManagerHandlers minesweeper_scene_handlers = {
    .on_enter_handlers = minesweeper_on_enter_handlers,
    .on_event_handlers = minesweeper_on_event_handlers,
    .on_exit_handlers = minesweeper_on_exit_handlers,
    .scene_num = MineSweeperSceneNum,
};
