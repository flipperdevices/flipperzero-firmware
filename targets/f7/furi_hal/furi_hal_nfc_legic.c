/*
 * @file furi_hal_nfc_legic.c
 * @brief LEGIC Prime technology layer for the ST25R3916.
 *
 * LEGIC Prime: 13.56 MHz carrier.
 *  - Reader→tag: pause-position 100% OOK.  Every bit is a ~19 µs pause
 *    followed by ~42 µs ('0') or ~80 µs ('1') of carrier, and the frame is
 *    closed by one extra pause: the tag measures pause-to-pause intervals,
 *    so without the closing pause it never sees the last bit.  Emitted
 *    through the FIFO in om_subcarrier_stream at stx_212 (4.72 µs/chip-bit).
 *  - Tag→reader: plain ASK on a 212 kHz load-modulation subcarrier, 99.1 µs
 *    per bit, no SOF/EOF/Manchester, starting 330 µs after the closing pause
 *    (Proxmark armsrc/legicrfsim.c).
 *  - The tag advances its keystream every 99.1 µs while idle, so frames are
 *    sent at fixed delays after the tag's reply (see
 *    furi_hal_nfc_legic_poller_set_next).
 *
 * RX uses the chip's subcarrier-stream receiver (scf_sc212, scp_8): each
 * FIFO bit reports subcarrier presence over ~18.9 µs.  A reception may end
 * in a '0' bit and the receiver does not re-arm by itself, so one reply
 * arrives as one or more receptions.  The receiver is re-armed after each
 * one, and the FIFO bits are placed into 99.1 µs bit windows by the
 * reception start time.  AGC and dynamic squelch are off: with them on,
 * later runs in a reply fade out.
 *
 * References:
 *  - Proxmark3 armsrc/legicrf.c (reader), armsrc/legicrfsim.c (tag side)
 *  - Tracking issue: github.com/flipperdevices/flipperzero-firmware/issues/1688
 */
#include "furi_hal_nfc_i.h"
#include "furi_hal_nfc_tech_i.h"

#include <furi.h>
#include <furi_hal_cortex.h>
#include <furi_hal_resources.h>

// Chip-bit buffers (x4 bytes).  The longest frame is a MIM1024 WRITE, 23 bits
// of up to 21 chips plus the closing pause = 487 chips; the chained ACK + READ
// is 130 + 47 + 235 = 412.  24 x 4 bytes (768 chips) leave headroom; a
// 512-chip buffer was too small for the ACK + WRITE chain (664 chips) that
// the writer used before WRITEs were chained behind the tag's ACK.
#define FURI_HAL_NFC_LEGIC_POLLER_MAX_BUFFER_SIZE (24U)
#define FURI_HAL_NFC_LEGIC_FIFO_SIZE              (512U)

#define FURI_HAL_NFC_LEGIC_POLLER_FWT_COMP_FC (-1300)

#define BITS_IN_BYTE (8U)

// All LEGIC timing is in units of one 212 kHz subcarrier period (64/fc,
// 4.72 µs), the same tick Proxmark uses.
#define LEGIC_TAG_FRAME_WAIT_CHIPS    (70U) // 330 µs from closing pause to reply
#define LEGIC_TAG_BIT_CHIPS           (21U) // 99.1 µs per reply bit
#define LEGIC_FIFO_BIT_CHIPS          (4U) // scp_8 report period, measured
// Offset between a reply's start on the tag's bit grid and the thread-side
// RxStart timestamp (seg[].t_rxs).  decode() and ack_window() apply it in
// the frame's own time base (against t_frame_end); reply_end() also
// subtracts it bare from a write ACK's RxStart, with no t_frame_end term,
// a use that only works as tuned together with LEGIC_NEXT_FRAME_AFTER_ACK_US
// (see LEGIC_ACK_WAKE_LATE_US).  Tuned on the decoder, not sniffed: it nets
// the chip's subcarrier detection, the IRQ-to-thread wake-up (40-58 µs, see
// LEGIC_ACK_WAKE_LATE_US) and the error of LEGIC_TX_CMD_LATENCY_US (itself a
// thread-side TxE figure, so it contains a wake-up too) against each other,
// which is why it is smaller than the wake-up alone.  ~25 µs for the first
// reception after TX and for a receiver re-armed long before the run, ~40 µs
// when re-armed within ~100 µs of the onset.
#define LEGIC_RX_LAG_CHIPS            (5U)
#define LEGIC_RX_REARM_LAG_CHIPS      (8U)
#define LEGIC_RX_REARM_SHORT_CHIPS    (21U)
// TRANSMIT command to first chip-bit on air, measured via TxE (TxE comes
// ~(45 + chips * 4.72 + 5) µs after the command).  The frame end is derived
// from the command time, not from TxE: the event thread can see TxE late.
#define LEGIC_TX_CMD_LATENCY_US       (45U)
// Pipelined frames: a real MIM256 drops the session unless the reader's next
// frame follows within a few 99.1 µs slots, which leaves no time to decode a
// reply first.  The poller therefore queues the next frame in advance and the
// HAL sends it this long after the tag's reply ends.  The tag steps its
// keystream 3 times if the frame starts 198-297 µs after its reply (PM3
// reader: ~225 µs); 260 µs leaves time to drain the receiver and load the FIFO.
#define LEGIC_NEXT_FRAME_US           (260U)
// After a write ACK, timed from the ACK's end as recovered from its
// thread-side RxStart (start - LEGIC_RX_LAG_CHIPS + one bit; the wake-up
// jitter puts ~10 µs on it, up to 25 µs before LEGIC_ACK_WAKE_LATE_US drops
// the frame).  Value and ACK end were tuned as one set in this time base
// (2026-10-15, before an IRQ timestamp existed): the PM3 sim takes the next
// frame at 240 and 260 but mostly not at 280, a real MIM256 at 240 and 260
// but drops 1 in 5 at 263-268.  Those are values of this constant, timed
// from the recovered ACK end.  On the PM3 sim (generic firmware, reply
// offset 0) that base comes out close to the air.  The sim's own trace
// (hf legic list, 4.72 µs ticks) logs the WRITE chained at 240 mostly 53
// ticks after its ACK (54-55 after a slower wake-up), and the READ sent 260
// after a data reply 57 ticks after that reply.  The second gap is 260 on
// air whatever the TX latency is (the reply end and the launch both derive
// from t_frame_end), so the sim reads ~9 µs long, and 240 is ~241 µs on air
// for a usual wake-up, up to ~250 for a slow one (~255-260 in the slowest
// 1-2 %).  The MIM256's ACK timing on air was not measured.
#define LEGIC_NEXT_FRAME_AFTER_ACK_US (240U)
// A queued frame is started with only this much of it in the FIFO and the
// rest is loaded while it goes out (37.8 µs per byte on air): loading a whole
// ACK + WRITE before the deadline took long enough to miss it at times.
#define LEGIC_NEXT_FIFO_HEAD_BYTES    (8U)
// Carrier between two chained frames (ACK + first READ), as the PM3 reader's
// 220 µs RWD_FRAME_WAIT: 47 chip-bits = 221.8 µs.
#define LEGIC_CHAIN_GAP_CHIPS         (47U)
// With a frame queued, stop the reception this long after the reply should
// have ended (its length is known) instead of waiting for the receiver to
// notice the subcarrier is gone (~125 µs after it stops), so the queued frame
// can go out on time.  Stopping drops the chip's last partial FIFO byte;
// 45 µs gave fewer missed final '1' bits than 30 µs (MIM256 card at ~1 cm:
// 95-96% vs 93-94% clean reads) with the same worst-case send_next entry
// (~113 µs of the 215 µs budget); at 90 µs the receiver's own end of
// reception pushed every queued frame past its slot.
#define LEGIC_RX_FORCE_END_US         (45U)
// Longest wait for TxEnd before giving up on it (longest frame ~1.5 ms).
#define LEGIC_TXE_MAX_WAIT_MS         (10U)
// The ACK's measured end, and so the launch time of a frame queued behind
// it, comes from the thread's RxStart timestamp.  The usual wake-up path
// (ISR, context switch, IRQ status read; 40-58 µs measured) is not removed
// by LEGIC_RX_LAG_CHIPS alone: that constant, LEGIC_TX_CMD_LATENCY_US and
// LEGIC_NEXT_FRAME_AFTER_ACK_US were tuned together with the usual wake-up
// inside, so 240 is the value that works from this timestamp (on the PM3
// sim also about the gap on air, see LEGIC_NEXT_FRAME_AFTER_ACK_US).  A
// wake-up later than usual (another thread inside a scheduler-suspended
// region, e.g. malloc) delays the frame one-for-one beyond what was tuned;
// the ACK-window gate only notices about half a bit window (~49 µs, less
// for a tag whose ACK sits late in its window) while a real MIM256 dropped
// 1 in 5 chained WRITEs 23-28 µs above the tuned value.  What matters is
// the excess over the usual path, so the value is that path's typical
// latency plus ~20 µs: a wake-up at this limit lands the frame at the
// MIM256's edge.
// Measured with LEGIC_WAKE_STATS (a 1000-byte chained write, its read-back
// and dumps on the PM3 sim, driven from the CLI): 5907 RxStart wake-ups, 1011
// of them write ACKs with a frame queued, all 40-58 µs, 97% within 40-49 µs,
// worst ACK 54 µs.  A first guess of 30 µs vetoed every chained WRITE.
// Re-measure if the wake-up path changes: a non-zero vetoed count on a
// healthy run means the value is too tight, and every veto costs a session.
#define LEGIC_ACK_WAKE_LATE_US        (65U)

