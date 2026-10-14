#include "legic_prime_poller_i.h"
#include "legic_prime_prng.h"
#include "legic_prime_crc.h"

#include <furi.h>
#include <furi_hal_cortex.h>
#include <furi_hal_nfc.h>

#define TAG "LegicPrimePoller"

static LegicPrimeError legic_prime_poller_map_nfc_error(NfcError err) {
    switch(err) {
    case NfcErrorNone:
        return LegicPrimeErrorNone;
    case NfcErrorTimeout:
        return LegicPrimeErrorTimeout;
    case NfcErrorIncompleteFrame:
        return LegicPrimeErrorCommunication;
    default:
        return LegicPrimeErrorNotPresent;
    }
}

/*
 * Keystream between frames.
 *
 * A LEGIC tag advances its keystream every 99.1 µs while idle (Proxmark
 * legicrfsim.c rx_frame).  If our next frame's first pause comes at t:
 *  - after a tag reply ending at E, the tag has advanced ceil((t - E) / 99.1 µs);
 *  - after our frame without a reply, ending at A, ceil((t - A) / 99.1 µs) - 1.
 * The HAL sends queued frames 260 µs after the tag's reply and chains the
 * first READ (or WRITE) 222 µs after the ACK (furi_hal_nfc_legic_poller_set_next),
 * so the steps are fixed: 3 after a reply and 2 after the ACK, as Proxmark's
 * reader with its 220 µs gaps.
 */
// Failed reads allowed per byte and session (CRC errors or disagreeing
// reads).  Each resumed session starts afresh, so a byte that ran out of
// attempts is not lost for good; the resume limit bounds the total.
#define LEGIC_PRIME_READ_ATTEMPTS          (6U)
// A read session is a single poller step, and nfc_poller_stop() waits for
// the step to end: a session stops after this many exchanges (~1.7 s) and
// the read continues in the next one.
#define LEGIC_PRIME_READ_SESSION_EXCHANGES (640U)

static uint8_t legic_prime_type_code(LegicPrimeType type) {
    return legic_prime_get_protocol_params(type)->type_code;
}

static uint8_t legic_prime_ack_code(LegicPrimeType type) {
    return legic_prime_get_protocol_params(type)->ack_code;
}

static void legic_prime_poller_set_type(LegicPrimePoller* instance, LegicPrimeType type) {
    instance->data->type = type;
    instance->data->data_size = legic_prime_get_mem_size(type);
    instance->cmd_size_bits = legic_prime_get_protocol_params(type)->cmd_size_bits;
}

static bool legic_prime_type_from_code(uint8_t code, LegicPrimeType* type) {
    for(LegicPrimeType t = 0; t < LegicPrimeTypeNum; t++) {
        if(legic_prime_type_code(t) == code) {
            *type = t;
            return true;
        }
    }
    return false;
}

/*
 * Build one reader frame on the running keystream: forward `pre_steps`
 * (the tag's idle steps since the previous frame or reply: 3 after a reply,
 * 2 after the ACK, for the ~220 µs gaps the HAL keeps), XOR the frame bits,
 * then, if a reply is expected, skip the tag's 2 steps and keep the reply's
 * keystream so the reply can be decoded after later frames were built.
 */
static void legic_prime_poller_build(
    LegicPrimeExchange* ex,
    uint32_t value,
    uint8_t bits,
    uint8_t pre_steps,
    uint8_t rx_bits) {
    legic_prime_prng_forward(pre_steps);
    ex->tx = 0;
    for(uint8_t i = 0; i < bits; i++) {
        ex->tx |= (uint32_t)(((value >> i) & 1u) ^ legic_prime_prng_get_bit()) << i;
        legic_prime_prng_forward(1);
    }
    ex->tx_bits = bits;
    ex->rx_bits = rx_bits;
    ex->ks = 0;
    if(rx_bits) {
        legic_prime_prng_forward(2);
        for(uint8_t i = 0; i < rx_bits; i++) {
            ex->ks |= (uint32_t)legic_prime_prng_get_bit() << i;
            legic_prime_prng_forward(1);
        }
    }
}

static void legic_prime_poller_frame_bytes(const LegicPrimeExchange* ex, uint8_t bytes[4]) {
    for(size_t i = 0; i < 4; i++) {
        bytes[i] = (ex->tx >> (8 * i)) & 0xFF;
    }
}

