#pragma once

#include "wifi_marauder_app.h"
#include "scenes/wifi_marauder_scene.h"
#include "wifi_marauder_custom_event.h"

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <gui/modules/variable_item_list.h>

struct WifiMarauderApp {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;

    VariableItemList* var_item_list;

    int selected_wifi_channel;
};

// Supported commands:
// https://github.com/justcallmekoko/ESP32Marauder/wiki/cli
//   Scan
//    -> If list is empty, then start a new scanap. (Tap any button to stop.)
//    -> If there's a list, provide option to rescan and dump list of targets to select.
//    -> Press BACK to go back to top-level.
//   Attack
//    -> Beacon
//    -> Deauth
//    -> Probe
//    -> Rickroll
//   Sniff
//    -> Beacon
//    -> Deauth
//    -> ESP
//    -> PMKID
//    -> Pwnagotchi
//   Channel
//   Update
//   Reboot

typedef enum {
    WifiMarauderAppViewVarItemList,
    WifiMarauderAppViewScan,
    WifiMarauderAppViewAttack,
    WifiMarauderAppViewSniff,
    WifiMarauderAppViewChannel,
    WifiMarauderAppViewUpdate,
    WifiMarauderAppViewReboot,
} WifiMarauderAppView;
