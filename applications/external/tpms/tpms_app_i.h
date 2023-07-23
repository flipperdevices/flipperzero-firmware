#pragma once

#include "helpers/tpms_types.h"

#include "scenes/tpms_scene.h"
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/widget.h>
#include <notification/notification_messages.h>
#include "views/tpms_receiver.h"
#include "views/tpms_receiver_info.h"
#include "tpms_history.h"

#include <lib/subghz/subghz_setting.h>
#include <lib/subghz/subghz_worker.h>
#include <lib/subghz/receiver.h>
#include <lib/subghz/transmitter.h>
#include <lib/subghz/registry.h>

#include "helpers/radio_device_loader.h"

typedef struct TPMSApp TPMSApp;

struct TPMSTxRx {
    SubGhzWorker* worker;

    const SubGhzDevice* radio_device;
    SubGhzEnvironment* environment;
    SubGhzReceiver* receiver;
    SubGhzRadioPreset* preset;
    TPMSHistory* history;
    uint16_t idx_menu_chosen;
    TPMSTxRxState txrx_state;
    TPMSHopperState hopper_state;
    uint8_t hopper_timeout;
    uint8_t hopper_idx_frequency;
    TPMSRxKeyState rx_key_state;
};

typedef struct TPMSTxRx TPMSTxRx;

struct TPMSApp {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    TPMSTxRx* txrx;
    SceneManager* scene_manager;
    NotificationApp* notifications;
    VariableItemList* variable_item_list;
    Submenu* submenu;
    Widget* widget;
    TPMSReceiver* tpms_receiver;
    TPMSReceiverInfo* tpms_receiver_info;
    TPMSLock lock;
    SubGhzSetting* setting;
    TPMSRelearn relearn;
    TPMSRelearnType relearn_type;
};

void tpms_preset_init(
    void* context,
    const char* preset_name,
    uint32_t frequency,
    uint8_t* preset_data,
    size_t preset_data_size);
bool tpms_set_preset(TPMSApp* app, const char* preset);
void tpms_get_frequency_modulation(TPMSApp* app, FuriString* frequency, FuriString* modulation);
void tpms_begin(TPMSApp* app, uint8_t* preset_data);
uint32_t tpms_rx(TPMSApp* app, uint32_t frequency);
void tpms_idle(TPMSApp* app);
void tpms_rx_end(TPMSApp* app);
void tpms_sleep(TPMSApp* app);
void tpms_hopper_update(TPMSApp* app);
