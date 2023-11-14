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

#include "../wch_swio_flasher.h"
#include "wch_swio_flasher_scene.h"
#include <gui/canvas.h>
#include <gui/modules/submenu.h>

#include <storage/storage.h>

void wch_swio_flasher_scene_main_submenu_callback(void* context, uint32_t index) {
    furi_assert(context);
    WchSwioFlasherApp* app = context;

    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void wch_swio_flasher_scene_main_on_enter(void* context) {
    WchSwioFlasherApp* app = context;
    Submenu* submenu = app->views.submenu;
    submenu_set_header(submenu, "WCH SWIO Flasher");
    submenu_add_item(
        submenu,
        "Start debugger",
        WchSwioFlasherSceneDebugger,
        wch_swio_flasher_scene_main_submenu_callback,
        app);
    submenu_add_item(
        submenu,
        "Flash CH32Vxx",
        WchSwioFlasherSceneFlash,
        wch_swio_flasher_scene_main_submenu_callback,
        app);
    submenu_add_item(
        submenu,
        "Wiring",
        WchSwioFlasherSceneWiring,
        wch_swio_flasher_scene_main_submenu_callback,
        app);
    submenu_add_item(
        submenu,
        "About",
        WchSwioFlasherSceneAbout,
        wch_swio_flasher_scene_main_submenu_callback,
        app);

    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(app->scene_manager, WchSwioFlasherSceneMain));

    view_dispatcher_switch_to_view(app->view_dispatcher, WchSwioFlasherViewSubmenu);
}

bool wch_swio_flasher_scene_main_on_event(void* context, SceneManagerEvent event) {
    furi_assert(context);

    WchSwioFlasherApp* app = context;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == WchSwioFlasherSceneDebugger) {
            scene_manager_next_scene(app->scene_manager, WchSwioFlasherSceneDebugger);
            consumed = true;
        } else if(event.event == WchSwioFlasherSceneFlash) {
            //scene_manager_set_scene_state(app->scene_manager, AvrIspSceneChipDetect, AvrIspViewProgrammer);
            //scene_manager_next_scene(app->scene_manager, AvrIspSceneChipDetect);
            scene_manager_next_scene(app->scene_manager, WchSwioFlasherSceneFlash);
            consumed = true;
        } else if(event.event == WchSwioFlasherSceneWiring) {
            scene_manager_next_scene(app->scene_manager, WchSwioFlasherSceneWiring);
            consumed = true;
        } else if(event.event == WchSwioFlasherSceneAbout) {
            scene_manager_next_scene(app->scene_manager, WchSwioFlasherSceneAbout);
            consumed = true;
        }
    }
    return consumed;
}

void wch_swio_flasher_scene_main_on_exit(void* context) {
    furi_assert(context);

    WchSwioFlasherApp* app = context;
    submenu_reset(app->views.submenu);
}
