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

// Chip-bit buffers (x4 bytes).  The longest transmission is the chained ACK
// + MIM1024 WRITE: 6 + 23 bits of up to 21 chips, two closing pauses and the
// 47-chip gap = 664 chips; 16 x 4 bytes (512 chips) cut the WRITE short.
#define FURI_HAL_NFC_LEGIC_POLLER_MAX_BUFFER_SIZE (24U)
#define FURI_HAL_NFC_LEGIC_FIFO_SIZE              (512U)

#define FURI_HAL_NFC_LEGIC_POLLER_FWT_COMP_FC (-1300)

#define BITS_IN_BYTE (8U)

// All LEGIC timing is in units of one 212 kHz subcarrier period (64/fc,
// 4.72 µs), the same tick Proxmark uses.
#define LEGIC_TAG_FRAME_WAIT_CHIPS    (70U) // 330 µs from closing pause to reply
#define LEGIC_TAG_BIT_CHIPS           (21U) // 99.1 µs per reply bit
#define LEGIC_FIFO_BIT_CHIPS          (4U) // scp_8 report period, measured
// Detection lag between subcarrier onset and the reported reception start:
// ~25 µs for the first reception after TX and for a receiver re-armed long
// before the run, ~40 µs when re-armed within ~100 µs of the onset.
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
// After a write ACK, timed from the ACK's measured end (which jitters by up to
// ~8 µs): Proxmark3's sim takes the next frame up to 260-280 µs later and a
// real MIM256 up to ~260 µs.
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

#define LEGIC_RX_GAP_MS       (2U)
#define LEGIC_RX_MAX_SEGMENTS (16U)
#define LEGIC_RX_WINDOW_CHIPS (LEGIC_TAG_FRAME_WAIT_CHIPS + 23U * LEGIC_TAG_BIT_CHIPS + 40U)

// Poller first-stage AM gain reduction, rg1_am = 2 (the default, "reduced
// gain").  Full gain lost runs of a strong tag (PM3 `hf legic sim`: 43%
// clean reads, all missed '1's); rg1_am 1/2/3/4/5/6 gave 91/95/93/83/76/61%.
// A real MIM256 reads 97-98% clean at both rg1_am 0 and 4.  A more weakly
// coupled simulator lost the card-type reply at rg1_am 2 but not at full
// gain, so the poller alternates the two after failures
// (furi_hal_nfc_legic_poller_set_rx_reduced_gain).
#define LEGIC_RX_CONF3_POLLER (0x40U)
#define LEGIC_RX_CONF3_FULL   (0x00U)

typedef struct {
    uint8_t frame_buf[FURI_HAL_NFC_LEGIC_POLLER_MAX_BUFFER_SIZE * 4];
    size_t frame_buf_bits;
    bool rx_reduced_gain;
    bool rx_gain_applied;
    // Queued next frame (furi_hal_nfc_legic_poller_set_next).
    uint8_t next_buf[FURI_HAL_NFC_LEGIC_POLLER_MAX_BUFFER_SIZE * 4];
    // The poller frame tx() must match to claim a queued frame: recorded at
    // queue time and copied to the in-flight record when it is sent (the
    // poller queues frame k+1 before claiming frame k).
    uint8_t next_claim[4];
    size_t next_claim_bits;
    uint8_t sent_claim[4];
    size_t sent_claim_bits;
    size_t next_chip_bits;
    bool next_pending;
    bool next_sent;
    bool next_txe_seen;
    bool synth_txe;
    uint32_t next_tx_cmd;
    uint32_t next_frame_end;
    // TxEnd still owed for the frame of the current exchange.
    bool txe_pending;
    uint8_t fifo_buf[FURI_HAL_NFC_LEGIC_FIFO_SIZE];
    size_t fifo_buf_bits;
    // Current frame timing (DWT cycles).
    uint32_t t_tx_cmd;
    uint32_t t_frame_end;
    size_t rx_bits_expected;
    // The next exchange's reply is a write ACK: one bit at a time set by the
    // tag.  Requested by set_rx_ack(), latched by tx() for that exchange only.
    bool rx_ack;
    uint8_t rx_ack_window;
    bool reply_is_ack;
    uint8_t reply_ack_window; // the bit window a queued frame was built for
    // RX segments of the current exchange: one per reception.
    bool rx_started;
    bool seg_open;
    uint32_t t_last_rx_event;
    size_t seg_count;
    uint32_t seg_t_rxs[LEGIC_RX_MAX_SEGMENTS];
    uint32_t seg_t_rxe[LEGIC_RX_MAX_SEGMENTS];
    uint16_t seg_bits[LEGIC_RX_MAX_SEGMENTS];
    uint16_t seg_off[LEGIC_RX_MAX_SEGMENTS];
} FuriHalNfcLegicPoller;