static void
    legic_prime_poller_queue(const LegicPrimeExchange* first, const LegicPrimeExchange* second) {
    uint8_t a[4];
    uint8_t b[4];
    legic_prime_poller_frame_bytes(first, a);
    if(second) legic_prime_poller_frame_bytes(second, b);
    furi_hal_nfc_legic_poller_set_next(
        a, first->tx_bits, second ? b : NULL, second ? second->tx_bits : 0);
}

// Send ex (or claim it if the HAL already sent it) and decode its reply.
static LegicPrimeError legic_prime_poller_exchange(
    LegicPrimePoller* instance,
    const LegicPrimeExchange* ex,
    uint32_t* reply) {
    bit_buffer_reset(instance->tx_buffer);
    for(uint8_t i = 0; i < ex->tx_bits; i++) {
        bit_buffer_append_bit(instance->tx_buffer, (ex->tx >> i) & 1u);
    }
    furi_hal_nfc_legic_poller_set_rx_bits(ex->rx_bits);

    NfcError nerr = nfc_poller_trx(
        instance->nfc, instance->tx_buffer, instance->rx_buffer, LEGIC_PRIME_FWT_FC);
    if(nerr != NfcErrorNone) return legic_prime_poller_map_nfc_error(nerr);

    uint32_t v = 0;
    const size_t rx_bits = bit_buffer_get_size(instance->rx_buffer);
    for(uint8_t i = 0; i < ex->rx_bits; i++) {
        bool bit = i < rx_bits &&
                   ((bit_buffer_get_byte(instance->rx_buffer, i / 8) >> (i % 8)) & 1u);
        v |= (uint32_t)bit << i;
    }
    *reply = v ^ ex->ks;
    return LegicPrimeErrorNone;
}

// The IV frame (plain) and the keystream of the tag's 6-bit type reply.
static void legic_prime_poller_build_iv(LegicPrimeExchange* ex, uint8_t iv) {
    ex->tx = iv;
    ex->tx_bits = 7;
    ex->rx_bits = 6;
    ex->ks = 0;
    legic_prime_prng_init(iv);
    legic_prime_prng_forward(2);
    for(uint8_t i = 0; i < 6; i++) {
        ex->ks |= (uint32_t)legic_prime_prng_get_bit() << i;
        legic_prime_prng_forward(1);
    }
}

// Detection: send the IV and decode the card type.  The tag is then waiting
// for an ACK, so reset the field before reading.
LegicPrimeError legic_prime_poller_setup(LegicPrimePoller* instance, uint8_t iv) {
    furi_assert(instance);

    // Hold the carrier briefly to power the tag (Proxmark uses ~5 ms).
    furi_delay_ms(5);

    LegicPrimeExchange iv_ex;
    legic_prime_poller_build_iv(&iv_ex, iv);
    uint32_t reply = 0;
    LegicPrimeError ret = legic_prime_poller_exchange(instance, &iv_ex, &reply);
    if(ret != LegicPrimeErrorNone) return ret;

    LegicPrimeType type;
    if(!legic_prime_type_from_code(reply & 0x3F, &type)) {
        FURI_LOG_E(TAG, "Unknown LEGIC type byte: 0x%02lX", reply & 0x3F);
        return LegicPrimeErrorUnknownType;
    }
    legic_prime_poller_set_type(instance, type);
    instance->data->iv = iv;
    return LegicPrimeErrorNone;
}

// A session's handshake answered with another card type than detection
// found.  The type codes differ by one bit, so detection may have misread
// it: as long as nothing was read or written, the next session uses this
// type.  After that the data proves the stored type and this handshake was
// misread.
static void legic_prime_poller_retype(LegicPrimePoller* instance, uint8_t code, bool proven) {
    LegicPrimeType type;
    if(proven || !legic_prime_type_from_code(code, &type)) return;
    legic_prime_poller_set_type(instance, type);
    memset(instance->seen, 0, sizeof(instance->seen));
}

// READ of byte `addr`; its 12-bit reply (data | CRC-4 << 8) is decoded with ex.
static void legic_prime_poller_build_read(
    LegicPrimePoller* instance,
    LegicPrimeExchange* ex,
    uint16_t addr,
    uint8_t pre_steps) {
    legic_prime_poller_build(ex, (addr << 1) | 1u, instance->cmd_size_bits, pre_steps, 12);
    ex->addr = addr;
}

static bool legic_prime_poller_read_crc_ok(
    const LegicPrimePoller* instance,
    const LegicPrimeExchange* ex,
    uint32_t reply) {
    return ((reply >> 8) & 0x0F) ==
           legic_prime_crc4((ex->addr << 1) | 1u, instance->cmd_size_bits, reply & 0xFF);
}

