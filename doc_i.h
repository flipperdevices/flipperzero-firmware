#pragma once

#include <helpers/dolphin_state.h>

//? -------------- Views --------------
#include <views/doc_selection.h>
#include <views/doc_description.h>
#include <gui/modules/text_input.h>
//? ------------ Views End ------------

#include <gui/gui.h>
#include <gui/view_dispatcher.h>

#include <dialogs/dialogs.h>

//! -------------- DEBUG --------------
#include <notification/notification_messages.h>
//! ------------ DEBUG END ------------

#define TAG "FlipperDoc"

typedef struct {
    DolphinState* dolphin;
    FuriTimer* dolphin_timer;
    uint8_t dolphin_timer_counter;

    //? -------------- Views --------------
    DocSelection* selection;
    bool in_selection;
    DocDescription* description;
    bool in_description;
    TextInput* text_input;
    //? ------------ Views End ------------

    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;

    DialogsApp* dialogs;
    FuriString* file_path;
    char text_input_array[50];

    //! -------------- DEBUG --------------
    NotificationApp* notification;
    //! ------------ DEBUG END ------------
} Doc;

enum {
    DocSelectionView,
    DocDescriptionView,
    DocTextInputView,
};