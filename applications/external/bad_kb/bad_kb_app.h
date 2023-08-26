#pragma once

#include "scenes/bad_kb_scene.h"
#include "helpers/ducky_script.h"

#include <gui/gui.h>
#include <bad_kb_icons.h>
#include <gui/scene_manager.h>
#include <dialogs/dialogs.h>
#include <notification/notification_messages.h>

#define BAD_KB_APP_SCRIPT_EXTENSION ".txt"
#define BAD_KB_APP_LAYOUT_EXTENSION ".kl"

typedef enum BadKbCustomEvent {
    BadKbAppCustomEventTextInputDone,
    BadKbAppCustomEventByteInputDone,
    BadKbCustomEventErrorBack
} BadKbCustomEvent;

typedef enum {
    BadKbAppViewWidget,
    BadKbAppViewWork,
    BadKbAppViewVarItemList,
    BadKbAppViewByteInput,
    BadKbAppViewTextInput
} BadKbAppView;
