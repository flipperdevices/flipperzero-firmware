#include "nfca.h"

#include <furi.h>
#include <nfc/nfc_common.h>

#define NFCA_CRC_INIT (0x6363)
#define NFCA_PROTOCOL_NAME "Unknown ISO14443-3A Tag"

const NfcProtocolBase nfc_protocol_iso14443_3a = {
    .alloc = (NfcProtocolAlloc)nfca_alloc,
    .free = (NfcProtocolFree)nfca_free,
    .reset = (NfcProtocolReset)nfca_reset,
    .copy = (NfcProtocolCopy)nfca_copy,
    .verify = (NfcProtocolVerify)nfca_verify,
    .load = (NfcProtocolLoad)nfca_load,
    .save = (NfcProtocolSave)nfca_save,
    .is_equal = (NfcProtocolEqual)nfca_is_equal,
    .get_name = (NfcProtocolGetName)nfca_get_name,
    .get_uid = (NfcProtocolGetUid)nfca_get_uid,
};

NfcaData* nfca_alloc() {
    NfcaData* data = malloc(sizeof(NfcaData));
    return data;
}

void nfca_free(NfcaData* data) {
    furi_assert(data);

    free(data);
}

void nfca_reset(NfcaData* data) {
    furi_assert(data);

    UNUSED(data);
}

void nfca_copy(NfcaData* data, const NfcaData* other) {
    furi_assert(data);
    furi_assert(other);

    *data = *other;
}

bool nfca_verify(NfcaData* data, const FuriString* device_type) {
    UNUSED(data);
    return furi_string_equal(device_type, "UID");
}

bool nfca_load(NfcaData* data, FlipperFormat* ff, uint32_t version) {
    return nfca_load_data(data, ff, version);
    return true;
}

bool nfca_save(const NfcaData* data, FlipperFormat* ff, uint32_t version) {
    bool saved = false;
    do {
        if(!flipper_format_write_string_cstr(ff, "Device type", "UID")) break;
        if(!nfca_save_data(data, ff, version)) break;
        saved = true;
    } while(false);

    return saved;
}

bool nfca_is_equal(const NfcaData* data, const NfcaData* other) {
    furi_assert(data);
    furi_assert(other);

    return memcmp(data, other, sizeof(NfcaData)) == 0;
}

const char* nfca_get_name(const NfcaData* data) {
    UNUSED(data);
    return NFCA_PROTOCOL_NAME;
}

const uint8_t* nfca_get_uid(const NfcaData* data, size_t* uid_len) {
    furi_assert(data);

    if(uid_len) {
        *uid_len = data->uid_len;
    }

    return data->uid;
}

uint16_t nfca_get_crc(uint8_t* buff, uint16_t len) {
    furi_assert(buff);

    uint16_t crc = NFCA_CRC_INIT;
    uint8_t byte = 0;

    for(uint8_t i = 0; i < len; i++) {
        byte = buff[i];
        byte ^= (uint8_t)(crc & 0xff);
        byte ^= byte << 4;
        crc = (crc >> 8) ^ (((uint16_t)byte) << 8) ^ (((uint16_t)byte) << 3) ^
              (((uint16_t)byte) >> 4);
    }

    return crc;
}

void nfca_append_crc(uint8_t* buff, uint16_t len) {
    furi_assert(buff);

    uint16_t crc = nfca_get_crc(buff, len);
    buff[len] = (uint8_t)crc;
    buff[len + 1] = (uint8_t)(crc >> 8);
}

bool nfca_check_crc(uint8_t* buff, uint16_t len) {
    bool crc_ok = false;

    if(len > 2) {
        uint16_t crc_calc = nfca_get_crc(buff, len - 2);
        uint16_t crc_received = (buff[len - 1] << 8) | buff[len - 2];
        crc_ok = (crc_calc == crc_received);
    }

    return crc_ok;
}

bool nfca_load_data(NfcaData* data, FlipperFormat* ff, uint32_t version) {
    furi_assert(data);

    uint32_t data_cnt = 0;
    bool parsed = false;

    do {
        if(!flipper_format_get_value_count(ff, "UID", &data_cnt)) break;
        if(!(data_cnt == 4 || data_cnt == 7)) break;
        data->uid_len = data_cnt;
        if(!flipper_format_read_hex(ff, "UID", data->uid, data->uid_len)) break;
        if(version == NFC_LSB_ATQA_FORMAT_VERSION) {
            if(!flipper_format_read_hex(ff, "ATQA", data->atqa, 2)) break;
        } else {
            uint8_t atqa[2] = {};
            if(!flipper_format_read_hex(ff, "ATQA", atqa, 2)) break;
            data->atqa[0] = atqa[1];
            data->atqa[1] = atqa[0];
        }
        if(!flipper_format_read_hex(ff, "SAK", &data->sak, 1)) break;

        parsed = true;
    } while(false);

    return parsed;
}

bool nfca_save_data(const NfcaData* data, FlipperFormat* ff, uint32_t version) {
    furi_assert(data);

    UNUSED(version);

    bool saved = false;
    do {
        // Write UID, ATQA, SAK
        if(!flipper_format_write_comment_cstr(ff, "UID, ATQA and SAK are common for all formats"))
            break;
        if(!flipper_format_write_hex(ff, "UID", data->uid, data->uid_len)) break;
        // Save ATQA in MSB order for correct companion apps display
        uint8_t atqa[2] = {data->atqa[1], data->atqa[0]};
        if(!flipper_format_write_hex(ff, "ATQA", atqa, 2)) break;
        if(!flipper_format_write_hex(ff, "SAK", &data->sak, 1)) break;
        saved = true;
    } while(false);

    return saved;
}
