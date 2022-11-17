#include "nfc_supported_card.h"
#include <gui/modules/widget.h>
#include <nfc_worker_i.h>

#include "furi_hal.h"

static const MfClassicAuthContext plantain_keys_all[] = {
    {.sector = 0, .key_a = 0xffffffffffff, .key_b = 0xffffffffffff},
    {.sector = 1, .key_a = 0xffffffffffff, .key_b = 0xffffffffffff},
    {.sector = 2, .key_a = 0xffffffffffff, .key_b = 0xffffffffffff},
    {.sector = 3, .key_a = 0xffffffffffff, .key_b = 0xffffffffffff},
    {.sector = 4, .key_a = 0xe56ac127dd45, .key_b = 0x19fc84a3784b},
    {.sector = 5, .key_a = 0x77dabc9825e1, .key_b = 0x9764fec3154a},
    {.sector = 6, .key_a = 0xffffffffffff, .key_b = 0xffffffffffff},
    {.sector = 7, .key_a = 0xffffffffffff, .key_b = 0xffffffffffff},
    {.sector = 8, .key_a = 0x26973ea74321, .key_b = 0xd27058c6e2c7},
    {.sector = 9, .key_a = 0xeb0a8ff88ade, .key_b = 0x578a9ada41e3},
    {.sector = 10, .key_a = 0xea0fd73cb149, .key_b = 0x29c35fa068fb},
    {.sector = 11, .key_a = 0xc76bf71a2509, .key_b = 0x9ba241db3f56},
    {.sector = 12, .key_a = 0xacffffffffff, .key_b = 0x71f3a315ad26},
    {.sector = 13, .key_a = 0xffffffffffff, .key_b = 0xffffffffffff},
    {.sector = 14, .key_a = 0xffffffffffff, .key_b = 0xffffffffffff},
    {.sector = 15, .key_a = 0xffffffffffff, .key_b = 0xffffffffffff},
    {.sector = 16, .key_a = 0x72f96bdd3714, .key_b = 0x462225cd34cf},
    {.sector = 17, .key_a = 0x044ce1872bc3, .key_b = 0x8c90c70cff4a},
    {.sector = 18, .key_a = 0xbc2d1791dec1, .key_b = 0xca96a487de0b},
    {.sector = 19, .key_a = 0x8791b2ccb5c4, .key_b = 0xc956c3b80da3},
    {.sector = 20, .key_a = 0x8e26e45e7d65, .key_b = 0x8e65b3af7d22},
    {.sector = 21, .key_a = 0x0f318130ed18, .key_b = 0x0c420a20e056},
    {.sector = 22, .key_a = 0x045ceca15535, .key_b = 0x31bec3d9e510},
    {.sector = 23, .key_a = 0x9d993c5d4ef4, .key_b = 0x86120e488abf},
    {.sector = 24, .key_a = 0xc65d4eaa645b, .key_b = 0xb69d40d1a439},
    {.sector = 25, .key_a = 0x3a8a139c20b4, .key_b = 0x8818a9c5d406},
    {.sector = 26, .key_a = 0xbaff3053b496, .key_b = 0x4b7cb25354d3},
    {.sector = 27, .key_a = 0x7413b599c4ea, .key_b = 0xb0a2AAF3A1BA},
    {.sector = 28, .key_a = 0x0ce7cd2cc72b, .key_b = 0xfa1fbb3f0f1f},
    {.sector = 29, .key_a = 0x0be5fac8b06a, .key_b = 0x6f95887a4fd3},
    {.sector = 30, .key_a = 0x0eb23cc8110b, .key_b = 0x04dc35277635},
    {.sector = 31, .key_a = 0xbc4580b7f20b, .key_b = 0xd0a4131fb290},
    {.sector = 32, .key_a = 0x7a396f0d633d, .key_b = 0xad2bdc097023},
    {.sector = 33, .key_a = 0xa3faa6daff67, .key_b = 0x7600e889adf9},
    {.sector = 34, .key_a = 0xfd8705e721b0, .key_b = 0x296fc317a513},
    {.sector = 35, .key_a = 0x22052b480d11, .key_b = 0xe19504c39461},
    {.sector = 36, .key_a = 0xa7141147d430, .key_b = 0xff16014fefc7},
    {.sector = 37, .key_a = 0x8a8d88151a00, .key_b = 0x038b5f9b5a2a},
    {.sector = 38, .key_a = 0xb27addfb64b0, .key_b = 0x152fd0c420a7},
    {.sector = 39, .key_a = 0x7259fa0197c6, .key_b = 0x5583698df085},
};