#define LEGIC_RX_GAP_MS       (2U)
#define LEGIC_RX_MAX_SEGMENTS (16U)
// Longest reply the poller asks for (the 60-window write ACK wait).
#define LEGIC_RX_MAX_BITS     (64U)

// Poller first-stage AM gain reduction, rg1_am = 2 (the default, "reduced
// gain").  Full gain lost runs of a strong tag (PM3 `hf legic sim`: 43%
// clean reads, all missed '1's); rg1_am 1/2/3/4/5/6 gave 91/95/93/83/76/61%.
// A real MIM256 reads 97-98% clean at both rg1_am 0 and 4.  A more weakly
// coupled simulator lost the card-type reply at rg1_am 2 but not at full
// gain, so the poller alternates the two after failures
// (furi_hal_nfc_legic_poller_set_rx_reduced_gain).
#define LEGIC_RX_CONF3_POLLER (0x40U)
#define LEGIC_RX_CONF3_FULL   (0x00U)

// RxStart wake-up latency statistics (IRQ edge to the thread's timestamp),
// for tuning LEGIC_ACK_WAKE_LATE_US: build with LEGIC_WAKE_STATS 1 (here or
// as a build define), run a chained-write bench and read legic_wake_stats
// with a debugger.
#ifndef LEGIC_WAKE_STATS
#define LEGIC_WAKE_STATS 0
#endif

// TxEnd owed for the frame of the current exchange.
typedef enum {
    LegicTxeNone, // seen, or never expected
    LegicTxeOwed, // the frame is on air, its TxEnd has not been seen
    LegicTxeSynth, // seen before the frame was claimed: report it once
} LegicTxeState;

// One reception of the current reply (DWT cycles, bits in fifo_buf).
typedef struct {
    uint32_t t_rxs;
    uint32_t t_rearm; // receiver re-armed before this reception (if rearmed)
    uint16_t bits;
    uint16_t off; // byte offset in fifo_buf
    bool rearmed; // false: first reception after TX
} LegicRxSegment;

typedef struct {
    bool rx_reduced_gain;
    bool rx_gain_applied;
    // Reply of the next exchange, requested by set_rx_bits()/set_rx_ack()
    // and latched by tx() for that exchange.
    size_t rx_bits_expected;
    int16_t rx_ack_window; // write ACK expected in this bit window, -1: data reply

    // Queued next frame (furi_hal_nfc_legic_poller_set_next), chip bits.
    uint8_t next_buf[FURI_HAL_NFC_LEGIC_POLLER_MAX_BUFFER_SIZE * 4];
    size_t next_chip_bits;
    bool next_pending;
    // In-flight queued frame: sent from the receive loop, not yet claimed by
    // the poller's tx() (furi_hal_nfc_legic_poller_claim_next).
    bool next_sent;
    bool claim_requested;
    bool next_txe_seen;
    uint32_t next_frame_end;

    // Current exchange (DWT cycles).
    uint32_t t_frame_end;
    uint32_t t_rx_window_end;
    int16_t reply_ack_window; // -1: data reply
    LegicTxeState txe;
    // RX segments of the current exchange: one per reception.
    bool rx_started;
    bool seg_open;
    uint32_t t_last_rx_event;
    // Last re-arm of the receiver in this exchange (UNMASK_RECEIVE_DATA).
    uint32_t t_last_rearm;
    bool rx_rearmed;
    size_t seg_count;
    LegicRxSegment seg[LEGIC_RX_MAX_SEGMENTS];
    uint8_t fifo_buf[FURI_HAL_NFC_LEGIC_FIFO_SIZE];
    size_t fifo_buf_bits;
} FuriHalNfcLegicPoller;

static FuriHalNfcLegicPoller* furi_hal_nfc_legic_poller = NULL;

#if LEGIC_WAKE_STATS
#define LEGIC_WAKE_STATS_BIN_US (5U)
#define LEGIC_WAKE_STATS_BINS   (32U)
typedef struct {
    uint32_t count; // RxStart wake-ups seen in the blocking branch
    uint32_t ack_count; // of which write ACKs with a frame queued
    uint32_t vetoed; // queued frames dropped by LEGIC_ACK_WAKE_LATE_US
    uint32_t max_us;
    uint32_t ack_max_us;
    uint32_t hist[LEGIC_WAKE_STATS_BINS]; // 5 µs bins, the last one open-ended
} LegicWakeStats;

static volatile LegicWakeStats legic_wake_stats;

static void legic_wake_stats_record(uint32_t latency_cycles, bool ack) {
    const uint32_t us = latency_cycles / furi_hal_cortex_instructions_per_microsecond();
    uint32_t bin = us / LEGIC_WAKE_STATS_BIN_US;
    if(bin >= LEGIC_WAKE_STATS_BINS) bin = LEGIC_WAKE_STATS_BINS - 1;
    legic_wake_stats.count++;
    legic_wake_stats.hist[bin]++;
    if(us > legic_wake_stats.max_us) legic_wake_stats.max_us = us;
    if(ack) {
        legic_wake_stats.ack_count++;
        if(us > legic_wake_stats.ack_max_us) legic_wake_stats.ack_max_us = us;
    }
}
#endif

static uint32_t legic_chips_to_cycles(uint32_t chips) {
    const uint64_t cpu_hz = (uint64_t)furi_hal_cortex_instructions_per_microsecond() * 1000000U;
    return (uint32_t)(chips * 64U * cpu_hz / 13560000U);
}

static FuriHalNfcError furi_hal_nfc_legic_common_init(const FuriHalSpiBusHandle* handle) {
    // AM channel, 12 kHz / 200 kHz zeros, 1200 kHz LPF, static full gain,
    // with scp_8 (see poller_init).  Tuned 2026-10-13 against a real MIM256
    // on its card-type reply (a 2-bit and a 3-bit run of subcarrier):
    //  - the real tag's subcarrier glitches ~75-110 µs into a run; with
    //    scp_4 and the 12/80 kHz zeros that ended the reception after 8
    //    FIFO bits.  12/200 kHz (or 60/400 kHz) + scp_8: 16/16 correct.
    //  - against the much stronger PM3 `hf legic sim`, lp_300khz + 12/80 kHz
    //    + scp_4 was the setting that worked (600 kHz LPF cut long runs).
    //    Re-checked 2026-10-14 with scp_8 and no debug formatting in the RX
    //    path: lp_300khz/lp_2000khz lose the real tag's runs after ~75 µs,
    //    lp_600khz/lp_1200khz read it; the sim needs the poller's first-stage
    //    gain reduction instead (LEGIC_RX_CONF3_POLLER).
    //  - AGC (any mode) and dynamic squelch made runs fade or vanish; the PM
    //    channel and CORR_CONF1=0x00 cut runs after 8 FIFO bits.
    st25r3916_write_reg(
        handle,
        ST25R3916_REG_RX_CONF1,
        ST25R3916_REG_RX_CONF1_hz_12_200khz | ST25R3916_REG_RX_CONF1_lp_1200khz);
    st25r3916_write_reg(handle, ST25R3916_REG_RX_CONF2, 0x00);
    st25r3916_write_reg(handle, ST25R3916_REG_RX_CONF3, 0x00);
    st25r3916_write_reg(handle, ST25R3916_REG_RX_CONF4, 0x00);

    st25r3916_write_reg(
        handle,
        ST25R3916_REG_CORR_CONF1,
        ST25R3916_REG_CORR_CONF1_corr_s0 | ST25R3916_REG_CORR_CONF1_corr_s1 |
            ST25R3916_REG_CORR_CONF1_corr_s4);

    st25r3916_write_reg(handle, ST25R3916_REG_CORR_CONF2, ST25R3916_REG_CORR_CONF2_corr_s8);

    return FuriHalNfcErrorNone;
}

