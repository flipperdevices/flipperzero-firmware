#include "legic_prime_listener.h"
#include "legic_prime_i.h"
#include "legic_prime_listener_i.h"
#include "legic_prime_listener_defs.h"
#include "legic_prime_prng.h"
#include "legic_prime_crc.h"

#include <furi.h>
#include <furi_hal_nfc.h>
#include <nfc/protocols/nfc_listener_base.h>
#include <toolbox/bit_buffer.h>

#define TAG "LegicPrimeListener"

/*
 * LEGIC Prime tag emulation, mirroring Proxmark armsrc/legicrfsim.c.
 *
 * The tag steps its keystream once per 99.1 µs slot while waiting for the
 * reader, so every received frame first forwards the PRNG by the number of
 * slots since the reference point (end of our last reply, or one slot after
 * the reader's ACK), see legic_prime_listener_idle_steps().  The HAL keeps
 * the reference point and measures the time since it.
 */
#define LEGIC_PRIME_LISTENER_MAX_BITS       (24U)
// Write ACK: a single unencrypted '1' bit 3.57 ms after the reader's frame,
// as legicrfsim.c tx_ack (TAG_ACK_WAIT), which steps the keystream 35 times
// while waiting and once for the ACK bit.
#define LEGIC_PRIME_LISTENER_ACK_WAIT_CHIPS (758U)
// Bytes 0-4 (MCD, MSN, MCC) can't be written.
#define LEGIC_PRIME_LISTENER_WRITABLE_FROM  (5U)

typedef enum {
    LegicPrimeListenerStateIdle, // waiting for IV
    LegicPrimeListenerStateWaitAck,
    LegicPrimeListenerStateConnected,
} LegicPrimeListenerState;

struct LegicPrimeListener {
    Nfc* nfc;
    LegicPrimeData* data;
    BitBuffer* tx_buffer;
    LegicPrimeListenerState state;
    uint8_t cmd_size_bits;
    uint8_t type_code;
    uint8_t ack_code;

    NfcGenericEvent generic_event;
    NfcGenericCallback callback;
    void* context;
};

static LegicPrimeListener* legic_prime_listener_alloc(Nfc* nfc, LegicPrimeData* data) {
    furi_assert(nfc);
    furi_assert(data);

    LegicPrimeListener* instance = malloc(sizeof(LegicPrimeListener));
    instance->nfc = nfc;
    instance->data = data;
    instance->tx_buffer = bit_buffer_alloc(LEGIC_PRIME_LISTENER_MAX_BITS / 8 + 1);
    instance->state = LegicPrimeListenerStateIdle;

    const LegicPrimeProtocolParams* params = legic_prime_get_protocol_params(data->type);
    instance->cmd_size_bits = params->cmd_size_bits;
    instance->type_code = params->type_code;
    instance->ack_code = params->ack_code;

    instance->generic_event.protocol = NfcProtocolLegicPrime;
    instance->generic_event.instance = instance;
    instance->generic_event.event_data = NULL;

    // The HAL schedules replies itself, 330 µs after the reader's frame.
    nfc_set_fdt_listen_fc(instance->nfc, 64);
    nfc_config(instance->nfc, NfcModeListener, NfcTechLegic);
    return instance;
}

static void legic_prime_listener_free(LegicPrimeListener* instance) {
    furi_assert(instance);
    bit_buffer_free(instance->tx_buffer);
    free(instance);
}

static void legic_prime_listener_set_callback(
    LegicPrimeListener* instance,
    NfcGenericCallback callback,
    void* context) {
    furi_assert(instance);
    instance->callback = callback;
    instance->context = context;
}

static const LegicPrimeData* legic_prime_listener_get_data(const LegicPrimeListener* instance) {
    furi_assert(instance);
    return instance->data;
}

static uint32_t legic_prime_listener_raw_value(const BitBuffer* rx_buffer, size_t bits) {
    uint32_t v = 0;
    for(size_t i = 0; i < bits; i++) {
        v |= (uint32_t)((bit_buffer_get_byte(rx_buffer, i / 8) >> (i % 8)) & 1u) << i;
    }
    return v;
}

static void legic_prime_listener_reply(LegicPrimeListener* instance, uint32_t value, size_t bits) {
    // The tag steps its PRNG twice between the reader frame and its reply.
    legic_prime_prng_forward(2);
    bit_buffer_reset(instance->tx_buffer);
    for(size_t i = 0; i < bits; i++) {
        bit_buffer_append_bit(
            instance->tx_buffer, ((value >> i) & 1u) ^ legic_prime_prng_get_bit());
        legic_prime_prng_forward(1);
    }
    nfc_listener_tx(instance->nfc, instance->tx_buffer);
    furi_hal_nfc_legic_listener_set_ref_to_tx_end();
}

