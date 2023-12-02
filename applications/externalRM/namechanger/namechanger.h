#pragma once

#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <storage/storage.h>

#include "namechanger_icons.h"

#include <gui/modules/submenu.h>
#include <gui/modules/popup.h>
#include <gui/modules/text_input.h>
#include <gui/modules/widget.h>

#include "namechanger_custom_event.h"
#include "scenes/namechanger_scene.h"

#define NAMECHANGER_TEXT_STORE_SIZE 9
#define NAMECHANGER_HEADER "Flipper Name File"

#define TAG "NameChanger"

typedef struct {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;

    Gui* gui;
    Storage* storage;

    char text_store[NAMECHANGER_TEXT_STORE_SIZE + 1];

    Submenu* submenu;
    TextInput* text_input;
    Popup* popup;
    Widget* widget;
} NameChanger;

typedef enum {
    NameChangerViewSubmenu,
    NameChangerViewTextInput,
    NameChangerViewPopup,
    NameChangerViewWidget,
} NameChangerView;

bool namechanger_make_app_folder(NameChanger* namechanger);
bool namechanger_name_write(NameChanger* namechanger, char* name);
void namechanger_text_store_set(NameChanger* namechanger, const char* text, ...);
