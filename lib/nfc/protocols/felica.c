#include <limits.h>
#include <inttypes.h>
#include <mbedtls/sha1.h>
#include "felica.h"
#include "nfc_util.h"
#include <furi.h>
#include "furi_hal_nfc.h"

#define TAG "FeliCa"

uint32_t felica_estimate_timing_us(uint8_t timing, uint8_t units) {
    uint32_t base_cost_factor = 1 + (timing & 0x7);
    uint32_t unit_cost_factor = 1 + ((timing >> 3) & 0x7);
    uint32_t scale = 1 << ((timing >> 6) * 2);
    return FELICA_MRT_TIME_CONSTANT_US * scale * (base_cost_factor + unit_cost_factor * units);
}

uint16_t felica_estimate_timing_furi(uint8_t timing, uint8_t units) {
    uint32_t us = felica_estimate_timing_us(timing, units);
    uint16_t ms = us / 1000;
    if(ms < FELICA_MRT_MIN_ALLOWED_MS) {
        ms = FELICA_MRT_MIN_ALLOWED_MS;
    } else if(us % 1000 != 0) {
        ms++;
    }
    return ms;
}

bool felica_check_ic_type(uint8_t* PMm) {
    uint8_t rom_type = PMm[0];
    uint8_t ic_type = PMm[1];

    bool is_valid_ic = false;
    if(ic_type == 0xff) { // RC-S967 in nfc-dep
        is_valid_ic = true;
    } else if(ic_type == 0xf2) { // RC-S732?
        is_valid_ic = true;
    } else if(ic_type == 0xf0 || ic_type == 0xf1) { // Lite(S)
        is_valid_ic = true;
    } else if(ic_type == 0xe1) { // RC-S967 in plug mode
        is_valid_ic = true;
    } else if(ic_type == 0xe0) { // RC-S926
        is_valid_ic = true;
    } else if(ic_type >= 0x44 && ic_type <= 0x48) { // SD2
        is_valid_ic = true;
    } else if(ic_type == 0x3e && rom_type == 0x03) { // RC-SA08
        return true;
    } else if(ic_type == 0x35) { // RC-SA01
        is_valid_ic = true;
    } else if(ic_type == 0x32) { // RC-SA00
        is_valid_ic = true;
    } else if(ic_type == 0x31) { // Suica/PASMO
        is_valid_ic = true;
    } else if(ic_type == 0x20) { // RC-S962
        is_valid_ic = true;
    } else if(ic_type >= 0x10 && ic_type <= 0x1f) { // Mobile IC version 2/3
        is_valid_ic = true;
    } else if(ic_type == 0x0d) { // RC-S960
        is_valid_ic = true;
    } else if(ic_type == 0x0c) { // RC-S954
        is_valid_ic = true;
    } else if(ic_type == 0x0b) { // Old Suica?
        is_valid_ic = true;
    } else if(ic_type == 0x09) { // RC-S953
        is_valid_ic = true;
    } else if(ic_type == 0x08) { // RC-S952
        is_valid_ic = true;
    } else if(ic_type == 0x06 || ic_type == 0x07) { // Mobile IC version 1
        is_valid_ic = true;
    } else if(ic_type == 0x02) { // RC-S919
        is_valid_ic = true;
    } else if(ic_type == 0x01) { // RC-S915
        is_valid_ic = true;
    } else if(ic_type == 0x00) { // RC-S830
        is_valid_ic = true;
    }

    if(!is_valid_ic) {
        return false;
    }

    // need more samples to confirm below
    /*
    if (rom_type != 0x01) {
        return false;
    }
    */

    return true;
}

FelicaICType felica_get_ic_type(uint8_t* PMm) {
    uint8_t rom_type = PMm[0];
    uint8_t ic_type = PMm[1];

    UNUSED(rom_type);
    switch(ic_type) {
    case 0xff:
        return FelicaICTypeLinkNDEF;
    case 0xf2:
        return FelicaICTypeLink;
    case 0xf1:
        return FelicaICTypeLiteS;
    case 0xf0:
        return FelicaICTypeLite;
    case 0xe1:
        return FelicaICTypeLink;
    case 0xe0:
        return FelicaICTypePlug;
    case 0x48:
        return FelicaICTypeSD2_6K;
    case 0x47:
        return FelicaICTypeRC_SA24_6K;
    case 0x46:
        return FelicaICTypeSD2_4K;
    case 0x45:
    case 0x44:
        return FelicaICTypeSD2WithDES;
    case 0x3e:
        return FelicaICTypeRC_SA08;
    case 0x35:
        return FelicaICTypeSD1;
    case 0x32:
        return FelicaICTypeSD1WithDES;
    case 0x31:
        return FelicaICTypeSuica;
    case 0x20:
        return FelicaICTypeFRAM_4K;
    case 0x1f:
    case 0x1e:
    case 0x1d:
    case 0x1c:
    case 0x1b:
    case 0x1a:
    case 0x19:
    case 0x18:
        return FelicaICTypeMobileIC_V4_1;
    case 0x17:
        return FelicaICTypeMobileIC_V4;
    case 0x16:
    case 0x15:
    case 0x14:
        return FelicaICTypeMobileIC_V3;
    case 0x13:
    case 0x12:
    case 0x11:
    case 0x10:
        return FelicaICTypeMobileIC_V2;
    case 0x0d:
        return FelicaICTypeFRAM_9K;
    case 0x0c:
        return FelicaICTypeEMV_36K;
    case 0x0b: // Old Suica?
        return FelicaICTypeSuica;
    case 0x09:
        return FelicaICTypeEMV_16K;
    case 0x08:
        return FelicaICTypeEMV_32K;
    case 0x07:
    case 0x06:
        return FelicaICTypeMobileIC_V1;
    case 0x02:
        return FelicaICType576B;
    case 0x01:
        return FelicaICType4K;
    case 0x00:
        return FelicaICType2K;
    }

    return FelicaICType2K;
}

