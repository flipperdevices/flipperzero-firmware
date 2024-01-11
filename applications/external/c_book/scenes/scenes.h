#ifndef SCENES_H
#define SCENES_H

#include "../app/app.h"

typedef enum {
    MainMenuScene,
    ChapterScene,
    TopicScene,
    SceneCount,
} MainScene;

typedef enum {
    SubmenuView,
    WidgetView,
} MainView;

typedef enum {
    MainMenuEvent,
} MainEvent;

void main_menu_scene_on_enter(void* context);
bool main_menu_scene_on_event(void* context, SceneManagerEvent event);
void main_menu_scene_on_exit(void* context);

void chapter_scene_on_enter(void* context);
bool chapter_scene_on_event(void* context, SceneManagerEvent event);
void chapter_scene_on_exit(void* context);

void topic_scene_on_enter(void* context);
bool topic_scene_on_event(void* context, SceneManagerEvent event);
void topic_scene_on_exit(void* context);

#endif // SCENES_H
