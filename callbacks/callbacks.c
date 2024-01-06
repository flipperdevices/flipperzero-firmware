#include "callbacks.h"
#include "../constants/topics.h"
#include "../scenes/scenes.h"
#include <stdlib.h>

void book_menu_callback(void* context, uint32_t index) {
    App* app = context;
    if(index < number_of_chapters) {
        app->current_chapter_index = index;
        scene_manager_next_scene(app->scene_manager, BookChapterScene);
    }
}

void book_chapter_callback(void* context, uint32_t index) {
    App* app = (App*)context;
    size_t chapterIndex = app->current_chapter_index;
    BookChapter currentChapter = chapters[chapterIndex];

    if(index < currentChapter.number_of_details) {
        app->current_detail_text = currentChapter.details[index].details;
        scene_manager_next_scene(app->scene_manager, BookTopicScene);
    }
}

bool book_back_event_callback(void* context) {
    furi_assert(context);
    App* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}