static void felica_parse_service_attrib(
    uint16_t node_attrib,
    FelicaServiceType* result_type,
    FelicaServiceAttribute* result_attrib) {
    FelicaServiceType type = node_attrib & 0b111100;
    if((type & 0b111000) == FelicaServiceTypePurse) {
        *result_type = FelicaServiceTypePurse;
        *result_attrib = node_attrib & 0b111;
        return;
    }
    *result_type = type;
    *result_attrib = node_attrib & 0b11;
}

/** Parse common FeliCa response headers.
 *
 * This parses and validates the most commonly occurring response header types.
 *
 * The header needs to match the (length, res, idm) format, and also (sf1, sf2) when always_succeed
 * is set to false.
 *
 * @param buf RX buffer.
 * @param len RX buffer length.
 * @param reader The FeliCa reader context.
 * @param expected_resp Expected response code. Must be an odd number.
 * @param always_succeed When set to true, skip status flags (sf1 and sf2) parsing.
 * @return The number of bytes parsed, or 0 when response is invalid or status flags are set.
 */
static uint8_t felica_consume_header(
    uint8_t* buf,
    uint8_t len,
    FelicaReader* reader,
    uint8_t expected_resp,
    bool always_succeed,
    bool update_idm) {
    furi_assert(expected_resp & 1);
    furi_assert(buf != NULL);
    furi_assert(reader != NULL);

    uint8_t header_size = always_succeed ? 10 : 12;
    if(len < header_size) {
        FURI_LOG_E(TAG, "Malformed header: too short.");
        return 0;
    }
    if(buf[1] != expected_resp) {
        FURI_LOG_E(TAG, "Expecting %u, got %u.", expected_resp, buf[1]);
        return 0;
    }
    if(!update_idm && memcmp(&buf[2], reader->current_idm, sizeof(reader->current_idm)) != 0) {
        FURI_LOG_E(TAG, "IDm mismatch.");
        return 0;
    } else {
        memcpy(reader->current_idm, &buf[2], sizeof(reader->current_idm));
    }
    if(!always_succeed) {
        reader->status_flags[0] = buf[10];
        reader->status_flags[1] = buf[11];
        if(reader->status_flags[0] != 0 || reader->status_flags[1] != 0) {
            FURI_LOG_D(
                TAG, "SF1: %02X SF2: %02X", reader->status_flags[0], reader->status_flags[1]);
        }
    }
    return header_size;
}

uint8_t felica_prepare_unencrypted_read(
    uint8_t* dest,
    const FelicaReader* reader,
    const uint16_t* service_code_list,
    uint8_t service_count,
    const FelicaRWRequestBlockDescriptor* block_list,
    uint8_t block_count) {
    dest[0] = FELICA_UNENCRYPTED_READ_CMD;
    memcpy(&dest[1], reader->current_idm, 8);

    dest[9] = service_count;
    uint8_t msg_len = 10;
    for(int i = 0; i < service_count; i++) {
        uint16_t service_code = service_code_list[i];
        dest[msg_len++] = service_code & 0xFF;
        dest[msg_len++] = service_code >> 8;
    }

    dest[msg_len++] = block_count;
    for(int i = 0; i < block_count; i++) {
        FelicaRWRequestBlockDescriptor block_desc = block_list[i];

        uint8_t flags = 0x0;
        if(block_desc.block_number < 0x100) {
            flags |= 0x80;
        }
        flags |= block_desc.access_mode << 4;
        flags |= block_desc.service_index & 0xf;

        dest[msg_len++] = flags;
        dest[msg_len++] = block_desc.block_number & 0xFF;
        if(block_desc.block_number >= 0x100) {
            dest[msg_len++] = block_desc.block_number >> 8;
        }
    }

    return msg_len;
}

uint8_t felica_lite_prepare_unencrypted_read(
    uint8_t* dest,
    const FelicaReader* reader,
    bool is_read_only,
    const uint8_t* block_list,
    uint8_t block_count) {
    dest[0] = FELICA_UNENCRYPTED_READ_CMD;
    memcpy(&dest[1], reader->current_idm, 8);

    dest[9] = 1;
    uint8_t msg_len = 10;
    uint8_t service_code =
        FelicaServiceTypeRandom |
        ((is_read_only) ? FelicaServiceAttributeUnauthRO : FelicaServiceAttributeUnauthRO);

    dest[msg_len++] = service_code & 0xFF;
    dest[msg_len++] = service_code >> 8;

    dest[msg_len++] = block_count;
    for(int i = 0; i < block_count; i++) {
        dest[msg_len++] = IS_2_BYTE_BLOCK_LIST_ELEMENT;
        dest[msg_len++] = block_list[i];
    }

    return msg_len;
}

uint16_t felica_parse_unencrypted_read(
    uint8_t* buf,
    uint8_t len,
    FelicaReader* reader,
    uint8_t* out,
    uint16_t out_len) {
    uint8_t consumed =
        felica_consume_header(buf, len, reader, FELICA_UNENCRYPTED_READ_RES, false, false);
    if(!consumed || reader->status_flags[0] != 0 || reader->status_flags[1] != 0) {
        return 0;
    }
    len -= consumed;
    buf += consumed;

    if(len < 1) {
        return 0;
    }
    uint16_t data_length = *buf * FELICA_BLOCK_SIZE;
    len--;
    buf++;

    if(len < data_length || out_len < data_length) {
        return 0;
    }

    memcpy(out, buf, data_length);

    return data_length;
}

uint8_t felica_prepare_unencrypted_write(
    uint8_t* dest,
    FelicaReader* reader,
    const uint16_t* service_code_list,
    uint8_t service_count,
    const uint32_t* block_list,
    uint8_t block_count,
    const uint8_t* block_data) {
    dest[0] = FELICA_UNENCRYPTED_WRITE_CMD;
    memcpy(&dest[1], reader->current_idm, 8);

    dest[9] = service_count;
    uint8_t msg_len = 10;
    for(int i = 0; i < service_count; i++) {
        uint16_t service_code = service_code_list[i];
        dest[msg_len++] = service_code & 0xFF;
        dest[msg_len++] = service_code >> 8;
    }

    dest[msg_len++] = block_count;
    for(int i = 0; i < block_count; i++) {
        uint16_t block_num = block_list[i];
        dest[msg_len++] = block_num & 0xFF;
        dest[msg_len++] = block_num >> 8;
    }

    uint16_t data_length = block_count * FELICA_BLOCK_SIZE;
    memcpy(dest + msg_len, block_data, data_length);
    msg_len += data_length;
    return msg_len;
}

