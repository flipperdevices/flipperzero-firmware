#pragma once

#include <furi.h>
#include <bt/bt_service/bt.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <notification/notification.h>

#include <gui/modules/submenu.h>
#include <gui/modules/dialog_ex.h>
#include <gui/modules/popup.h>
#include "views/bt_hid_keynote.h"
#include "views/bt_hid_keyboard.h"
#include "views/bt_hid_media.h"
#include "views/bt_hid_mouse.h"
#include "views/bt_hid_tiktok.h"
#include "views/hid_error.h"

typedef enum { HidKeyboardConnectionTypeNone, HidKeyboardConnectionTypeBluetooth, HidKeyboardConnectionTypeUsb } HidKeyboardConnectionType;

typedef struct {
    Bt* bt;
    Gui* gui;
    NotificationApp* notifications;
    ViewDispatcher* view_dispatcher;
    Submenu* device_type_submenu;
    Submenu* conn_type_submenu;
    DialogEx* dialog;
    BtHidKeynote* bt_hid_keynote;
    BtHidKeyboard* bt_hid_keyboard;
    BtHidMedia* bt_hid_media;
    BtHidMouse* bt_hid_mouse;
    BtHidTikTok* bt_hid_tiktok;
    HidError* hid_error;
    HidKeyboardConnectionType hid_conn_type;
    uint32_t view_id;
} BtHid;

