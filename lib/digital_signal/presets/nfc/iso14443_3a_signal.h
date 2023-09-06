#pragma once

#include <furi_hal_resources.h>

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Iso14443_3aSignal Iso14443_3aSignal;

Iso14443_3aSignal* iso14443_3a_signal_alloc(const GpioPin* pin);

void iso14443_3a_signal_free(Iso14443_3aSignal* instance);

void iso14443_3a_signal_tx(
    Iso14443_3aSignal* instance,
    const uint8_t* tx_data,
    const uint8_t* tx_parity,
    size_t tx_bits);

#ifdef __cplusplus
}
#endif