static FuriHalNfcError furi_hal_nfc_legic_poller_init(const FuriHalSpiBusHandle* handle) {
    furi_check(furi_hal_nfc_legic_poller == NULL);

    furi_hal_nfc_legic_poller = malloc(sizeof(FuriHalNfcLegicPoller));
    memset(furi_hal_nfc_legic_poller, 0, sizeof(FuriHalNfcLegicPoller));
    furi_hal_nfc_legic_poller->rx_ack_window = -1;
    furi_hal_nfc_legic_poller->reply_ack_window = -1;

    // Subcarrier-stream mode + OOK: FIFO chip-bits are OOK-modulated directly
    // on TX, and RX demodulates a 212 kHz subcarrier into the FIFO.
    st25r3916_change_reg_bits(
        handle,
        ST25R3916_REG_MODE,
        ST25R3916_REG_MODE_om_mask | ST25R3916_REG_MODE_tr_am,
        ST25R3916_REG_MODE_om_subcarrier_stream | ST25R3916_REG_MODE_tr_am_ook);

    //  - scf_sc212: 212 kHz subcarrier (LEGIC tag).
    //  - scp_8pulses: one FIFO bit per ~18.9 µs; rides through the real
    //    tag's brief subcarrier glitches (scp_1/scp_2 give false RxStart
    //    ~68 µs after TxEnd and no data).
    //  - stx_212: 4.72 µs/chip-bit (PM3 sim timing showed stx_424 = 2.36 µs).
    st25r3916_write_reg(
        handle,
        ST25R3916_REG_STREAM_MODE,
        ST25R3916_REG_STREAM_MODE_scf_sc212 | ST25R3916_REG_STREAM_MODE_stx_212 |
            ST25R3916_REG_STREAM_MODE_scp_8pulses);

    st25r3916_clear_reg_bits(
        handle,
        ST25R3916_REG_AUX_MOD,
        ST25R3916_REG_AUX_MOD_dis_reg_am | ST25R3916_REG_AUX_MOD_res_am);

    FuriHalNfcError err = furi_hal_nfc_legic_common_init(handle);

    FuriHalNfcLegicPoller* instance = furi_hal_nfc_legic_poller;
    st25r3916_write_reg(handle, ST25R3916_REG_RX_CONF3, LEGIC_RX_CONF3_POLLER);
    instance->rx_reduced_gain = true;
    instance->rx_gain_applied = true;
    return err;
}

static FuriHalNfcError furi_hal_nfc_legic_poller_deinit(const FuriHalSpiBusHandle* handle) {
    UNUSED(handle);
    furi_check(furi_hal_nfc_legic_poller);

    free(furi_hal_nfc_legic_poller);
    furi_hal_nfc_legic_poller = NULL;
    return FuriHalNfcErrorNone;
}

void furi_hal_nfc_legic_poller_set_rx_bits(size_t bits) {
    furi_check(furi_hal_nfc_legic_poller);
    furi_check(bits > 0 && bits <= LEGIC_RX_MAX_BITS);
    furi_hal_nfc_legic_poller->rx_bits_expected = bits;
}

void furi_hal_nfc_legic_poller_set_rx_ack(size_t window) {
    furi_check(furi_hal_nfc_legic_poller);
    furi_check(window < LEGIC_RX_MAX_BITS);
    furi_hal_nfc_legic_poller->rx_ack_window = window;
}

void furi_hal_nfc_legic_poller_set_rx_reduced_gain(bool reduced) {
    furi_check(furi_hal_nfc_legic_poller);
    furi_hal_nfc_legic_poller->rx_reduced_gain = reduced;
}

/*
 * LEGIC Prime pause-position encoder.
 *
 * In om_subcarrier_stream + OOK the chip emits one OOK symbol per FIFO
 * chip-bit (LSB-first): chip-bit 1 = pause (carrier off), 0 = carrier on.
 * At stx_212 one chip-bit is ~4.72 µs, so:
 *
 *   pause       = 4 chip-bits  = 18.9 µs
 *   '0' pulse   = 9 chip-bits  = 42.4 µs
 *   '1' pulse   = 17 chip-bits = 80.2 µs
 *
 * matching Proxmark legicrfsim.c RWD_TIME_* (in 4.72 µs ticks).
 */
#define LEGIC_PAUSE_CHIP_BITS  4U
#define LEGIC_BIT0_PULSE_CHIPS 9U
#define LEGIC_BIT1_PULSE_CHIPS 17U

typedef struct {
    uint8_t buf[FURI_HAL_NFC_LEGIC_POLLER_MAX_BUFFER_SIZE * 4];
    size_t bit_pos;
} LegicChipBitWriter;

static void legic_chip_bit_writer_emit(LegicChipBitWriter* w, bool bit, size_t count) {
    // A truncated frame would reach the tag as a different command (a WRITE
    // cannot be undone), so a buffer too small for a frame fails loudly.
    furi_check(w->bit_pos + count <= sizeof(w->buf) * BITS_IN_BYTE);
    for(size_t i = 0; i < count; i++) {
        if(bit) {
            w->buf[w->bit_pos / 8] |= (1u << (w->bit_pos % 8));
        }
        w->bit_pos++;
    }
}

static void legic_encode_bit(LegicChipBitWriter* w, bool legic_bit) {
    legic_chip_bit_writer_emit(w, true, LEGIC_PAUSE_CHIP_BITS);
    legic_chip_bit_writer_emit(
        w, false, legic_bit ? LEGIC_BIT1_PULSE_CHIPS : LEGIC_BIT0_PULSE_CHIPS);
}

// One reader frame (LSB-first bits) followed by the closing pause: the tag
// decodes a bit when it sees the *next* pause.
static void legic_encode_frame(LegicChipBitWriter* w, const uint8_t* data, size_t bits) {
    for(size_t i = 0; i < bits; i++) {
        legic_encode_bit(w, (data[i / 8] >> (i % 8)) & 1u);
    }
    legic_chip_bit_writer_emit(w, true, LEGIC_PAUSE_CHIP_BITS);
}

void furi_hal_nfc_legic_poller_set_next(
    const uint8_t* first,
    size_t first_bits,
    const uint8_t* second,
    size_t second_bits) {
    FuriHalNfcLegicPoller* instance = furi_hal_nfc_legic_poller;
    furi_check(instance);

    LegicChipBitWriter w = {0};
    legic_encode_frame(&w, first, first_bits);
    if(second && second_bits) {
        legic_chip_bit_writer_emit(&w, false, LEGIC_CHAIN_GAP_CHIPS);
        legic_encode_frame(&w, second, second_bits);
    }
    memcpy(instance->next_buf, w.buf, sizeof(instance->next_buf));
    instance->next_chip_bits = w.bit_pos;
    // next_sent refers to the previous queued frame, which may still be on
    // air waiting to be claimed; leave it alone.
    instance->next_pending = true;
}

void furi_hal_nfc_legic_poller_claim_next(void) {
    furi_check(furi_hal_nfc_legic_poller);
    furi_hal_nfc_legic_poller->claim_requested = true;
}

void furi_hal_nfc_legic_poller_cancel_next(void) {
    FuriHalNfcLegicPoller* instance = furi_hal_nfc_legic_poller;
    furi_check(instance);
    instance->next_pending = false;
    instance->next_sent = false;
    instance->next_txe_seen = false;
    instance->claim_requested = false;
}

bool furi_hal_nfc_legic_poller_next_sent(void) {
    furi_check(furi_hal_nfc_legic_poller);
    return furi_hal_nfc_legic_poller->next_sent;
}

// A queued frame is sent from the receive path of the exchange before it;
// until the poller's tx() claims it, that frame or its reply is on air and
// the STOP of a trx_reset would cut it off.
static FuriHalNfcError furi_hal_nfc_legic_poller_trx_reset(const FuriHalSpiBusHandle* handle) {
    FuriHalNfcLegicPoller* instance = furi_hal_nfc_legic_poller;
    if(instance && instance->next_sent) return FuriHalNfcErrorNone;

    st25r3916_direct_cmd(handle, ST25R3916_CMD_STOP);
    return FuriHalNfcErrorNone;
}

