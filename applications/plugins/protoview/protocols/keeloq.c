/* Microchip HCS200/HCS300/HSC301 KeeLoq, rolling code remotes.
 *
 * Usually 443.92 Mhz OOK, ~200us or ~400us pulse len, depending
 * on the configuration.
 *
 * Preamble: 12 pairs of alternating pulse/gap.
 * Sync: long gap of around 10 times the duration of the short-pulse.
 * Data: pulse width encoded data. Each bit takes three cycles:
 *
 * 0 = 110
 * 1 = 100
 *
 * There are a total of 66 bits transmitted.
 *  0..31: 32 bits of encrypted rolling code.
 * 32..59: Remote ID, 28 bits
 * 60..63: Buttons pressed
 * 64..64: Low battery if set
 * 65..65: Always set to 1
 *
 * Bits in bytes are inverted: least significant bit is first.
 * For some reason there is no checksum whatsoever, so we only decode
 * if we find everything well formed.
 */

#include "../app.h"

static bool decode(uint8_t* bits, uint32_t numbytes, uint32_t numbits, ProtoViewMsgInfo* info) {
    /* In the sync pattern, we require the 12 high/low pulses and at least
     * half the gap we expect (5 pulses times, one is the final zero in the
     * 24 symbols high/low sequence, then other 4). */
    const char* sync_pattern = "101010101010101010101010"
                               "0000";
    uint8_t sync_len = 24 + 4;
    if(numbits - sync_len + sync_len < 3 * 66) return false;
    uint32_t off = bitmap_seek_bits(bits, numbytes, 0, numbits, sync_pattern);
    if(off == BITMAP_SEEK_NOT_FOUND) return false;

    info->start_off = off;
    off += sync_len; // Seek start of message.

    /* Now there is half the gap left, but we allow from 3 to 7, instead of 5
     * symbols of gap, to avoid missing the signal for a matter of wrong
     * timing. */
    uint8_t gap_len = 0;
    while(gap_len <= 7 && bitmap_get(bits, numbytes, off + gap_len) == 0) gap_len++;
    if(gap_len < 3 || gap_len > 7) return false;

    off += gap_len;
    FURI_LOG_E(TAG, "Keeloq preamble+sync found");

    uint8_t raw[9] = {0};
    uint32_t decoded = convert_from_line_code(
        raw, sizeof(raw), bits, numbytes, off, "110", "100"); /* Pulse width modulation. */
    FURI_LOG_E(TAG, "Keeloq decoded bits: %lu", decoded);
    if(decoded < 66) return false; /* Require the full 66 bits. */

    info->pulses_count = (off + 66 * 3) - info->start_off;

    bitmap_reverse_bytes(raw, sizeof(raw)); /* Keeloq is LSB first. */

    int buttons = raw[7] >> 4;
    int remote_id = ((raw[7] & 0x0f) << 24) | (raw[6] << 16) | (raw[5] << 8) | (raw[4] << 0);
    int lowbat = (raw[8] & 0x80) != 0;

    fieldset_add_bytes(info->fieldset, "raw", raw, 9 * 2);
    fieldset_add_bytes(info->fieldset, "encr", raw, 4 * 2);
    fieldset_add_hex(info->fieldset, "id", remote_id, 28);
    fieldset_add_bin(info->fieldset, "s2 s1 s0 s3", buttons, 4);
    fieldset_add_bin(info->fieldset, "low battery", lowbat, 1);
    return true;
}

ProtoViewDecoder KeeloqDecoder =
    {.name = "Keeloq", .decode = decode, .get_fields = NULL, .build_message = NULL};
