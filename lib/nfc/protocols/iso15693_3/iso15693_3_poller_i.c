#include "iso15693_3_poller_i.h"

#include <nfc/helpers/iso13239_crc.h>

#define TAG "Iso15693_3Poller"

#define BITS_IN_BYTE (8)

#define ISO15693_RESP_SOF_SIZE (5)
#define ISO15693_RESP_EOF_SIZE (5)
#define ISO15693_RESP_SOF_MASK (0x1FU)
#define ISO15693_RESP_SOF_PATTERN (0x17U)
#define ISO15693_RESP_EOF_PATTERN (0x1DU)

#define ISO15693_RESP_PATTERN_MASK (0x03U)
#define ISO15693_RESP_PATTERN_0 (0x01U)
#define ISO15693_RESP_PATTERN_1 (0x02U)

static Iso15693_3Error iso15693_3_poller_process_error(NfcError error) {
    switch(error) {
    case NfcErrorNone:
        return Iso15693_3ErrorNone;
    case NfcErrorTimeout:
        return Iso15693_3ErrorTimeout;
    default:
        return Iso15693_3ErrorNotPresent;
    }
}

static Iso15693_3Error iso15693_3_poller_prepare_trx(Iso15693_3Poller* instance) {
    furi_assert(instance);

    if(instance->state == Iso15693_3PollerStateIdle) {
        return iso15693_3_poller_async_activate(instance, NULL);
    }

    return Iso15693_3ErrorNone;
}

static void iso15693_3_poller_encode_byte(uint8_t data, BitBuffer* out) {
    static const uint8_t bit_patterns_1_out_of_4[] = {0x02, 0x08, 0x20, 0x80};

    for(uint32_t i = 0; i < BITS_IN_BYTE; i += (BITS_IN_BYTE) / 4) {
        const uint8_t bit_pair = (data >> i) & 0x03;
        bit_buffer_append_byte(out, bit_patterns_1_out_of_4[bit_pair]);
    }
}

static void iso15693_3_poller_encode_frame(const BitBuffer* data, BitBuffer* frame_data) {
    bit_buffer_append_byte(frame_data, 0x21); // Add SOF 1 out of 4

    for(size_t i = 0; i < bit_buffer_get_size_bytes(data); ++i) {
        iso15693_3_poller_encode_byte(bit_buffer_get_byte(data, i), frame_data);
    }

    bit_buffer_append_byte(frame_data, 0x04); // Add EOF
}

static bool iso15693_3_poller_decode_frame(BitBuffer* data, const BitBuffer* frame_data) {
    bool decoded = false;

    do {
        if(bit_buffer_get_size(frame_data) == 0) break;
        // Check SOF
        if((bit_buffer_get_byte(frame_data, 0) & ISO15693_RESP_SOF_MASK) !=
           ISO15693_RESP_SOF_PATTERN)
            break;

        // 2 response bits = 1 data bit
        for(uint32_t i = ISO15693_RESP_SOF_SIZE;
            i < bit_buffer_get_size(frame_data) - ISO15693_RESP_SOF_SIZE;
            i += BITS_IN_BYTE / 4) {
            const uint8_t resp_byte = bit_buffer_get_byte_from_bit(frame_data, i);

            // Check EOF
            if(resp_byte == ISO15693_RESP_EOF_PATTERN) {
                decoded = true;
                break;
            }

            const uint8_t bit_pattern = resp_byte & ISO15693_RESP_PATTERN_MASK;

            if(bit_pattern == ISO15693_RESP_PATTERN_0) {
                bit_buffer_append_bit(data, false);
            } else if(bit_pattern == ISO15693_RESP_PATTERN_1) {
                bit_buffer_append_bit(data, true);
            } else {
                break;
            }
        }

    } while(false);

    return decoded;
}