static FuriHalNfcError furi_hal_nfc_legic_poller_tx(
    const FuriHalSpiBusHandle* handle,
    const uint8_t* tx_data,
    size_t tx_bits) {
    FuriHalNfcLegicPoller* instance = furi_hal_nfc_legic_poller;
    instance->reply_ack_window = instance->rx_ack_window;
    instance->rx_ack_window = -1;

    // A claim (furi_hal_nfc_legic_poller_claim_next) names this frame as the
    // one sent from the previous exchange's receive loop.  A sent frame
    // nobody claims belongs to an abandoned session: forget it, or it would
    // swallow this frame (which would then never be transmitted).
    const bool claims_queued = instance->claim_requested && instance->next_sent;
    instance->claim_requested = false;
    instance->next_sent = false;

    instance->rx_started = false;
    instance->seg_open = false;
    instance->rx_rearmed = false;
    instance->seg_count = 0;
    instance->fifo_buf_bits = 0;
    const uint32_t rx_window_chips =
        LEGIC_TAG_FRAME_WAIT_CHIPS + instance->rx_bits_expected * LEGIC_TAG_BIT_CHIPS + 10U;

    if(claims_queued) {
        // Already on air: only set up the bookkeeping for its reply.  Its
        // TxEnd may already have been consumed by the block-TX wait loop.
        // tx_prepare's status read is skipped on purpose, as it would eat
        // this frame's TxEnd; anything stale from the previous exchange was
        // drained by send_next before the TRANSMIT.
        instance->t_frame_end = instance->next_frame_end;
        instance->txe = instance->next_txe_seen ? LegicTxeSynth : LegicTxeOwed;
        instance->next_txe_seen = false;
        instance->t_rx_window_end = instance->t_frame_end + legic_chips_to_cycles(rx_window_chips);
        return FuriHalNfcErrorNone;
    }
    // A frame queued with set_next() before this tx() stays queued: it goes
    // out after this frame's reply.

    LegicChipBitWriter w = {0};
    // tx_data is LSB-first: frame bit i is bit (i % 8) of byte i / 8.
    legic_encode_frame(&w, tx_data, tx_bits);

    if(instance->rx_reduced_gain != instance->rx_gain_applied) {
        st25r3916_write_reg(
            handle,
            ST25R3916_REG_RX_CONF3,
            instance->rx_reduced_gain ? LEGIC_RX_CONF3_POLLER : LEGIC_RX_CONF3_FULL);
        instance->rx_gain_applied = instance->rx_reduced_gain;
    }

    furi_hal_nfc_poller_tx_prepare(handle);
    // Round up to whole bytes for the chip FIFO; padding chip-bits are 0
    // (carrier on).
    st25r3916_write_fifo(handle, w.buf, ((w.bit_pos + 7) / 8) * 8);

    const uint32_t cmd_latency =
        LEGIC_TX_CMD_LATENCY_US * furi_hal_cortex_instructions_per_microsecond();
    const uint32_t t_tx_cmd = DWT->CYCCNT;
    st25r3916_direct_cmd(handle, ST25R3916_CMD_TRANSMIT_WITHOUT_CRC);
    instance->txe = LegicTxeOwed;
    instance->t_frame_end = t_tx_cmd + cmd_latency + legic_chips_to_cycles(w.bit_pos);
    instance->t_rx_window_end = instance->t_frame_end + legic_chips_to_cycles(rx_window_chips);

    return FuriHalNfcErrorNone;
}

// Append the FIFO content of one finished reception to fifo_buf and re-arm
// the receiver for the next run of '1' bits.
static void
    furi_hal_nfc_legic_collect_segment(FuriHalNfcLegicPoller* instance, uint32_t now, bool rearm) {
    const FuriHalSpiBusHandle* handle = &furi_hal_spi_bus_handle_nfc;
    uint8_t seg[64];
    size_t seg_bits = 0;
    if(!st25r3916_read_fifo(handle, seg, sizeof(seg), &seg_bits)) {
        // An empty reception (shorter than one FIFO bit).  More than
        // sizeof(seg) cannot build up, gap_over ends a reception within
        // ~2 ms (~14 bytes).
        seg_bits = 0;
    }
    const bool started = instance->seg_open;
    instance->seg_open = false;

    // The re-arm before this reception picks its RX lag offset (decode();
    // LEGIC_RX_LAG_CHIPS / LEGIC_RX_REARM_LAG_CHIPS); take it before this
    // one's re-arm, which is for the next reception.
    const bool rearmed = instance->rx_rearmed;
    const uint32_t t_rearm = instance->t_last_rearm;
    if(rearm && (int32_t)(now - instance->t_rx_window_end) < 0) {
        st25r3916_direct_cmd(handle, ST25R3916_CMD_UNMASK_RECEIVE_DATA);
        instance->t_last_rearm = now;
        instance->rx_rearmed = true;
    }

    // Without an RxStart the run has no start time and cannot be placed; the
    // FIFO is drained above so it does not land in the next segment.
    if(!started) return;
    // An empty reception adds nothing to decode() and its re-arm is kept
    // above, so it takes no slot: noise must not crowd out the reply.
    if(!seg_bits) return;
    if(instance->seg_count >= LEGIC_RX_MAX_SEGMENTS) return;
    // Byte-align each segment: seg.off is a byte offset and decode() reads a
    // segment from bit 0 of its first byte.
    const size_t pos = (instance->fifo_buf_bits + 7) / BITS_IN_BYTE;
    const size_t seg_bytes = (seg_bits + 7) / BITS_IN_BYTE;
    if(pos + seg_bytes > sizeof(instance->fifo_buf)) return;

    memcpy(&instance->fifo_buf[pos], seg, seg_bytes);
    instance->fifo_buf_bits = pos * BITS_IN_BYTE + seg_bits;
    LegicRxSegment* s = &instance->seg[instance->seg_count++];
    s->t_rearm = t_rearm;
    s->rearmed = rearmed;
    s->bits = seg_bits;
    s->off = pos;
}

// Bit window of a write ACK, counted like reply bits from 330 µs after our
// frame: the window start nearest to its first reception, as the decoder's
// half-coverage rule places a one-bit run.  -1 if it started too early.
// nfc_test.c legic_prime_write_chain_steps_test transcribes this rounding.
static int32_t furi_hal_nfc_legic_ack_window(const FuriHalNfcLegicPoller* instance) {
    const uint32_t t0 = instance->t_frame_end + legic_chips_to_cycles(LEGIC_TAG_FRAME_WAIT_CHIPS);
    const int32_t tb = legic_chips_to_cycles(LEGIC_TAG_BIT_CHIPS);
    const int32_t start =
        (int32_t)(instance->seg[0].t_rxs - legic_chips_to_cycles(LEGIC_RX_LAG_CHIPS) - t0);
    return start < -tb / 2 ? -1 : (start + tb / 2) / tb;
}

// End of the current tag reply.  A data reply starts 330 µs after our frame
// and its length is known, so it ends on the tag's bit grid.  A write ACK is a
// single bit that comes when the tag has finished writing; its end is taken
// from its reception, as tags time the next frame from their own ACK and
// don't all place it on the grid (Proxmark3's sim sends it ~23 µs early).
// ack_win is the exchange's ACK window, read once with next_in_step().
static bool furi_hal_nfc_legic_reply_end(
    const FuriHalNfcLegicPoller* instance,
    int32_t ack_win,
    uint32_t* end) {
    if(instance->reply_ack_window < 0) {
        *end = instance->t_frame_end +
               legic_chips_to_cycles(
                   LEGIC_TAG_FRAME_WAIT_CHIPS + instance->rx_bits_expected * LEGIC_TAG_BIT_CHIPS);
        return true;
    }
    if(ack_win < 0) return false;
    *end = instance->seg[0].t_rxs - legic_chips_to_cycles(LEGIC_RX_LAG_CHIPS) +
           legic_chips_to_cycles(LEGIC_TAG_BIT_CHIPS);
    return true;
}

// A frame queued behind a write ACK was built for the bit window the poller
// expected the ACK in: the tag steps its keystream once per window it spends
// writing, so in any other window the frame would reach the tag garbled (and
// a garbled WRITE passes its CRC-4 one time in 16).
static bool
    furi_hal_nfc_legic_next_in_step(const FuriHalNfcLegicPoller* instance, int32_t ack_win) {
    return instance->reply_ack_window < 0 || ack_win == instance->reply_ack_window;
}

// What the collecting loop keeps of the exchange's first reception for the
// queued frame: the write-ACK window, the reply's end (the frame's launch
// time) and the in-step gate, from one reading of seg[0].t_rxs.
typedef struct {
    int32_t ack_win;
    uint32_t reply_end;
    bool reply_end_known;
    bool next_in_step;
} LegicReplyRef;

static void
    furi_hal_nfc_legic_take_reply_ref(const FuriHalNfcLegicPoller* instance, LegicReplyRef* ref) {
    ref->ack_win = furi_hal_nfc_legic_ack_window(instance);
    ref->reply_end_known = furi_hal_nfc_legic_reply_end(instance, ref->ack_win, &ref->reply_end);
    ref->next_in_step = furi_hal_nfc_legic_next_in_step(instance, ref->ack_win);
}

// A write ACK's reference names the reception that stamped seg[0].t_rxs.
// Once that reception has ended without a FIFO bit (no segment recorded,
// none open) it was noise: neither force_end nor the launch may follow it.
// The exchange then waits for the real ACK, which ends it normally (one
// more than LEGIC_RX_GAP_MS after the noise's last event is still lost to
// gap_over, as before); the chain stops there and the poller continues in
// a new session.  A data reply's end comes from t_frame_end and stays.
static void
    furi_hal_nfc_legic_drop_empty_ref(const FuriHalNfcLegicPoller* instance, LegicReplyRef* ref) {
    if(instance->reply_ack_window < 0) return;
    if(instance->seg_count == 0 && !instance->seg_open) {
        ref->reply_end_known = false;
        ref->next_in_step = false;
    }
}

