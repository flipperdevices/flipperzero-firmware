#include "weather_station_app_i.h"

#define TAG "WeatherStation"

void weather_station_init(
    void* context,
    const char* preset_name,
    uint32_t frequency,
    FuriHalSubGhzPreset preset) {
    furi_assert(context);
    WeatherStationApp* app = context;
    string_set(app->txrx->preset->name, preset_name);
    app->txrx->preset->frequency = frequency;
    app->txrx->preset->preset = preset;
}

void weather_station_begin(WeatherStationApp* app, FuriHalSubGhzPreset preset) {
    furi_assert(app);
    furi_hal_subghz_reset();
    furi_hal_subghz_idle();
    furi_hal_subghz_load_preset(preset);
    furi_hal_gpio_init(&gpio_cc1101_g0, GpioModeInput, GpioPullNo, GpioSpeedLow);
    app->txrx->txrx_state = WeatherStationTxRxStateIDLE;
}

uint32_t weather_station_rx(WeatherStationApp* app, uint32_t frequency) {
    furi_assert(app);
    if(!furi_hal_subghz_is_frequency_valid(frequency)) {
        furi_crash("WeatherStation: Incorrect RX frequency.");
    }
    furi_assert(
        app->txrx->txrx_state != WeatherStationTxRxStateRx &&
        app->txrx->txrx_state != WeatherStationTxRxStateSleep);

    furi_hal_subghz_idle();
    uint32_t value = furi_hal_subghz_set_frequency_and_path(frequency);
    furi_hal_gpio_init(&gpio_cc1101_g0, GpioModeInput, GpioPullNo, GpioSpeedLow);
    furi_hal_subghz_flush_rx();
    furi_hal_subghz_rx();

    furi_hal_subghz_start_async_rx(subghz_worker_rx_callback, app->txrx->worker);
    subghz_worker_start(app->txrx->worker);
    app->txrx->txrx_state = WeatherStationTxRxStateRx;
    return value;
}

void weather_station_idle(WeatherStationApp* app) {
    furi_assert(app);
    furi_assert(app->txrx->txrx_state != WeatherStationTxRxStateSleep);
    furi_hal_subghz_idle();
    app->txrx->txrx_state = WeatherStationTxRxStateIDLE;
}

void weather_station_rx_end(WeatherStationApp* app) {
    furi_assert(app);
    furi_assert(app->txrx->txrx_state == WeatherStationTxRxStateRx);
    if(subghz_worker_is_running(app->txrx->worker)) {
        subghz_worker_stop(app->txrx->worker);
        furi_hal_subghz_stop_async_rx();
    }
    furi_hal_subghz_idle();
    app->txrx->txrx_state = WeatherStationTxRxStateIDLE;
}

void weather_station_sleep(WeatherStationApp* app) {
    furi_assert(app);
    furi_hal_subghz_sleep();
    app->txrx->txrx_state = WeatherStationTxRxStateSleep;
}