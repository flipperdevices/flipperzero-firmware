#include "legic_prime_poller_i.h"
#include "legic_prime_prng.h"
#include "legic_prime_crc.h"

#include <furi.h>
#include <furi_hal_nfc.h>
#include <lib/bit_lib/bit_lib.h>

#define TAG "LegicPrimePoller"

static LegicPrimeError legic_prime_poller_map_nfc_error(NfcError err) {
    switch(err) {
    case NfcErrorNone:
        return LegicPrimeErrorNone;
    case NfcErrorTimeout:
        return LegicPrimeErrorTimeout;
    default:
        return LegicPrimeErrorCommunication;
    }
}

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
    legic_prime_set_type(instance->data, type);
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

// The next `bits` keystream bits, LSB first.
static uint32_t legic_prime_poller_keystream(uint8_t bits) {
    uint32_t ks = 0;
    for(uint8_t i = 0; i < bits; i++) {
        ks |= (uint32_t)legic_prime_prng_get_bit() << i;
        legic_prime_prng_forward(1);
    }
    return ks;
}

void legic_prime_poller_build(
    LegicPrimeExchange* ex,
    uint32_t value,
    uint8_t bits,
    uint8_t pre_steps,
    uint8_t rx_bits) {
    legic_prime_prng_forward(pre_steps);
    ex->tx = (value & ((1UL << bits) - 1)) ^ legic_prime_poller_keystream(bits);
    ex->tx_bits = bits;
    ex->rx_bits = rx_bits;
    ex->ks = 0;
    if(rx_bits) {
        legic_prime_prng_forward(LEGIC_PRIME_REPLY_STEPS);
        ex->ks = legic_prime_poller_keystream(rx_bits);
    }
}

void legic_prime_poller_build_iv(LegicPrimeExchange* ex, uint8_t iv) {
    ex->tx = iv;
    ex->tx_bits = LEGIC_PRIME_IV_BITS;
    ex->rx_bits = LEGIC_PRIME_TYPE_BITS;
    legic_prime_prng_init(iv);
    legic_prime_prng_forward(LEGIC_PRIME_REPLY_STEPS);
    ex->ks = legic_prime_poller_keystream(LEGIC_PRIME_TYPE_BITS);
}

static void
    legic_prime_poller_queue(const LegicPrimeExchange* first, const LegicPrimeExchange* second) {
    uint8_t a[sizeof(first->tx)];
    uint8_t b[sizeof(first->tx)];
    bit_lib_num_to_bytes_le(first->tx, sizeof(a), a);
    if(second) bit_lib_num_to_bytes_le(second->tx, sizeof(b), b);
    furi_hal_nfc_legic_poller_set_next(
        a, first->tx_bits, second ? b : NULL, second ? second->tx_bits : 0);
}

static void legic_prime_poller_load_tx(LegicPrimePoller* instance, const LegicPrimeExchange* ex) {
    uint8_t bytes[sizeof(ex->tx)];
    bit_lib_num_to_bytes_le(ex->tx, sizeof(bytes), bytes);
    bit_buffer_copy_bits(instance->tx_buffer, bytes, ex->tx_bits);
}

// Send ex (or claim it if the HAL already sent it) and decode its reply.
static LegicPrimeError legic_prime_poller_exchange(
    LegicPrimePoller* instance,
    const LegicPrimeExchange* ex,
    uint32_t* reply) {
    legic_prime_poller_load_tx(instance, ex);
    furi_hal_nfc_legic_poller_set_rx_bits(ex->rx_bits);

    NfcError nerr = nfc_poller_trx(
        instance->nfc, instance->tx_buffer, instance->rx_buffer, LEGIC_PRIME_FWT_FC);
    if(nerr != NfcErrorNone) return legic_prime_poller_map_nfc_error(nerr);

    // Bits the tag didn't send read as 0.
    const size_t bits = MIN(bit_buffer_get_size(instance->rx_buffer), (size_t)ex->rx_bits);
    const uint64_t raw =
        bit_lib_bytes_to_num_le(bit_buffer_get_data(instance->rx_buffer), (bits + 7) / 8);
    *reply = (uint32_t)(raw & ((1ULL << bits) - 1)) ^ ex->ks;
    return LegicPrimeErrorNone;
}

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
    if(!legic_prime_type_from_code(reply & LEGIC_PRIME_TYPE_MASK, &type)) {
        FURI_LOG_D(TAG, "Unknown LEGIC type byte: 0x%02lX", reply & LEGIC_PRIME_TYPE_MASK);
        return LegicPrimeErrorUnknownType;
    }
    legic_prime_poller_set_type(instance, type);
    instance->iv = iv;
    return LegicPrimeErrorNone;
}

