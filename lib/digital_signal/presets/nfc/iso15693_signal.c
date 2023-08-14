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
} Iso15693SignalComponents;

struct Iso15693Signal {
    const GpioPin* pin;
    DigitalSignal* tx_signal;
    Iso15693SignalComponents* components[Iso15693SignalDataRateNum];
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

static void
    iso15693_add_byte(Iso15693Signal* instance, Iso15693SignalDataRate data_rate, uint8_t byte) {
    const Iso15693SignalComponents* components = instance->components[data_rate];

    for(size_t i = 0; i < BITS_IN_BYTE; i++) {
        const uint8_t bit = byte & (1U << i);
        digital_signal_append(instance->tx_signal, bit ? components->one : components->zero);
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

static void iso15693_signal_encode(
    Iso15693Signal* instance,
    Iso15693SignalDataRate data_rate,
    const uint8_t* tx_data,
    size_t tx_data_size) {
    instance->tx_signal->edge_cnt = 0;
    instance->tx_signal->start_level = false;

    const Iso15693SignalComponents* components = instance->components[data_rate];

    digital_signal_append(instance->tx_signal, components->sof);

    for(size_t i = 0; i < tx_data_size; i++) {
        iso15693_add_byte(instance, data_rate, tx_data[i]);
    }

    digital_signal_append(instance->tx_signal, components->eof);
}

static Iso15693SignalComponents*
    iso15693_signal_components_alloc(Iso15693SignalDataRate data_rate) {
    Iso15693SignalComponents* instance = malloc(sizeof(Iso15693SignalComponents));

    const uint32_t k = data_rate == Iso15693SignalDataRateHi ? ISO15693_SIGNAL_COEFF_HI :
                                                               ISO15693_SIGNAL_COEFF_LO;
    // TODO: possible a couple of wasted edges when k > 1
    instance->sof = digital_signal_alloc(ISO15693_SIGNAL_SOF_EDGES * k);
    instance->eof = digital_signal_alloc(ISO15693_SIGNAL_EOF_EDGES * k);
    instance->one = digital_signal_alloc(ISO15693_SIGNAL_ONE_EDGES * k);
    instance->zero = digital_signal_alloc(ISO15693_SIGNAL_ZERO_EDGES * k);

    iso15693_add_sof(instance->sof, data_rate);
    iso15693_add_eof(instance->eof, data_rate);
    iso15693_add_bit(instance->one, data_rate, true);
    iso15693_add_bit(instance->zero, data_rate, false);

    return instance;
}

static void iso15693_signal_components_free(Iso15693SignalComponents* instance) {
    digital_signal_free(instance->sof);
    digital_signal_free(instance->eof);
    digital_signal_free(instance->one);
    digital_signal_free(instance->zero);
}

Iso15693Signal* iso15693_signal_alloc(const GpioPin* pin) {
    furi_assert(pin);

    Iso15693Signal* instance = malloc(sizeof(Iso15693Signal));

    instance->pin = pin;
    instance->tx_signal = digital_signal_alloc(ISO15693_SIGNAL_EDGES);

    for(uint32_t i = 0; i < Iso15693SignalDataRateNum; ++i) {
        instance->components[i] = iso15693_signal_components_alloc(i);
    }

    return instance;
}

void iso15693_signal_free(Iso15693Signal* instance) {
    furi_assert(instance);

    for(uint32_t i = 0; i < Iso15693SignalDataRateNum; ++i) {
        iso15693_signal_components_free(instance->components[i]);
    }

    digital_signal_free(instance->tx_signal);

    free(instance);
}

void iso15693_signal_tx(
    Iso15693Signal* instance,
    Iso15693SignalDataRate data_rate,
    const uint8_t* tx_data,
    size_t tx_data_size) {
    furi_assert(instance);
    furi_assert(data_rate < Iso15693SignalDataRateNum);
    furi_assert(tx_data);
    furi_assert(tx_data_size / BITS_IN_BYTE);

    FURI_CRITICAL_ENTER();
    iso15693_signal_encode(instance, data_rate, tx_data, tx_data_size);
    digital_signal_send(instance->tx_signal, instance->pin);
    FURI_CRITICAL_EXIT();
}