// Send the queued frame LEGIC_NEXT_FRAME_US (LEGIC_NEXT_FRAME_AFTER_ACK_US after a
// write ACK) after the current reply ends.
// Timing for the current exchange is left untouched (rx() still decodes it);
// the new frame's is applied when the poller's tx() claims it.
static void furi_hal_nfc_legic_send_next(FuriHalNfcLegicPoller* instance, uint32_t reply_end) {
    const FuriHalSpiBusHandle* handle = &furi_hal_spi_bus_handle_nfc;
    const uint32_t mhz = furi_hal_cortex_instructions_per_microsecond();
    const uint32_t cmd_latency = LEGIC_TX_CMD_LATENCY_US * mhz;
    instance->next_pending = false;

    const uint32_t gap_us = instance->reply_ack_window >= 0 ? LEGIC_NEXT_FRAME_AFTER_ACK_US :
                                                              LEGIC_NEXT_FRAME_US;
    const uint32_t cmd_at = reply_end + gap_us * mhz - cmd_latency;

    const size_t fifo_bits = ((instance->next_chip_bits + 7) / 8) * 8;
    const size_t fifo_bytes = fifo_bits / 8;
    const size_t head_bytes = MIN(fifo_bytes, LEGIC_NEXT_FIFO_HEAD_BYTES);
    // The receiver may have been re-armed after the reply's last run; a
    // transmission started with it armed got cut off after a few bits.
    st25r3916_direct_cmd(handle, ST25R3916_CMD_STOP);
    // A reception that began or ended between the collecting loop's last
    // flag read and the STOP is still in the chip's status and the thread's
    // flag, and the claim path never reads the status (tx() skips
    // tx_prepare).  Carried into the claimed exchange it opens segment 0 at
    // a time before our frame (or ends it with no segment at all), and a
    // write ACK is then never sampled.  Nothing wanted is pending here: the
    // receiver is stopped and TRANSMIT has not been issued, so the frame's
    // TxEnd cannot be lost.  Thread flag first, as wait_event_common does,
    // so a late ISR can only leave a flag that reads as no event.
    furi_thread_flags_clear(FuriHalNfcEventInternalTypeIrq);
    furi_hal_nfc_get_irq(handle);
    st25r3916_direct_cmd(handle, ST25R3916_CMD_CLEAR_FIFO);
    st25r3916_write_reg(handle, ST25R3916_REG_NUM_TX_BYTES2, (uint8_t)(fifo_bits & 0xFFU));
    st25r3916_write_reg(handle, ST25R3916_REG_NUM_TX_BYTES1, (uint8_t)((fifo_bits >> 8) & 0xFFU));
    st25r3916_reg_write_fifo(handle, instance->next_buf, head_bytes);
    if((int32_t)(cmd_at - DWT->CYCCNT) <= 0) {
        // Too late: the tag would read it in the wrong keystream slot.
        return;
    }
    // The rest of the wait (up to ~100 µs) is a cycle-accurate launch: the
    // frame has to start within ~20 µs of its slot (a real MIM256 lost 1 in
    // 5 chained WRITEs at +263-268 µs), so a timer wake-up here would still
    // end in a short spin.
    while((int32_t)(cmd_at - DWT->CYCCNT) > 0) {
    }
    const uint32_t tx_cmd = DWT->CYCCNT;
    st25r3916_direct_cmd(handle, ST25R3916_CMD_TRANSMIT_WITHOUT_CRC);
    if(fifo_bytes > head_bytes) {
        st25r3916_reg_write_fifo(handle, &instance->next_buf[head_bytes], fifo_bytes - head_bytes);
    }
    instance->next_frame_end =
        tx_cmd + cmd_latency + legic_chips_to_cycles(instance->next_chip_bits);
    instance->next_txe_seen = false;
    instance->next_sent = true;
}

// The frame queued behind a write ACK is launched from the ACK's thread-side
// RxStart timestamp (see LEGIC_ACK_WAKE_LATE_US).  A wake-up later than the
// usual path would send it late by the same amount, past what the tag
// accepts, while the ACK-window gate still passes: drop it instead (one
// session lost) rather than risk a WRITE decoded on a stepped keystream.
// The chip holds its IRQ line until the status registers are read, so the
// ISR timestamp is the RxStart edge even when RxEnd is pending too.  An EXTI
// delayed by a critical section is late in both timestamps and not caught.
static void furi_hal_nfc_legic_check_wake(FuriHalNfcLegicPoller* instance, uint32_t now) {
    const uint32_t latency = now - furi_hal_nfc_get_irq_timestamp();
    const bool ack_with_next = instance->reply_ack_window >= 0 && instance->next_pending;
#if LEGIC_WAKE_STATS
    legic_wake_stats_record(latency, ack_with_next);
#endif
    if(!ack_with_next) return;
    if(latency > LEGIC_ACK_WAKE_LATE_US * furi_hal_cortex_instructions_per_microsecond()) {
        instance->next_pending = false;
#if LEGIC_WAKE_STATS
        legic_wake_stats.vetoed++;
#endif
    }
}

static FuriHalNfcEvent furi_hal_nfc_legic_poller_wait_event(uint32_t timeout_ms) {
    FuriHalNfcLegicPoller* instance = furi_hal_nfc_legic_poller;

    if(instance->txe == LegicTxeSynth) {
        instance->txe = LegicTxeNone;
        return FuriHalNfcEventTxEnd;
    }

    if(!instance->rx_started) {
        // Never block forever on a TxEnd: the transceive state machine only
        // starts its frame-wait timer after TxEnd, so a lost one would hang.
        const bool owed = instance->txe == LegicTxeOwed;
        FuriHalNfcEvent event =
            furi_hal_nfc_wait_event_common(owed ? LEGIC_TXE_MAX_WAIT_MS : timeout_ms);
        const uint32_t now = DWT->CYCCNT;
        if(owed && (event & FuriHalNfcEventTimeout)) {
            instance->txe = LegicTxeNone;
            return FuriHalNfcEventTxEnd;
        }
        if(event & FuriHalNfcEventTxEnd) {
            instance->txe = LegicTxeNone;
            // TxEnd of a queued frame before the poller claimed it.
            if(instance->next_sent) instance->next_txe_seen = true;
        }
        if(event & FuriHalNfcEventRxStart) {
            instance->rx_started = true;
            instance->seg_open = true;
            // The thread-side timestamp: the RX lag offsets
            // (LEGIC_RX_LAG_CHIPS, LEGIC_RX_REARM_LAG_CHIPS) were tuned on it.
            instance->seg[0].t_rxs = now;
            instance->t_last_rx_event = now;
            furi_hal_nfc_legic_check_wake(instance, now);
            if(event & FuriHalNfcEventRxEnd)
                furi_hal_nfc_legic_collect_segment(instance, now, true);
            // Report only RxStart now; RxEnd is reported once collection ends.
            event &= ~FuriHalNfcEventRxEnd;
        }
        return event;
    }

    // Collecting: swallow further RxStart/RxEnd until the reply window has
    // passed or the line stays quiet.  This polls without blocking, about
    // 1.2 ms of every ~2.6 ms exchange at the NFC worker's priority: the
    // queued frame has to leave LEGIC_RX_FORCE_END_US (45 µs) after the
    // reply's end, which no tick wait meets (90 µs already made every queued
    // frame late), and real tags drop the session if the reader waits too
    // long (PM3 uses 220 µs gaps).  Arming the HAL's one-pulse block-TX timer
    // for the deadline and blocking in wait_event_common would reclaim the
    // time (nfc.c stops both timers at RxStart), but it moves every segment
    // timestamp by a few µs and so needs the sim + card clean-read bench
    // re-run; left for a follow-up.  Everything constant for the exchange is
    // computed once here, so an iteration is a flag peek and three compares.
    // flags_wait() with a zero timeout returns FuriFlagErrorResource (all
    // bits set) when nothing is pending, which wait_event_common would read
    // as every event including abort; peek first.
    // ack_window(), reply_end() and next_in_step() read seg[0].t_rxs, the
    // stamp of the exchange's first reception, once (ref): the queued
    // frame's launch time and the in-step gate come from the same value.
    // The loop rewrites that field on every RxStart while no segment is
    // recorded (an empty reception takes no slot, collect_segment), so the
    // first RxStart need not be the reception decode() places.  For a write
    // ACK the reference is therefore dropped once the reception it names
    // has ended empty (noise must neither arm force_end nor time the
    // frame), and the frame leaves only behind a recorded segment.  Both
    // act per RxEnd and at the exit; an iteration stays a flag peek and
    // three compares.
    const uint32_t mhz = furi_hal_cortex_instructions_per_microsecond();
    LegicReplyRef ref;
    furi_hal_nfc_legic_take_reply_ref(instance, &ref);
    // The blocking branch may have collected the first reception already.
    furi_hal_nfc_legic_drop_empty_ref(instance, &ref);
    const int32_t force_cycles = LEGIC_RX_FORCE_END_US * mhz;
    const int32_t gap_cycles = LEGIC_RX_GAP_MS * 1000U * mhz;
    while(true) {
        FuriHalNfcEvent event = 0;
        if(furi_thread_flags_get() & FURI_HAL_NFC_EVENT_INTERNAL_ALL) {
            event = furi_hal_nfc_wait_event_common(0);
        }
        const uint32_t now = DWT->CYCCNT;
        if(event & (FuriHalNfcEventRxStart | FuriHalNfcEventRxEnd))
            instance->t_last_rx_event = now;
        // RxStart and RxEnd in one mask are one short reception, in that
        // order.  With a segment still open the pair cannot occur (a new
        // RxStart needs the re-arm collect_segment issues on the thread's
        // RxEnd); if it ever did, close the open segment first and let the
        // RxEnd below close the new one at once, rather than leave a segment
        // open until force_end or the 2 ms gap ends the exchange.
        if((event & (FuriHalNfcEventRxStart | FuriHalNfcEventRxEnd)) ==
               (FuriHalNfcEventRxStart | FuriHalNfcEventRxEnd) &&
           instance->seg_open) {
            furi_hal_nfc_legic_collect_segment(instance, now, true);
            furi_hal_nfc_legic_drop_empty_ref(instance, &ref);
        }
        if(event & FuriHalNfcEventRxStart) {
            instance->seg_open = true;
            if(instance->seg_count < LEGIC_RX_MAX_SEGMENTS) {
                instance->seg[instance->seg_count].t_rxs = now;
            }
        }
        if(event & FuriHalNfcEventRxEnd) {
            furi_hal_nfc_legic_collect_segment(instance, now, true);
            furi_hal_nfc_legic_drop_empty_ref(instance, &ref);
        }

        const bool window_over = (int32_t)(now - instance->t_rx_window_end) > 0;
        const bool force_end = instance->next_pending && ref.reply_end_known &&
                               (int32_t)(now - ref.reply_end) > force_cycles;
        const bool gap_over = (int32_t)(now - instance->t_last_rx_event) > gap_cycles;
        if((window_over && !instance->seg_open) || force_end || gap_over ||
           (event & FuriHalNfcEventAbortRequest)) {
            if(instance->seg_open) {
                // A reception started but never ended: keep its data, then
                // stop it (STOP discards whatever is still in the FIFO).
                furi_hal_nfc_legic_collect_segment(instance, now, false);
                st25r3916_direct_cmd(&furi_hal_spi_bus_handle_nfc, ST25R3916_CMD_STOP);
            }
            instance->rx_started = false;
            if(instance->next_pending && !(event & FuriHalNfcEventAbortRequest)) {
                // A frame behind a write ACK leaves only behind a recorded
                // segment: with none, decode() finds no ACK and the frame
                // would be the WRITE sent behind a missing ACK.
                const bool recorded = instance->reply_ack_window < 0 || instance->seg_count > 0;
                if(ref.reply_end_known && ref.next_in_step && recorded) {
                    furi_hal_nfc_legic_send_next(instance, ref.reply_end);
                } else {
                    // No ACK, or not in the expected window: the queued frame
                    // would be out of step with the tag's keystream.
                    instance->next_pending = false;
                }
            }
            return FuriHalNfcEventRxEnd | (event & FuriHalNfcEventAbortRequest);
        }
    }
}

