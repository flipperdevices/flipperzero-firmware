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

#define ISO15693_3_POLLER_NUM_BLOCKS_PER_QUERY (32U)

static Iso15693_3Error iso15693_3_poller_process_nfc_error(NfcError error) {
    switch(error) {
    case NfcErrorNone:
        return Iso15693_3ErrorNone;
    case NfcErrorTimeout:
        return Iso15693_3ErrorTimeout;
    default:
        return Iso15693_3ErrorNotPresent;
    }
}

static Iso15693_3Error iso15693_3_poller_filter_error(Iso15693_3Error error) {
    switch(error) {
    /* If a particular optional command is not supported, the card might
     * respond with a "Not supported" error or not respond at all.
     * Therefore, treat these errors as non-critical ones. */
    case Iso15693_3ErrorNotSupported:
    case Iso15693_3ErrorTimeout:
        return Iso15693_3ErrorNone;
    default:
        return error;
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
            ret = iso15693_3_poller_process_nfc_error(error);
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

    iso15693_3_reset(data);

    Iso15693_3Error ret;

    do {
        instance->state = Iso15693_3PollerStateColResInProgress;

        // Inventory: Mandatory command
        ret = iso15693_3_poller_async_inventory(instance, data->uid);
        if(ret != Iso15693_3ErrorNone) {
            instance->state = Iso15693_3PollerStateColResFailed;
            break;
        }

        instance->state = Iso15693_3PollerStateActivationInProgress;

        // Get system info: Optional command
        Iso15693_3SystemInfo* system_info = &data->system_info;
        ret = iso15693_3_poller_async_get_system_info(instance, system_info);
        if(ret != Iso15693_3ErrorNone) {
            ret = iso15693_3_poller_filter_error(ret);
            break;
        }

        // Read blocks: Optional command
        simple_array_init(data->block_data, system_info->block_count * system_info->block_size);
        ret = iso15693_3_poller_async_read_blocks(
            instance,
            simple_array_get_data(data->block_data),
            system_info->block_count,
            system_info->block_size);
        if(ret != Iso15693_3ErrorNone) {
            ret = iso15693_3_poller_filter_error(ret);
            break;
        }

        // Get block security status: Optional command
        simple_array_init(data->block_security, system_info->block_count);

        ret = iso15693_3_poller_async_get_blocks_security(
            instance, simple_array_get_data(data->block_security), system_info->block_count);
        if(ret != Iso15693_3ErrorNone) {
            ret = iso15693_3_poller_filter_error(ret);
            break;
        }

        instance->state = Iso15693_3PollerStateActivated;

    } while(false);

    return ret;
}

Iso15693_3Error iso15693_3_poller_async_inventory(Iso15693_3Poller* instance, uint8_t* uid) {
    furi_assert(instance);
    furi_assert(instance->nfc);
    furi_assert(uid);

    bit_buffer_reset(instance->tx_buffer);
    bit_buffer_reset(instance->rx_buffer);

    // Send INVENTORY
    bit_buffer_append_byte(
        instance->tx_buffer,
        ISO15693_3_REQ_FLAG_SUBCARRIER_1 | ISO15693_3_REQ_FLAG_DATA_RATE_HI |
            ISO15693_3_REQ_FLAG_INVENTORY_T5 | ISO15693_3_REQ_FLAG_T5_N_SLOTS_1);
    bit_buffer_append_byte(instance->tx_buffer, ISO15693_3_CMD_INVENTORY);
    bit_buffer_append_byte(instance->tx_buffer, 0x00);

    Iso15693_3Error ret;

    do {
        ret = iso15693_3_poller_frame_exchange(
            instance, instance->tx_buffer, instance->rx_buffer, ISO15693_3_FDT_POLL_FC);
        if(ret != Iso15693_3ErrorNone) break;

        ret = iso15693_3_inventory_response_parse(uid, instance->rx_buffer);
    } while(false);

    return ret;
}

Iso15693_3Error iso15693_3_poller_async_get_system_info(
    Iso15693_3Poller* instance,
    Iso15693_3SystemInfo* data) {
    furi_assert(instance);
    furi_assert(data);

    bit_buffer_reset(instance->tx_buffer);
    bit_buffer_reset(instance->rx_buffer);

    // Send GET SYSTEM INFO
    bit_buffer_append_byte(
        instance->tx_buffer, ISO15693_3_REQ_FLAG_SUBCARRIER_1 | ISO15693_3_REQ_FLAG_DATA_RATE_HI);

    bit_buffer_append_byte(instance->tx_buffer, ISO15693_3_CMD_GET_SYS_INFO);

    Iso15693_3Error ret;

    do {
        ret = iso15693_3_poller_frame_exchange(
            instance, instance->tx_buffer, instance->rx_buffer, ISO15693_3_FDT_POLL_FC);
        if(ret != Iso15693_3ErrorNone) break;

        ret = iso15693_3_system_info_response_parse(data, instance->rx_buffer);
    } while(false);

    return ret;
}

Iso15693_3Error iso15693_3_poller_async_read_block(
    Iso15693_3Poller* instance,
    uint8_t* data,
    uint8_t block_number,
    uint8_t block_size) {
    furi_assert(instance);
    furi_assert(data);

    bit_buffer_reset(instance->tx_buffer);
    bit_buffer_reset(instance->rx_buffer);

    bit_buffer_append_byte(
        instance->tx_buffer, ISO15693_3_REQ_FLAG_SUBCARRIER_1 | ISO15693_3_REQ_FLAG_DATA_RATE_HI);
    bit_buffer_append_byte(instance->tx_buffer, ISO15693_3_CMD_READ_BLOCK);
    bit_buffer_append_byte(instance->tx_buffer, block_number);

    Iso15693_3Error ret;

    do {
        ret = iso15693_3_poller_send_frame(
            instance, instance->tx_buffer, instance->rx_buffer, ISO15693_3_FDT_POLL_FC);
        if(ret != Iso15693_3ErrorNone) break;

        ret = iso15693_3_read_block_response_parse(data, block_size, instance->rx_buffer);
    } while(false);

    return ret;
}

Iso15693_3Error iso15693_3_poller_async_read_blocks(
    Iso15693_3Poller* instance,
    uint8_t* data,
    uint16_t block_count,
    uint8_t block_size) {
    furi_assert(instance);
    furi_assert(data);
    furi_assert(block_count);
    furi_assert(block_size);

    Iso15693_3Error ret = Iso15693_3ErrorNone;

    for(uint32_t i = 0; i < block_count; ++i) {
        ret = iso15693_3_poller_async_read_block(instance, &data[block_size * i], i, block_size);
        if(ret != Iso15693_3ErrorNone) break;
    }

    return ret;
}

Iso15693_3Error iso15693_3_poller_async_get_blocks_security(
    Iso15693_3Poller* instance,
    uint8_t* data,
    uint16_t block_count) {
    furi_assert(instance);
    furi_assert(data);

    // Limit the number of blocks to 32 in a single query
    const uint32_t num_queries = block_count / ISO15693_3_POLLER_NUM_BLOCKS_PER_QUERY +
                                 (block_count % ISO15693_3_POLLER_NUM_BLOCKS_PER_QUERY ? 1 : 0);

    Iso15693_3Error ret = Iso15693_3ErrorNone;

    for(uint32_t i = 0; i < num_queries; ++i) {
        bit_buffer_reset(instance->tx_buffer);
        bit_buffer_reset(instance->rx_buffer);

        bit_buffer_append_byte(
            instance->tx_buffer,
            ISO15693_3_REQ_FLAG_SUBCARRIER_1 | ISO15693_3_REQ_FLAG_DATA_RATE_HI);

        bit_buffer_append_byte(instance->tx_buffer, ISO15693_3_CMD_GET_BLOCKS_SECURITY);

        const uint8_t start_block_num = i * ISO15693_3_POLLER_NUM_BLOCKS_PER_QUERY;
        bit_buffer_append_byte(instance->tx_buffer, start_block_num);

        const uint8_t block_count_per_query =
            MIN(block_count - start_block_num, (uint16_t)ISO15693_3_POLLER_NUM_BLOCKS_PER_QUERY);
        // Block count byte must be 1 less than the desired count
        bit_buffer_append_byte(instance->tx_buffer, block_count_per_query - 1);

        ret = iso15693_3_poller_send_frame(
            instance, instance->tx_buffer, instance->rx_buffer, ISO15693_3_FDT_POLL_FC);
        if(ret != Iso15693_3ErrorNone) break;

        ret = iso15693_3_get_block_security_response_parse(
            &data[start_block_num], block_count_per_query, instance->rx_buffer);
        if(ret != Iso15693_3ErrorNone) break;
    }

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
