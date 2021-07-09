#include "scene_manager_i.h"
#include <furi.h>

SceneManager* scene_manager_alloc(void* context) {
    furi_assert(context);

    SceneManager* scene_manager = furi_alloc(sizeof(SceneManager));
    scene_manager->context = context;
    SceneManagerArray_init(scene_manager->scene_array);

    return scene_manager;
}

void scene_manager_free(SceneManager* scene_manager) {
    furi_assert(scene_manager);
    SceneManagerArray_clear(scene_manager->scene_array);

    free(scene_manager);
}

bool scene_manager_handle_custom_event(SceneManager* scene_manager, uint32_t custom_event) {
    furi_assert(scene_manager);

    AppScene* scene = *SceneManagerArray_back(scene_manager->scene_array);
    SceneManagerEvent event = {
        .type = SceneManagerEventTypeCustom,
        .event = custom_event,
    };
    return scene->on_event(scene_manager->context, event);
}

bool scene_manager_handle_navigation_event(SceneManager* scene_manager) {
    furi_assert(scene_manager);

    AppScene* scene = *SceneManagerArray_back(scene_manager->scene_array);
    SceneManagerEvent event = {
        .type = SceneManagerEventTypeNavigation,
    };
    bool consumed = scene->on_event(scene_manager->context, event);
    if(!consumed) {
        consumed = scene_manager_previous_scene(scene_manager);
    }
    return consumed;
}

void scene_manager_add_next_scene(SceneManager* scene_manager, AppScene* scene) {
    furi_assert(scene_manager);
    furi_assert(scene);

    SceneManagerArray_push_back(scene_manager->scene_array, scene);
}

void scene_manager_start(SceneManager* scene_manager) {
    furi_assert(scene_manager);

    AppScene* scene = *SceneManagerArray_front(scene_manager->scene_array);
    furi_assert(scene);
    scene->on_enter(scene_manager->context);
}

bool scene_manager_next_scene(SceneManager* scene_manager) {
    furi_assert(scene_manager);

    SceneManagerArray_it_t scene_it;
    SceneManagerArray_it_last(scene_it, scene_manager->scene_array);
    SceneManagerArray_previous(scene_it);
    AppScene* current_scene = *SceneManagerArray_ref(scene_it);
    AppScene* next_scene = *SceneManagerArray_back(scene_manager->scene_array);
    if(current_scene && next_scene) {
        current_scene->on_exit(scene_manager->context);
        next_scene->on_enter(scene_manager->context);
        return true;
    }
    return false;
}

bool scene_manager_previous_scene(SceneManager* scene_manager) {
    furi_assert(scene_manager);

    AppScene* current_scene = NULL;
    SceneManagerArray_pop_back(&current_scene, scene_manager->scene_array);
    if(SceneManagerArray_size(scene_manager->scene_array) == 0) {
        // Handle exit from start scene separately
        current_scene->on_exit(scene_manager->context);
        return false;
    }
    AppScene* previous_scene = *SceneManagerArray_back(scene_manager->scene_array);
    if(current_scene && previous_scene) {
        current_scene->on_exit(scene_manager->context);
        previous_scene->on_enter(scene_manager->context);
        return true;
    }
    return false;
}

bool scene_manager_search_previous_scene(SceneManager* scene_manager, uint32_t scene_id) {
    furi_assert(scene_manager);

    AppScene* previous_scene = NULL;
    AppScene* current_scene = *SceneManagerArray_back(scene_manager->scene_array);
    SceneManagerArray_it_t scene_it;
    SceneManagerArray_it_last(scene_it, scene_manager->scene_array);
    bool scene_found = false;
    while(!scene_found) {
        SceneManagerArray_previous(scene_it);
        previous_scene = *SceneManagerArray_ref(scene_it);
        if(previous_scene == NULL) {
            return false;
        }
        if(previous_scene->id == scene_id) {
            scene_found = true;
        }
    }
    SceneManagerArray_next(scene_it);
    SceneManagerArray_pop_until(scene_manager->scene_array, scene_it);
    current_scene->on_exit(scene_manager->context);
    previous_scene->on_enter(scene_manager->context);
    return true;
}