/*
 * Place the received runs into LEGIC bit windows.  Bit k occupies
 * [t0 + k*99.1 µs, t0 + (k+1)*99.1 µs) with t0 330 µs after our closing
 * pause; it is '1' when FIFO '1' samples, shifted back by the RX lag offset,
 * cover at least half of the window.  (A 40% threshold without the lag
 * misread MIM1024's card type 110110 as 111110 in 25% of replies.)
 * One pass over the samples: a sample (18.9 µs) is shorter than a window, so
 * it covers parts of at most two.
 */
static size_t furi_hal_nfc_legic_decode(
    FuriHalNfcLegicPoller* instance,
    uint8_t* out,
    size_t out_size,
    size_t max_bits) {
    memset(out, 0, out_size);
    const uint32_t t0 = instance->t_frame_end + legic_chips_to_cycles(LEGIC_TAG_FRAME_WAIT_CHIPS);
    const int32_t tb = legic_chips_to_cycles(LEGIC_TAG_BIT_CHIPS);
    const int32_t fb = legic_chips_to_cycles(LEGIC_FIFO_BIT_CHIPS);
    const int32_t lag_first = legic_chips_to_cycles(LEGIC_RX_LAG_CHIPS);
    const int32_t lag_rearm = legic_chips_to_cycles(LEGIC_RX_REARM_LAG_CHIPS);
    const int32_t rearm_short = legic_chips_to_cycles(LEGIC_RX_REARM_SHORT_CHIPS);

    size_t nbits = max_bits;
    if(nbits > out_size * BITS_IN_BYTE) nbits = out_size * BITS_IN_BYTE;
    furi_check(nbits <= LEGIC_RX_MAX_BITS);

    int32_t ones[LEGIC_RX_MAX_BITS] = {0};
    for(size_t i = 0; i < instance->seg_count; i++) {
        const LegicRxSegment* sg = &instance->seg[i];
        const bool just_rearmed = sg->rearmed && (int32_t)(sg->t_rxs - sg->t_rearm) < rearm_short;
        const int32_t s = (int32_t)(sg->t_rxs - t0) - (just_rearmed ? lag_rearm : lag_first);
        const uint8_t* samples = &instance->fifo_buf[sg->off];
        for(size_t j = 0; j < sg->bits; j++) {
            if(!((samples[j / 8] >> (j % 8)) & 1u)) continue;
            int32_t a = s + (int32_t)j * fb;
            const int32_t b = a + fb;
            if(b <= 0) continue;
            if(a < 0) a = 0;
            for(size_t k = a / tb; k < nbits && (int32_t)k * tb < b; k++) {
                const int32_t w0 = k * tb;
                const int32_t w1 = w0 + tb;
                ones[k] += MIN(b, w1) - MAX(a, w0);
            }
        }
    }
    for(size_t k = 0; k < nbits; k++) {
        if(ones[k] * 2 >= tb) {
            out[k / BITS_IN_BYTE] |= 1u << (k % BITS_IN_BYTE);
        }
    }
    return nbits;
}

static FuriHalNfcError furi_hal_nfc_legic_poller_rx(
    const FuriHalSpiBusHandle* handle,
    uint8_t* rx_data,
    size_t rx_data_size,
    size_t* rx_bits) {
    UNUSED(handle);
    FuriHalNfcLegicPoller* instance = furi_hal_nfc_legic_poller;

    *rx_bits =
        furi_hal_nfc_legic_decode(instance, rx_data, rx_data_size, instance->rx_bits_expected);

    return FuriHalNfcErrorNone;
}

/*
 * Listener (tag emulation).
 *
 * Target mode + transparent mode, as in the ISO15693 listener: the chip's IRQ
 * pin then carries the demodulated field (high during a reader pause) and
 * MOSI drives load modulation.  Reader frames are decoded from EXTI edge
 * timestamps; replies are a 212 kHz subcarrier bit-banged on MOSI by the
 * CPU on cycle-counter deadlines, started 330 µs after the reader's closing
 * pause.  digital_sequence was tried first: its ~130 µs DMA prefill latency
 * is longer than the time left after decoding a frame.
 *
 * A reply is sent inside FURI_CRITICAL, which raises BASEPRI to 5: SysTick
 * and every interrupt at priority 5-15 wait (0-4 still run) for ~1189 µs for
 * a 12-bit READ answer, ~595 µs for the 6-bit card type and ~99 µs for a
 * write ACK, plus up to 5 µs of alignment.  The ISO14443-3A and ISO15693
 * listeners hold the same section for their whole reply too, across
 * digital_sequence_transmit() (~1.5 ms for an 18-byte MIFARE Classic reply).
 * A tick is only lost when two SysTicks fall inside one section: ~0.19 per
 * READ answer on average.
 */
#define LEGIC_LISTENER_EDGES            (256U)
// A frame is over once no pause follows within the longest bit (99 µs).
#define LEGIC_LISTENER_FRAME_GAP_US     (120U)
#define LEGIC_LISTENER_MAX_FRAME_US     (4000U)
#define LEGIC_LISTENER_MAX_PAUSES       (32U)
// Reader bit = pause-to-pause interval: '0' ≈ 61 µs, '1' ≈ 99 µs.
#define LEGIC_LISTENER_BIT_MIN_US       (25U)
#define LEGIC_LISTENER_BIT_SPLIT_US     (80U)
#define LEGIC_LISTENER_BIT_MAX_US       (150U)
// Reader pauses read 12-14 µs high on the IRQ pin; shorter pulses are glitches
// (seen ~34 µs into the first bit of frames following our reply).
#define LEGIC_LISTENER_PAUSE_MIN_US     (7U)
// A second full-width "pause" sometimes shows up ~34 µs into the first bit of
// a frame; no real bit is shorter than ~59 µs, so drop pauses that follow the
// previous one too closely.
#define LEGIC_LISTENER_PAUSE_GAP_MIN_US (45U)
#define LEGIC_SUBCARRIER_HALVES         (42U) // 21 subcarrier cycles per 99.1 µs bit

