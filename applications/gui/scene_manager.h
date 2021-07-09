#pragma once

#include "app_scene.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SceneManagerEventNext = 0x00000000UL,
    SceneManagerEventBack = 0xFFFFFFFFUL,
} SceneManagerEvent;

typedef struct SceneManager SceneManager;

SceneManager* scene_manager_alloc(void* context);
void scene_manager_free(SceneManager* scene_manager);

bool scene_manager_handle_custom_event(SceneManager* scene_manager, uint32_t event);
bool scene_manager_handle_navigation_event(SceneManager* scene_manager, uint32_t event);
bool scene_manager_handle_back_search_scene_event(SceneManager* scene_manager, uint32_t event);

void scene_manager_add_next_scene(SceneManager* scene_manager, AppScene* scene);
void scene_manager_start(SceneManager* scene_manager);

#ifdef __cplusplus
}
#endif
