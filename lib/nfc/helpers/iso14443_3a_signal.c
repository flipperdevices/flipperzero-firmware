#include "iso14443_3a_signal.h"

#include <digital_signal/digital_signal.h>

struct Iso14443_3aSignal {
    const GpioPin* pin;
    DigitalSignal* one;
    DigitalSignal* zero;
    DigitalSignal* tx_signal;
};

Iso14443_3aSignal* iso14443_3a_signal_alloc(const GpioPin* pin) {
    furi_assert(pin);

    Iso14443_3aSignal* instance = malloc(sizeof(Iso14443_3aSignal));
    instance->pin = pin;
    instance->one = digital_signal_alloc(10);
    instance->zero = digital_signal_alloc(10);
    nfca_add_bit(instance->one, true);
    nfca_add_bit(instance->zero, false);
    instance->tx_signal = digital_signal_alloc(NFCA_SIGNAL_MAX_EDGES);

}

void iso14443_3a_signal_free(Iso14443_3aSignal* instance);

void iso14443_3a_signal_tx(
    Iso14443_3aSignal* instance,
    const uint8_t* tx_data,
    const uint8_t* tx_parity,
    size_t tx_bits);
