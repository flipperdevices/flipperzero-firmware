#ifndef SCENES_H
#define SCENES_H

#include "../app/app.h"

typedef enum {
    BookMainMenuScene,
    BookChapterScene,
    BookTopicScene,
    BookSceneCount,
} BookScene;

typedef enum {
    BookSubmenuView,
    BookWidgetView,
} BookView;

typedef enum {
    BookChapterEvent,
} BookEvent;


bool book_main_menu_scene_on_event(void* context, SceneManagerEvent event);
void book_main_menu_scene_on_enter(void* context);
void book_main_menu_scene_on_exit(void* context);

bool book_chapter_scene_on_event(void* context, SceneManagerEvent event);
void book_chapter_scene_on_enter(void* context);
void book_chapter_scene_on_exit(void* context);

bool book_topic_scene_on_event(void* context, SceneManagerEvent event);
void book_topic_scene_on_enter(void* context);
void book_topic_scene_on_exit(void* context);

#endif // SCENES_H
