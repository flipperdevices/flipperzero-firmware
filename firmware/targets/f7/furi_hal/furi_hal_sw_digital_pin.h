#pragma once

#include <stddef.h>
#include <stdint.h>
#include <furi_hal_gpio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t* rx_buff;
    size_t rx_buff_size;
} SwDigitalPinRx;

typedef uint32_t (*FuriHalSwDigitalPinTxCallbackYield)(void* context);
typedef void (*FuriHalSwDigitalPinTxCallbackEnd)(void* context);
typedef void (*FuriHalSwDigitalPinRxCallback)(void* context, SwDigitalPinRx data);

typedef enum {
    SwDigitalPinStateInit,

    SwDigitalPinStateIdle,

    //SwDigitalPinStateAsyncRx,

    SwDigitalPinStateTx,
    SwDigitalPinStateTxLast,
    SwDigitalPinStateTxEnd,

} SwDigitalPinState;




void furi_hal_sw_digital_pin_tim_init(uint16_t psc, uint16_t arr);
void furi_hal_sw_digital_pin_tim_deinit(void);
void furi_hal_sw_digital_pin_set_tim_clock(uint16_t psc, uint16_t arr);
void furi_hal_sw_digital_pin_sync_init(const GpioPin* gpio);
void furi_hal_sw_digital_pin_sync_start(void);
void furi_hal_sw_digital_pin_sync_stop(void);
void furi_hal_sw_digital_pin_sync_deinit(void);
void furi_hal_sw_digital_pin_tx_init(
    FuriHalSwDigitalPinTxCallbackYield tx_callback_yield,
    FuriHalSwDigitalPinTxCallbackEnd tx_callback_end,
    void* context,
    size_t samples,
    const GpioPin* gpio);
void furi_hal_sw_digital_pin_tx_start(void);
bool furi_hal_sw_digital_pin_is_tx_complete(void);
void furi_hal_sw_digital_pin_tx_stop(void);
void furi_hal_sw_digital_pin_tx_deinit(void);
void furi_hal_sw_digital_pin_rx_init(
    FuriHalSwDigitalPinRxCallback rx_callback,
    void* context,
    size_t samples,
    const GpioPin* gpio);
void furi_hal_sw_digital_pin_rx_start(void);
void furi_hal_sw_digital_pin_rx_stop(void);
void furi_hal_sw_digital_pin_rx_deinit(void);

#ifdef __cplusplus
}
#endif
