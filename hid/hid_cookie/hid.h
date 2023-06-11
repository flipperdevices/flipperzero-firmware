#pragma once

#include <furi.h>
#include <furi_hal_bt.h>
#include <furi_hal_bt_hid.h>
#include <furi_hal_usb.h>
#include <furi_hal_usb_hid.h>

#include <bt/bt_service/bt.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <notification/notification.h>
#include <storage/storage.h>

#include <gui/modules/submenu.h>
#include <gui/modules/dialog_ex.h>
#include <gui/modules/popup.h>
#include <gui/modules/widget.h>
#include <gui/modules/variable_item_list.h>
#include "views/hid_cc.h"

#define HID_BT_KEYS_STORAGE_NAME ".bt_hid.keys"

typedef struct Hid Hid;

struct Hid {
    Bt* bt;
    Gui* gui;
    NotificationApp* notifications;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    Widget* widget_instructions;
    VariableItemList* variable_item_list;
    Widget* widget_credits;
    HidCC* hid_cc;
    uint32_t view_id;
    uint8_t offset_repeat;
    uint8_t offset_speed;
    uint8_t offset_x;
    uint8_t offset_y;
};

void hid_hal_mouse_move(Hid* instance, int8_t dx, int8_t dy);
void hid_hal_mouse_press(Hid* instance, uint16_t event);
void hid_hal_mouse_release(Hid* instance, uint16_t event);
void hid_hal_mouse_release_all(Hid* instance);