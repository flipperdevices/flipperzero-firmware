#include <limits.h>
#include <mbedtls/sha1.h>
#include "felica.h"
#include "nfc_util.h"
#include <furi.h>
#include "furi_hal_nfc.h"

#define TAG "FeliCa"

bool felica_check_ic_type(uint8_t* PMm) {
    uint8_t ic_type = PMm[0];
    uint8_t rom_type = PMm[1];

    bool is_valid_ic = false;
    if(ic_type == 0xff) { // RC-S967 in nfc-dep
        is_valid_ic = true;
    } else if(ic_type == 0xf0 || ic_type == 0xf2) { // Lite(S)
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
        return FelicaICTypeLink;
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

uint8_t felica_prepare_unencrypted_read(
    uint8_t* dest,
    const FelicaReader* reader,
    const uint16_t* service_code_list,
    uint8_t service_count,
    const uint32_t* block_list,
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
        uint16_t block_num = block_list[i];
        dest[msg_len++] = block_num & 0xFF;
        dest[msg_len++] = block_num >> 8;
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
    if(len < 12) {
        return false;
    }
    len--;
    buf++;

    if(*buf != FELICA_UNENCRYPTED_READ_RES) {
        return false;
    }
    len--;
    buf++;

    if(memcmp(buf, reader->current_idm, 8) != 0) {
        return false;
    }
    len -= 8;
    buf += 8;

    reader->status_flags[0] = buf[0];
    reader->status_flags[1] = buf[1];
    len -= 2;
    buf += 2;
    if(reader->status_flags[0] != 0) {
        FURI_LOG_W(TAG, "SF1: %02X SF2: %02X", reader->status_flags[0], reader->status_flags[1]);
        return 0;
    }

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
    if(len < 12) {
        return false;
    }
    len--;
    buf++;

    if(*buf != FELICA_UNENCRYPTED_WRITE_RES) {
        return false;
    }
    len--;
    buf++;

    if(memcmp(buf, reader->current_idm, 8) != 0) {
        return false;
    }
    len -= 8;
    buf += 8;

    reader->status_flags[0] = buf[0];
    reader->status_flags[1] = buf[1];
    len -= 2;
    buf += 2;
    if(reader->status_flags[0] != 0) {
        FURI_LOG_W(TAG, "SF1: %02X SF2: %02X", reader->status_flags[0], reader->status_flags[1]);
        return 0;
    }

    return true;
}

void felica_parse_system_info(FelicaSystem* system, uint8_t* IDm, uint8_t* PMm) {
    memcpy(system->idm, IDm, 8);
    memcpy(system->pmm, PMm, 8);
    for(int i = 0; i < 6; i++) {
        char MRT_byte = PMm[2 + i];
        FelicaMRTParts* mrt_data = &system->maximum_response_times[i];
        mrt_data->real_a = (MRT_byte & 7) + 1;
        MRT_byte >>= 3;
        mrt_data->real_b = (MRT_byte & 7) + 1;
        MRT_byte >>= 3;
        mrt_data->exponent = (MRT_byte & 3);
    }
}

bool felica_lite_can_read_without_mac(uint8_t* mc_r_restr, uint8_t block_number) {
    if(block_number > REG_LITE_BLOCK) {
        return true;
    }
    uint8_t byte = mc_r_restr[block_number < 8 ? 0 : 1];
    return ((byte >> (block_number % 8)) & 1) == 0;
}

void felica_define_normal_block(FelicaService* service, uint16_t number, uint8_t* data) {
    FelicaBlock* block = malloc(sizeof(FelicaBlock));
    memcpy(block->data, data, FELICA_BLOCK_SIZE);
    FelicaBlockList_set_at(service->blocks, number, block);
}

bool felica_read_lite_system(
    FuriHalNfcTxRxContext* tx_rx,
    FelicaReader* reader,
    FelicaData* data,
    FelicaSystem* system) {
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

    uint8_t block_data[FELICA_BLOCK_SIZE * 4];

    tx_rx->tx_bits =
        8 * felica_lite_prepare_unencrypted_read(tx_rx->tx_data, reader, true, fixed_blocks, 1);
    if(!furi_hal_nfc_tx_rx_full(tx_rx)) {
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
    if(!furi_hal_nfc_tx_rx_full(tx_rx)) {
        FURI_LOG_W(TAG, "Bad exchange reading D_ID");
        return false;
    }
    if(felica_parse_unencrypted_read(
           tx_rx->rx_data, tx_rx->rx_bits / 8, reader, block_data, sizeof(block_data)) !=
       FELICA_BLOCK_SIZE) {
        FURI_LOG_W(TAG, "Bad response to Read without Encryption (D_ID)");
        return false;
    }
    if(memcmp(system->idm, block_data, 8) != 0 || memcmp(system->pmm, block_data + 8, 8) != 0) {
        FURI_LOG_W(TAG, "Mismatching values for D_ID");
        return false;
    }

    system->code = LITE_SYSTEM_CODE;

    FelicaLiteInfo* lite_info = &system->lite_info;
    lite_info->card_key_1 = NULL;
    lite_info->card_key_2 = NULL;

    tx_rx->tx_bits = 8 * felica_lite_prepare_unencrypted_read(
                             tx_rx->tx_data, reader, true, &fixed_blocks[2], 1);
    if(!furi_hal_nfc_tx_rx_full(tx_rx)) {
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
    if(!furi_hal_nfc_tx_rx_full(tx_rx)) {
        FURI_LOG_W(TAG, "Bad exchange writing random challenge");
        return false;
    }
    if(!felica_parse_unencrypted_write(tx_rx->rx_data, tx_rx->rx_bits / 8, reader)) {
        FURI_LOG_W(TAG, "Bad response to Write without Encryption (RC)");
        return false;
    }

    tx_rx->tx_bits = 8 * felica_lite_prepare_unencrypted_read(
                             tx_rx->tx_data, reader, true, &fixed_blocks[4], 2);
    if(!furi_hal_nfc_tx_rx_full(tx_rx)) {
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
    if(!furi_hal_nfc_tx_rx_full(tx_rx)) {
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
        if(!furi_hal_nfc_tx_rx_full(tx_rx)) {
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
        if(!furi_hal_nfc_tx_rx_full(tx_rx)) {
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
        if(!furi_hal_nfc_tx_rx_full(tx_rx)) {
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

bool felica_read_card(
    FuriHalNfcTxRxContext* tx_rx,
    FelicaData* data,
    uint8_t* polled_idm,
    uint8_t* polled_pmm) {
    furi_assert(tx_rx);
    furi_assert(polled_idm);
    furi_assert(polled_pmm);

    bool card_read = false;
    do {
        FelicaReader reader;
        memcpy(reader.current_idm, polled_idm, 8);
        memcpy(reader.current_pmm, polled_pmm, 8);

        FelicaSystem* current_system = malloc(sizeof(FelicaSystem));
        FelicaSystemList_init(data->systems);
        FelicaSystemList_push_back(data->systems, current_system);

        felica_parse_system_info(current_system, polled_idm, polled_pmm);

        if(data->type == FelicaICTypeLite || data->type == FelicaICTypeLiteS) {
            FURI_LOG_I(TAG, "Reading Felica Lite system");
            felica_read_lite_system(tx_rx, &reader, data, current_system);
            card_read = true;
            break;
        }
    } while(false);

    return card_read;
}

void felica_service_clear(FelicaService* service) {
    FelicaBlockList_it_t it;
    for(FelicaBlockList_it(it, service->blocks); !FelicaBlockList_end_p(it);
        FelicaBlockList_next(it)) {
        FelicaBlock* block = *FelicaBlockList_ref(it);
        free(block);
    }
    FelicaBlockList_clear(service->blocks);
}

void felica_lite_clear(FelicaLiteInfo* lite_info) {
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

void felica_area_clear(FelicaArea* area) {
    FelicaNodeList_it_t it;
    for(FelicaNodeList_it(it, area->nodes); !FelicaNodeList_end_p(it); FelicaNodeList_next(it)) {
        FelicaNode* node = *FelicaNodeList_ref(it);
        if(node->type == FelicaNodeTypeArea) {
            felica_area_clear(node->area);
        } else if(node->type == FelicaNodeTypeService) {
            felica_service_clear(node->service);
        }
        free(node);
    }
    FelicaNodeList_clear(area->nodes);
}

void felica_clear(FelicaData* data) {
    FelicaSystemList_it_t it;
    for(FelicaSystemList_it(it, data->systems); !FelicaSystemList_end_p(it);
        FelicaSystemList_next(it)) {
        FelicaSystem* system = *FelicaSystemList_ref(it);
        if(system->code == LITE_SYSTEM_CODE) {
            felica_lite_clear(&system->lite_info);
            ;
        } else {
            felica_area_clear(&system->root_area);
        }
    }
    FelicaSystemList_clear(data->systems);
}