// A session's handshake answered with another card type than detection
// found.  The type codes differ by one bit, so detection may have misread
// it: until a session confirmed the UID, the next session uses this type.
// After that the UID reads prove the stored type and this handshake was
// misread.
static void legic_prime_poller_retype(LegicPrimePoller* instance, uint8_t code) {
    LegicPrimeType type;
    if(instance->uid_known || !legic_prime_type_from_code(code, &type)) return;
    legic_prime_poller_set_type(instance, type);
    // Bytes read with the misread type are no use for the new one.
    if(instance->state == LegicPrimePollerStateRead) legic_prime_poller_read_reset(instance);
}

// READ of byte `addr`; its 12-bit reply (data | CRC-4 << 8) is decoded with ex.
static void legic_prime_poller_build_read(
    LegicPrimePoller* instance,
    LegicPrimeExchange* ex,
    uint16_t addr,
    uint8_t pre_steps) {
    legic_prime_poller_build(
        ex, (addr << 1) | 1u, instance->cmd_size_bits, pre_steps, LEGIC_PRIME_READ_REPLY_BITS);
    ex->addr = addr;
}

static bool legic_prime_poller_read_crc_ok(
    const LegicPrimePoller* instance,
    const LegicPrimeExchange* ex,
    uint32_t reply) {
    return ((reply >> 8) & 0x0F) ==
           legic_prime_crc4((ex->addr << 1) | 1u, instance->cmd_size_bits, reply & 0xFF);
}

typedef enum {
    LegicPrimePollerReadFirst, // the byte's first read: kept
    LegicPrimePollerReadAgreed, // agrees with the kept read: the byte is confirmed
    LegicPrimePollerReadDisagreed, // differs from the kept read and replaces it
} LegicPrimePollerRead;

// A 4-bit CRC passes 1 in 16 misdecoded replies, so a byte is only accepted
// once two CRC-valid reads agree.  Records one such read of a byte that is
// not confirmed yet, in its progress flags and its kept value.
static LegicPrimePollerRead
    legic_prime_poller_record_read(uint8_t* progress, uint8_t* kept, uint8_t value) {
    if(!(*progress & LEGIC_PRIME_POLLER_SEEN)) {
        *progress |= LEGIC_PRIME_POLLER_SEEN;
        *kept = value;
        return LegicPrimePollerReadFirst;
    }
    if(*kept == value) {
        *progress |= LEGIC_PRIME_POLLER_DONE;
        return LegicPrimePollerReadAgreed;
    }
    *kept = value;
    return LegicPrimePollerReadDisagreed;
}

/*
 * Every session reads the UID (bytes 0-3) first and compares it with the
 * first session's, so that a card swapped in during a field reset neither
 * gets the rest of a write nor completes another card's dump.  Each byte
 * needs two agreeing reads: a session always starts at the same keystream
 * position, and a reply that misdecodes there would otherwise fail (or fool)
 * the check every time.
 */
#define LEGIC_PRIME_UID_MAX_READS (16U)