static const MfClassicAuthContext alt_plantain_keys[] = {
    //not constantly
    {.sector = 2, .key_a = 0xa0a1a2a3a4a5, .key_b = 0xb0b2b3b4b5b5},
    {.sector = 3, .key_a = 0xa0a1a2a3a4a5, .key_b = 0xb0b2b3b4b5b5},
    {.sector = 7, .key_a = 0x2066f4727129, .key_b = 0xf7a65799c6ee},
    {.sector = 15, .key_a = 0xa0a1a2a3a4a5, .key_b = 0x103c08acceb2},

    {.sector = 25, .key_a = 0x46d78e850a7e, .key_b = 0xa740f8130991},
    {.sector = 26, .key_a = 0x42e9b54e51ab, .key_b = 0x0231b86df52e},
    {.sector = 27, .key_a = 0x0f01ceff2742, .key_b = 0x6fec74559ca7},
    {.sector = 28, .key_a = 0xb81f2b0c2f66, .key_b = 0xa7e2d95f0003},
    {.sector = 29, .key_a = 0x9ea3387a63c1, .key_b = 0x437e59f57561},

    //EKP Keys
    {.sector = 32, .key_a = 0x7A396F0D633D, .key_b = 0xAD2BDC097023},
    {.sector = 33, .key_a = 0xA3FAA6DAFF67, .key_b = 0x7600E889ADF9},
    {.sector = 34, .key_a = 0xFD8705E721B0, .key_b = 0x296FC317A513},
    {.sector = 35, .key_a = 0xC9822A101508, .key_b = 0x88819B6B2632},
    {.sector = 36, .key_a = 0x424DA92F838A, .key_b = 0xFDDFF8FE692E},
    {.sector = 37, .key_a = 0x6141FE928401, .key_b = 0xC744FF0BAA94},
    {.sector = 38, .key_a = 0x1EDF969F11D0, .key_b = 0x34DB430A0498},
    {.sector = 39, .key_a = 0xE341067BFB71, .key_b = 0x61C928D7231B},

    // Type 3
    {.sector = 0, .key_a = 0xa0a1a2a3a4a5, .key_b = 0xb0b2b3b4b5b5},
    {.sector = 4, .key_a = 0xffffffffffff, .key_b = 0xffffffffffff},
    {.sector = 5, .key_a = 0xffffffffffff, .key_b = 0xffffffffffff},
    {.sector = 9, .key_a = 0xeb0a8ff88ade, .key_b = 0x578a9ada41e3},
    {.sector = 12, .key_a = 0x000000000000, .key_b = 0x71f3a315ad26},
    {.sector = 13, .key_a = 0xac70ca327a04, .key_b = 0xf29411c2663c},
    {.sector = 14, .key_a = 0x51044efb5aab, .key_b = 0xebdc720dd1ce},

    {.sector = 4, .key_a = 0xe56ac127dd45, .key_b = 0x19fc84a3784b},
    {.sector = 5, .key_a = 0x77dabc9825e1, .key_b = 0x9764fec3154a},
    {.sector = 8, .key_a = 0xa73f5dc1d333, .key_b = 0xd27058c6e2c7}, //B unknown, TODO
};

bool plantain_parser_verify(NfcWorker* nfc_worker, FuriHalNfcTxRxContext* tx_rx) {
    furi_assert(nfc_worker);
    UNUSED(nfc_worker);

    if(nfc_worker->dev_data->mf_classic_data.type != MfClassicType4k &&
       nfc_worker->dev_data->mf_classic_data.type != MfClassicType1k) {
        return false;
    }

    uint8_t sector = 8;
    uint8_t block = mf_classic_get_sector_trailer_block_num_by_sector(sector);
    FURI_LOG_D("Plant", "Verifying sector %d", sector);
    if(mf_classic_authenticate(tx_rx, block, 0x26973ea74321, MfClassicKeyA)) {
        FURI_LOG_D("Plant", "Sector %d verified", sector);
        return true;
    }
    return false;
}

