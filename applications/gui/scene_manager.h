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

typedef void (*AppSceneOnEnterCallback)(void* context);
typedef bool (*AppSceneOnEventCallback)(void* context, SceneManagerEvent event);
typedef void (*AppSceneOnExitCallback)(void* context);

typedef struct {
    const AppSceneOnEnterCallback* on_enter_handlers;
    const AppSceneOnEventCallback* on_event_handlers;
    const AppSceneOnExitCallback* on_exit_handlers;
    uint32_t scene_num;
} AppSceneHandlers;

typedef struct {
    uint32_t id;
    uint32_t state;
    AppSceneOnEnterCallback on_enter;
    AppSceneOnEventCallback on_event;
    AppSceneOnExitCallback on_exit;
} AppScene;

typedef struct SceneManager SceneManager;

void scene_manager_set_scene_state(SceneManager* scene_manager, uint32_t scene_id, uint32_t state);
uint32_t scene_manager_get_scene_state(SceneManager* scene_manager, uint32_t scene_id);

SceneManager* scene_manager_alloc(AppSceneHandlers app_scene_handlers, void* context);
void scene_manager_free(SceneManager* scene_manager);

void scene_manager_add_next_scene(SceneManager* scene_manager, uint32_t scene_id);
void scene_manager_start(SceneManager* scene_manager);

bool scene_manager_handle_custom_event(SceneManager* scene_manager, uint32_t custom_event);
bool scene_manager_handle_navigation_event(SceneManager* scene_manager);

bool scene_manager_next_scene(SceneManager* scene_manager);
bool scene_manager_previous_scene(SceneManager* scene_manager);
bool scene_manager_search_previous_scene(SceneManager* scene_manager, uint32_t scene_id);

#ifdef __cplusplus
}
#endif
