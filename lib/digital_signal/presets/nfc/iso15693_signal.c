#include "iso15693_signal.h"

#include <digital_signal/digital_signal.h>

#define BITS_IN_BYTE (8U)

#define ISO15693_SIGNAL_COEFF_HI (1U)
#define ISO15693_SIGNAL_COEFF_LO (4U)

#define ISO15693_SIGNAL_ZERO_EDGES (16U)
#define ISO15693_SIGNAL_ONE_EDGES (ISO15693_SIGNAL_ZERO_EDGES + 1U)
#define ISO15693_SIGNAL_EOF_EDGES (64U)
#define ISO15693_SIGNAL_SOF_EDGES (ISO15693_SIGNAL_EOF_EDGES + 1U)
#define ISO15693_SIGNAL_EDGES (1350U)

#define ISO15693_SIGNAL_FC (13.56e6)
#define ISO15693_SIGNAL_FC_16 (16.0e11 / ISO15693_SIGNAL_FC)
#define ISO15693_SIGNAL_FC_256 (256.0e11 / ISO15693_SIGNAL_FC)
#define ISO15693_SIGNAL_FC_768 (768.0e11 / ISO15693_SIGNAL_FC)

typedef struct {
    DigitalSignal* sof;
    DigitalSignal* eof;
    DigitalSignal* one;
    DigitalSignal* zero;
} Iso15693Signals;

struct Iso15693Signal {
    const GpioPin* pin;
    Iso15693Signals signals[Iso15693SignalDataRateNum];
    DigitalSignal* sof;
    DigitalSignal* eof;
    DigitalSignal* one;
    DigitalSignal* zero;
    DigitalSignal* tx_signal;
};

// TODO: Rewrite all of this with digital sequence!

// Add an unmodulated signal for the length of Fc / 256 * k (where k = 1 or 4)
static void iso15693_add_silence(DigitalSignal* signal, Iso15693SignalDataRate data_rate) {
    const uint32_t k = data_rate == Iso15693SignalDataRateHi ? ISO15693_SIGNAL_COEFF_HI :
                                                               ISO15693_SIGNAL_COEFF_LO;
    digital_signal_add_pulse(signal, ISO15693_SIGNAL_FC_256 * k, false);
}

// Add 8 * k subcarrier pulses of Fc / 16 (where k = 1 or 4)
static void iso15693_add_subcarrier(DigitalSignal* signal, Iso15693SignalDataRate data_rate) {
    const uint32_t k = data_rate == Iso15693SignalDataRateHi ? ISO15693_SIGNAL_COEFF_HI :
                                                               ISO15693_SIGNAL_COEFF_LO;
    for(uint32_t i = 0; i < ISO15693_SIGNAL_ZERO_EDGES * k; ++i) {
        digital_signal_add_pulse(signal, ISO15693_SIGNAL_FC_16, !(i % 2));
    }
}

static void iso15693_add_bit(DigitalSignal* signal, Iso15693SignalDataRate data_rate, bool bit) {
    if(bit) {
        iso15693_add_silence(signal, data_rate);
        iso15693_add_subcarrier(signal, data_rate);
    } else {
        iso15693_add_subcarrier(signal, data_rate);
        iso15693_add_silence(signal, data_rate);
    }
}

static void iso15693_add_byte(Iso15693Signal* instance, uint8_t byte) {
    for(size_t i = 0; i < BITS_IN_BYTE; i++) {
        const uint8_t bit = byte & (1U << i);
        digital_signal_append(instance->tx_signal, bit ? instance->one : instance->zero);
    }
}

static void iso15693_add_sof(DigitalSignal* signal, Iso15693SignalDataRate data_rate) {
    for(uint32_t i = 0; i < ISO15693_SIGNAL_FC_768 / ISO15693_SIGNAL_FC_256; ++i) {
        iso15693_add_silence(signal, data_rate);
    }

    for(uint32_t i = 0; i < ISO15693_SIGNAL_FC_768 / ISO15693_SIGNAL_FC_256; ++i) {
        iso15693_add_subcarrier(signal, data_rate);
    }

    iso15693_add_bit(signal, data_rate, true);
}

static void iso15693_add_eof(DigitalSignal* signal, Iso15693SignalDataRate data_rate) {
    iso15693_add_bit(signal, data_rate, false);

    for(uint32_t i = 0; i < ISO15693_SIGNAL_FC_768 / ISO15693_SIGNAL_FC_256; ++i) {
        iso15693_add_subcarrier(signal, data_rate);
    }

    for(uint32_t i = 0; i < ISO15693_SIGNAL_FC_768 / ISO15693_SIGNAL_FC_256; ++i) {
        iso15693_add_silence(signal, data_rate);
    }
}

static void
    iso15693_signal_encode(Iso15693Signal* instance, const uint8_t* tx_data, size_t tx_data_size) {
    instance->tx_signal->edge_cnt = 0;
    instance->tx_signal->start_level = false;

    digital_signal_append(instance->tx_signal, instance->sof);

    for(size_t i = 0; i < tx_data_size; i++) {
        iso15693_add_byte(instance, tx_data[i]);
    }

    digital_signal_append(instance->tx_signal, instance->eof);
}

Iso15693Signal* iso15693_signal_alloc(const GpioPin* pin) {
    furi_assert(pin);

    Iso15693Signal* instance = malloc(sizeof(Iso15693Signal));

    instance->pin = pin;
    instance->sof = digital_signal_alloc(ISO15693_SIGNAL_SOF_EDGES);
    instance->eof = digital_signal_alloc(ISO15693_SIGNAL_EOF_EDGES);
    instance->one = digital_signal_alloc(ISO15693_SIGNAL_ONE_EDGES);
    instance->zero = digital_signal_alloc(ISO15693_SIGNAL_ZERO_EDGES);
    instance->tx_signal = digital_signal_alloc(ISO15693_SIGNAL_EDGES);

    iso15693_add_sof(instance->sof, Iso15693SignalDataRateHi);
    iso15693_add_eof(instance->eof, Iso15693SignalDataRateHi);
    iso15693_add_bit(instance->one, Iso15693SignalDataRateHi, true);
    iso15693_add_bit(instance->zero, Iso15693SignalDataRateHi, false);

    return instance;
}

void iso15693_signal_free(Iso15693Signal* instance) {
    furi_assert(instance);

    digital_signal_free(instance->sof);
    digital_signal_free(instance->eof);
    digital_signal_free(instance->one);
    digital_signal_free(instance->zero);
    digital_signal_free(instance->tx_signal);

    free(instance);
}

void iso15693_signal_tx(Iso15693Signal* instance, const uint8_t* tx_data, size_t tx_data_size) {
    furi_assert(instance);
    furi_assert(tx_data);
    furi_assert(tx_data_size / BITS_IN_BYTE);

    FURI_CRITICAL_ENTER();
    iso15693_signal_encode(instance, tx_data, tx_data_size);
    digital_signal_send(instance->tx_signal, instance->pin);
    FURI_CRITICAL_EXIT();
}
