#ifndef APP_H
#define APP_H

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/widget.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>

#include <gui/scene_manager.h>
#include <storage/filesystem_api_defines.h>
#include <storage/storage.h>
#include <toolbox/stream/stream.h>
#include <toolbox/stream/file_stream.h>
#include "../resource/resource.h"

typedef struct App {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    Widget* widget;
    const char* current_topic;
    size_t current_chapter_index;
    Stream* file_stream;
} App;


App* app_alloc();
void app_free(App* app);

#endif // APP_H
