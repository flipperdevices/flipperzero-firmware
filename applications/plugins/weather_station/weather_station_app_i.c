#include "weather_station_app_i.h"

#define TAG "WeatherStation"
#include <flipper_format/flipper_format_i.h>

void ws_preset_init(
    void* context,
    const char* preset_name,
    uint32_t frequency,
    uint8_t* preset_data,
    size_t preset_data_size) {
    furi_assert(context);
    WeatherStationApp* app = context;
    string_set(app ->txrx->preset->name, preset_name);
    app ->txrx->preset->frequency = frequency;
    app ->txrx->preset->data = preset_data;
    app ->txrx->preset->data_size = preset_data_size;
}

bool ws_set_preset(WeatherStationApp* app, const char* preset) {
    if(!strcmp(preset, "FuriHalSubGhzPresetOok270Async")) {
        string_set(app ->txrx->preset->name, "AM270");
    } else if(!strcmp(preset, "FuriHalSubGhzPresetOok650Async")) {
        string_set(app ->txrx->preset->name, "AM650");
    } else if(!strcmp(preset, "FuriHalSubGhzPreset2FSKDev238Async")) {
        string_set(app ->txrx->preset->name, "FM238");
    } else if(!strcmp(preset, "FuriHalSubGhzPreset2FSKDev476Async")) {
        string_set(app ->txrx->preset->name, "FM476");
    } else if(!strcmp(preset, "FuriHalSubGhzPresetCustom")) {
        string_set(app ->txrx->preset->name, "CUSTOM");
    } else {
        FURI_LOG_E(TAG, "Unknown preset");
        return false;
    }
    return true;
}

void ws_get_frequency_modulation(WeatherStationApp* app, string_t frequency, string_t modulation) {
    furi_assert(app);
    if(frequency != NULL) {
        string_printf(
            frequency,
            "%03ld.%02ld",
            app ->txrx->preset->frequency / 1000000 % 1000,
            app ->txrx->preset->frequency / 10000 % 100);
    }
    if(modulation != NULL) {
        string_printf(modulation, "%0.2s", string_get_cstr(app ->txrx->preset->name));
    }
}

void ws_begin(WeatherStationApp* app, uint8_t* preset_data) {
    furi_assert(app);
    UNUSED (preset_data);
    furi_hal_subghz_reset();
    furi_hal_subghz_idle();
    furi_hal_subghz_load_custom_preset(preset_data);
    furi_hal_gpio_init(&gpio_cc1101_g0, GpioModeInput, GpioPullNo, GpioSpeedLow);
    app ->txrx->txrx_state = WSTxRxStateIDLE;
}

uint32_t ws_rx(WeatherStationApp* app, uint32_t frequency) {
    furi_assert(app);
    if(!furi_hal_subghz_is_frequency_valid(frequency)) {
        furi_crash("WeatherStation: Incorrect RX frequency.");
    }
    furi_assert(
        app ->txrx->txrx_state != WSTxRxStateRx &&
        app ->txrx->txrx_state != WSTxRxStateSleep);

    furi_hal_subghz_idle();
    uint32_t value = furi_hal_subghz_set_frequency_and_path(frequency);
    furi_hal_gpio_init(&gpio_cc1101_g0, GpioModeInput, GpioPullNo, GpioSpeedLow);
    furi_hal_subghz_flush_rx();
    furi_hal_subghz_rx();

    furi_hal_subghz_start_async_rx(subghz_worker_rx_callback, app ->txrx->worker);
    subghz_worker_start(app ->txrx->worker);
    app ->txrx->txrx_state = WSTxRxStateRx;
    return value;
}

// static bool ws_tx(WeatherStationApp* app, uint32_t frequency) {
//     furi_assert(app);
//     if(!furi_hal_subghz_is_frequency_valid(frequency)) {
//         furi_crash("WeatherStation: Incorrect TX frequency.");
//     }
//     furi_assert(app->txrx->txrx_state != WSTxRxStateSleep);
//     furi_hal_subghz_idle();
//     furi_hal_subghz_set_frequency_and_path(frequency);
//     furi_hal_gpio_init(&gpio_cc1101_g0, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
//     furi_hal_gpio_write(&gpio_cc1101_g0, true);
//     bool ret = furi_hal_subghz_tx();
//     app->txrx->txrx_state = WSTxRxStateTx;
//     return ret;
// }

void ws_idle(WeatherStationApp* app) {
    furi_assert(app);
    furi_assert(app->txrx->txrx_state != WSTxRxStateSleep);
    furi_hal_subghz_idle();
    app->txrx->txrx_state = WSTxRxStateIDLE;
}