uint8_t plantain_calculate_luhn_str(const char* num) {
    // No.
    // Yes :)

    uint8_t sum = 0;
    uint8_t len = strlen(num);
    for(uint8_t i = 0; i < len; i++) {
        uint8_t digit = num[i] - '0';
        if(i % 2 == 0) {
            digit *= 2;
            if(digit > 9) {
                digit -= 9;
            }
        }
        sum += digit;
    }
    return (10 - (sum % 10)) % 10;
}

bool plantain_parser_read(NfcWorker* nfc_worker, FuriHalNfcTxRxContext* tx_rx) {
    furi_assert(nfc_worker);
    uint8_t block;
    MfClassicData* mf_classic_data = &nfc_worker->dev_data->mf_classic_data;

    MfClassicType type = mf_classic_data->type;
    size_t sectors = type == MfClassicType4k ? 40 : 16;
    FURI_LOG_D("Plant", "Card type: %s", type == MfClassicType4k ? "4K" : "1K");
    bool key_a_found;
    bool key_b_found;
    for(size_t i = 0; i < sectors; i++) {
        key_a_found = false;
        key_b_found = false;
        FURI_LOG_D("Plant", "Authenticating sector %d", plantain_keys_all[i].sector);
        for(size_t j = 0; j < COUNT_OF(alt_plantain_keys); j++) {
            if(plantain_keys_all[i].sector != alt_plantain_keys[j].sector) {
                continue;
            }
            block = mf_classic_get_sector_trailer_block_num_by_sector(alt_plantain_keys[j].sector);

            if(!key_a_found) {
                if(mf_classic_authenticate(
                       tx_rx, block, alt_plantain_keys[j].key_a, MfClassicKeyA)) {
                    mf_classic_set_key_found(
                        mf_classic_data,
                        alt_plantain_keys[j].sector,
                        MfClassicKeyA,
                        alt_plantain_keys[j].key_a);
                    key_a_found = true;
                    FURI_LOG_D("Plant", "Sector %d alt key a found", plantain_keys_all[i].sector);
                }
                furi_hal_nfc_sleep();
            }
            if(!key_b_found) {
                if(mf_classic_authenticate(
                       tx_rx, block, alt_plantain_keys[j].key_b, MfClassicKeyB)) {
                    mf_classic_set_key_found(
                        mf_classic_data,
                        alt_plantain_keys[j].sector,
                        MfClassicKeyB,
                        alt_plantain_keys[j].key_b);
                    key_b_found = true;
                    FURI_LOG_D("Plant", "Sector %d alt key b found", plantain_keys_all[i].sector);
                }
                furi_hal_nfc_sleep();
            }
        }
        if(!key_a_found) {
            mf_classic_set_key_found(
                mf_classic_data,
                plantain_keys_all[i].sector,
                MfClassicKeyA,
                plantain_keys_all[i].key_a);
            FURI_LOG_D("Plant", "Sector %d def key a found", plantain_keys_all[i].sector);
        }
        if(!key_b_found) {
            mf_classic_set_key_found(
                mf_classic_data,
                plantain_keys_all[i].sector,
                MfClassicKeyB,
                plantain_keys_all[i].key_b);
            FURI_LOG_D("Plant", "Sector %d def key b found", plantain_keys_all[i].sector);
        }
    }
    uint8_t result;
    for(size_t i = 0; i < 4; i++) {
        result = mf_classic_update_card(tx_rx, mf_classic_data);
        FURI_LOG_D("Plant", "Update card result %d", result);
        if(result == sectors) {
            return true;
        }
    }
    FURI_LOG_D("Plant", "Card not readed, total %d", result);

    return result == sectors;
}

bool is_with_key_readed(MfClassicData* mf_classic_data, uint8_t sector, uint64_t key) {
    MfClassicSectorTrailer* sec_tr =
        mf_classic_get_sector_trailer_by_sector(mf_classic_data, sector);
    if(nfc_util_bytes2num(sec_tr->key_a, 6) == key) {
        return true;
    }
    return false;
}