static bool
    legic_prime_listener_handle_iv(LegicPrimeListener* instance, const BitBuffer* rx, size_t bits) {
    if(bits != 7) return false;
    const uint8_t iv = legic_prime_listener_raw_value(rx, 7);
    legic_prime_prng_init(iv);
    legic_prime_listener_reply(instance, instance->type_code, 6);
    instance->state = LegicPrimeListenerStateWaitAck;
    return true;
}

static NfcCommand legic_prime_listener_run(NfcGenericEvent event, void* context) {
    furi_assert(context);
    furi_assert(event.event_data);

    LegicPrimeListener* instance = context;
    NfcEvent* nfc_event = event.event_data;
    if(nfc_event->type != NfcEventTypeRxEnd) return NfcCommandContinue;

    const BitBuffer* rx = nfc_event->data.buffer;
    const size_t bits = bit_buffer_get_size(rx);

    uint32_t steps = 0;
    if(instance->state != LegicPrimeListenerStateIdle) {
        steps = legic_prime_listener_idle_steps(furi_hal_nfc_legic_listener_get_elapsed_fc());
        if(steps == 0) instance->state = LegicPrimeListenerStateIdle;
    }

    if(instance->state == LegicPrimeListenerStateIdle) {
        legic_prime_listener_handle_iv(instance, rx, bits);
        return NfcCommandContinue;
    }

    legic_prime_prng_forward((int)steps);

    uint32_t value = 0;
    for(size_t i = 0; i < bits; i++) {
        value |= (uint32_t)(((bit_buffer_get_byte(rx, i / 8) >> (i % 8)) & 1u) ^
                            legic_prime_prng_get_bit())
                 << i;
        legic_prime_prng_forward(1);
    }

    if(instance->state == LegicPrimeListenerStateWaitAck) {
        if(bits == 6 && value == instance->ack_code) {
            instance->state = LegicPrimeListenerStateConnected;
            // Slots count from one slot after the ACK, as legicrfsim.c init_card.
            furi_hal_nfc_legic_listener_set_ref_to_frame_end(LEGIC_PRIME_LISTENER_BIT_FC);
        } else {
            instance->state = LegicPrimeListenerStateIdle;
            legic_prime_listener_handle_iv(instance, rx, bits);
        }
        return NfcCommandContinue;
    }

    // Connected: READ = cmd_size bits with bit 0 set.
    if(bits == instance->cmd_size_bits && (value & 1u)) {
        const uint16_t addr = value >> 1;
        const uint8_t byte = addr < instance->data->data_size ? instance->data->data[addr] : 0;
        const uint8_t crc = legic_prime_crc4(value, instance->cmd_size_bits, byte);
        legic_prime_listener_reply(instance, ((uint32_t)crc << 8) | byte, 12);
    } else if(bits == instance->cmd_size_bits) {
        // Write header without data: nothing to answer.
        furi_hal_nfc_legic_listener_set_ref_to_frame_end(0);
    } else if(bits == instance->cmd_size_bits + 8U + 4U && !(value & 1u)) {
        // WRITE: address | data byte | CRC-4 over both.
        const uint32_t addr_mask = (1u << (instance->cmd_size_bits - 1)) - 1;
        const uint16_t addr = (value >> 1) & addr_mask;
        const uint8_t byte = (value >> instance->cmd_size_bits) & 0xFF;
        const uint8_t crc = (value >> (instance->cmd_size_bits + 8U)) & 0x0F;
        if(crc == legic_prime_crc4(addr << 1, instance->cmd_size_bits, byte) &&
           addr >= LEGIC_PRIME_LISTENER_WRITABLE_FROM && addr < instance->data->data_size) {
            instance->data->data[addr] = byte;
            legic_prime_prng_forward(
                LEGIC_PRIME_LISTENER_ACK_WAIT_CHIPS / LEGIC_PRIME_LISTENER_BIT_CHIPS - 1);
            bit_buffer_reset(instance->tx_buffer);
            bit_buffer_append_bit(instance->tx_buffer, true);
            legic_prime_prng_forward(1);
            furi_hal_nfc_legic_listener_set_tx_delay(LEGIC_PRIME_LISTENER_ACK_WAIT_CHIPS);
            nfc_listener_tx(instance->nfc, instance->tx_buffer);
            furi_hal_nfc_legic_listener_set_ref_to_tx_end();
        } else {
            furi_hal_nfc_legic_listener_set_ref_to_frame_end(0);
        }
    } else {
        instance->state = LegicPrimeListenerStateIdle;
        legic_prime_listener_handle_iv(instance, rx, bits);
    }

    return NfcCommandContinue;
}

const NfcListenerBase nfc_listener_legic_prime = {
    .alloc = (NfcListenerAlloc)legic_prime_listener_alloc,
    .free = (NfcListenerFree)legic_prime_listener_free,
    .set_callback = (NfcListenerSetCallback)legic_prime_listener_set_callback,
    .get_data = (NfcListenerGetData)legic_prime_listener_get_data,
    .run = (NfcListenerRun)legic_prime_listener_run,
};
