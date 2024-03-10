#pragma once

#include "findmy.h"
#include "findmy_state.h"
#include <furi_hal_bt.h>
#include <extra_beacon.h>
#include "findmy_icons.h"
#include <toolbox/stream/file_stream.h>
#include <toolbox/hex.h>
#include <toolbox/path.h>
#include <gui/gui.h>
#include <storage/storage.h>
#include <dialogs/dialogs.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>
#include "views/findmy_main.h"
#include <gui/modules/byte_input.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/popup.h>
#include "scenes/findmy_scene.h"
#include "helpers/base64.h"
#if FW_ORIGIN_Official
void furi_hal_bt_reverse_mac_addr(uint8_t mac_addr[GAP_MAC_ADDR_SIZE]);
#endif

struct FindMy {
    Gui* gui;
    Storage* storage;
    DialogsApp* dialogs;
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;

    FindMyMain* findmy_main;
    ByteInput* byte_input;
    VariableItemList* var_item_list;
    Popup* popup;

    uint8_t mac_buf[EXTRA_BEACON_MAC_ADDR_SIZE];
    uint8_t packet_buf[EXTRA_BEACON_MAX_DATA_SIZE];

    FindMyState state;
};

typedef enum {
    FindMyViewMain,
    FindMyViewByteInput,
    FindMyViewVarItemList,
    FindMyViewPopup,
} FindMyView;

enum FindMyType {
    FindMyTypeApple,
    FindMyTypeSamsung,
};

void findmy_change_broadcast_interval(FindMy* app, uint8_t value);
void findmy_change_transmit_power(FindMy* app, uint8_t value);
void findmy_toggle_beacon(FindMy* app);
FindMyType findmy_data_get_type(uint8_t data[EXTRA_BEACON_MAX_DATA_SIZE]);
