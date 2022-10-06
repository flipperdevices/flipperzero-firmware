#pragma once

#include "scenes/weather_station_scene.h"
#include "m-string.h"

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/submenu.h>
#include "views/weather_station_show.h"

#include <lib/subghz/subghz_worker.h>
#include <lib/subghz/receiver.h>
#include <lib/subghz/transmitter.h>
#include <lib/subghz/registry.h>

typedef struct WeatherStationApp WeatherStationApp;

struct WeatherStationPresetDefinition {
    string_t name;
    uint32_t frequency;
    FuriHalSubGhzPreset preset;
    //uint8_t* data;
    //size_t data_size;
};

typedef struct WeatherStationPresetDefinition WeatherStationPresetDefinition;

/** WeatherStationTxRx state */
typedef enum {
    WeatherStationTxRxStateIDLE,
    WeatherStationTxRxStateRx,
    WeatherStationTxRxStateTx,
    WeatherStationTxRxStateSleep,
} WeatherStationTxRxState;

struct WeatherStationTxRx {
    SubGhzWorker* worker;

    SubGhzEnvironment* environment;
    SubGhzReceiver* receiver;
    //SubGhzTransmitter* transmitter;
    //SubGhzProtocolDecoderBase* decoder_result;
    //FlipperFormat* fff_data;

    WeatherStationPresetDefinition* preset;

    //SubGhzHistory* history;
    //uint16_t idx_menu_chosen;
    WeatherStationTxRxState txrx_state;
    //SubGhzHopperState hopper_state;
    //uint8_t hopper_timeout;
    //uint8_t hopper_idx_frequency;
    //SubGhzRxKeyState rx_key_state;
};

typedef struct WeatherStationTxRx WeatherStationTxRx;

struct WeatherStationApp {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    WeatherStationTxRx* txrx;
    SceneManager* scene_manager;

    // VariableItemList* var_item_list;
    Submenu* submenu;
    WeatherStationShow* weather_station_show;

    // FuriHalClockMcoSourceId mco_src;
    // FuriHalClockMcoDivisorId mco_div;

    // FuriHalPwmOutputId pwm_ch_prev;
    // FuriHalPwmOutputId pwm_ch;
    // uint32_t pwm_freq;
    // uint8_t pwm_duty;
};

typedef enum {
    //WeatherStationViewVarItemList,
    WeatherStationViewSubmenu,
    WeatherStationViewShow,
} WeatherStationView;

// typedef enum {
//     //WeatherStationMcoEventUpdate,
//     //WeatherStationPwmEventUpdate,
//     //WeatherStationPwmEventChannelChange,
// } WeatherStationCustomEvent;

void weather_station_init(
    void* context,
    const char* preset_name,
    uint32_t frequency,
    FuriHalSubGhzPreset preset);
void weather_station_begin(WeatherStationApp* app, FuriHalSubGhzPreset preset);
uint32_t weather_station_rx(WeatherStationApp* app, uint32_t frequency);
void weather_station_idle(WeatherStationApp* app);
void weather_station_rx_end(WeatherStationApp* app);
void weather_station_sleep(WeatherStationApp* app);