// On entry *cur is the READ of byte 0, sent behind the handshake's ACK.
// Returns with *cur, a READ of a UID byte, sent but not claimed and nothing
// queued behind it: the caller queues its next frame, then claims *cur.
static LegicPrimeError
    legic_prime_poller_confirm_uid(LegicPrimePoller* instance, LegicPrimeExchange* cur) {
    uint8_t uid[LEGIC_PRIME_UID_SIZE];
    uint8_t progress[LEGIC_PRIME_UID_SIZE] = {0};
    for(size_t reads = 0;; reads++) {
        if(!furi_hal_nfc_legic_poller_next_sent()) return LegicPrimeErrorTimeout;
        // The next unconfirmed byte after cur's, cur's own last.
        uint16_t addr = cur->addr;
        bool confirmed = true;
        for(uint16_t i = 1; confirmed && i <= LEGIC_PRIME_UID_SIZE; i++) {
            addr = (cur->addr + i) % LEGIC_PRIME_UID_SIZE;
            confirmed = progress[addr] & LEGIC_PRIME_POLLER_DONE;
        }
        if(confirmed) break;
        if(reads == LEGIC_PRIME_UID_MAX_READS) return LegicPrimeErrorUidUnconfirmed;

        LegicPrimeExchange next;
        legic_prime_poller_build_read(instance, &next, addr, LEGIC_PRIME_STEPS_AFTER_REPLY);
        legic_prime_poller_queue(&next, NULL);

        uint32_t reply = 0;
        const LegicPrimeError ret = legic_prime_poller_exchange(instance, cur, &reply);
        if(ret != LegicPrimeErrorNone) return ret;
        if(legic_prime_poller_read_crc_ok(instance, cur, reply)) {
            legic_prime_poller_record_read(&progress[cur->addr], &uid[cur->addr], reply & 0xFF);
        }
        *cur = next;
    }

    if(!instance->uid_known) {
        memcpy(instance->uid, uid, sizeof(uid));
        instance->uid_known = true;
    } else if(memcmp(instance->uid, uid, sizeof(uid)) != 0) {
        return LegicPrimeErrorWrongCard;
    }
    return LegicPrimeErrorNone;
}

// Starts a session from a fresh field: the handshake, with the ACK and the
// first UID READ chained behind the type reply, then the UID check.  Returns
// as legic_prime_poller_confirm_uid() does.
static LegicPrimeError
    legic_prime_poller_start_session(LegicPrimePoller* instance, LegicPrimeExchange* cur) {
    // Hold the carrier briefly to power the tag, as for the detection.
    furi_delay_ms(5);

    const LegicPrimeType type = instance->data->type;
    LegicPrimeExchange iv_ex, ack;
    legic_prime_poller_build_iv(&iv_ex, instance->iv);
    legic_prime_poller_build(
        &ack, legic_prime_ack_code(type), LEGIC_PRIME_ACK_BITS, LEGIC_PRIME_STEPS_AFTER_REPLY, 0);
    legic_prime_poller_build_read(instance, cur, 0, LEGIC_PRIME_STEPS_AFTER_READER_ACK);
    legic_prime_poller_queue(&ack, cur);

    uint32_t reply = 0;
    LegicPrimeError ret = legic_prime_poller_exchange(instance, &iv_ex, &reply);
    if(ret != LegicPrimeErrorNone) return ret;
    if((reply & LEGIC_PRIME_TYPE_MASK) != legic_prime_type_code(type)) {
        // ACK and READ were built for the stored type: end the session.
        legic_prime_poller_retype(instance, reply & LEGIC_PRIME_TYPE_MASK);
        return LegicPrimeErrorUnknownType;
    }
    return legic_prime_poller_confirm_uid(instance, cur);
}

/*
 * Write: like Proxmark3 armsrc/legicrf.c write_byte.  The frame is the
 * address with bit 0 clear, the data byte and a CRC-4 over both.  The tag
 * answers with a single unencrypted '1' bit ~3.6 ms later; the reader listens
 * for 60 bit periods (6 ms) after the usual 330 µs.
 *
 * Bytes go from the last to the first, as Proxmark3 writes them (only then
 * is the DCF writable), and several in one session: the HAL sends the next
 * WRITE 240 µs after the measured end of the tag's ACK.  That frame has to
 * be built before the ACK comes, and its keystream depends on the bit window
 * k of the ACK: the tag steps k + 6 times between one WRITE and the next
 * (Proxmark3's reader steps 3 after its frame, once per window it samples
 * and 2 before the next frame; its legicrfsim.c ACKs in window 33).  The
 * window is predicted from the previous ACK and the HAL drops the frame if
 * the ACK comes in another one; the session then ends and the next one uses
 * the measured window.
 */
#define LEGIC_PRIME_WRITE_ACK_WINDOW_BITS (60U)
#define LEGIC_PRIME_WRITE_FWT_FC          (95000U) // ≈ 7 ms

uint32_t legic_prime_poller_write_frame(uint16_t addr, uint8_t value, uint8_t cmd_bits) {
    const uint32_t cmd = (uint32_t)addr << 1;
    const uint8_t crc = legic_prime_crc4(cmd, cmd_bits, value);
    return cmd | ((uint32_t)value << cmd_bits) | ((uint32_t)crc << (cmd_bits + 8));
}