static FuriHalNfcLegicPoller* furi_hal_nfc_legic_poller = NULL;

static uint32_t legic_chips_to_cycles(uint32_t chips) {
    const uint64_t cpu_hz = (uint64_t)furi_hal_cortex_instructions_per_microsecond() * 1000000U;
    return (uint32_t)(chips * 64U * cpu_hz / 13560000U);
}

static int32_t legic_cycles_to_us(int32_t cycles) {
    return cycles / (int32_t)furi_hal_cortex_instructions_per_microsecond();
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
    furi_hal_nfc_legic_poller->rx_bits_expected = bits;
}

void furi_hal_nfc_legic_poller_set_rx_ack(size_t window) {
    furi_check(furi_hal_nfc_legic_poller);
    furi_check(window <= UINT8_MAX);
    furi_hal_nfc_legic_poller->rx_ack = true;
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
    for(size_t i = 0; i < count; i++) {
        if(w->bit_pos >= sizeof(w->buf) * 8) return; // full
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
    const uint8_t* claim = (second && second_bits) ? second : first;
    instance->next_claim_bits = (second && second_bits) ? second_bits : first_bits;
    furi_check(instance->next_claim_bits <= sizeof(instance->next_claim) * 8);
    memcpy(instance->next_claim, claim, (instance->next_claim_bits + 7) / 8);
    instance->next_chip_bits = w.bit_pos;
    // next_sent refers to the previous queued frame, which may still be on
    // air waiting to be claimed; leave it alone.
    instance->next_pending = true;
}

bool furi_hal_nfc_legic_poller_next_sent(void) {
    furi_check(furi_hal_nfc_legic_poller);
    return furi_hal_nfc_legic_poller->next_sent;
}

bool furi_hal_nfc_legic_poller_tx_in_flight(void) {
    return furi_hal_nfc_legic_poller && furi_hal_nfc_legic_poller->next_sent;
}

static FuriHalNfcError furi_hal_nfc_legic_poller_tx(
    const FuriHalSpiBusHandle* handle,
    const uint8_t* tx_data,
    size_t tx_bits) {
    FuriHalNfcLegicPoller* instance = furi_hal_nfc_legic_poller;
    instance->reply_is_ack = instance->rx_ack;
    instance->reply_ack_window = instance->rx_ack_window;
    instance->rx_ack = false;

    // Compare bit by bit: bytes past tx_bits may hold stale data.
    bool claims_queued = instance->next_sent && tx_bits == instance->sent_claim_bits;
    for(size_t i = 0; claims_queued && i < tx_bits; i++) {
        claims_queued = ((tx_data[i / 8] ^ instance->sent_claim[i / 8]) >> (i % 8) & 1u) == 0;
    }
    // A queued frame left over from an abandoned session must not swallow a
    // new frame (it would never be transmitted).
    instance->next_sent = instance->next_sent && claims_queued;
    if(claims_queued) {
        // Queued frame already on air (sent from the previous exchange's
        // receive loop): only set up the bookkeeping for its reply.
        instance->next_sent = false;
        instance->t_tx_cmd = instance->next_tx_cmd;
        instance->t_frame_end = instance->next_frame_end;
        instance->rx_started = false;
        instance->seg_open = false;
        instance->seg_count = 0;
        instance->fifo_buf_bits = 0;
        // Its TxEnd may already have been consumed by the block-TX wait loop.
        instance->synth_txe = instance->next_txe_seen;
        instance->txe_pending = !instance->next_txe_seen;
        instance->next_txe_seen = false;
        return FuriHalNfcErrorNone;
    }
    // A frame queued with set_next() before this tx() stays queued: it goes
    // out after this frame's reply.
    instance->synth_txe = false;

    LegicChipBitWriter w = {0};
    // tx_data is LSB-first: frame bit i is bit (i % 8) of byte i / 8.
    legic_encode_frame(&w, tx_data, tx_bits);

    // Round up to whole bytes for the chip FIFO; padding chip-bits are 0
    // (carrier on).
    instance->frame_buf_bits = ((w.bit_pos + 7) / 8) * 8;
    memcpy(instance->frame_buf, w.buf, instance->frame_buf_bits / 8);

    instance->rx_started = false;
    instance->seg_open = false;
    instance->seg_count = 0;
    instance->fifo_buf_bits = 0;
    if(instance->rx_reduced_gain != instance->rx_gain_applied) {
        st25r3916_write_reg(
            handle,
            ST25R3916_REG_RX_CONF3,
            instance->rx_reduced_gain ? LEGIC_RX_CONF3_POLLER : LEGIC_RX_CONF3_FULL);
        instance->rx_gain_applied = instance->rx_reduced_gain;
    }

    // Same preparation as furi_hal_nfc_poller_tx_common.
    st25r3916_direct_cmd(handle, ST25R3916_CMD_CLEAR_FIFO);
    st25r3916_clear_reg_bits(
        handle, ST25R3916_REG_TIMER_EMV_CONTROL, ST25R3916_REG_TIMER_EMV_CONTROL_nrt_emv);
    st25r3916_change_reg_bits(
        handle,
        ST25R3916_REG_ISO14443A_NFC,
        (ST25R3916_REG_ISO14443A_NFC_no_tx_par | ST25R3916_REG_ISO14443A_NFC_no_rx_par),
        (ST25R3916_REG_ISO14443A_NFC_no_tx_par_off | ST25R3916_REG_ISO14443A_NFC_no_rx_par_off));
    uint32_t interrupts =
        (ST25R3916_IRQ_MASK_FWL | ST25R3916_IRQ_MASK_TXE | ST25R3916_IRQ_MASK_RXS |
         ST25R3916_IRQ_MASK_RXE | ST25R3916_IRQ_MASK_PAR | ST25R3916_IRQ_MASK_CRC |
         ST25R3916_IRQ_MASK_ERR1 | ST25R3916_IRQ_MASK_ERR2 | ST25R3916_IRQ_MASK_NRE);
    st25r3916_get_irq(handle);
    st25r3916_mask_irq(handle, ~interrupts);
    st25r3916_write_fifo(handle, instance->frame_buf, instance->frame_buf_bits);

    const uint32_t cmd_latency =
        LEGIC_TX_CMD_LATENCY_US * furi_hal_cortex_instructions_per_microsecond();
    instance->t_tx_cmd = DWT->CYCCNT;
    st25r3916_direct_cmd(handle, ST25R3916_CMD_TRANSMIT_WITHOUT_CRC);
    instance->txe_pending = true;
    instance->t_frame_end = instance->t_tx_cmd + cmd_latency + legic_chips_to_cycles(w.bit_pos);

    return FuriHalNfcErrorNone;
}

static uint32_t furi_hal_nfc_legic_rx_window_end(FuriHalNfcLegicPoller* instance) {
    uint32_t chips =
        instance->rx_bits_expected ?
            LEGIC_TAG_FRAME_WAIT_CHIPS + instance->rx_bits_expected * LEGIC_TAG_BIT_CHIPS + 10U :
            LEGIC_RX_WINDOW_CHIPS;
    return instance->t_frame_end + legic_chips_to_cycles(chips);
}

// Append the FIFO content of one finished reception to fifo_buf and re-arm
// the receiver for the next run of '1' bits.
static void
    furi_hal_nfc_legic_collect_segment(FuriHalNfcLegicPoller* instance, uint32_t now, bool rearm) {
    const FuriHalSpiBusHandle* handle = &furi_hal_spi_bus_handle_nfc;
    uint8_t seg[64];
    size_t seg_bits = 0;
    st25r3916_read_fifo(handle, seg, sizeof(seg), &seg_bits);
    instance->seg_open = false;

    if(rearm && (int32_t)(now - furi_hal_nfc_legic_rx_window_end(instance)) < 0) {
        st25r3916_direct_cmd(handle, ST25R3916_CMD_UNMASK_RECEIVE_DATA);
    }

    // Byte-align each segment in fifo_buf so segments stay readable in the log.
    size_t pos = (instance->fifo_buf_bits + 7) / BITS_IN_BYTE;
    size_t seg_bytes = (seg_bits + 7) / BITS_IN_BYTE;
    if(seg_bytes > sizeof(seg)) seg_bytes = sizeof(seg);
    if(instance->seg_count >= LEGIC_RX_MAX_SEGMENTS) return;
    if(pos + seg_bytes > sizeof(instance->fifo_buf)) return;

    memcpy(&instance->fifo_buf[pos], seg, seg_bytes);
    instance->fifo_buf_bits = pos * BITS_IN_BYTE + seg_bits;
    instance->seg_t_rxe[instance->seg_count] = now;
    instance->seg_bits[instance->seg_count] = seg_bits;
    instance->seg_off[instance->seg_count] = pos;
    instance->seg_count++;
}

// Bit window of a write ACK, counted like reply bits from 330 µs after our
// frame: the window start nearest to its first reception, as the decoder's
// half-coverage rule places a one-bit run.  -1 if it started too early.
static int32_t furi_hal_nfc_legic_ack_window(FuriHalNfcLegicPoller* instance) {
    const uint32_t t0 = instance->t_frame_end + legic_chips_to_cycles(LEGIC_TAG_FRAME_WAIT_CHIPS);
    const int32_t tb = legic_chips_to_cycles(LEGIC_TAG_BIT_CHIPS);
    const int32_t start =
        (int32_t)(instance->seg_t_rxs[0] - legic_chips_to_cycles(LEGIC_RX_LAG_CHIPS) - t0);
    return start < -tb / 2 ? -1 : (start + tb / 2) / tb;
}

// End of the current tag reply.  A data reply starts 330 µs after our frame
// and its length is known, so it ends on the tag's bit grid.  A write ACK is a
// single bit that comes when the tag has finished writing; its end is taken
// from its reception, as tags time the next frame from their own ACK and
// don't all place it on the grid (Proxmark3's sim sends it ~23 µs early).
static bool furi_hal_nfc_legic_reply_end(FuriHalNfcLegicPoller* instance, uint32_t* end) {
    if(!instance->reply_is_ack) {
        *end = instance->t_frame_end +
               legic_chips_to_cycles(
                   LEGIC_TAG_FRAME_WAIT_CHIPS + instance->rx_bits_expected * LEGIC_TAG_BIT_CHIPS);
        return true;
    }
    if(!instance->rx_started || furi_hal_nfc_legic_ack_window(instance) < 0) return false;
    *end = instance->seg_t_rxs[0] - legic_chips_to_cycles(LEGIC_RX_LAG_CHIPS) +
           legic_chips_to_cycles(LEGIC_TAG_BIT_CHIPS);
    return true;
}

// A frame queued behind a write ACK was built for the bit window the poller
// expected the ACK in: the tag steps its keystream once per window it spends
// writing, so in any other window the frame would reach the tag garbled (and
// a garbled WRITE passes its CRC-4 one time in 16).
static bool furi_hal_nfc_legic_next_in_step(FuriHalNfcLegicPoller* instance) {
    return !instance->reply_is_ack ||
           furi_hal_nfc_legic_ack_window(instance) == instance->reply_ack_window;
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

    const uint32_t gap_us = instance->reply_is_ack ? LEGIC_NEXT_FRAME_AFTER_ACK_US :
                                                     LEGIC_NEXT_FRAME_US;
    const uint32_t cmd_at = reply_end + gap_us * mhz - cmd_latency;

    const size_t fifo_bits = ((instance->next_chip_bits + 7) / 8) * 8;
    const size_t fifo_bytes = fifo_bits / 8;
    const size_t head_bytes = MIN(fifo_bytes, LEGIC_NEXT_FIFO_HEAD_BYTES);
    // The receiver may have been re-armed after the reply's last run; a
    // transmission started with it armed got cut off after a few bits.
    st25r3916_direct_cmd(handle, ST25R3916_CMD_STOP);
    st25r3916_direct_cmd(handle, ST25R3916_CMD_CLEAR_FIFO);
    st25r3916_write_reg(handle, ST25R3916_REG_NUM_TX_BYTES2, (uint8_t)(fifo_bits & 0xFFU));
    st25r3916_write_reg(handle, ST25R3916_REG_NUM_TX_BYTES1, (uint8_t)((fifo_bits >> 8) & 0xFFU));
    st25r3916_reg_write_fifo(handle, instance->next_buf, head_bytes);
    if((int32_t)(cmd_at - DWT->CYCCNT) <= 0) {
        // Too late: the tag would read it in the wrong keystream slot.
        return;
    }
    while((int32_t)(cmd_at - DWT->CYCCNT) > 0) {
    }
    instance->next_tx_cmd = DWT->CYCCNT;
    st25r3916_direct_cmd(handle, ST25R3916_CMD_TRANSMIT_WITHOUT_CRC);
    if(fifo_bytes > head_bytes) {
        st25r3916_reg_write_fifo(handle, &instance->next_buf[head_bytes], fifo_bytes - head_bytes);
    }
    instance->next_frame_end =
        instance->next_tx_cmd + cmd_latency + legic_chips_to_cycles(instance->next_chip_bits);
    memcpy(instance->sent_claim, instance->next_claim, sizeof(instance->sent_claim));
    instance->sent_claim_bits = instance->next_claim_bits;
    instance->next_txe_seen = false;
    instance->next_sent = true;
}

static FuriHalNfcEvent furi_hal_nfc_legic_poller_wait_event(uint32_t timeout_ms) {
    FuriHalNfcLegicPoller* instance = furi_hal_nfc_legic_poller;

    if(instance->synth_txe) {
        instance->synth_txe = false;
        return FuriHalNfcEventTxEnd;
    }

    if(!instance->rx_started) {
        // Never block forever on a TxEnd: the transceive state machine only
        // starts its frame-wait timer after TxEnd, so a lost one would hang.
        const bool owed = instance->txe_pending;
        FuriHalNfcEvent event =
            furi_hal_nfc_wait_event_common(owed ? LEGIC_TXE_MAX_WAIT_MS : timeout_ms);
        uint32_t now = DWT->CYCCNT;
        if(owed && (event & FuriHalNfcEventTimeout)) {
            instance->txe_pending = false;
            return FuriHalNfcEventTxEnd;
        }
        if(event & FuriHalNfcEventTxEnd) instance->txe_pending = false;
        if((event & FuriHalNfcEventTxEnd) && instance->next_sent) {
            // TxEnd of a queued frame before the poller claimed it.
            instance->next_txe_seen = true;
        }
        if(event & FuriHalNfcEventRxStart) {
            instance->rx_started = true;
            instance->seg_open = true;
            instance->seg_t_rxs[0] = now;
            instance->t_last_rx_event = now;
            if(event & FuriHalNfcEventRxEnd)
                furi_hal_nfc_legic_collect_segment(instance, now, true);
            // Report only RxStart now; RxEnd is reported once collection ends.
            event &= ~FuriHalNfcEventRxEnd;
        }
        return event;
    }

    // Collecting: swallow further RxStart/RxEnd until the reply window has
    // passed or the line stays quiet.  Poll without blocking: a 1 ms tick
    // wait here delays the next frame, and real tags drop the session if the
    // reader waits too long (PM3 uses 220 µs gaps).
    while(true) {
        // flags_wait() with a zero timeout returns FuriFlagErrorResource (all
        // bits set) when nothing is pending, which wait_event_common would
        // read as every event including abort; peek first.
        FuriHalNfcEvent event = 0;
        if(furi_thread_flags_get() & FURI_HAL_NFC_EVENT_INTERNAL_ALL) {
            event = furi_hal_nfc_wait_event_common(0);
        }
        uint32_t now = DWT->CYCCNT;
        if(event & (FuriHalNfcEventRxStart | FuriHalNfcEventRxEnd))
            instance->t_last_rx_event = now;
        if(event & FuriHalNfcEventRxStart) {
            instance->seg_open = true;
            if(instance->seg_count < LEGIC_RX_MAX_SEGMENTS) {
                instance->seg_t_rxs[instance->seg_count] = now;
            }
        }
        if(event & FuriHalNfcEventRxEnd) furi_hal_nfc_legic_collect_segment(instance, now, true);

        const int32_t past_window = (int32_t)(now - furi_hal_nfc_legic_rx_window_end(instance));
        bool window_over = past_window > 0;
        uint32_t reply_end = 0;
        const bool reply_end_known = furi_hal_nfc_legic_reply_end(instance, &reply_end);
        bool force_end =
            instance->next_pending && reply_end_known &&
            (int32_t)(now - reply_end) >
                (int32_t)(LEGIC_RX_FORCE_END_US * furi_hal_cortex_instructions_per_microsecond());
        bool gap_over = legic_cycles_to_us(now - instance->t_last_rx_event) >
                        (int32_t)(LEGIC_RX_GAP_MS * 1000U);
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
                if(reply_end_known && furi_hal_nfc_legic_next_in_step(instance)) {
                    furi_hal_nfc_legic_send_next(instance, reply_end);
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
 * pause; it is '1' when FIFO '1' samples, shifted back by the detection lag,
 * cover at least half of the window.  (A 40% threshold without the lag
 * misread MIM1024's card type 110110 as 111110 in 25% of replies.)
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

    size_t last_one = 0;
    for(size_t k = 0; k < nbits; k++) {
        const int32_t w0 = k * tb;
        const int32_t w1 = w0 + tb;
        int32_t ones = 0;
        for(size_t i = 0; i < instance->seg_count; i++) {
            const bool just_rearmed =
                i && (int32_t)(instance->seg_t_rxs[i] - instance->seg_t_rxe[i - 1]) < rearm_short;
            int32_t s =
                (int32_t)(instance->seg_t_rxs[i] - t0) - (just_rearmed ? lag_rearm : lag_first);
            const uint8_t* seg = &instance->fifo_buf[instance->seg_off[i]];
            for(size_t j = 0; j < instance->seg_bits[i]; j++) {
                if(!((seg[j / 8] >> (j % 8)) & 1u)) continue;
                int32_t a = s + (int32_t)j * fb;
                int32_t b = a + fb;
                if(a < w0) a = w0;
                if(b > w1) b = w1;
                if(b > a) ones += b - a;
            }
        }
        if(ones * 2 >= tb) {
            out[k / BITS_IN_BYTE] |= 1u << (k % BITS_IN_BYTE);
            last_one = k + 1;
        }
    }
    return instance->rx_bits_expected ? nbits : last_one;
}

static FuriHalNfcError furi_hal_nfc_legic_poller_rx(
    const FuriHalSpiBusHandle* handle,
    uint8_t* rx_data,
    size_t rx_data_size,
    size_t* rx_bits) {
    UNUSED(handle);
    FuriHalNfcLegicPoller* instance = furi_hal_nfc_legic_poller;

    size_t max_bits = instance->rx_bits_expected ? instance->rx_bits_expected : 23U;
    *rx_bits = furi_hal_nfc_legic_decode(instance, rx_data, rx_data_size, max_bits);

    return FuriHalNfcErrorNone;
}

/*
 * Listener (tag emulation).
 *
 * Target mode + transparent mode, as in the ISO15693 listener: the chip's IRQ
 * pin then carries the demodulated field (high during a reader pause) and
 * MOSI drives load modulation.  Reader frames are decoded from EXTI edge
 * timestamps; replies are a 212 kHz subcarrier bit-banged on MOSI by the
 * CPU (cycle-counter deadlines, interrupts off), started 330 µs after the
 * reader's closing pause.  digital_sequence was tried first: its ~130 µs
 * DMA prefill latency is longer than the time left after decoding a frame.
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
#define LEGIC_LISTENER_TX_OFFSET_US (-10)

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
    uint32_t t_tx_end;
    uint32_t tx_delay_chips; // one-shot reply delay override, 0 = default
} FuriHalNfcLegicListener;

static FuriHalNfcLegicListener* furi_hal_nfc_legic_listener = NULL;

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

uint32_t furi_hal_nfc_legic_listener_get_frame_start(void) {
    furi_check(furi_hal_nfc_legic_listener);
    return furi_hal_nfc_legic_listener->t_frame_start;
}

uint32_t furi_hal_nfc_legic_listener_get_frame_end(void) {
    furi_check(furi_hal_nfc_legic_listener);
    return furi_hal_nfc_legic_listener->t_frame_end;
}

uint32_t furi_hal_nfc_legic_listener_get_tx_end(void) {
    furi_check(furi_hal_nfc_legic_listener);
    return furi_hal_nfc_legic_listener->t_tx_end;
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
    // write ACK ~3.6 ms after it, see set_tx_delay).
    // The early start (TX_OFFSET) only applies to normal replies.  A write
    // ACK goes out on legicrfsim.c's timing: 20 µs early, the gap to the
    // reader's next frame (sent ~220 µs after it samples the ACK) crossed
    // into a third keystream slot and the tag fell out of step.
    const bool delayed = l->tx_delay_chips != 0;
    const uint32_t delay_chips = delayed ? l->tx_delay_chips : LEGIC_TAG_FRAME_WAIT_CHIPS;
    l->tx_delay_chips = 0;
    const uint32_t start = l->t_frame_end + legic_chips_to_cycles(delay_chips) +
                           (delayed ? 0 : LEGIC_LISTENER_TX_OFFSET_US * (int32_t)mhz);
    const int32_t wait = (int32_t)(start - DWT->CYCCNT);
    if(wait > (int32_t)(5U * mhz) && wait < (int32_t)(5000U * mhz)) {
        while((int32_t)(start - (5U * mhz) - DWT->CYCCNT) > 0) {
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
    l->t_tx_end = t_start + (acc_q8 >> 8);

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
