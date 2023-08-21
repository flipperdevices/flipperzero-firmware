#pragma once

#include <furi_hal_resources.h>

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Iso15693Signal Iso15693Signal;

typedef enum {
    Iso15693SignalDataRateHi,
    Iso15693SignalDataRateLo,
    Iso15693SignalDataRateNum,
} Iso15693SignalDataRate;

Iso15693Signal* iso15693_signal_alloc(const GpioPin* pin);

void iso15693_signal_free(Iso15693Signal* instance);

void iso15693_signal_tx(
    Iso15693Signal* instance,
    Iso15693SignalDataRate data_rate,
    const uint8_t* tx_data,
    size_t tx_data_size);

#ifdef __cplusplus
}
#endif
