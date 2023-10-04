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

#pragma once

#include <furi.h>

#include <gui/gui.h>
#include <gui/scene_manager.h>
#include <gui/modules/widget.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/text_input.h>
#include <gui/modules/submenu.h>
#include <gui/modules/widget.h>
#include <gui/modules/popup.h>
#include <dialogs/dialogs.h>
#include <notification/notification_messages.h>

#include "views/debugger_emulator.h"

#include "minichlink_debugger.h"
#include "nhc_link042_emulator.h"
#include "helpers/riscv_debug.h"
#include "helpers/swio.h"
#include "helpers/wch_flasher.h"

typedef struct App {
    Gui* gui;
    NotificationApp* notification;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    //NotificationApp* notifications;
    //DialogsApp* dialogs;
    //Popup* popup;

    //
    //TextInput* text_input;

    WchSwioFlasher_SWIO* swio;
    WchSwioFlasher_RiscVDebug* riscv_debug;
    WchSwioFlasher_WchFlasher* flasher;

    WchSwioFlasher_MinichlinkDebugger* mini_debugger;
    WchSwioFlasher_NhcLink042Emu* emulator;

    struct {
        Widget* widget;
        DebuggerEmulator* debuger_emulator;
        Submenu* submenu;
        FuriString* file_path;
        FileBrowser* file_browser;
    } views;
} WchSwioFlasherApp;

typedef enum {
    WchSwioFlasherEventDebuggerExit,
} WchSwioFlasherEvent;

typedef enum {
    WchSwioFlasherViewWidget,
    WchSwioFlasherViewSubmenu,
    WchSwioFlasherViewFileBrowser,
    WchSwioFlasherViewDebuggerEmulator,
} WchSwioFlasherView;