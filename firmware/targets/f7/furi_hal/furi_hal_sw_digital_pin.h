#pragma once

#include <stddef.h>
#include <stdint.h>
#include <furi_hal_gpio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint16_t* rx_buff;
    size_t rx_buff_size;
} SwDigitalPinRx;

typedef uint32_t (*FuriHalSwDigitalPinTxCallbackYield)(void* context);
typedef void (*FuriHalSwDigitalPinTxCallbackEnd)(void* context);
typedef void (*FuriHalSwDigitalPinRxCallback)(void* context, SwDigitalPinRx data);

void furi_hal_sw_digital_pin_sync_init(const GpioPin* gpio, bool inverse);
void furi_hal_sw_digital_pin_sync_start(void);
void furi_hal_sw_digital_pin_sync_stop(void);
bool furi_hal_sw_digital_pin_is_sync_running(void);
void furi_hal_sw_digital_pin_sync_deinit(void);

void furi_hal_sw_digital_pin_tx_init(
    FuriHalSwDigitalPinTxCallbackYield tx_callback_yield,
    FuriHalSwDigitalPinTxCallbackEnd tx_callback_end,
    void* context,
    uint16_t tim_psc,
    uint32_t tim_arr,
    size_t samples,
    const GpioPin* gpio);
void furi_hal_sw_digital_pin_tx_start(void);
bool furi_hal_sw_digital_pin_is_tx_complete(void);
void furi_hal_sw_digital_pin_tx_stop(void);
void furi_hal_sw_digital_pin_tx_deinit(void);

void furi_hal_sw_digital_pin_rx_init(
    FuriHalSwDigitalPinRxCallback rx_callback,
    void* context,
    uint16_t tim_psc,
    uint32_t tim_arr,
    size_t samples,
    const GpioPin* gpio);
void furi_hal_sw_digital_pin_rx_start(void);
void furi_hal_sw_digital_pin_rx_stop(void);
bool furi_hal_sw_digital_pin_is_rx_running(void);
void furi_hal_sw_digital_pin_rx_deinit(void);

void furi_hal_sw_digital_pin_switch_rx_to_tx(void);
void furi_hal_sw_digital_pin_switch_tx_to_rx(void);

#ifdef __cplusplus
}
#endif
