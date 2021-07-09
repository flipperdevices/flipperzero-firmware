#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    SceneManagerEventTypeCustom,
    SceneManagerEventTypeNavigation,
} SceneManagerEventType;

typedef struct {
    SceneManagerEventType type;
    uint32_t event;
} SceneManagerEvent;

typedef struct {
    uint32_t id;
    uint32_t state;
    const void (*on_enter)(void* context);
    const bool (*on_event)(void* context, SceneManagerEvent event);
    const void (*on_exit)(void* context);
} AppScene;

typedef struct SceneManager SceneManager;

SceneManager* scene_manager_alloc(void* context);
void scene_manager_free(SceneManager* scene_manager);

void scene_manager_add_next_scene(SceneManager* scene_manager, AppScene* scene);
void scene_manager_start(SceneManager* scene_manager);

bool scene_manager_handle_custom_event(SceneManager* scene_manager, uint32_t custom_event);
bool scene_manager_handle_navigation_event(SceneManager* scene_manager);

bool scene_manager_next_scene(SceneManager* scene_manager);
bool scene_manager_previous_scene(SceneManager* scene_manager);
bool scene_manager_search_previous_scene(SceneManager* scene_manager, uint32_t scene_id);

#ifdef __cplusplus
}
#endif
