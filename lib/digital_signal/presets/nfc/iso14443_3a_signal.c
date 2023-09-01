#include "iso14443_3a_signal.h"

#include <digital_signal/digital_signal.h>

#define ISO14443_3A_SIGNAL_BIT_MAX_EDGES (10)
#define ISO14443_3A_SIGNAL_MAX_EDGES (1350)

#define ISO14443_3A_SIGNAL_F_SIG (13560000.0)
#define ISO14443_3A_SIGNAL_T_SIG 7374 //73.746ns*100
#define ISO14443_3A_SIGNAL_T_SIG_X8 58992 //T_SIG*8
#define ISO14443_3A_SIGNAL_T_SIG_X8_X8 471936 //T_SIG*8*8
#define ISO14443_3A_SIGNAL_T_SIG_X8_X9 530928 //T_SIG*8*9

struct Iso14443_3aSignal {
    const GpioPin* pin;
    DigitalSignal* one;
    DigitalSignal* zero;
    DigitalSignal* tx_signal;
};

static void iso14443_3a_add_bit(DigitalSignal* signal, bool bit) {
    if(bit) {
        signal->start_level = true;
        for(size_t i = 0; i < 7; i++) {
            signal->edge_timings[i] = ISO14443_3A_SIGNAL_T_SIG_X8;
        }
        signal->edge_timings[7] = ISO14443_3A_SIGNAL_T_SIG_X8_X9;
        signal->edge_cnt = 8;
    } else {
        signal->start_level = false;
        signal->edge_timings[0] = ISO14443_3A_SIGNAL_T_SIG_X8_X8;
        for(size_t i = 1; i < 9; i++) {
            signal->edge_timings[i] = ISO14443_3A_SIGNAL_T_SIG_X8;
        }
        signal->edge_cnt = 9;
    }
}

static void iso14443_3a_add_byte(Iso14443_3aSignal* instance, uint8_t byte, bool parity) {
    for(size_t i = 0; i < 8; i++) {
        if(byte & (1 << i)) {
            digital_signal_append(instance->tx_signal, instance->one);
        } else {
            digital_signal_append(instance->tx_signal, instance->zero);
        }
    }
    if(parity) {
        digital_signal_append(instance->tx_signal, instance->one);
    } else {
        digital_signal_append(instance->tx_signal, instance->zero);
    }
}

static void iso14443_3a_signal_encode(
    Iso14443_3aSignal* instance,
    const uint8_t* tx_data,
    const uint8_t* tx_parity,
    size_t tx_bits) {
    furi_assert(instance);
    furi_assert(tx_data);
    furi_assert(tx_parity);

    instance->tx_signal->edge_cnt = 0;
    instance->tx_signal->start_level = true;
    // Start of frame
    digital_signal_append(instance->tx_signal, instance->one);

    if(tx_bits < 8) {
        for(size_t i = 0; i < tx_bits; i++) {
            if(FURI_BIT(tx_data[0], i)) {
                digital_signal_append(instance->tx_signal, instance->one);
            } else {
                digital_signal_append(instance->tx_signal, instance->zero);
            }
        }
    } else {
        for(size_t i = 0; i < tx_bits / 8; i++) {
            bool parity = FURI_BIT(tx_parity[i / 8], i % 8);
            iso14443_3a_add_byte(instance, tx_data[i], parity);
        }
    }
}

Iso14443_3aSignal* iso14443_3a_signal_alloc(const GpioPin* pin) {
    furi_assert(pin);

    Iso14443_3aSignal* instance = malloc(sizeof(Iso14443_3aSignal));
    instance->pin = pin;
    instance->one = digital_signal_alloc(ISO14443_3A_SIGNAL_BIT_MAX_EDGES);
    instance->zero = digital_signal_alloc(ISO14443_3A_SIGNAL_BIT_MAX_EDGES);
    iso14443_3a_add_bit(instance->one, true);
    iso14443_3a_add_bit(instance->zero, false);
    instance->tx_signal = digital_signal_alloc(ISO14443_3A_SIGNAL_MAX_EDGES);

    return instance;
}

void iso14443_3a_signal_free(Iso14443_3aSignal* instance) {
    furi_assert(instance);
    furi_assert(instance->one);
    furi_assert(instance->zero);
    furi_assert(instance->tx_signal);

    digital_signal_free(instance->one);
    digital_signal_free(instance->zero);
    digital_signal_free(instance->tx_signal);
    free(instance);
}

void iso14443_3a_signal_tx(
    Iso14443_3aSignal* instance,
    const uint8_t* tx_data,
    const uint8_t* tx_parity,
    size_t tx_bits) {
    furi_assert(instance);
    furi_assert(tx_data);
    furi_assert(tx_parity);

    FURI_CRITICAL_ENTER();
    iso14443_3a_signal_encode(instance, tx_data, tx_parity, tx_bits);
    digital_signal_send(instance->tx_signal, instance->pin);
    FURI_CRITICAL_EXIT();
}
