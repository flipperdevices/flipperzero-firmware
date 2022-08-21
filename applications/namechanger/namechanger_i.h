#pragma once

#include <furi_hal.h>
#include "namechanger.h"
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <storage/storage.h>
#include <dialogs/dialogs.h>
#include <gui/modules/popup.h>
#include <gui/modules/dialog_ex.h>
#include <gui/modules/text_input.h>
#include <gui/modules/widget.h>

#include "namechanger_custom_event.h"
#include "scenes/namechanger_scene.h"

#define NAMECHANGER_TEXT_STORE_SIZE 9
#define NAMECHANGER_HEADER "Flipper Name File"

#define TAG "NameChanger"

struct NameChanger {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;

    Gui* gui;
    Storage* storage;
    DialogsApp* dialogs;

    char text_store[NAMECHANGER_TEXT_STORE_SIZE+1];

    TextInput* text_input;
    Popup* popup;
    Widget* widget;
    DialogEx* dialog_ex;
};

typedef enum {
    NameChangerViewTextInput,
    NameChangerViewPopup,
    NameChangerViewWidget,
    NameChangerViewDialogEx,
} NameChangerView;

bool namechanger_name_read_write(NameChanger* namechanger, char* name, uint8_t mode);
void namechanger_text_store_set(NameChanger* namechanger, const char* text, ...);
void namechanger_text_store_clear(NameChanger* namechanger);