uint8_t felica_lite_prepare_unencrypted_write(
    uint8_t* dest,
    const FelicaReader* reader,
    const uint8_t* block_list,
    uint8_t block_count,
    const uint8_t* block_data) {
    dest[0] = FELICA_UNENCRYPTED_WRITE_CMD;
    memcpy(&dest[1], reader->current_idm, 8);

    dest[9] = 1;
    uint8_t msg_len = 10;
    uint8_t service_code = FelicaServiceTypeRandom | FelicaServiceAttributeUnauthRW;
    dest[msg_len++] = service_code & 0xFF;
    dest[msg_len++] = service_code >> 8;

    dest[msg_len++] = block_count;
    for(int i = 0; i < block_count; i++) {
        dest[msg_len++] = block_list[i];
        dest[msg_len++] = IS_2_BYTE_BLOCK_LIST_ELEMENT;
    }

    uint16_t data_length = block_count * FELICA_BLOCK_SIZE;
    memcpy(dest + msg_len, block_data, data_length);
    msg_len += data_length;
    return msg_len;
}

bool felica_parse_unencrypted_write(uint8_t* buf, uint8_t len, FelicaReader* reader) {
    uint8_t consumed =
        felica_consume_header(buf, len, reader, FELICA_UNENCRYPTED_WRITE_RES, false, false);
    if(!consumed || reader->status_flags[0] != 0 || reader->status_flags[1] != 0) {
        return false;
    }
    return true;
}

uint8_t felica_prepare_request_system_code(uint8_t* dest, FelicaReader* reader) {
    dest[0] = FELICA_REQUEST_SYSTEM_CODE_CMD;
    memcpy(&dest[1], reader->current_idm, 8);
    return 9;
}

bool felica_parse_request_system_code(
    uint8_t* buf,
    uint8_t len,
    FelicaReader* reader,
    FelicaSystemArray_t systems) {
    uint8_t consumed =
        felica_consume_header(buf, len, reader, FELICA_REQUEST_SYSTEM_CODE_RES, true, false);
    if(consumed == 0) {
        return false;
    }
    len -= consumed;
    buf += consumed;

    uint8_t entries = *buf;
    len--;
    buf++;

    if(len < 2 * entries) {
        FURI_LOG_E(TAG, "FELICA_REQUEST_SYSTEM_CODE_RES: Response too short");
        return false;
    }

    for(uint8_t idx = 0; idx < entries; idx++) {
        FelicaSystem* system = FelicaSystemArray_push_new(systems);
        furi_assert(system != NULL);

        felica_std_system_init(system, idx, (buf[2 * idx] << 8) | buf[2 * idx + 1]);

        FURI_LOG_D(TAG, "Found system code %04X", system->code);
    }

    return true;
}

uint8_t felica_prepare_search_service_code(uint8_t* dest, FelicaReader* reader, uint16_t cursor) {
    dest[0] = FELICA_SEARCH_SERVICE_CODE_CMD;
    memcpy(&dest[1], reader->current_idm, 8);
    dest[9] = cursor & 0xff;
    dest[10] = (cursor >> 8) & 0xff;
    return 11;
}

bool felica_parse_search_service_code(
    uint8_t* buf,
    uint8_t len,
    FelicaReader* reader,
    FelicaSearchServiceCodeResult* result) {
    uint8_t consumed =
        felica_consume_header(buf, len, reader, FELICA_SEARCH_SERVICE_CODE_RES, true, false);
    if(consumed == 0) {
        return false;
    }
    len -= consumed;
    buf += consumed;

    if(len != 2 && len != 4) {
        FURI_LOG_E(TAG, "Invalid response length for Search Service Code command");
        return false;
    }

    uint16_t node_code = buf[0] | (buf[1] << 8);

    if(node_code == FELICA_NODE_CODE_INVALID) {
        result->is_valid = false;
        return true;
    }

    bool is_area = (len == 4);
    uint16_t node_number = node_code >> 6;
    uint16_t node_attrib = node_code & 0b111111;
    uint16_t node_end = 0;

    if(is_area) {
        node_end = buf[2] | (buf[3] << 8);
    }

    FURI_LOG_D(
        TAG,
        "Found node %#x (%s #%d, attrib %#x)",
        node_code,
        is_area ? "area" : "svc",
        node_number,
        node_attrib);
    if(is_area) {
        FURI_LOG_D(TAG, "Area ends at %#x", node_end);
    }

    result->is_valid = true;
    result->is_area = is_area;
    result->code = node_code;
    result->number = node_number;
    if(is_area) {
        result->can_create_subareas = !(node_attrib & 1);
        result->area_end = node_end;
    } else {
        felica_parse_service_attrib(
            node_attrib, &(result->service_type), &(result->service_attrib));
    }

    return true;
}

uint8_t felica_prepare_polling(uint8_t* dest, uint16_t system, uint8_t request, uint8_t timeslot) {
    dest[0] = 0x00;
    dest[1] = system >> 8;
    dest[2] = system & 0xff;
    dest[3] = request;
    dest[4] = timeslot;
    return 5;
}

bool felica_parse_polling(
    uint8_t* buf,
    uint8_t len,
    FelicaReader* reader,
    uint16_t* request_response) {
    uint8_t consumed = felica_consume_header(buf, len, reader, 0x01, true, true);
    if(consumed == 0) {
        return false;
    }
    len -= consumed;
    buf += consumed;

    if(len < sizeof(reader->current_pmm) || len > sizeof(reader->current_pmm) + 2) {
        FURI_LOG_E(TAG, "Invalid response length for Polling command");
        return false;
    }

    if(len >= sizeof(reader->current_pmm)) {
        memcpy(reader->current_pmm, buf, sizeof(reader->current_pmm));
    }

    len += sizeof(reader->current_pmm);
    buf += sizeof(reader->current_pmm);

    if(len >= 2) {
        FURI_LOG_D(TAG, "Polling response: %02X %02X", buf[0], buf[1]);
        if(request_response != NULL) {
            *request_response = (buf[0] << 8) | buf[1];
        }
    }

    return true;
}