// Reply start relative to the nominal 330 µs, at the centre of the window a
// Proxmark3 reader accepts (`hf legic dump`, 3 runs per offset): -34..+13 µs
// pass, -37 and +16 fail.  The PM3 samples on a 100 µs grid while tag bits
// are 99.1 µs, and our frame-end timestamp (falling edge of the last pause on
// the IRQ pin) lags the field.
#define LEGIC_LISTENER_TX_OFFSET_US   (-10)
// The last stretch before a reply is waited with interrupts masked.
#define LEGIC_LISTENER_TX_ALIGN_US    (5U)
// No reply slot is further out than this (the write ACK's 3.57 ms is the
// longest delay), so the masked wait stays within TX_ALIGN.  A slot further
// out is skipped; the reference still lands on its nominal end.
#define LEGIC_LISTENER_TX_MAX_WAIT_US (5000U)

typedef struct {
    volatile uint32_t edge_t[LEGIC_LISTENER_EDGES];
    volatile uint8_t edge_level[LEGIC_LISTENER_EDGES];
    volatile uint32_t edge_head;
    uint32_t edge_tail;
    volatile bool notified;
    FuriThreadId thread;

    uint8_t rx_data[4];
    size_t rx_bits;
    uint32_t t_frame_start;
    uint32_t t_frame_end;
    uint32_t t_tx_end; // data reply: nominal end, write ACK: measured end
    uint32_t t_ref; // keystream reference, chosen by the protocol
    uint32_t tx_delay_chips; // one-shot reply delay override, 0 = default
} FuriHalNfcLegicListener;

static FuriHalNfcLegicListener* furi_hal_nfc_legic_listener = NULL;

static uint32_t legic_fc_to_cycles(uint32_t fc) {
    const uint64_t cpu_hz = (uint64_t)furi_hal_cortex_instructions_per_microsecond() * 1000000U;
    return (uint32_t)(fc * cpu_hz / FURI_HAL_NFC_CARRIER_HZ);
}

static void furi_hal_nfc_legic_listener_edge_isr(void* context) {
    FuriHalNfcLegicListener* l = context;
    const uint32_t now = DWT->CYCCNT;
    const uint32_t i = l->edge_head;
    l->edge_t[i % LEGIC_LISTENER_EDGES] = now;
    l->edge_level[i % LEGIC_LISTENER_EDGES] = furi_hal_gpio_read(&gpio_nfc_irq_rfid_pull);
    l->edge_head = i + 1;
    if(!l->notified && l->thread) {
        l->notified = true;
        furi_thread_flags_set(l->thread, FuriHalNfcEventInternalTypeTransparentDataReceived);
    }
}

