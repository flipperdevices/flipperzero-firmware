#pragma once

#include <furi_hal_gpio.h>
#include <digital_signal/digital_signal.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DigitalSequence DigitalSequence;

DigitalSequence* digital_sequence_alloc(uint32_t size, const GpioPin* gpio);

void digital_sequence_free(DigitalSequence* sequence);

void digital_sequence_set_signal(
    DigitalSequence* sequence,
    uint8_t signal_index,
    DigitalSignal* signal);

void digital_sequence_add_signal(DigitalSequence* sequence, uint8_t signal_index);

void digital_sequence_send(DigitalSequence* sequence);

void digital_sequence_clear(DigitalSequence* sequence);

#ifdef __cplusplus
}
#endif
