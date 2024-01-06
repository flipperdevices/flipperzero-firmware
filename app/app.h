#ifndef APP_H
#define APP_H

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/widget.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>

typedef struct App {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    Widget* widget;
    const char* current_detail_text;
    size_t current_chapter_index;
} App;

App* app_alloc();
void app_free(App* app);

#endif // APP_H
