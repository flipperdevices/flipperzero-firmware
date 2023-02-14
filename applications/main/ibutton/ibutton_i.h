#pragma once

#include "ibutton.h"

#include <gui/gui.h>
#include <gui/view.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>
#include <notification/notification_messages.h>

#include <one_wire/ibutton/ibutton_worker.h>
#include <one_wire/ibutton/ibutton_protocols.h>

#include <storage/storage.h>
#include <dialogs/dialogs.h>

#include <gui/modules/submenu.h>
#include <gui/modules/popup.h>
#include <gui/modules/text_input.h>
#include <gui/modules/byte_input.h>
#include <gui/modules/text_box.h>
#include <gui/modules/widget.h>

#include <assets_icons.h>

#include "ibutton_custom_event.h"
#include "scenes/ibutton_scene.h"

#define IBUTTON_APP_FOLDER ANY_PATH("ibutton")
#define IBUTTON_APP_EXTENSION ".ibtn"

#define IBUTTON_KEY_NAME_SIZE 22

struct iButton {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;

    Gui* gui;
    Storage* storage;
    DialogsApp* dialogs;
    NotificationApp* notifications;

    iButtonWorker* key_worker;
    iButtonKey* key;

    FuriString* file_path;
    char key_name[IBUTTON_KEY_NAME_SIZE + 1];

    Submenu* submenu;
    ByteInput* byte_input;
    TextInput* text_input;
    TextBox* text_box;
    Popup* popup;
    Widget* widget;

    void* rpc_ctx;
};

typedef enum {
    iButtonViewSubmenu,
    iButtonViewByteInput,
    iButtonViewTextInput,
    iButtonViewTextBox,
    iButtonViewPopup,
    iButtonViewWidget,
} iButtonView;

typedef enum {
    iButtonNotificationMessageError,
    iButtonNotificationMessageSuccess,
    iButtonNotificationMessageReadStart,
    iButtonNotificationMessageEmulateStart,
    iButtonNotificationMessageYellowBlink,
    iButtonNotificationMessageEmulateBlink,
    iButtonNotificationMessageRedOn,
    iButtonNotificationMessageRedOff,
    iButtonNotificationMessageGreenOn,
    iButtonNotificationMessageGreenOff,
    iButtonNotificationMessageBlinkStop,
} iButtonNotificationMessage;

bool ibutton_select_key(iButton* ibutton);
bool ibutton_save_key(iButton* ibutton);
bool ibutton_delete_key(iButton* ibutton);
void ibutton_notification_message(iButton* ibutton, uint32_t message);

void ibutton_submenu_callback(void* context, uint32_t index);
void ibutton_widget_callback(GuiButtonType result, InputType type, void* context);