/*
 * Write: like Proxmark3 armsrc/legicrf.c write_byte.  The frame is the
 * address with bit 0 clear, the data byte and a CRC-4 over both.  The tag
 * answers with a single unencrypted '1' bit ~3.6 ms later; the reader listens
 * for 60 bit periods (6 ms) after the usual 330 µs.
 *
 * Bytes go from the last to the first, as Proxmark3 writes them (only then
 * is the DCF writable), and several in one session: the HAL sends the next
 * WRITE 260 µs after the tag's ACK.  That frame has to be built before the
 * ACK comes, and its keystream depends on the bit window k of the ACK: the
 * tag steps k + 6 times between one WRITE and the next (Proxmark3's reader
 * steps 3 after its frame, once per window it samples and 2 before the next
 * frame; its legicrfsim.c ACKs in window 33).  The window is predicted from
 * the previous ACK and the HAL drops the frame if the ACK comes in another
 * one; the session then ends and the next one uses the measured window.
 *
 * Every session first reads the UID (bytes 0-3) and compares it with the
 * first session's, so that a card swapped in during a field reset doesn't
 * get the remaining bytes.  Each byte needs two agreeing reads: a session
 * always starts at the same keystream position, and a reply that misdecodes
 * there would otherwise fail (or fool) the check every time.
 */
#define LEGIC_PRIME_WRITE_ACK_WINDOW_BITS (60U)
#define LEGIC_PRIME_WRITE_FWT_FC          (95000U) // ≈ 7 ms
#define LEGIC_PRIME_WRITE_STEPS_AFTER_ACK (6U)
#define LEGIC_PRIME_WRITE_UID_MAX_READS   (16U)

// Build the WRITE of the byte that comes after `done` written bytes.
static void legic_prime_poller_build_write(
    LegicPrimePoller* instance,
    LegicPrimeExchange* ex,
    uint16_t done,
    uint8_t pre_steps) {
    const uint16_t index = instance->write_size - 1 - done;
    const uint8_t cmd_bits = instance->cmd_size_bits;
    const uint8_t value = instance->write_data[index];
    const uint32_t cmd = (uint32_t)(instance->write_offset + index) << 1;
    const uint8_t crc = legic_prime_crc4(cmd, cmd_bits, value);
    const uint32_t frame = cmd | ((uint32_t)value << cmd_bits) | ((uint32_t)crc << (cmd_bits + 8));
    legic_prime_poller_build(ex, frame, cmd_bits + 12, pre_steps, 0);
    ex->addr = instance->write_offset + index;
}

// Claim ex (already sent) and wait for the tag's ACK; returns its bit window.
static LegicPrimeError legic_prime_poller_write_exchange(
    LegicPrimePoller* instance,
    const LegicPrimeExchange* ex,
    uint8_t next_window,
    uint8_t* ack_window) {
    bit_buffer_reset(instance->tx_buffer);
    for(uint8_t i = 0; i < ex->tx_bits; i++) {
        bit_buffer_append_bit(instance->tx_buffer, (ex->tx >> i) & 1u);
    }
    furi_hal_nfc_legic_poller_set_rx_bits(LEGIC_PRIME_WRITE_ACK_WINDOW_BITS);
    furi_hal_nfc_legic_poller_set_rx_ack(next_window);
    NfcError nerr = nfc_poller_trx(
        instance->nfc, instance->tx_buffer, instance->rx_buffer, LEGIC_PRIME_WRITE_FWT_FC);
    if(nerr == NfcErrorTimeout) return LegicPrimeErrorNoAck;
    if(nerr != NfcErrorNone) return legic_prime_poller_map_nfc_error(nerr);

    const size_t rx_bits = bit_buffer_get_size(instance->rx_buffer);
    for(size_t i = 0; i < rx_bits; i++) {
        if((bit_buffer_get_byte(instance->rx_buffer, i / 8) >> (i % 8)) & 1u) {
            *ack_window = i;
            return LegicPrimeErrorNone;
        }
    }
    return LegicPrimeErrorNoAck;
}

