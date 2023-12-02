#include <gui/scene_manager.h>

extern const SceneManagerHandlers calendar_scene_handlers;

// Generate scene id and total number
#define ADD_SCENE(prefix, name, id) CalendarScene##id,
typedef enum {
#include "calendar_scene_config.h"
    CalendarSceneNum,
} CalendarScene;
#undef ADD_SCENE

// Generate scene on_enter handlers declaration
#define ADD_SCENE(prefix, name, id) void prefix##_scene_##name##_on_enter(void*);
#include "calendar_scene_config.h"
#undef ADD_SCENE

// Generate scene on_event handlers declaration
#define ADD_SCENE(prefix, name, id) \
    bool prefix##_scene_##name##_on_event(void* context, SceneManagerEvent event);
#include "calendar_scene_config.h"
#undef ADD_SCENE

// Generate scene on_exit handlers declaration
#define ADD_SCENE(prefix, name, id) void prefix##_scene_##name##_on_exit(void* context);
#include "calendar_scene_config.h"
#undef ADD_SCENE