uint8_t get_SPBcard_type(MfClassicData* mf_classic_data) {
    if(is_with_key_readed(mf_classic_data, 4, 0xe56ac127dd45) &&
       is_with_key_readed(mf_classic_data, 5, 0x77dabc9825e1) &&
       is_with_key_readed(mf_classic_data, 8, 0x26973ea74321) &&
       is_with_key_readed(mf_classic_data, 12, 0xacffffffffff)) {
        return 1;
    }
    if(is_with_key_readed(mf_classic_data, 4, 0xe56ac127dd45) &&
       is_with_key_readed(mf_classic_data, 5, 0x77dabc9825e1) &&
       is_with_key_readed(mf_classic_data, 8, 0xa73f5dc1d333)) {
        return 2;
    }
    if(is_with_key_readed(mf_classic_data, 8, 0x26973ea74321) &&
       is_with_key_readed(mf_classic_data, 9, 0xeb0a8ff88ade) &&
       is_with_key_readed(mf_classic_data, 12, 0x000000000000) &&
       is_with_key_readed(mf_classic_data, 13, 0xac70ca327a04) &&
       is_with_key_readed(mf_classic_data, 14, 0x51044efb5aab)) {
        return 3;
    }
    return 0;
}

uint64_t getEKPnum(MfClassicData* mf_classic_data) {
    if(is_with_key_readed(mf_classic_data, 32, 0x7A396F0D633D)) {
        // Point to block 0 of sector 32
        uint8_t* temp_ptr = &mf_classic_data->block[32 * 4].value[0];
        // Read bytes 1-8 of block 0 of sector 32
        uint64_t ekp_num = (uint64_t)(*(uint64_t*)&temp_ptr[0]);
        // Convert to little endian
        ekp_num = __builtin_bswap64(ekp_num);
        return ekp_num;
    }
    return 0;
}

