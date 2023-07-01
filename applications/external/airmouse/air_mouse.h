#pragma once

#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/modules/dialog_ex.h>
#include <storage/storage.h>

#include "views/bt_mouse.h"
#include "views/usb_mouse.h"
#include "views/calibration.h"
#define CALIBRATION_OLD_DATA_PATH EXT_PATH(".calibration.data")
#define CALIBRATION_DATA_PATH EXT_PATH("apps_data/BMI160_Air_Mouse/.calibration.data")

typedef struct {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    DialogEx* dialog;
    BtMouse* bt_mouse;
    UsbMouse* usb_mouse;
    Calibration* calibration;
    uint32_t view_id;
} AirMouse;

typedef enum {
    AirMouseViewSubmenu,
    AirMouseViewBtMouse,
    AirMouseViewUsbMouse,
    AirMouseViewCalibration,
    AirMouseViewExitConfirm,
} AirMouseView;
