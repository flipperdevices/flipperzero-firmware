#pragma once

#include <stddef.h>
#include <stdint.h>
#include <furi_hal_gpio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t (*FuriHalSwDigitalPinTxCallback)(void* context);

typedef enum {
    SwDigitalPinStateInit,

    SwDigitalPinStateIdle,

    //SwDigitalPinStateAsyncRx,

    SwDigitalPinStateTx,
    SwDigitalPinStateTxLast,
    SwDigitalPinStateTxEnd,

} SwDigitalPinState;

void furi_hal_sw_digital_pin_init(void);
void furi_hal_sw_digital_pin_tx_start(
    FuriHalSwDigitalPinTxCallback callback,
    void* context,
    size_t samples,
    const GpioPin* gpio);
bool furi_hal_sw_digital_pin_is_tx_complete(void);
void furi_hal_sw_digital_pin_tx_stop(void);

#ifdef __cplusplus
}
#endif