bool plantain_parser_parse(NfcDeviceData* dev_data) {
    MfClassicData* data = &dev_data->mf_classic_data;

    // Verify key
    MfClassicSectorTrailer* sec_tr = mf_classic_get_sector_trailer_by_sector(data, 8);
    uint64_t key = nfc_util_bytes2num(sec_tr->key_a, 6);
    if(key != plantain_keys_all[8].key_a) return false;

    uint8_t card_type = get_SPBcard_type(data);

    // uint64_t EKPnum = getEKPnum(data);
    // uint8_t isEKPcard = EKPnum != 0;

    // Point to block 0 of sector 4, value 0
    uint8_t* temp_ptr = &data->block[4 * 4].value[0];
    // Read first 4 bytes of block 0 of sector 4 from last to first and convert them to uint32_t
    // 38 18 00 00 becomes 00 00 18 38, and equals to 6200 decimal
    uint32_t balance =
        ((temp_ptr[3] << 24) | (temp_ptr[2] << 16) | (temp_ptr[1] << 8) | temp_ptr[0]) / 100;
    // Read card number
    // Point to block 0 of sector 0, value 0
    temp_ptr = &data->block[0 * 4].value[0];
    // Read first 7 bytes of block 0 of sector 0 from last to first and convert them to uint64_t
    // 80 5C 23 8A 16 31 04 becomes 04 31 16 8A 23 5C 80, and equals to 36130104729284868 decimal
    uint8_t card_number_arr[7];
    for(size_t i = 0; i < 7; i++) {
        card_number_arr[i] = temp_ptr[6 - i];
    }
    // Copy card number to uint64_t
    uint64_t card_number = 0;
    for(size_t i = 0; i < 7; i++) {
        card_number = (card_number << 8) | card_number_arr[i];
    }
    // Convert card number to string
    FuriString* card_number_str;
    card_number_str = furi_string_alloc();

    furi_string_printf(card_number_str, "%llu", card_number);

    FuriString* plantain_card_prefix;
    FuriString* full_number;
    FuriString* last_day;
    plantain_card_prefix = furi_string_alloc();
    full_number = furi_string_alloc();
    last_day = furi_string_alloc();
    uint8_t luhn;

    if(card_type == 0) {
        // Unknown card type
        furi_string_printf(
            dev_data->parsed_data,
            "\e#Unknown SPB card\nN:%s\nBalance:%ld\n",
            furi_string_get_cstr(card_number_str),
            balance);
    } else if(card_type == 1) {
        // Plantain card

        furi_string_printf(plantain_card_prefix, "96433078");
        furi_string_printf(
            full_number,
            "%s%s",
            furi_string_get_cstr(plantain_card_prefix),
            furi_string_get_cstr(card_number_str));
        luhn = plantain_calculate_luhn_str(furi_string_get_cstr(full_number));
        FURI_LOG_D("Plantain", "Luhn: %d, trans: %s", luhn, furi_string_get_cstr(full_number));

        furi_string_printf(
            dev_data->parsed_data,
            "\e#Plantain card\nNumber:\n%s\n%s%01d\nBalance:%ld\n",
            furi_string_get_cstr(plantain_card_prefix),
            furi_string_get_cstr(card_number_str),
            luhn,
            balance);
        furi_string_free(plantain_card_prefix);
    } else if(card_type == 2) {
        // Strange card
        furi_string_printf(plantain_card_prefix, "96433078");
        furi_string_printf(
            full_number,
            "%s%s",
            furi_string_get_cstr(plantain_card_prefix),
            furi_string_get_cstr(card_number_str));
        luhn = plantain_calculate_luhn_str(furi_string_get_cstr(full_number));

        furi_string_printf(
            dev_data->parsed_data,
            "\e#Strange SPB card\nNumber:\n%s\n%s%01d\nBalance:%ld\n",
            furi_string_get_cstr(plantain_card_prefix),
            furi_string_get_cstr(card_number_str),
            luhn,
            balance);
    } else if(card_type == 3) {
        // Concession card

        temp_ptr = &data->block[8 * 4].value[10];

        furi_string_printf(
            last_day, "%02d.%02d.%04d", temp_ptr[2], temp_ptr[1], temp_ptr[0] + 2000);

        furi_string_printf(plantain_card_prefix, "96433078"); // TODO: check real prefix
        furi_string_printf(
            full_number,
            "%s%s",
            furi_string_get_cstr(plantain_card_prefix),
            furi_string_get_cstr(card_number_str));
        luhn = plantain_calculate_luhn_str(furi_string_get_cstr(full_number));

        temp_ptr = &data->block[8 * 4 + 1].value[0];
        // Read bytes 3-8 of block 1 of sector 8
        FuriString* passport_series;
        passport_series = furi_string_alloc();
        furi_string_cat_printf(
            passport_series, "%c%c%c%c", temp_ptr[3], temp_ptr[4], temp_ptr[6], temp_ptr[7]);
        temp_ptr = &data->block[8 * 4 + 1].value[0];
        // Read bytes 9-11 of block 1 of sector 8
        uint32_t passport_number = (temp_ptr[11] << 16) | (temp_ptr[10] << 8) | temp_ptr[9];
        furi_string_printf(
            dev_data->parsed_data,
            "\e#Concession SPB card\nNumber:\n%s\n%s%01d\nLast day: %s\nPassport num: %s %06ld",
            furi_string_get_cstr(plantain_card_prefix),
            furi_string_get_cstr(card_number_str),
            luhn,
            furi_string_get_cstr(last_day),
            furi_string_get_cstr(passport_series),
            passport_number);
        furi_string_free(passport_series);
    } else {
        // Unknown card type
        furi_string_printf(
            dev_data->parsed_data,
            "\e#Unknown SPB card\nN:%s\nBalance:%ld\n",
            furi_string_get_cstr(card_number_str),
            balance);
    }

    // furi_string_printf(
    //     dev_data->parsed_data,
    //     "\e#Plantain\nN:%s\nBalance:%ld\nType:%d EKP:%d",
    //     furi_string_get_cstr(card_number_str),
    //     balance,
    //     card_type,
    //     isEKPcard);
    // if(isEKPcard) {
    //     furi_string_cat_printf(dev_data->parsed_data, "\nEKP:%lld", EKPnum);
    // }
    furi_string_free(card_number_str);
    furi_string_free(full_number);
    furi_string_free(last_day);
    return true;
}