LegicPrimeError legic_prime_poller_write_session(LegicPrimePoller* instance) {
    furi_assert(instance);
    furi_assert(instance->write_pos < instance->write_size);
    // The type (and size) may have changed since the range was checked.
    if(instance->write_offset + instance->write_size > instance->data->data_size) {
        return LegicPrimeErrorReadOnly;
    }

    furi_delay_ms(5);

    const LegicPrimeType type = instance->data->type;
    LegicPrimeExchange iv_ex, ack, cur, next;
    legic_prime_prng_init(0);
    legic_prime_poller_build_iv(&iv_ex, instance->data->iv);
    legic_prime_poller_build(&ack, legic_prime_ack_code(type), 6, 3, 0);
    legic_prime_poller_build_read(instance, &cur, 0, 2);
    legic_prime_poller_queue(&ack, &cur);

    uint32_t reply = 0;
    LegicPrimeError ret = legic_prime_poller_exchange(instance, &iv_ex, &reply);
    if(ret != LegicPrimeErrorNone) return ret;
    if((reply & 0x3F) != legic_prime_type_code(type)) {
        legic_prime_poller_retype(
            instance, reply & 0x3F, instance->write_pos || instance->write_uid_known);
        return LegicPrimeErrorUnknownType;
    }

    // UID bytes: two agreeing reads with a valid CRC each, as in
    // legic_prime_poller_read().  A READ is queued before the previous reply
    // is decoded, so the first WRITE follows one READ after the last byte.
    uint8_t uid[LEGIC_PRIME_UID_SIZE];
    uint8_t seen = 0, confirmed = 0;
    const uint8_t uid_all = (1u << LEGIC_PRIME_UID_SIZE) - 1;
    bool write_queued = false;
    for(size_t reads = 0; !write_queued; reads++) {
        if(!furi_hal_nfc_legic_poller_next_sent()) return LegicPrimeErrorTimeout;
        if(confirmed == uid_all) {
            if(!instance->write_uid_known) {
                memcpy(instance->write_uid, uid, sizeof(uid));
                instance->write_uid_known = true;
            } else if(memcmp(instance->write_uid, uid, sizeof(uid)) != 0) {
                return LegicPrimeErrorWrongCard;
            }
            legic_prime_poller_build_write(instance, &next, instance->write_pos, 3);
            write_queued = true;
        } else if(reads == LEGIC_PRIME_WRITE_UID_MAX_READS) {
            return LegicPrimeErrorWrongCrc;
        } else {
            // The next unconfirmed byte after cur's, cur's own last.
            uint16_t addr = cur.addr;
            for(uint16_t i = 1; i <= LEGIC_PRIME_UID_SIZE; i++) {
                const uint16_t a = (cur.addr + i) % LEGIC_PRIME_UID_SIZE;
                if(!(confirmed & (1u << a))) {
                    addr = a;
                    break;
                }
            }
            legic_prime_poller_build_read(instance, &next, addr, 3);
        }
        legic_prime_poller_queue(&next, NULL);

        ret = legic_prime_poller_exchange(instance, &cur, &reply);
        if(ret != LegicPrimeErrorNone) return ret;
        const uint8_t bit = 1u << cur.addr;
        if(legic_prime_poller_read_crc_ok(instance, &cur, reply)) {
            if((seen & bit) && uid[cur.addr] == (reply & 0xFF)) {
                confirmed |= bit;
            } else {
                uid[cur.addr] = reply & 0xFF;
                seen |= bit;
            }
        }
        cur = next;
    }
    if(!furi_hal_nfc_legic_poller_next_sent()) return LegicPrimeErrorTimeout;

    while(true) {
        const uint16_t done = instance->write_pos;
        const bool have_next = done + 1 < instance->write_size;
        const uint8_t predicted = instance->write_ack_window;
        if(have_next) {
            legic_prime_poller_build_write(
                instance, &next, done + 1, predicted + LEGIC_PRIME_WRITE_STEPS_AFTER_ACK);
            legic_prime_poller_queue(&next, NULL);
        }

        uint8_t window = 0;
        ret = legic_prime_poller_write_exchange(instance, &cur, predicted, &window);
        if(ret != LegicPrimeErrorNone) return ret;

        instance->data->data[cur.addr] = instance->write_data[cur.addr - instance->write_offset];
        instance->write_pos = done + 1;
        if(!have_next) return LegicPrimeErrorNone;
        if(!furi_hal_nfc_legic_poller_next_sent()) {
            // The ACK came in another window than predicted (or the frame
            // would have been late): continue in a new session.
            instance->write_ack_window = window;
            return LegicPrimeErrorNone;
        }
        cur = next;
    }
}