static FuriHalNfcError furi_hal_nfc_legic_listener_init(const FuriHalSpiBusHandle* handle) {
    furi_check(furi_hal_nfc_legic_listener == NULL);

    FuriHalNfcLegicListener* l = malloc(sizeof(FuriHalNfcLegicListener));
    memset(l, 0, sizeof(FuriHalNfcLegicListener));
    furi_hal_nfc_legic_listener = l;

    // Passive target, as the ISO15693 listener.
    st25r3916_change_reg_bits(
        handle,
        ST25R3916_REG_MODE,
        ST25R3916_REG_MODE_om_mask | ST25R3916_REG_MODE_tr_am,
        ST25R3916_REG_MODE_om_targ_nfca | ST25R3916_REG_MODE_tr_am_ook);
    st25r3916_change_reg_bits(
        handle,
        ST25R3916_REG_OP_CONTROL,
        ST25R3916_REG_OP_CONTROL_rx_en,
        ST25R3916_REG_OP_CONTROL_rx_en);
    st25r3916_change_reg_bits(
        handle, ST25R3916_REG_MODE, ST25R3916_REG_MODE_targ, ST25R3916_REG_MODE_targ_targ);
    FuriHalNfcError error = furi_hal_nfc_legic_common_init(handle);
    // The reader pauses reach the IRQ pin through the RX filter.  The
    // emulator's pause timing (and so its reply timing) was tuned with the
    // 300 kHz LPF and 12/80 kHz zeros; the poller's filter broke replies.
    st25r3916_write_reg(
        handle,
        ST25R3916_REG_RX_CONF1,
        ST25R3916_REG_RX_CONF1_z12k | ST25R3916_REG_RX_CONF1_h80 |
            ST25R3916_REG_RX_CONF1_lp_300khz);

    // Transparent mode: IRQ pin = demodulated field, MOSI = load modulation.
    st25r3916_direct_cmd(handle, ST25R3916_CMD_TRANSPARENT_MODE);
    furi_hal_spi_bus_handle_deinit(handle);
    furi_hal_nfc_deinit_gpio_isr();

    furi_hal_gpio_write(&gpio_spi_r_mosi, false);
    furi_hal_gpio_init(&gpio_spi_r_mosi, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_init(
        &gpio_nfc_irq_rfid_pull, GpioModeInterruptRiseFall, GpioPullDown, GpioSpeedVeryHigh);
    furi_hal_gpio_add_int_callback(
        &gpio_nfc_irq_rfid_pull, furi_hal_nfc_legic_listener_edge_isr, l);
    furi_hal_gpio_enable_int_callback(&gpio_nfc_irq_rfid_pull);

    return error;
}

static FuriHalNfcError furi_hal_nfc_legic_listener_deinit(const FuriHalSpiBusHandle* handle) {
    FuriHalNfcLegicListener* l = furi_hal_nfc_legic_listener;
    furi_check(l);

    furi_hal_gpio_remove_int_callback(&gpio_nfc_irq_rfid_pull);
    furi_hal_nfc_init_gpio_isr();
    furi_hal_spi_bus_handle_init(handle);
    st25r3916_direct_cmd(handle, ST25R3916_CMD_UNMASK_RECEIVE_DATA);

    free(l);
    furi_hal_nfc_legic_listener = NULL;

    return FuriHalNfcErrorNone;
}

void furi_hal_nfc_legic_listener_set_ref_to_frame_end(uint32_t extra_fc) {
    FuriHalNfcLegicListener* l = furi_hal_nfc_legic_listener;
    furi_check(l);
    l->t_ref = l->t_frame_end + legic_fc_to_cycles(extra_fc);
}

void furi_hal_nfc_legic_listener_set_ref_to_tx_end(void) {
    FuriHalNfcLegicListener* l = furi_hal_nfc_legic_listener;
    furi_check(l);
    l->t_ref = l->t_tx_end;
}

uint32_t furi_hal_nfc_legic_listener_get_elapsed_fc(void) {
    FuriHalNfcLegicListener* l = furi_hal_nfc_legic_listener;
    furi_check(l);
    // Subtract in cycles first, so a cycle counter wrap in between cancels.
    const uint64_t cycles = (uint32_t)(l->t_frame_start - l->t_ref);
    const uint64_t cpu_hz = (uint64_t)furi_hal_cortex_instructions_per_microsecond() * 1000000U;
    return (uint32_t)(cycles * FURI_HAL_NFC_CARRIER_HZ / cpu_hz);
}

void furi_hal_nfc_legic_listener_set_tx_delay(uint32_t chips) {
    furi_check(furi_hal_nfc_legic_listener);
    furi_hal_nfc_legic_listener->tx_delay_chips = chips;
}

// Decode one reader frame from the edges in [edge_tail, head).  Runs between
// the end of the reader frame and our reply.
static bool furi_hal_nfc_legic_listener_decode(FuriHalNfcLegicListener* l, uint32_t head) {
    const int32_t mhz = furi_hal_cortex_instructions_per_microsecond();
    uint32_t tail = l->edge_tail;
    if(head - tail > LEGIC_LISTENER_EDGES) tail = head - LEGIC_LISTENER_EDGES;
    l->edge_tail = head;

    uint32_t starts[LEGIC_LISTENER_MAX_PAUSES];
    size_t n_starts = 0;
    uint32_t last_fall = 0;
    bool have_fall = false;
    uint32_t rise = 0;
    bool in_pause = false;
    for(uint32_t i = tail; i < head; i++) {
        const uint32_t t = l->edge_t[i % LEGIC_LISTENER_EDGES];
        if(l->edge_level[i % LEGIC_LISTENER_EDGES]) {
            rise = t;
            in_pause = true;
        } else if(in_pause) {
            in_pause = false;
            const int32_t width_us = (int32_t)(t - rise) / mhz;
            if(width_us < (int32_t)LEGIC_LISTENER_PAUSE_MIN_US) continue;
            if(n_starts && (int32_t)(rise - starts[n_starts - 1]) / mhz <
                               (int32_t)LEGIC_LISTENER_PAUSE_GAP_MIN_US) {
                continue;
            }
            if(n_starts < LEGIC_LISTENER_MAX_PAUSES) starts[n_starts++] = rise;
            last_fall = t;
            have_fall = true;
        }
    }

    memset(l->rx_data, 0, sizeof(l->rx_data));
    l->rx_bits = 0;

    if(n_starts < 2) return false;

    for(size_t k = 1; k < n_starts; k++) {
        const int32_t d_us = (int32_t)(starts[k] - starts[k - 1]) / mhz;
        if(d_us < (int32_t)LEGIC_LISTENER_BIT_MIN_US ||
           d_us > (int32_t)LEGIC_LISTENER_BIT_MAX_US ||
           l->rx_bits >= sizeof(l->rx_data) * BITS_IN_BYTE) {
            return false;
        }
        if(d_us > (int32_t)LEGIC_LISTENER_BIT_SPLIT_US) {
            l->rx_data[l->rx_bits / BITS_IN_BYTE] |= 1u << (l->rx_bits % BITS_IN_BYTE);
        }
        l->rx_bits++;
    }

    l->t_frame_start = starts[0];
    l->t_frame_end = have_fall ?
                         last_fall :
                         starts[n_starts - 1] + legic_chips_to_cycles(LEGIC_PAUSE_CHIP_BITS);
    return true;
}

static FuriHalNfcEvent furi_hal_nfc_legic_listener_wait_event(uint32_t timeout_ms) {
    FuriHalNfcLegicListener* l = furi_hal_nfc_legic_listener;
    const uint32_t mhz = furi_hal_cortex_instructions_per_microsecond();
    l->thread = furi_thread_get_current_id();

    while(true) {
        uint32_t flag = furi_thread_flags_wait(
            FuriHalNfcEventInternalTypeAbort | FuriHalNfcEventInternalTypeTransparentDataReceived,
            FuriFlagWaitAny,
            timeout_ms);
        if(flag & FuriFlagError) return FuriHalNfcEventTimeout;
        furi_thread_flags_clear(flag);
        if(flag & FuriHalNfcEventInternalTypeAbort) return FuriHalNfcEventAbortRequest;

        // A frame started: wait until the line has been quiet for a while.
        // This spins (interrupts on) for the rest of the reader frame, up to
        // ~1 ms.  Waking on every edge instead would not remove it: the frame
        // ends 120 µs after its last edge, when no edge comes to wake the
        // thread, and flag-wait timeouts are whole 1 ms ticks.  Blocking
        // would need a one-pulse timer re-armed by each edge.
        const uint32_t t_begin = DWT->CYCCNT;
        while(true) {
            const uint32_t head = l->edge_head;
            const uint32_t now = DWT->CYCCNT;
            const uint32_t last = head ? l->edge_t[(head - 1) % LEGIC_LISTENER_EDGES] : t_begin;
            if(now - last > LEGIC_LISTENER_FRAME_GAP_US * mhz) break;
            if(now - t_begin > LEGIC_LISTENER_MAX_FRAME_US * mhz) break;
        }
        l->notified = false;
        if(furi_hal_nfc_legic_listener_decode(l, l->edge_head)) return FuriHalNfcEventRxEnd;
    }
}

static FuriHalNfcError furi_hal_nfc_legic_listener_tx(
    const FuriHalSpiBusHandle* handle,
    const uint8_t* tx_data,
    size_t tx_bits) {
    UNUSED(handle);
    FuriHalNfcLegicListener* l = furi_hal_nfc_legic_listener;
    const uint32_t mhz = furi_hal_cortex_instructions_per_microsecond();

    // Reply starts 330 µs after the end of the reader's closing pause (a
    // write ACK ~3.6 ms after it, see set_tx_delay).  TX_OFFSET only moves
    // when a normal reply is modulated.  The reply end kept as the keystream
    // reference stays nominal (330 µs + 99.1 µs per bit), because a reader
    // sends its next frame on its own grid, not after the reply it sampled;
    // counted from the advanced end, a Flipper reader's next frame was only
    // ~12 µs from a fourth keystream slot.  A write ACK is not advanced: the
    // -34..+13 µs window was measured on data replies, and a Flipper reader
    // times the frame after an ACK from the ACK it measured, so moving the
    // ACK would move that frame against the reference.  For the same reason
    // a write ACK keeps its measured end, also when it went out late.
    const bool delayed = l->tx_delay_chips != 0;
    const uint32_t delay_chips = delayed ? l->tx_delay_chips : LEGIC_TAG_FRAME_WAIT_CHIPS;
    l->tx_delay_chips = 0;
    const uint32_t reply_start = l->t_frame_end + legic_chips_to_cycles(delay_chips);
    const uint32_t start =
        reply_start + (delayed ? 0 : LEGIC_LISTENER_TX_OFFSET_US * (int32_t)mhz);
    const int32_t wait = (int32_t)(start - DWT->CYCCNT);
    if(wait >= (int32_t)(LEGIC_LISTENER_TX_MAX_WAIT_US * mhz)) {
        // Only a stale frame end or a bogus delay gets here: skip the reply.
        // The protocol takes t_tx_end as its keystream reference regardless
        // (it already stepped the PRNG for every reply bit), so leave the
        // nominal end of the reply slot, as a sent reply would.
        l->t_tx_end = reply_start + legic_chips_to_cycles((uint32_t)tx_bits * LEGIC_TAG_BIT_CHIPS);
        return FuriHalNfcErrorCommunicationTimeout;
    }
    if(wait > (int32_t)(LEGIC_LISTENER_TX_ALIGN_US * mhz)) {
        while((int32_t)(start - (LEGIC_LISTENER_TX_ALIGN_US * mhz) - DWT->CYCCNT) > 0) {
        }
    }

    // Half-period of the 212 kHz subcarrier (32/fc) in cycles, Q8 fixed point.
    const uint32_t half_q8 = (uint32_t)(32ULL * 256U * mhz * 1000000ULL / 13560000ULL);
    const uint32_t t_start = wait > 0 ? start : DWT->CYCCNT;
    uint32_t acc_q8 = 0;
    FURI_CRITICAL_ENTER();
    while((int32_t)(t_start - DWT->CYCCNT) > 0) {
    }
    for(size_t i = 0; i < tx_bits; i++) {
        const bool bit = (tx_data[i / BITS_IN_BYTE] >> (i % BITS_IN_BYTE)) & 1u;
        for(uint32_t h = 0; h < LEGIC_SUBCARRIER_HALVES; h++) {
            furi_hal_gpio_write(&gpio_spi_r_mosi, bit && !(h & 1u));
            acc_q8 += half_q8;
            const uint32_t next = t_start + (acc_q8 >> 8);
            while((int32_t)(next - DWT->CYCCNT) > 0) {
            }
        }
    }
    furi_hal_gpio_write(&gpio_spi_r_mosi, false);
    FURI_CRITICAL_EXIT();
    l->t_tx_end = (delayed ? t_start : reply_start) + (acc_q8 >> 8);

    // Drop edges caused by our own load modulation.
    furi_delay_us(10);
    l->edge_tail = l->edge_head;

    return FuriHalNfcErrorNone;
}

static FuriHalNfcError furi_hal_nfc_legic_listener_rx(
    const FuriHalSpiBusHandle* handle,
    uint8_t* rx_data,
    size_t rx_data_size,
    size_t* rx_bits) {
    UNUSED(handle);
    FuriHalNfcLegicListener* l = furi_hal_nfc_legic_listener;
    const size_t bytes = (l->rx_bits + 7) / BITS_IN_BYTE;
    if(bytes > rx_data_size) return FuriHalNfcErrorBufferOverflow;
    memcpy(rx_data, l->rx_data, bytes);
    *rx_bits = l->rx_bits;
    return FuriHalNfcErrorNone;
}

static FuriHalNfcError furi_hal_nfc_legic_listener_noop(const FuriHalSpiBusHandle* handle) {
    UNUSED(handle);
    return FuriHalNfcErrorNone;
}

const FuriHalNfcTechBase furi_hal_nfc_legic = {
    .poller =
        {
            .compensation =
                {
                    .fdt = FURI_HAL_NFC_POLLER_FDT_COMP_FC,
                    .fwt = FURI_HAL_NFC_LEGIC_POLLER_FWT_COMP_FC,
                },
            .init = furi_hal_nfc_legic_poller_init,
            .deinit = furi_hal_nfc_legic_poller_deinit,
            .wait_event = furi_hal_nfc_legic_poller_wait_event,
            .tx = furi_hal_nfc_legic_poller_tx,
            .rx = furi_hal_nfc_legic_poller_rx,
            .trx_reset = furi_hal_nfc_legic_poller_trx_reset,
        },
    .listener =
        {
            .compensation = {0},
            .init = furi_hal_nfc_legic_listener_init,
            .deinit = furi_hal_nfc_legic_listener_deinit,
            .wait_event = furi_hal_nfc_legic_listener_wait_event,
            .tx = furi_hal_nfc_legic_listener_tx,
            .rx = furi_hal_nfc_legic_listener_rx,
            .sleep = furi_hal_nfc_legic_listener_noop,
            .idle = furi_hal_nfc_legic_listener_noop,
        },
};