void ws_rx_end(WeatherStationApp* app) {
    furi_assert(app);
    furi_assert(app->txrx->txrx_state == WSTxRxStateRx);
    if(subghz_worker_is_running(app->txrx->worker)) {
        subghz_worker_stop(app->txrx->worker);
        furi_hal_subghz_stop_async_rx();
    }
    furi_hal_subghz_idle();
    app->txrx->txrx_state = WSTxRxStateIDLE;
}

void ws_sleep(WeatherStationApp* app) {
    furi_assert(app);
    furi_hal_subghz_sleep();
    app->txrx->txrx_state = WSTxRxStateSleep;
}



// void weather_station_init(
//     void* context,
//     const char* preset_name,
//     uint32_t frequency,
//     FuriHalSubGhzPreset preset) {
//     furi_assert(context);
//     WeatherStationApp* app = context;
//     string_set(app->txrx->preset->name, preset_name);
//     app->txrx->preset->frequency = frequency;
//     app->txrx->preset->preset = preset;
// }

// void weather_station_begin(WeatherStationApp* app, FuriHalSubGhzPreset preset) {
//     furi_assert(app);
//     furi_hal_subghz_reset();
//     furi_hal_subghz_idle();
//     furi_hal_subghz_load_preset(preset);
//     furi_hal_gpio_init(&gpio_cc1101_g0, GpioModeInput, GpioPullNo, GpioSpeedLow);
//     app->txrx->txrx_state = WSTxRxStateIDLE;
// }

// uint32_t weather_station_rx(WeatherStationApp* app, uint32_t frequency) {
//     furi_assert(app);
//     if(!furi_hal_subghz_is_frequency_valid(frequency)) {
//         furi_crash("WeatherStation: Incorrect RX frequency.");
//     }
//     furi_assert(
//         app->txrx->txrx_state != WSTxRxStateRx &&
//         app->txrx->txrx_state != WSTxRxStateSleep);

//     furi_hal_subghz_idle();
//     uint32_t value = furi_hal_subghz_set_frequency_and_path(frequency);
//     furi_hal_gpio_init(&gpio_cc1101_g0, GpioModeInput, GpioPullNo, GpioSpeedLow);
//     furi_hal_subghz_flush_rx();
//     furi_hal_subghz_rx();

//     furi_hal_subghz_start_async_rx(subghz_worker_rx_callback, app->txrx->worker);
//     subghz_worker_start(app->txrx->worker);
//     app->txrx->txrx_state = WSTxRxStateRx;
//     return value;
// }

// void weather_station_idle(WeatherStationApp* app) {
//     furi_assert(app);
//     furi_assert(app->txrx->txrx_state != WSTxRxStateSleep);
//     furi_hal_subghz_idle();
//     app->txrx->txrx_state = WSTxRxStateIDLE;
// }

// void weather_station_rx_end(WeatherStationApp* app) {
//     furi_assert(app);
//     furi_assert(app->txrx->txrx_state == WSTxRxStateRx);
//     if(subghz_worker_is_running(app->txrx->worker)) {
//         subghz_worker_stop(app->txrx->worker);
//         furi_hal_subghz_stop_async_rx();
//     }
//     furi_hal_subghz_idle();
//     app->txrx->txrx_state = WSTxRxStateIDLE;
// }

// void weather_station_sleep(WeatherStationApp* app) {
//     furi_assert(app);
//     furi_hal_subghz_sleep();
//     app->txrx->txrx_state = WSTxRxStateSleep;
// }

// void tx(WeatherStationApp* app) {
//     string_t flipper_format_string;
//     string_init_printf(
//         flipper_format_string,
//         "Protocol: Princeton\n"
//         "Bit: 24\n"
//         "Key: 00 00 00 00 00 15 15 AA\n"
//         "TE: 400\n"
//         "Repeat: 20\n");
//     FlipperFormat* flipper_format = flipper_format_string_alloc();
//     Stream* stream = flipper_format_get_raw_stream(flipper_format);
//     stream_clean(stream);
//     stream_write_cstring(stream, string_get_cstr(flipper_format_string));

//     SubGhzTransmitter* transmitter =
//         subghz_transmitter_alloc_init(app->txrx->environment, "Princeton");
//     subghz_transmitter_deserialize(transmitter, flipper_format);
//     weather_station_begin(app, FuriHalSubGhzPresetOok650Async);

//     furi_hal_subghz_set_frequency_and_path(433920000);
//     furi_hal_power_suppress_charge_enter();
//     furi_hal_subghz_start_async_tx(subghz_transmitter_yield, transmitter);

//     while(!(furi_hal_subghz_is_async_tx_complete())) {
//         printf(".");
//         fflush(stdout);
//         furi_delay_ms(333);
//     }
//     furi_hal_subghz_stop_async_tx();
//     furi_hal_subghz_sleep();

//     furi_hal_power_suppress_charge_exit();

//     flipper_format_free(flipper_format);
//     subghz_transmitter_free(transmitter);
// }