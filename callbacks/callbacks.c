#include "callbacks.h"
#include "../resource/resource.h"
#include "../scenes/scenes.h"
#include <stdlib.h>

void menu_callback(void* context, uint32_t index) {
    App* app = context;
    if(index < number_of_chapters) {
        app->current_chapter_index = index;
        scene_manager_next_scene(app->scene_manager, ChapterScene);
    }
}

void chapter_callback(void* context, uint32_t index) {
    App* app = (App*)context;
    size_t chapterIndex = app->current_chapter_index;
    Chapter currentChapter = chapters[chapterIndex];

    if(index < currentChapter.number_of_topics) {
        app->current_topic = currentChapter.content[index].file_path;
        scene_manager_next_scene(app->scene_manager, TopicScene);
    }
}

bool back_event_callback(void* context) {
    furi_assert(context);
    App* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}