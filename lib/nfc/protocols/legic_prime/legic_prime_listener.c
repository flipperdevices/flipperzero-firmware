#include "legic_prime_listener.h"
#include "legic_prime_i.h"
#include "legic_prime_listener_i.h"
#include "legic_prime_listener_defs.h"
#include "legic_prime_prng.h"
#include "legic_prime_crc.h"

#include <furi.h>
#include <nfc/protocols/nfc_listener_base.h>
#include <toolbox/bit_buffer.h>
#include <lib/bit_lib/bit_lib.h>

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
// nfc_listener_tx() first waits this long after the reader's frame; the HAL
// itself starts the reply 330 µs after it, so one subcarrier period will do.
#define LEGIC_PRIME_LISTENER_FDT_FC         (64U)
// Write ACK: a single unencrypted '1' bit 3.57 ms after the reader's frame,
// as legicrfsim.c tx_ack (TAG_ACK_WAIT), which steps the keystream 35 times
// while waiting and once for the ACK bit.
#define LEGIC_PRIME_LISTENER_ACK_WAIT_CHIPS (758U)

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
    uint16_t mem_size;
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
    instance->mem_size = legic_prime_get_mem_size(data->type);

    nfc_set_fdt_listen_fc(instance->nfc, LEGIC_PRIME_LISTENER_FDT_FC);
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
    // No events: reader writes reach the app through the emulated data, as
    // for MIFARE Classic and Ultralight.
    UNUSED(instance);
    UNUSED(callback);
    UNUSED(context);
}

static const LegicPrimeData* legic_prime_listener_get_data(const LegicPrimeListener* instance) {
    furi_assert(instance);
    return instance->data;
}

// The first `bits` bits of a reader frame, LSB first (at most 24).
static uint32_t legic_prime_listener_raw_value(const BitBuffer* rx_buffer, size_t bits) {
    const uint32_t v = bit_lib_bytes_to_num_le(bit_buffer_get_data(rx_buffer), (bits + 7) / 8);
    return v & ((1UL << bits) - 1);
}

static void legic_prime_listener_reply(LegicPrimeListener* instance, uint32_t value, size_t bits) {
    legic_prime_prng_forward(LEGIC_PRIME_REPLY_STEPS);
    bit_buffer_reset(instance->tx_buffer);
    for(size_t i = 0; i < bits; i++) {
        bit_buffer_append_bit(
            instance->tx_buffer, ((value >> i) & 1u) ^ legic_prime_prng_get_bit());
        legic_prime_prng_forward(1);
    }
    nfc_listener_tx(instance->nfc, instance->tx_buffer);
    nfc_legic_listener_set_ref_to_tx_end(instance->nfc);
}

