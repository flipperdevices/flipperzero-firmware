/**
MIT License

Copyright (c) 2023 Vojtech Suk (https://github.com/sukvojte)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <gui/canvas.h>
#include <gui/modules/file_browser.h>
#include "../wch_swio_flasher.h"
#include "wch_swio_flasher_scene.h"

static void file_browser_callback(void* context) {
    WchSwioFlasherApp* app = context;
    furi_assert(app);
    view_dispatcher_send_custom_event(app->view_dispatcher, SceneManagerEventTypeCustom);
}

void wch_swio_flasher_scene_file_browser_on_enter(void* context) {
    WchSwioFlasherApp* app = context;

    file_browser_set_callback(app->views.file_browser, file_browser_callback, app);

    file_browser_start(app->views.file_browser, app->views.file_path);

    view_dispatcher_switch_to_view(app->view_dispatcher, WchSwioFlasherViewFileBrowser);
}

bool wch_swio_flasher_scene_file_browser_on_event(void* context, SceneManagerEvent event) {
    WchSwioFlasherApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_next_scene(app->scene_manager, WchSwioFlasherSceneMain);
        consumed = true;
    } else if(event.type == SceneManagerEventTypeTick) {
    }

    furi_assert(app);
    view_dispatcher_send_custom_event(app->view_dispatcher, SceneManagerEventTypeCustom);

    return consumed;
}

void wch_swio_flasher_scene_file_browser_on_exit(void* context) {
    WchSwioFlasherApp* app = context;
    file_browser_stop(app->views.file_browser);
}
