#pragma once

#include "scenes/bad_bt_scene.h"
#include "helpers/ducky_script.h"

#include <gui/gui.h>
#include "bad_bt_icons.h"
#include <gui/scene_manager.h>
#include <dialogs/dialogs.h>
#include <notification/notification_messages.h>

#define BAD_BT_APP_SCRIPT_EXTENSION ".txt"
#define BAD_BT_APP_LAYOUT_EXTENSION ".kl"

typedef enum BadBtCustomEvent {
    BadBtAppCustomEventTextInputDone,
    BadBtAppCustomEventByteInputDone,
    BadBtCustomEventErrorBack
} BadBtCustomEvent;

typedef enum {
    BadBtAppViewWidget,
    BadBtAppViewWork,
    BadBtAppViewVarItemList,
    BadBtAppViewByteInput,
    BadBtAppViewTextInput
} BadBtAppView;