static FelicaSystem* felica_gen_monolithic_system_code(
    FelicaReader* reader,
    FelicaSystemArray_t systems,
    uint16_t system_code) {
    FelicaSystem* system = FelicaSystemArray_push_new(systems);
    furi_assert(reader != NULL);
    furi_assert(system != NULL);

    system->code = system_code;

    return system;
}

bool felica_lite_can_read_without_mac(uint8_t* mc_r_restr, uint8_t block_number) {
    if(block_number > REG_LITE_BLOCK) {
        return true;
    }
    uint8_t byte = mc_r_restr[block_number < 8 ? 0 : 1];
    return ((byte >> (block_number % 8)) & 1) == 0;
}

void felica_define_normal_block(FelicaService* service, uint16_t number, uint8_t* data) {
    FelicaBlock* block = FelicaBlockArray_safe_get(service->blocks, number);
    memcpy(block->data, data, FELICA_BLOCK_SIZE);
}

void felica_push_normal_block(FelicaService* service, uint8_t* data) {
    FelicaBlock* block = FelicaBlockArray_push_new(service->blocks);
    memcpy(block->data, data, FELICA_BLOCK_SIZE);
}

bool felica_lite_dump_data(
    FuriHalNfcTxRxContext* tx_rx,
    FelicaReader* reader,
    FelicaData* data,
    FelicaSystem* system) {
    system->is_lite = true;
    system->is_monolithic_ndef = false;
    const uint8_t fixed_blocks[] = {
        SYS_CODE_LITE_BLOCK,
        DEVICE_ID_LITE_BLOCK,
        ID_LITE_BLOCK,
        RC_LITE_BLOCK,
        CARD_KEY_LITE_BLOCK,
        MAC_LITE_BLOCK,
        CARD_KEY_VER_LITE_BLOCK,
        MEM_CONFIG_LITE_BLOCK,
    };

    uint16_t read_timeout =
        felica_estimate_timing_furi(reader->current_pmm[FELICA_PMM_READ_MRT], 1);
    uint16_t write_timeout =
        felica_estimate_timing_furi(reader->current_pmm[FELICA_PMM_WRITE_MRT], 1);
    uint16_t read_timeout_double =
        felica_estimate_timing_furi(reader->current_pmm[FELICA_PMM_READ_MRT], 2);

    uint8_t block_data[FELICA_BLOCK_SIZE * 4];

    tx_rx->tx_bits =
        8 * felica_lite_prepare_unencrypted_read(tx_rx->tx_data, reader, true, fixed_blocks, 1);
    if(!furi_hal_nfc_tx_rx(tx_rx, read_timeout)) {
        FURI_LOG_W(TAG, "Bad exchange verifying Lite system code");
        return false;
    }
    if(felica_parse_unencrypted_read(
           tx_rx->rx_data, tx_rx->rx_bits / 8, reader, block_data, sizeof(block_data)) !=
       FELICA_BLOCK_SIZE) {
        FURI_LOG_W(TAG, "Bad response to Read without Encryption (SYS_C)");
        return false;
    }
    if(nfc_util_bytes2num(block_data, 2) != LITE_SYSTEM_CODE) {
        FURI_LOG_W(TAG, "Unexpected SYS_C value");
        return false;
    }

    tx_rx->tx_bits = 8 * felica_lite_prepare_unencrypted_read(
                             tx_rx->tx_data, reader, true, &fixed_blocks[1], 1);
    if(!furi_hal_nfc_tx_rx(tx_rx, read_timeout)) {
        FURI_LOG_W(TAG, "Bad exchange reading D_ID");
        return false;
    }
    if(felica_parse_unencrypted_read(
           tx_rx->rx_data, tx_rx->rx_bits / 8, reader, block_data, sizeof(block_data)) !=
       FELICA_BLOCK_SIZE) {
        FURI_LOG_W(TAG, "Bad response to Read without Encryption (D_ID)");
        return false;
    }
    if(memcmp(reader->current_idm, block_data, 8) != 0 ||
       memcmp(reader->current_pmm, block_data + 8, 8) != 0) {
        FURI_LOG_W(TAG, "Mismatching values for D_ID");
        return false;
    }

    FelicaLiteInfo* lite_info = &system->lite_info;
    lite_info->card_key_1 = NULL;
    lite_info->card_key_2 = NULL;

    tx_rx->tx_bits = 8 * felica_lite_prepare_unencrypted_read(
                             tx_rx->tx_data, reader, true, &fixed_blocks[2], 1);
    if(!furi_hal_nfc_tx_rx(tx_rx, read_timeout)) {
        FURI_LOG_W(TAG, "Bad exchange reading ID");
        return false;
    }
    if(felica_parse_unencrypted_read(
           tx_rx->rx_data, tx_rx->rx_bits / 8, reader, block_data, sizeof(block_data)) !=
       FELICA_BLOCK_SIZE) {
        FURI_LOG_W(TAG, "Bad response to Read without Encryption (ID)");
        return false;
    }
    lite_info->data_format_code = nfc_util_bytes2num(block_data + 8, 2);
    memcpy(lite_info->ID_value, block_data + 10, 6);
    FURI_LOG_I(TAG, "ID:");
    for(int i = 0; i < FELICA_BLOCK_SIZE; i++) {
        FURI_LOG_I(TAG, "%02X", block_data[i]);
    }

    memset(block_data, 0, FELICA_BLOCK_SIZE);
    tx_rx->tx_bits = 8 * felica_lite_prepare_unencrypted_write(
                             tx_rx->tx_data, reader, &fixed_blocks[3], 1, block_data);
    if(!furi_hal_nfc_tx_rx(tx_rx, write_timeout)) {
        FURI_LOG_W(TAG, "Bad exchange writing random challenge");
        return false;
    }
    if(!felica_parse_unencrypted_write(tx_rx->rx_data, tx_rx->rx_bits / 8, reader)) {
        FURI_LOG_W(TAG, "Bad response to Write without Encryption (RC)");
        return false;
    }

    tx_rx->tx_bits = 8 * felica_lite_prepare_unencrypted_read(
                             tx_rx->tx_data, reader, true, &fixed_blocks[4], 2);
    if(!furi_hal_nfc_tx_rx(tx_rx, read_timeout_double)) {
        FURI_LOG_W(TAG, "Bad exchange reading CK and MAC");
        return false;
    }
    if(felica_parse_unencrypted_read(
           tx_rx->rx_data, tx_rx->rx_bits / 8, reader, block_data, sizeof(block_data)) !=
       FELICA_BLOCK_SIZE * 2) {
        FURI_LOG_W(TAG, "Bad response to Read without Encryption (CK, MAC)");
        return false;
    }
    memcpy(lite_info->MAC, block_data + FELICA_BLOCK_SIZE, 8);
    FURI_LOG_I(TAG, "MAC:");
    for(int i = 0; i < FELICA_BLOCK_SIZE; i++) {
        FURI_LOG_I(TAG, "%02X", block_data[i + FELICA_BLOCK_SIZE]);
    }

    tx_rx->tx_bits = 8 * felica_lite_prepare_unencrypted_read(
                             tx_rx->tx_data, reader, true, &fixed_blocks[6], 2);
    if(!furi_hal_nfc_tx_rx(tx_rx, read_timeout_double)) {
        FURI_LOG_W(TAG, "Bad exchange reading CKV and MC");
        return false;
    }
    if(felica_parse_unencrypted_read(
           tx_rx->rx_data, tx_rx->rx_bits / 8, reader, block_data, sizeof(block_data)) !=
       FELICA_BLOCK_SIZE * 2) {
        FURI_LOG_W(TAG, "Bad response to Read without Encryption (CKV, MC)");
        return false;
    }
    lite_info->card_key_version = nfc_util_bytes2num(block_data, 2);
    memcpy(lite_info->memory_config, block_data + FELICA_BLOCK_SIZE, FELICA_BLOCK_SIZE);

    // Read SPAD and REG accordingly to MC
    uint8_t* mc_data = lite_info->memory_config;
    for(uint8_t block_number = 0; block_number <= REG_LITE_BLOCK; block_number++) {
        if(!felica_lite_can_read_without_mac(mc_data + 6, block_number)) {
            if(block_number < REG_LITE_BLOCK) {
                lite_info->S_PAD[block_number] = NULL;
            } else {
                lite_info->REG = NULL;
            }
            continue;
        }

        tx_rx->tx_bits = 8 * felica_lite_prepare_unencrypted_read(
                                 tx_rx->tx_data, reader, true, &block_number, 1);
        if(!furi_hal_nfc_tx_rx(tx_rx, read_timeout)) {
            FURI_LOG_W(TAG, "Bad exchange reading blocks");
            return false;
        }
        if(felica_parse_unencrypted_read(
               tx_rx->rx_data, tx_rx->rx_bits / 8, reader, block_data, sizeof(block_data)) !=
           FELICA_BLOCK_SIZE) {
            FURI_LOG_W(TAG, "Bad response to Read without Encryption (block %d)", block_number);
            return false;
        }
        uint8_t* block = malloc(FELICA_BLOCK_SIZE);
        memcpy(block, block_data, FELICA_BLOCK_SIZE);
        if(block_number < REG_LITE_BLOCK) {
            lite_info->S_PAD[block_number] = block;
        } else {
            lite_info->REG = block;
        }
    }
    if(data->type == FelicaICTypeLiteS) {
        const uint8_t fixed_s_blocks[] = {
            CARD_KEY_LITE_BLOCK,
            MAC_A_LITE_BLOCK,
            WRITE_COUNT_LITE_BLOCK,
            CRC_CHECK_LITE_BLOCK,
        };

        tx_rx->tx_bits = 8 * felica_lite_prepare_unencrypted_read(
                                 tx_rx->tx_data, reader, true, fixed_s_blocks, 2);
        if(!furi_hal_nfc_tx_rx(tx_rx, read_timeout_double)) {
            FURI_LOG_W(TAG, "Bad exchange reading ID with MAC_A");
            return false;
        }
        if(felica_parse_unencrypted_read(
               tx_rx->rx_data, tx_rx->rx_bits / 8, reader, block_data, sizeof(block_data)) !=
           FELICA_BLOCK_SIZE * 2) {
            FURI_LOG_W(TAG, "Bad response to Read without Encryption (CK, MAC_A)");
            return false;
        }
        memcpy(lite_info->MAC_A, block_data + FELICA_BLOCK_SIZE, FELICA_BLOCK_SIZE);

        tx_rx->tx_bits = 8 * felica_lite_prepare_unencrypted_read(
                                 tx_rx->tx_data, reader, true, &fixed_s_blocks[2], 2);
        if(!furi_hal_nfc_tx_rx(tx_rx, read_timeout_double)) {
            FURI_LOG_W(TAG, "Bad exchange reading ID with MAC_A");
            return false;
        }
        if(felica_parse_unencrypted_read(
               tx_rx->rx_data, tx_rx->rx_bits / 8, reader, block_data, sizeof(block_data)) !=
           FELICA_BLOCK_SIZE * 2) {
            FURI_LOG_W(TAG, "Bad response to Read without Encryption (WC, CRC_CHECK)");
            return false;
        }
        lite_info->write_count = nfc_util_bytes2num(block_data, 3);
        lite_info->crc_valid = block_data[FELICA_BLOCK_SIZE] == 0x00;
    }

    return true;
}

