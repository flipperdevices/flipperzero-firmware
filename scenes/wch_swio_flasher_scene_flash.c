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
#include <gui/modules/widget.h>
#include <wch_swio_flasher_icons.h>
#include "../wch_swio_flasher.h"
#include "wch_swio_flasher_scene.h"

void wch_swio_flasher_scene_flash_on_enter(void* context) {
    WchSwioFlasherApp* app = context;

    widget_add_icon_element(app->views.widget, 72, 17, &I_DolphinCommon_56x48);

    widget_add_string_element(
        app->views.widget, 2, 10, AlignLeft, AlignCenter, FontPrimary, "One day...");

    widget_add_string_element(
        app->views.widget, 2, 25, AlignLeft, AlignCenter, FontSecondary, "... there will be...");

    widget_add_string_element(
        app->views.widget, 2, 40, AlignLeft, AlignCenter, FontSecondary, "... a flasher !");

    view_dispatcher_switch_to_view(app->view_dispatcher, WchSwioFlasherViewWidget);
}

bool wch_swio_flasher_scene_flash_on_event(void* context, SceneManagerEvent event) {
    WchSwioFlasherApp* app = context;
    UNUSED(app);
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        //view_dispatcher_stop(app->view_dispatcher);
        consumed = true;
    }

    return consumed;
}

void wch_swio_flasher_scene_flash_on_exit(void* context) {
    WchSwioFlasherApp* app = context;
    widget_reset(app->views.widget);
}
