#include "scenes.h"
#include "scene_manager.h"

void(*const book_scene_on_enter_handlers[])(void*) = {
    book_main_menu_scene_on_enter,
    book_chapter_scene_on_enter,
    book_topic_scene_on_enter,
};

bool(*const book_scene_on_event_handlers[])(void*, SceneManagerEvent) = {
    book_main_menu_scene_on_event,
    book_chapter_scene_on_event,
    book_topic_scene_on_event,
};

void(*const book_scene_on_exit_handlers[])(void*) = {
    book_main_menu_scene_on_exit,
    book_chapter_scene_on_exit,
    book_topic_scene_on_exit,
};

const SceneManagerHandlers book_scene_manager_handlers = {
    .on_enter_handlers = book_scene_on_enter_handlers,
    .on_event_handlers = book_scene_on_event_handlers,
    .on_exit_handlers = book_scene_on_exit_handlers,
    .scene_num = BookSceneCount,
};