bool felica_ndef_dump_data(
    FuriHalNfcTxRxContext* tx_rx,
    FelicaReader* reader,
    FelicaData* data,
    FelicaSystem* system) {
    UNUSED(data);
    system->is_monolithic_ndef = true;
    system->is_lite = false;
    felica_node_init_as_service(
        &system->ndef_node, FELICA_INODE_INVALID, 0, FelicaServiceTypeRandom);
    FelicaServiceAttributeSet_push(
        system->ndef_node.service->access_control_list, FelicaServiceAttributeUnauthRO);
    return felica_std_dump_public_service(
        tx_rx,
        reader,
        system->ndef_node.service,
        FelicaServiceTypeRandom | FelicaServiceAttributeUnauthRO);
}

FelicaNode* felica_std_inode_lookup(FelicaSystem* system, FelicaINode inode) {
    furi_assert(system != NULL);
    if(inode <= FELICA_INODE_INVALID) {
        return NULL;
    }
    return FelicaNodeArray_get(system->nodes, inode);
}

FelicaNode* felica_std_get_root_node(FelicaSystem* system) {
    return felica_std_inode_lookup(system, 0);
}

bool felica_std_select_system(
    FuriHalNfcTxRxContext* tx_rx,
    FelicaReader* reader,
    uint16_t system_code) {
    FURI_LOG_D(TAG, "Selecting system %04X", system_code);

    tx_rx->tx_bits = 8 * felica_prepare_polling(tx_rx->tx_data, system_code, 0x0, 0);
    if(!furi_hal_nfc_tx_rx(tx_rx, 300)) {
        FURI_LOG_E(TAG, "Bad exchange when selecting system with Polling command");
        return false;
    }
    if(!felica_parse_polling(tx_rx->rx_data, tx_rx->rx_bits / 8, reader, NULL)) {
        FURI_LOG_E(TAG, "Bad response to Polling command");
        return false;
    }
    return true;
}

