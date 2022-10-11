#pragma once

#include "helpers/weather_station_types.h"

#include "scenes/weather_station_scene.h"
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/submenu.h>
#include "views/weather_station_show.h"
#include "views/weather_station_receiver.h"

#include <lib/subghz/subghz_setting.h>
#include <lib/subghz/subghz_worker.h>
#include <lib/subghz/receiver.h>
#include <lib/subghz/transmitter.h>
#include <lib/subghz/registry.h>

#include "weather_station_history.h"

typedef struct WeatherStationApp WeatherStationApp;

struct WeatherStationTxRx {
    SubGhzWorker* worker;

    SubGhzEnvironment* environment;
    SubGhzReceiver* receiver;
    //SubGhzTransmitter* transmitter;
    //SubGhzProtocolDecoderBase* decoder_result;
    //FlipperFormat* fff_data;

    SubGhzPresetDefinition* preset;

    WSHistory* history;
    uint16_t idx_menu_chosen;
    WSTxRxState txrx_state;
    WSHopperState hopper_state;
    uint8_t hopper_timeout;
    uint8_t hopper_idx_frequency;
    WSRxKeyState rx_key_state;
};

typedef struct WeatherStationTxRx WeatherStationTxRx;

struct WeatherStationApp {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    WeatherStationTxRx* txrx;
    SceneManager* scene_manager;

    VariableItemList* variable_item_list;
    Submenu* submenu;
    //todo detete weather_station_show
    WeatherStationShow* weather_station_show;
    WSReceiver* ws_receiver;
    WSLock lock;
    SubGhzSetting* setting;

    // FuriHalClockMcoSourceId mco_src;
    // FuriHalClockMcoDivisorId mco_div;

    // FuriHalPwmOutputId pwm_ch_prev;
    // FuriHalPwmOutputId pwm_ch;
    // uint32_t pwm_freq;
    // uint8_t pwm_duty;
};

void ws_preset_init(
    void* context,
    const char* preset_name,
    uint32_t frequency,
    uint8_t* preset_data,
    size_t preset_data_size);
bool ws_set_preset(WeatherStationApp* app, const char* preset);
void ws_get_frequency_modulation(
    WeatherStationApp* app,
    FuriString* frequency,
    FuriString* modulation);
void ws_begin(WeatherStationApp* app, uint8_t* preset_data);
uint32_t ws_rx(WeatherStationApp* app, uint32_t frequency);
void ws_idle(WeatherStationApp* app);
void ws_rx_end(WeatherStationApp* app);
void ws_sleep(WeatherStationApp* app);
void ws_hopper_update(WeatherStationApp* app);

//void tx(WeatherStationApp* app);