static bool
    legic_prime_listener_handle_iv(LegicPrimeListener* instance, const BitBuffer* rx, size_t bits) {
    if(bits != LEGIC_PRIME_IV_BITS) return false;
    const uint8_t iv = legic_prime_listener_raw_value(rx, LEGIC_PRIME_IV_BITS);
    legic_prime_prng_init(iv);
    legic_prime_listener_reply(instance, instance->type_code, LEGIC_PRIME_TYPE_BITS);
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
    // No reader frame is longer than a MIM1024 WRITE: a longer one is noise
    // and ends the session, as any other frame that fits no state does.
    if(bits > LEGIC_PRIME_LISTENER_MAX_BITS) {
        instance->state = LegicPrimeListenerStateIdle;
        return NfcCommandContinue;
    }

    uint32_t steps = 0;
    if(instance->state != LegicPrimeListenerStateIdle) {
        steps = legic_prime_listener_idle_steps(nfc_legic_listener_get_elapsed_fc(instance->nfc));
        if(steps == 0) instance->state = LegicPrimeListenerStateIdle;
    }

    if(instance->state == LegicPrimeListenerStateIdle) {
        legic_prime_listener_handle_iv(instance, rx, bits);
        return NfcCommandContinue;
    }

    legic_prime_prng_forward((int)steps);

    // Decrypt: one keystream bit per frame bit.
    uint32_t value = legic_prime_listener_raw_value(rx, bits);
    for(size_t i = 0; i < bits; i++) {
        value ^= (uint32_t)legic_prime_prng_get_bit() << i;
        legic_prime_prng_forward(1);
    }

    if(instance->state == LegicPrimeListenerStateWaitAck) {
        if(bits == LEGIC_PRIME_ACK_BITS && value == instance->ack_code) {
            instance->state = LegicPrimeListenerStateConnected;
            // Slots count from one slot after the ACK, as legicrfsim.c init_card.
            nfc_legic_listener_set_ref_to_frame_end(instance->nfc, LEGIC_PRIME_LISTENER_BIT_FC);
        } else {
            instance->state = LegicPrimeListenerStateIdle;
            legic_prime_listener_handle_iv(instance, rx, bits);
        }
        return NfcCommandContinue;
    }

    // Connected: READ = cmd_size bits with bit 0 set.
    if(bits == instance->cmd_size_bits && (value & 1u)) {
        // Only a MIM22 has addresses past its memory (22-31).  They answer
        // 0x00 with a valid CRC, as legicrfsim.c answers any address from its
        // emulator memory: a READ has no error reply, and what a real MIM22
        // sends there is unknown.
        const uint16_t addr = value >> 1;
        const uint8_t byte = addr < instance->mem_size ? instance->data->data[addr] : 0;
        const uint8_t crc = legic_prime_crc4(value, instance->cmd_size_bits, byte);
        legic_prime_listener_reply(
            instance, ((uint32_t)crc << 8) | byte, LEGIC_PRIME_READ_REPLY_BITS);
    } else if(bits == instance->cmd_size_bits) {
        // Write header without data: nothing to answer.
        nfc_legic_listener_set_ref_to_frame_end(instance->nfc, 0);
    } else if(bits == LEGIC_PRIME_WRITE_FRAME_BITS(instance->cmd_size_bits) && !(value & 1u)) {
        // WRITE: address | data byte | CRC-4 over both.
        const uint32_t addr_mask = (1u << (instance->cmd_size_bits - 1)) - 1;
        const uint16_t addr = (value >> 1) & addr_mask;
        const uint8_t byte = (value >> instance->cmd_size_bits) & 0xFF;
        const uint8_t crc = (value >> (instance->cmd_size_bits + 8U)) &
                            ((1U << LEGIC_PRIME_CRC4_BITS) - 1);
        // Writes to the DCF (5-6) and bytes 7-8 are accepted as a real tag
        // accepts them: that is how an encoder turns new media into an IM or
        // IM-S card (legicrfsim.c even accepts 0-4).  The DCF's decrement-only
        // rule is not emulated.  A write only changes this emulated copy,
        // which the app keeps as a shadow file that "Restore to Original
        // State" undoes; guarding the DCF of a physical card is the reader
        // client's policy, as in Proxmark3.
        if(crc == legic_prime_crc4(addr << 1, instance->cmd_size_bits, byte) &&
           addr >= LEGIC_PRIME_WRITABLE_FROM && addr < instance->mem_size) {
            instance->data->data[addr] = byte;
            legic_prime_prng_forward(
                LEGIC_PRIME_LISTENER_ACK_WAIT_CHIPS / LEGIC_PRIME_LISTENER_BIT_CHIPS - 1);
            bit_buffer_reset(instance->tx_buffer);
            bit_buffer_append_bit(instance->tx_buffer, true);
            legic_prime_prng_forward(1);
            nfc_legic_listener_set_tx_delay(instance->nfc, LEGIC_PRIME_LISTENER_ACK_WAIT_CHIPS);
            nfc_listener_tx(instance->nfc, instance->tx_buffer);
            nfc_legic_listener_set_ref_to_tx_end(instance->nfc);
        } else {
            nfc_legic_listener_set_ref_to_frame_end(instance->nfc, 0);
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
