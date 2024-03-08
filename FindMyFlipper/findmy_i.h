#pragma once

#include "findmy.h"
#include <furi_hal_bt.h>
#include <extra_beacon.h>
#include <assets_icons.h>
#include <gui/gui.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>
#include "views/findmy_main.h"
#include <gui/modules/byte_input.h>
#include <gui/modules/variable_item_list.h>
#include "scenes/findmy_scene.h"

struct FindMy {
    Gui* gui;
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;

    FindMyMain* findmy_main;
    ByteInput* byte_input;
    VariableItemList* var_item_list;

    uint8_t mac_buf[EXTRA_BEACON_MAC_ADDR_SIZE];
    uint8_t packet_buf[EXTRA_BEACON_MAX_DATA_SIZE];

    GapExtraBeaconConfig config;
    bool apple;
    bool beacon_active;
    uint8_t broadcast_interval;
    uint8_t transmit_power;
};

typedef enum {
    FindMyViewMain,
    FindMyViewByteInput,
    FindMyViewVarItemList,
} FindMyView;

void findmy_change_broadcast_interval(FindMy* app, uint8_t value);
void findmy_change_transmit_power(FindMy* app, uint8_t value);
void findmy_toggle_beacon(FindMy* app);
