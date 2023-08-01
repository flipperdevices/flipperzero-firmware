#pragma once

#include "ublox.h"
#include "ublox_worker.h"
#include "ublox_device.h"

#include "helpers/ublox_types.h"

#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <gui/modules/widget.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/text_input.h>

#include <notification/notification_messages.h>

#include <storage/storage.h>
#include <storage/filesystem_api_defines.h>

#include "scenes/ublox_scene.h"
#include <ublox_icons.h>
#include "views/data_display_view.h"
#include "helpers/ublox_custom_event.h"
#include "helpers/kml.h"

struct Ublox {
    ViewDispatcher* view_dispatcher;
    Gui* gui;
    SceneManager* scene_manager;

    Submenu* submenu;
    Widget* widget;
    VariableItemList* variable_item_list;
    TextInput* text_input;
    DataDisplayView* data_display;

    Storage* storage;
    NotificationApp* notifications;

    UbloxWorker* worker;

    // file stuff
    KMLFile kmlfile;
    UbloxLogState log_state;
    char text_store[100];
    Ublox_NAV_PVT_Message nav_pvt;
    Ublox_NAV_ODO_Message nav_odo;
    Ublox_NAV_TIMEUTC_Message nav_timeutc;

    UbloxDataDisplayState data_display_state;
    UbloxDeviceState device_state;
    bool gps_initted;
};

typedef enum {
    UbloxViewMenu,
    UbloxViewWidget,
    UbloxViewVariableItemList,
    UbloxViewTextInput,
    // custom
    UbloxViewDataDisplay,
} UbloxView;

Ublox* ublox_alloc();
