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
#include <gui/canvas.h>
#include <gui/scene_manager.h>
#include <storage/storage.h>

#include "../minichlink_debugger.h"
#include "../nhc_link042_emulator.h"
#include "../minichlink_debugger.h"

const NotificationSequence sequence_backlight_on = {
    &message_display_backlight_on,
    NULL,
};

const NotificationSequence sequence_session_init = {
    &message_blink_start_10,
    &message_blink_set_color_red,
    &message_do_not_reset,
    NULL,
};

const NotificationSequence sequence_session_end = {
    &message_red_0,
    &message_blink_stop,
    NULL,
};

static void debugger_event_handler(void* context, WchSwioFlasher_MinichlinkDebugger_Events ev) {
    WchSwioFlasherApp* app = context;

    switch(ev) {
    case WchSwioFlasher_MinichlinkDebugger_InitSessionEvent:
        debugger_emulator_set_animation_status(app->views.debuger_emulator, 1);
        notification_message(app->notification, &sequence_backlight_on);
        notification_message(app->notification, &sequence_session_init);
        break;
    case WchSwioFlasher_MinichlinkDebugger_EndSessionEvent:
        debugger_emulator_set_animation_status(app->views.debuger_emulator, 0);
        notification_message(app->notification, &sequence_session_end);
        break;
    default:
        break;
    }
}

void wch_swio_flasher_scene_debugger_on_enter(void* context) {
    WchSwioFlasherApp* app = context;

    WchSwioFlasher_MinichlinkDebugger_registerEvent(
        app->services.mini_debugger,
        WchSwioFlasher_MinichlinkDebugger_InitSessionEvent,
        debugger_event_handler,
        context);
    WchSwioFlasher_MinichlinkDebugger_registerEvent(
        app->services.mini_debugger,
        WchSwioFlasher_MinichlinkDebugger_EndSessionEvent,
        debugger_event_handler,
        context);

    WchSwioFlasher_NhcLink042Emu_attach(app->services.emulator);

    view_dispatcher_switch_to_view(app->view_dispatcher, WchSwioFlasherViewDebuggerEmulator);
}

bool wch_swio_flasher_scene_debugger_on_event(void* context, SceneManagerEvent event) {
    WchSwioFlasherApp* app = context;
    UNUSED(app);
    bool consumed = false;

    if(event.type == SceneManagerEventTypeBack) {
        // TODO: stop debugger
        //consumed = true;
    } else if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case WchSwioFlasherEventDebuggerExit:
            break;
        }
        //view_dispatcher_stop(app->view_dispatcher);
        consumed = true;
    }

    return consumed;
}

void wch_swio_flasher_scene_debugger_on_exit(void* context) {
    WchSwioFlasherApp* app = context;
    notification_message(app->notification, &sequence_session_end);

    WchSwioFlasher_MinichlinkDebugger_unregisterEvent(
        app->services.mini_debugger, WchSwioFlasher_MinichlinkDebugger_InitSessionEvent);
    WchSwioFlasher_MinichlinkDebugger_unregisterEvent(
        app->services.mini_debugger, WchSwioFlasher_MinichlinkDebugger_EndSessionEvent);

    WchSwioFlasher_NhcLink042Emu_detach(app->services.emulator);
    widget_reset(app->views.widget);
}