static Iso15693_3Error iso15693_3_poller_frame_exchange(
    Iso15693_3Poller* instance,
    const BitBuffer* tx_buffer,
    BitBuffer* rx_buffer,
    uint32_t fwt) {
    furi_assert(instance);

    Iso15693_3Error ret = Iso15693_3ErrorNone;

    do {
        if(bit_buffer_get_size_bytes(tx_buffer) >
           bit_buffer_get_capacity_bytes(instance->tx_buffer) - ISO13239_CRC_SIZE) {
            ret = Iso15693_3ErrorBufferOverflow;
            break;
        }

        bit_buffer_copy(instance->tx_buffer, tx_buffer);
        iso13239_crc_append(Iso13239CrcTypeDefault, instance->tx_buffer);

        bit_buffer_reset(instance->tx_frame_buffer);
        bit_buffer_reset(instance->rx_frame_buffer);

        iso15693_3_poller_encode_frame(instance->tx_buffer, instance->tx_frame_buffer);

        NfcError error =
            nfc_trx(instance->nfc, instance->tx_frame_buffer, instance->rx_frame_buffer, fwt);
        if(error != NfcErrorNone) {
            ret = iso15693_3_poller_process_error(error);
            break;
        }

        if(!iso15693_3_poller_decode_frame(instance->rx_buffer, instance->rx_frame_buffer)) {
            ret = Iso15693_3ErrorFraming;
            break;
        }

        if(!iso13239_crc_check(Iso13239CrcTypeDefault, instance->rx_buffer)) {
            ret = Iso15693_3ErrorWrongCrc;
            break;
        }

        iso13239_crc_trim(instance->rx_buffer);
        bit_buffer_copy(rx_buffer, instance->rx_buffer);
    } while(false);

    return ret;
}

Iso15693_3Error
    iso15693_3_poller_async_activate(Iso15693_3Poller* instance, Iso15693_3Data* data) {
    furi_assert(instance);
    furi_assert(instance->nfc);

    iso15693_3_reset(instance->data);

    Iso15693_3Error ret = Iso15693_3ErrorTimeout;

    do {
        instance->state = Iso15693_3PollerStateColResInProgress;

        bit_buffer_reset(instance->tx_buffer);
        bit_buffer_reset(instance->rx_buffer);

        // TODO: Activation procedure

        // Send INVENTORY
        bit_buffer_append_byte(
            instance->tx_buffer,
            ISO15693_3_REQ_FLAG_SUBCARRIER_1 | ISO15693_3_REQ_FLAG_DATA_RATE_HI |
                ISO15693_3_REQ_FLAG_INVENTORY_T5 | ISO15693_3_REQ_FLAG_T5_N_SLOTS_1);
        bit_buffer_append_byte(instance->tx_buffer, ISO15693_3_CMD_INVENTORY);
        bit_buffer_append_byte(instance->tx_buffer, 0x00);

        ret = iso15693_3_poller_frame_exchange(
            instance, instance->tx_buffer, instance->rx_buffer, ISO15693_3_FDT_POLL_FC);
        if(ret != Iso15693_3ErrorNone) {
            break;
        }

        if(!iso15693_3_inventory_response_parse(instance->data, instance->rx_buffer)) {
            ret = Iso15693_3ErrorCommunication;
            break;
        }

        instance->state = Iso15693_3PollerStateActivated;

        if(data) {
            iso15693_3_copy(data, instance->data);
        }

    } while(false);

    return ret;
}

Iso15693_3Error iso15693_3_poller_halt(Iso15693_3Poller* instance) {
    furi_assert(instance);

    bit_buffer_reset(instance->tx_buffer);
    bit_buffer_reset(instance->rx_buffer);

    Iso15693_3Error ret = Iso15693_3ErrorNone;

    do {
        // TODO: Implement HALT command
        instance->state = Iso15693_3PollerStateIdle;
    } while(false);

    return ret;
}

Iso15693_3Error iso15693_3_poller_send_frame(
    Iso15693_3Poller* instance,
    const BitBuffer* tx_buffer,
    BitBuffer* rx_buffer,
    uint32_t fwt) {
    Iso15693_3Error ret;

    do {
        ret = iso15693_3_poller_prepare_trx(instance);
        if(ret != Iso15693_3ErrorNone) break;

        ret = iso15693_3_poller_frame_exchange(instance, tx_buffer, rx_buffer, fwt);
    } while(false);

    return ret;
}
