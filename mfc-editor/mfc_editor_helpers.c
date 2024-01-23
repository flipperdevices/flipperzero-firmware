#include "mfc_editor_app_i.h"

uint8_t mfc_editor_calculate_uid_bcc(uint8_t* uid, uint8_t uid_len) {
    furi_check(uid_len > 0);

    uint8_t bcc = uid[0];
    for(int i = 1; i < uid_len; i++) {
        bcc ^= uid[i];
    }

    return bcc;
}

MfcEditorAccessBits
    mfc_editor_get_block_access_bits(const MfClassicData* data, uint8_t block_num) {
    MfcEditorAccessBits result;

    uint8_t sector_num = mf_classic_get_sector_by_block(block_num);
    uint8_t trailer_block_num = mf_classic_get_sector_trailer_num_by_sector(sector_num);
    uint8_t relative_block_num = block_num - mf_classic_get_first_block_num_of_sector(sector_num);
    if(sector_num >= 32) {
        // 4K large sector - access bits affect range of blocks
        relative_block_num /= 5;
    }

    uint8_t access_byte_1 = data->block[trailer_block_num].data[6];
    uint8_t access_byte_2 = data->block[trailer_block_num].data[7];
    uint8_t access_byte_3 = data->block[trailer_block_num].data[8];

    result.bits = (FURI_BIT(access_byte_3, 4 + relative_block_num) << 2) |
                  (FURI_BIT(access_byte_3, relative_block_num) << 1) |
                  (FURI_BIT(access_byte_2, 4 + relative_block_num));
    result.check_bits = (FURI_BIT(access_byte_2, relative_block_num) << 2) |
                        (FURI_BIT(access_byte_1, 4 + relative_block_num) << 1) |
                        (FURI_BIT(access_byte_1, relative_block_num));
    // Check bits are inverted in storage, flip them to match actual bits
    result.check_bits ^= 0b111;

    return result;
}

void mfc_editor_set_block_access_bits(
    MfClassicData* data,
    uint8_t block_num,
    const MfcEditorAccessBits* access_bits) {
    uint8_t sector_num = mf_classic_get_sector_by_block(block_num);
    uint8_t trailer_block_num = mf_classic_get_sector_trailer_num_by_sector(sector_num);
    uint8_t relative_block_num = block_num - mf_classic_get_first_block_num_of_sector(sector_num);
    if(sector_num >= 32) {
        // 4K large sector - access bits affect range of blocks
        relative_block_num /= 5;
    }

    uint8_t* access_byte_1 = &data->block[trailer_block_num].data[6];
    uint8_t* access_byte_2 = &data->block[trailer_block_num].data[7];
    uint8_t* access_byte_3 = &data->block[trailer_block_num].data[8];

    if(FURI_BIT(access_bits->bits, 0)) {
        FURI_BIT_SET(*access_byte_2, 4 + relative_block_num);
    } else {
        FURI_BIT_CLEAR(*access_byte_2, 4 + relative_block_num);
    }

    if(FURI_BIT(access_bits->bits, 1)) {
        FURI_BIT_SET(*access_byte_3, relative_block_num);
    } else {
        FURI_BIT_CLEAR(*access_byte_3, relative_block_num);
    }

    if(FURI_BIT(access_bits->bits, 2)) {
        FURI_BIT_SET(*access_byte_3, 4 + relative_block_num);
    } else {
        FURI_BIT_CLEAR(*access_byte_3, 4 + relative_block_num);
    }

    // Check bits are the inverse of the the actual bits
    if(FURI_BIT(access_bits->check_bits, 0)) {
        FURI_BIT_CLEAR(*access_byte_1, relative_block_num);
    } else {
        FURI_BIT_SET(*access_byte_1, relative_block_num);
    }

    if(FURI_BIT(access_bits->check_bits, 1)) {
        FURI_BIT_CLEAR(*access_byte_1, 4 + relative_block_num);
    } else {
        FURI_BIT_SET(*access_byte_1, 4 + relative_block_num);
    }

    if(FURI_BIT(access_bits->check_bits, 2)) {
        FURI_BIT_CLEAR(*access_byte_2, relative_block_num);
    } else {
        FURI_BIT_SET(*access_byte_2, relative_block_num);
    }
}