bool felica_std_request_system_code(
    FuriHalNfcTxRxContext* tx_rx,
    FelicaReader* reader,
    FelicaSystemArray_t systems) {
    uint16_t timeout = felica_estimate_timing_furi(reader->current_pmm[FELICA_PMM_FIXED_MRT], 0);
    tx_rx->tx_bits = 8 * felica_prepare_request_system_code(tx_rx->tx_data, reader);
    if(!furi_hal_nfc_tx_rx(tx_rx, timeout)) {
        FURI_LOG_E(TAG, "Bad exchange requesting system code");
        return false;
    }
    if(!felica_parse_request_system_code(tx_rx->rx_data, tx_rx->rx_bits / 8, reader, systems)) {
        FURI_LOG_E(TAG, "Bad response to Request System Code command");
        return false;
    }
    return true;
}

bool felica_std_traverse_system(
    FuriHalNfcTxRxContext* tx_rx,
    FelicaReader* reader,
    FelicaSystem* system) {
    FelicaSearchServiceCodeResult search_result = {0};
    bool result = false;
    int_least32_t last_node_code = -1;
    // Cache currently opened service node. Must be updated every time the array is mutated.
    FelicaNode* curr_open_snode = NULL;
    // FELICA_INODE_INVALID is also used here to mark root node (no parent)
    FelicaINode curr_inode = FELICA_INODE_INVALID, curr_working_inode = FELICA_INODE_INVALID;

    if(system->is_lite || system->is_monolithic_ndef) {
        return false;
    }

    uint16_t timeout = felica_estimate_timing_furi(reader->current_pmm[FELICA_PMM_FIXED_MRT], 0);

    // Traverse all areas and services, populate filesystem tree and update public_services table.
    // Do NOT early return in this loop. There will be leaks.
    // Instead use break without setting result to true.
    for(uint_least32_t cursor = 0; cursor < 0x10000; cursor++) {
        // Execute transaction
        tx_rx->tx_bits =
            8 * felica_prepare_search_service_code(tx_rx->tx_data, reader, cursor & 0xffff);
        if(!furi_hal_nfc_tx_rx(tx_rx, timeout)) {
            FURI_LOG_E(TAG, "Bad exchange when searching node");
            break;
        }
        if(!felica_parse_search_service_code(
               tx_rx->rx_data, tx_rx->rx_bits / 8, reader, &search_result)) {
            break;
        }

        // End search when reaching end of the node list
        if(!search_result.is_valid) {
            FURI_LOG_D(TAG, "Ending service search");
            result = true;
            break;
        }

        // Reject non-incremental node code
        int_least32_t curr_node_code = search_result.code;
        if(curr_node_code <= last_node_code) {
            FURI_LOG_E(
                TAG, "Node code returned by Search Service Code must be strictly incrementing");
            break;
        }
        last_node_code = curr_node_code;

        // Find the area the newly discovered node belongs to
        while(true) {
            if(curr_working_inode <= FELICA_INODE_INVALID) {
                break;
            }

            FelicaNode* curr_working_anode = felica_std_inode_lookup(system, curr_working_inode);
            furi_assert(curr_working_anode != NULL);
            furi_assert(curr_working_anode->type == FelicaNodeTypeArea);
            furi_assert(curr_working_anode->area != NULL);

            // If current code is out of range, cd .. and check again
            if(search_result.code > curr_working_anode->area->end_service_code) {
                FelicaINode parent_inode = curr_working_anode->parent;
                curr_working_inode = parent_inode;
                // Close service on area switching
                curr_open_snode = NULL;
            } else {
                break;
            }
        }

        // New area discovered
        if(search_result.is_area) {
            FelicaNode* new_anode = FelicaNodeArray_push_new(system->nodes);
            felica_node_init_as_area(
                new_anode,
                curr_working_inode,
                search_result.number,
                search_result.can_create_subareas,
                search_result.area_end);

            // Record new inode and cache the node object
            FelicaINode new_inode = ++curr_inode;
            if(new_inode > 0) {
                FelicaNode* curr_working_anode =
                    felica_std_inode_lookup(system, curr_working_inode);
                furi_assert(curr_working_anode != NULL);
                furi_assert(curr_working_anode->type == FelicaNodeTypeArea);
                furi_assert(curr_working_anode->area != NULL);
                FelicaINodeArray_push_back(curr_working_anode->area->nodes, new_inode);
            }

            curr_working_inode = new_inode;

            // Since this is a new area node, the previously opened service should be closed.
            curr_open_snode = NULL;
            continue;
        }

        if(curr_working_inode <= FELICA_INODE_INVALID || curr_inode <= FELICA_INODE_INVALID) {
            FURI_LOG_E(TAG, "Bad card format: Root area does not exist");
            break;
        }

        // Fetch or create the service node of the correct number and type
        // This takes advantage on the nature of service code structure to cut down unnecessary
        // searches
        if(curr_open_snode == NULL || curr_open_snode->service->number != search_result.number ||
           curr_open_snode->service->type != search_result.service_type) {
            curr_open_snode = FelicaNodeArray_push_new(system->nodes);
            felica_node_init_as_service(
                curr_open_snode,
                curr_working_inode,
                search_result.number,
                search_result.service_type);
            FelicaNode* curr_working_anode = felica_std_inode_lookup(system, curr_working_inode);
            furi_assert(curr_working_anode != NULL);
            furi_assert(curr_working_anode->type == FelicaNodeTypeArea);
            furi_assert(curr_working_anode->area != NULL);
            FelicaINodeArray_push_back(curr_working_anode->area->nodes, ++curr_inode);
        }

        // Add current access condition to the set
        FelicaServiceAttributeSet_push(
            curr_open_snode->service->access_control_list, search_result.service_attrib);

        // Also add to public service dict if the service is publicly readable
        // (Purse/value block increment/decrement should be considered write-only so they will be
        //  excluded even when auth is not required)
        if(search_result.service_attrib == FelicaServiceAttributeUnauthRO ||
           search_result.service_attrib == FelicaServiceAttributeUnauthRW ||
           search_result.service_attrib == FelicaServiceAttributeUnauthDirectAccess ||
           search_result.service_attrib == FelicaServiceAttributeUnauthPurseRO) {
            FelicaPublicServiceDict_set_at(
                system->public_services, search_result.code, curr_inode);
        }
    }

    FURI_LOG_D(
        TAG, "%d nodes found under system %04X", FelicaNodeArray_size(system->nodes), system->code);

    return result;
}