static bool legic_prime_poller_is_done(const LegicPrimePoller* instance, uint16_t addr) {
    return instance->done[addr / 8] & (1u << (addr % 8));
}

// Next address to read after `after`, cycling, `after` itself last.
static bool
    legic_prime_poller_next_addr(LegicPrimePoller* instance, uint16_t after, uint16_t* addr) {
    const uint16_t size = instance->data->data_size;
    for(uint16_t i = 1; i <= size; i++) {
        const uint16_t a = (after + i) % size;
        if(!legic_prime_poller_is_done(instance, a) &&
           instance->attempts[a] < LEGIC_PRIME_READ_ATTEMPTS) {
            *addr = a;
            return true;
        }
    }
    return false;
}

/*
 * One read session, mirroring Proxmark armsrc/legicrf.c but pipelined: the
 * ACK and the first READ go out chained behind the type reply, and every
 * further READ is queued before the previous reply is decoded, so each frame
 * follows the tag's reply by ~220 µs as real tags require.  Returns
 * LegicPrimeErrorNone once every byte was read.
 */
LegicPrimeError legic_prime_poller_read(LegicPrimePoller* instance) {
    furi_assert(instance);
    furi_assert(instance->data->data_size <= LEGIC_PRIME_MEM_MAX);

    memset(instance->attempts, 0, sizeof(instance->attempts));
    uint16_t addr;
    if(!legic_prime_poller_next_addr(instance, instance->data->data_size - 1, &addr)) {
        return instance->done_count == instance->data->data_size ? LegicPrimeErrorNone :
                                                                   LegicPrimeErrorWrongCrc;
    }

    furi_delay_ms(5);

    const LegicPrimeType type = instance->data->type;
    LegicPrimeExchange iv_ex, ack, cur, next;
    legic_prime_prng_init(0);
    legic_prime_poller_build_iv(&iv_ex, instance->data->iv);
    legic_prime_poller_build(&ack, legic_prime_ack_code(type), 6, 3, 0);
    legic_prime_poller_build_read(instance, &cur, addr, 2);
    legic_prime_poller_queue(&ack, &cur);

    uint32_t reply = 0;
    LegicPrimeError ret = legic_prime_poller_exchange(instance, &iv_ex, &reply);
    if(ret != LegicPrimeErrorNone) return ret;
    if((reply & 0x3F) != legic_prime_type_code(type)) {
        // ACK and READ were built for the stored type: end the session.
        legic_prime_poller_retype(instance, reply & 0x3F, instance->done_count);
        return LegicPrimeErrorUnknownType;
    }

    const uint32_t budget_max =
        MIN(instance->data->data_size * LEGIC_PRIME_READ_ATTEMPTS + 16U,
            LEGIC_PRIME_READ_SESSION_EXCHANGES);
    for(uint32_t budget = budget_max; budget; budget--) {
        if(!furi_hal_nfc_legic_poller_next_sent()) return LegicPrimeErrorTimeout;

        uint16_t next_addr;
        const bool have_next = legic_prime_poller_next_addr(instance, cur.addr, &next_addr);
        if(have_next) {
            legic_prime_poller_build_read(instance, &next, next_addr, 3);
            legic_prime_poller_queue(&next, NULL);
        }

        ret = legic_prime_poller_exchange(instance, &cur, &reply);
        if(ret != LegicPrimeErrorNone) return ret;

        // A 4-bit CRC passes 1 in 16 misdecoded replies, so a byte is only
        // accepted once two reads (with different keystreams) agree.
        const uint8_t data = reply & 0xFF;
        const uint8_t bit = 1u << (cur.addr % 8);
        if(legic_prime_poller_is_done(instance, cur.addr)) {
            // Redundant read of an already confirmed byte.
        } else if(legic_prime_poller_read_crc_ok(instance, &cur, reply)) {
            if((instance->seen[cur.addr / 8] & bit) && instance->data->data[cur.addr] == data) {
                instance->done[cur.addr / 8] |= bit;
                instance->done_count++;
            } else {
                if(instance->seen[cur.addr / 8] & bit) instance->attempts[cur.addr]++;
                instance->data->data[cur.addr] = data;
                instance->seen[cur.addr / 8] |= bit;
            }
        } else {
            // No logging here: the next frame is already on air.
            instance->attempts[cur.addr]++;
        }

        if(instance->done_count == instance->data->data_size) return LegicPrimeErrorNone;
        if(!have_next) break;
        cur = next;
    }

    return LegicPrimeErrorWrongCrc;
}