// Build the WRITE of the byte that comes after `done` written bytes.
static void legic_prime_poller_build_write(
    LegicPrimePoller* instance,
    LegicPrimeExchange* ex,
    uint16_t done,
    uint8_t pre_steps) {
    const uint16_t addr = instance->write_offset + instance->write_size - 1 - done;
    const uint8_t cmd_bits = instance->cmd_size_bits;
    const uint32_t frame = legic_prime_poller_write_frame(
        addr, instance->write_data[addr - instance->write_offset], cmd_bits);
    legic_prime_poller_build(ex, frame, LEGIC_PRIME_WRITE_FRAME_BITS(cmd_bits), pre_steps, 0);
    ex->addr = addr;
}

// Claim ex (already sent) and wait for the tag's ACK; returns its bit window.
static LegicPrimeError legic_prime_poller_write_exchange(
    LegicPrimePoller* instance,
    const LegicPrimeExchange* ex,
    uint8_t next_window,
    uint8_t* ack_window) {
    legic_prime_poller_load_tx(instance, ex);
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
    furi_check(instance->cmd_size_bits);

    LegicPrimeExchange cur, next;
    LegicPrimeError ret = legic_prime_poller_start_session(instance, &cur);
    if(ret != LegicPrimeErrorNone) return ret;
    // Only now, with the type confirmed by this handshake: one misread type
    // reply, at detection or here, must not fail an in-range write.
    if(!legic_prime_is_writable(
           instance->data->data_size, instance->write_offset, instance->write_size)) {
        return LegicPrimeErrorReadOnly;
    }

    // The first WRITE follows the reply to the last UID READ.
    legic_prime_poller_build_write(
        instance, &next, instance->write_pos, LEGIC_PRIME_STEPS_AFTER_REPLY);
    legic_prime_poller_queue(&next, NULL);
    uint32_t reply = 0;
    ret = legic_prime_poller_exchange(instance, &cur, &reply);
    if(ret != LegicPrimeErrorNone) return ret;
    cur = next;
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

// Not confirmed yet, and attempts left in this session.
static bool legic_prime_poller_is_pending(const LegicPrimePoller* instance, uint16_t addr) {
    const uint8_t progress = instance->progress[addr];
    return !(progress & LEGIC_PRIME_POLLER_DONE) &&
           progress / LEGIC_PRIME_POLLER_ATTEMPT < LEGIC_PRIME_READ_ATTEMPTS;
}

// Next address of the range to read after `after` (in the range), cycling,
// `after` itself last.
static bool
    legic_prime_poller_next_addr(LegicPrimePoller* instance, uint16_t after, uint16_t* addr) {
    const uint16_t start = instance->read_start;
    const uint16_t end = instance->read_end;
    uint16_t a = after;
    for(uint16_t i = start; i < end; i++) {
        if(++a == end) a = start;
        if(legic_prime_poller_is_pending(instance, a)) {
            *addr = a;
            return true;
        }
    }
    return false;
}

void legic_prime_poller_read_reset(LegicPrimePoller* instance) {
    furi_assert(instance);
    furi_assert(instance->progress);

    const uint16_t size = instance->data->data_size;
    if(instance->read_size) {
        instance->read_start = MIN(instance->read_offset, size);
        instance->read_end = MIN((uint32_t)instance->read_start + instance->read_size, size);
    } else {
        instance->read_start = 0;
        instance->read_end = size;
    }
    memset(instance->progress, 0, LEGIC_PRIME_MEM_MAX);
    // Outside the range only the UID gets filled in.
    memset(instance->data->data, 0, sizeof(instance->data->data));
    instance->done_count = 0;
    // The first read goes to the start of the range.
    instance->read_cursor = instance->read_end - 1;
    instance->uid_known = false;
}

/*
 * One read session, mirroring Proxmark armsrc/legicrf.c but pipelined: the
 * ACK and the first READ go out chained behind the type reply, and every
 * further READ is queued before the previous reply is decoded, so each frame
 * follows the tag's reply by 260 µs (Proxmark3: 220 µs) as real tags require.
 *
 * Every session starts at the same keystream position, so a session goes on
 * from where the previous one stopped: the read that confirms a byte doesn't
 * repeat the keystream of the read before it.
 */
LegicPrimeError legic_prime_poller_read(LegicPrimePoller* instance) {
    furi_assert(instance);
    furi_assert(instance->progress);
    furi_check(instance->cmd_size_bits);

    uint8_t* progress = instance->progress;
    uint8_t* data = instance->data->data;
    // Failed reads only count within a session.
    for(uint16_t a = instance->read_start; a < instance->read_end; a++) {
        progress[a] &= LEGIC_PRIME_POLLER_DONE | LEGIC_PRIME_POLLER_SEEN;
    }

    LegicPrimeExchange cur, next;
    LegicPrimeError ret = legic_prime_poller_start_session(instance, &cur);
    if(ret == LegicPrimeErrorWrongCard) {
        // What was read so far is another card's: read this one from scratch.
        legic_prime_poller_read_reset(instance);
    }
    if(ret != LegicPrimeErrorNone) return ret;
    // The UID check confirmed bytes 0-3, two agreeing reads each.  They are
    // marked done outside the range too: the first claim below is a UID READ.
    for(uint16_t a = 0; a < LEGIC_PRIME_UID_SIZE; a++) {
        if(progress[a] & LEGIC_PRIME_POLLER_DONE) continue;
        progress[a] = LEGIC_PRIME_POLLER_DONE | LEGIC_PRIME_POLLER_SEEN;
        data[a] = instance->uid[a];
        if(a >= instance->read_start && a < instance->read_end) instance->done_count++;
    }

    const uint16_t size = instance->read_end - instance->read_start;
    const uint16_t done_start = instance->done_count;
    // A byte read earlier came up again: the session could confirm bytes.
    bool confirmable = false;
    // A byte passed its CRC for the first time.
    bool fresh = false;
    const uint32_t budget_max =
        MIN(size * LEGIC_PRIME_READ_ATTEMPTS + 16U, LEGIC_PRIME_READ_SESSION_EXCHANGES);
    uint32_t reply = 0;
    // The first claim is the last UID READ, a byte that is confirmed already.
    for(uint32_t budget = budget_max; budget; budget--) {
        if(!furi_hal_nfc_legic_poller_next_sent()) return LegicPrimeErrorTimeout;

        uint16_t next_addr;
        const bool have_next =
            legic_prime_poller_next_addr(instance, instance->read_cursor, &next_addr);
        if(have_next) {
            legic_prime_poller_build_read(
                instance, &next, next_addr, LEGIC_PRIME_STEPS_AFTER_REPLY);
            legic_prime_poller_queue(&next, NULL);
            instance->read_cursor = next_addr;
        }

        ret = legic_prime_poller_exchange(instance, &cur, &reply);
        if(ret != LegicPrimeErrorNone) return ret;

        uint8_t* byte_progress = &progress[cur.addr];
        if(!(*byte_progress & LEGIC_PRIME_POLLER_DONE)) {
            confirmable |= (*byte_progress & LEGIC_PRIME_POLLER_SEEN) != 0;
            if(!legic_prime_poller_read_crc_ok(instance, &cur, reply)) {
                // No logging here: the next frame is already on air.
                *byte_progress += LEGIC_PRIME_POLLER_ATTEMPT;
            } else {
                const LegicPrimePollerRead read =
                    legic_prime_poller_record_read(byte_progress, &data[cur.addr], reply & 0xFF);
                if(read == LegicPrimePollerReadFirst) {
                    fresh = true;
                } else if(read == LegicPrimePollerReadAgreed) {
                    instance->done_count++;
                } else {
                    *byte_progress += LEGIC_PRIME_POLLER_ATTEMPT;
                }
            }
        }

        if(instance->done_count == size) return LegicPrimeErrorNone;
        // Every byte left failed LEGIC_PRIME_READ_ATTEMPTS times.
        if(!have_next) return LegicPrimeErrorWrongCrc;
        cur = next;
    }

    // Running out of budget is no failure by itself: a MIM1024's first
    // session reads each byte once, so its progress is new bytes, not
    // confirmed ones.  It is one if nothing was confirmed although bytes came
    // up again, or if no new byte passed its CRC either.
    return instance->done_count == done_start && (confirmable || !fresh) ?
               LegicPrimeErrorWrongCrc :
               LegicPrimeErrorNone;
}