bool felica_std_dump_public_service(
    FuriHalNfcTxRxContext* tx_rx,
    FelicaReader* reader,
    FelicaService* service,
    uint16_t service_code) {
    FURI_LOG_D(TAG, "Dumping service %04X", service_code);

    uint16_t service_codes[1] = {service_code};
    bool result = false;

    if(service->is_extended_overlap) {
        return false;
    }

    uint16_t timeout = felica_estimate_timing_furi(reader->current_pmm[FELICA_PMM_READ_MRT], 1);

    for(int_least32_t cursor = 0; cursor < 0x10000; cursor++) {
        FelicaBlock* new_block = FelicaBlockArray_push_new(service->blocks);
        furi_assert(new_block != NULL);

        FelicaRWRequestBlockDescriptor blocks[1] = {{
            .access_mode = FelicaBlockAccessModeDefault,
            .block_number = cursor & 0xffff,
            .service_index = 0,
        }};
        tx_rx->tx_bits = 8 * felica_prepare_unencrypted_read(
                                 tx_rx->tx_data,
                                 reader,
                                 service_codes,
                                 sizeof(service_codes) / sizeof(service_codes[0]),
                                 blocks,
                                 1);
        if(!furi_hal_nfc_tx_rx(tx_rx, timeout)) {
            FURI_LOG_E(TAG, "Bad exchange when reading service");
            break;
        }

        if(!felica_parse_unencrypted_read(
               tx_rx->rx_data,
               tx_rx->rx_bits / 8,
               reader,
               new_block->data,
               sizeof(new_block->data))) {
            // Hitting an invalid block address (usually means end of service)
            // Some tags (namely 12fc) also uses access denied to signal end of tag
            if(reader->status_flags[0] != 0x00 &&
               (reader->status_flags[1] == 0xa8 || reader->status_flags[1] == 0xa5)) {
                FURI_LOG_D(TAG, "End of service reached");
                FelicaBlockArray_pop_back(NULL, service->blocks);
                result = true;
                break;
            }
            // Otherwise there's an error. Ending the read
            FURI_LOG_E(
                TAG, "Bad response to Read without Encryption (block %" PRIdLEAST32 ")", cursor);
            break;
        }
    }

    // Delete all blocks we currently have if dump has failed
    if(!result) {
        FelicaBlockArray_reset(service->blocks);
    }

    FURI_LOG_D(TAG, "%d blocks dumped", FelicaBlockArray_size(service->blocks));
    return result;
}

bool felica_std_dump_data(FuriHalNfcTxRxContext* tx_rx, FelicaReader* reader, FelicaData* data) {
    for
        M_EACH(system, data->systems, FelicaSystemArray_t) {
            // select and traverse the system
            if(!felica_std_select_system(tx_rx, reader, system->code)) {
                return false;
            }
            if(!felica_std_traverse_system(tx_rx, reader, system)) {
                return false;
            }
            for
                M_EACH(service_kv, system->public_services, FelicaPublicServiceDict_t) {
                    uint16_t service_code = service_kv->key;
                    FelicaNode* node = felica_std_inode_lookup(system, service_kv->value);
                    furi_assert(node != NULL);
                    furi_assert(node->type == FelicaNodeTypeService);
                    furi_assert(node->service != NULL);
                    if(!felica_std_dump_public_service(
                           tx_rx, reader, node->service, service_code)) {
                        return false;
                    }
                }
        }

    return true;
}

void felica_init(FelicaData* data, FelicaICType ic_type) {
    furi_assert(data != NULL);
    FelicaSystemArray_init(data->systems);
    data->type = ic_type;
    data->is_monolithic = false;
}

void felica_reader_init(FelicaReader* reader, uint8_t* idm, uint8_t* pmm) {
    furi_assert(reader != NULL && idm != NULL && pmm != NULL);
    memset(reader, 0, sizeof(*reader));
    memcpy(reader->current_idm, idm, 8);
    memcpy(reader->current_pmm, pmm, 8);
}

