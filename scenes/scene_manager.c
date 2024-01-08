#include "scenes.h"
#include "scene_manager.h"

void(*const on_enter_handlers[])(void*) = {
    main_menu_scene_on_enter,
    chapter_scene_on_enter,
    topic_scene_on_enter,
};

bool(*const on_event_handlers[])(void*, SceneManagerEvent) = {
    main_menu_scene_on_event,
    chapter_scene_on_event,
    topic_scene_on_event,
};

void(*const on_exit_handlers[])(void*) = {
    main_menu_scene_on_exit,
    chapter_scene_on_exit,
    topic_scene_on_exit,
};

const SceneManagerHandlers scene_manager_handlers = {
    .on_enter_handlers = on_enter_handlers,
    .on_event_handlers = on_event_handlers,
    .on_exit_handlers = on_exit_handlers,
    .scene_num = SceneCount,
};