#pragma once

#include <gui/scene_manager.h>

// Generate scene id and total number
#define ADD_SCENE(prefix, name, id) Pi_TerminalScene##id,
typedef enum {
#include "pi_terminal_scene_config.h"
    Pi_TerminalSceneNum,
} Pi_TerminalScene;
#undef ADD_SCENE

extern const SceneManagerHandlers Pi_Terminal_scene_handlers;

// Generate scene on_enter handlers declaration
#define ADD_SCENE(prefix, name, id) void prefix##_scene_##name##_on_enter(void*);
#include "pi_terminal_scene_config.h"
#undef ADD_SCENE

// Generate scene on_event handlers declaration
#define ADD_SCENE(prefix, name, id) \
    bool prefix##_scene_##name##_on_event(void* context, SceneManagerEvent event);
#include "pi_terminal_scene_config.h"
#undef ADD_SCENE

// Generate scene on_exit handlers declaration
#define ADD_SCENE(prefix, name, id) void prefix##_scene_##name##_on_exit(void* context);
#include "pi_terminal_scene_config.h"
#undef ADD_SCENE

#define UART_PINS_ITEM_IDX (0)
#define BAUDRATE_ITEM_IDX (1)
#define HEX_MODE_ITEM_IDX (2)

#define DEFAULT_BAUDRATE_OPT_IDX (18)