FelicaReadResult felica_lite_detect_and_read(
    FuriHalNfcTxRxContext* tx_rx,
    FelicaData* data,
    FelicaReader* reader) {
    furi_assert(tx_rx != NULL);
    furi_assert(data != NULL);
    furi_assert(reader != NULL);

    if(!felica_std_select_system(tx_rx, reader, LITE_SYSTEM_CODE)) {
        return FelicaReadResultTypeMismatch;
    }

    FURI_LOG_I(TAG, "Reading Felica Lite system");
    data->is_monolithic = true;

    FelicaSystem* lite_system =
        felica_gen_monolithic_system_code(reader, data->systems, LITE_SYSTEM_CODE);
    if(!felica_lite_dump_data(tx_rx, reader, data, lite_system)) {
        return FelicaReadResultTagLost;
    }
    return FelicaReadResultSuccess;
}

FelicaReadResult felica_std_detect_and_read(
    FuriHalNfcTxRxContext* tx_rx,
    FelicaData* data,
    FelicaReader* reader) {
    furi_assert(tx_rx != NULL);
    furi_assert(data != NULL);
    furi_assert(reader != NULL);

    if(!felica_std_request_system_code(tx_rx, reader, data->systems)) {
        return FelicaReadResultTypeMismatch;
    }

    FURI_LOG_I(TAG, "Reading Felica Standard system");
    data->is_monolithic = false;

    if(!felica_std_dump_data(tx_rx, reader, data)) {
        return FelicaReadResultTagLost;
    }

    return FelicaReadResultSuccess;
}

FelicaReadResult felica_ndef_detect_and_read(
    FuriHalNfcTxRxContext* tx_rx,
    FelicaData* data,
    FelicaReader* reader) {
    furi_assert(tx_rx != NULL);
    furi_assert(data != NULL);
    furi_assert(reader != NULL);

    if(!felica_std_select_system(tx_rx, reader, NDEF_SYSTEM_CODE)) {
        return FelicaReadResultTypeMismatch;
    }

    FURI_LOG_I(TAG, "Reading Felica NDEF system");
    data->is_monolithic = true;

    FelicaSystem* ndef_system =
        felica_gen_monolithic_system_code(reader, data->systems, NDEF_SYSTEM_CODE);
    if(!felica_ndef_dump_data(tx_rx, reader, data, ndef_system)) {
        return FelicaReadResultTagLost;
    }
    return FelicaReadResultSuccess;
}

void felica_service_clear(FelicaService* service) {
    furi_assert(service != NULL);
    FelicaBlockArray_clear(service->blocks);
    FelicaServiceAttributeSet_init(service->access_control_list);
}

void felica_lite_clear(FelicaLiteInfo* lite_info) {
    furi_assert(lite_info != NULL);
    for(int i = 0; i < REG_LITE_BLOCK; i++) {
        uint8_t* block = lite_info->S_PAD[i];
        if(block != NULL) {
            free(block);
        }
    }

    if(lite_info->REG != NULL) {
        free(lite_info->REG);
    }

    if(lite_info->card_key_1 != NULL) {
        free(lite_info->card_key_1);
    }
    if(lite_info->card_key_2 != NULL) {
        free(lite_info->card_key_2);
    }
}

void felica_system_init(FelicaSystem* system, uint8_t number, uint16_t code) {
    furi_assert(system != NULL);
    system->number = number;
    system->code = code;
}

void felica_std_system_init(FelicaSystem* system, uint8_t number, uint16_t code) {
    felica_system_init(system, number, code);
    system->is_lite = false;
    system->is_monolithic_ndef = false;
    FelicaNodeArray_init(system->nodes);
    FelicaPublicServiceDict_init(system->public_services);
}

void felica_node_init_as_area(
    FelicaNode* node,
    FelicaINode parent,
    uint16_t area_number,
    bool can_create_subareas,
    uint16_t end_service_code) {
    furi_assert(node != NULL);
    node->parent = parent;
    node->type = FelicaNodeTypeArea;
    node->area = calloc(1, sizeof(*(node->area)));
    furi_assert(node->area != NULL);
    node->area->number = area_number;
    node->area->can_create_subareas = can_create_subareas;
    node->area->end_service_code = end_service_code;
    FelicaINodeArray_init(node->area->nodes);
}

void felica_node_init_as_service(
    FelicaNode* node,
    FelicaINode parent,
    uint16_t service_number,
    FelicaServiceType service_type) {
    furi_assert(node != NULL);
    node->parent = parent;
    node->type = FelicaNodeTypeService;
    node->service = calloc(1, sizeof(*(node->service)));
    furi_assert(node->service != NULL);

    node->service->number = service_number;
    node->service->type = service_type;
    node->service->is_extended_overlap = false;

    FelicaServiceAttributeSet_init(node->service->access_control_list);
    FelicaBlockArray_init(node->service->blocks);
}

void felica_node_clear(FelicaNode* node);

void felica_area_clear(FelicaArea* area) {
    furi_assert(area != NULL);
    FelicaINodeArray_clear(area->nodes);
}

void felica_node_clear(FelicaNode* node) {
    furi_assert(node != NULL);
    if(node->type == FelicaNodeTypeArea) {
        felica_area_clear(node->area);
        free(node->area);
    } else if(node->type == FelicaNodeTypeService) {
        felica_service_clear(node->service);
        free(node->service);
    }
}

void felica_reset(FelicaData* data) {
    furi_assert(data != NULL);
    for
        M_EACH(system, data->systems, FelicaSystemArray_t) {
            if(system->is_lite) {
                felica_lite_clear(&system->lite_info);
            } else if(system->is_monolithic_ndef) {
                felica_node_clear(&system->ndef_node);
            } else {
                // TODO refactor FelicaNodeArray_t to use mlib objects instead of POD?
                for
                    M_EACH(node, system->nodes, FelicaNodeArray_t) {
                        felica_node_clear(node);
                    }
                FelicaPublicServiceDict_clear(system->public_services);
            }
        }
    FelicaSystemArray_reset(data->systems);
}

void felica_clear(FelicaData* data) {
    felica_reset(data);
    FelicaSystemArray_clear(data->systems);
}
