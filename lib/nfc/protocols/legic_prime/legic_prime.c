#include "legic_prime_i.h"
#include "legic_prime_crc.h"

#include <flipper_format/flipper_format.h>
#include <furi.h>

#include <nfc/nfc_common.h>

#define LEGIC_PRIME_PROTOCOL_NAME "LEGIC Prime"
#define LEGIC_PRIME_TYPE_KEY      "Legic Prime Type"
#define LEGIC_PRIME_DATA_KEY      "Data"

typedef struct {
    uint16_t mem_size;
    const char* type_name;
    const char* full_name;
    // As Proxmark3 armsrc/legicrf.c init_card and setup_phase.
    LegicPrimeProtocolParams params;
} LegicPrimeFeatures;

static const LegicPrimeFeatures legic_prime_features[LegicPrimeTypeNum] = {
    [LegicPrimeTypeMim22] =
        {
            .mem_size = LEGIC_PRIME_MEM_SIZE_MIM22,
            .type_name = "MIM22",
            .full_name = "LEGIC Prime MIM22",
            .params = {.cmd_size_bits = 6, .type_code = 0x0D, .ack_code = 0x19},
        },
    [LegicPrimeTypeMim256] =
        {
            .mem_size = LEGIC_PRIME_MEM_SIZE_MIM256,
            .type_name = "MIM256",
            .full_name = "LEGIC Prime MIM256",
            .params = {.cmd_size_bits = 9, .type_code = 0x1D, .ack_code = 0x39},
        },
    [LegicPrimeTypeMim1024] =
        {
            .mem_size = LEGIC_PRIME_MEM_SIZE_MIM1024,
            .type_name = "MIM1024",
            .full_name = "LEGIC Prime MIM1024",
            .params = {.cmd_size_bits = 11, .type_code = 0x3D, .ack_code = 0x39},
        },
};

const NfcDeviceBase nfc_device_legic_prime = {
    .protocol_name = LEGIC_PRIME_PROTOCOL_NAME,
    .alloc = (NfcDeviceAlloc)legic_prime_alloc,
    .free = (NfcDeviceFree)legic_prime_free,
    .reset = (NfcDeviceReset)legic_prime_reset,
    .copy = (NfcDeviceCopy)legic_prime_copy,
    .verify = (NfcDeviceVerify)legic_prime_verify,
    .load = (NfcDeviceLoad)legic_prime_load,
    .save = (NfcDeviceSave)legic_prime_save,
    .is_equal = (NfcDeviceEqual)legic_prime_is_equal,
    .get_name = (NfcDeviceGetName)legic_prime_get_device_name,
    .get_uid = (NfcDeviceGetUid)legic_prime_get_uid,
    .set_uid = (NfcDeviceSetUid)legic_prime_set_uid,
    .get_base_data = (NfcDeviceGetBaseData)legic_prime_get_base_data,
};

LegicPrimeData* legic_prime_alloc(void) {
    LegicPrimeData* data = malloc(sizeof(LegicPrimeData));
    legic_prime_reset(data);
    return data;
}

void legic_prime_free(LegicPrimeData* data) {
    furi_check(data);
    free(data);
}

void legic_prime_reset(LegicPrimeData* data) {
    furi_check(data);
    memset(data, 0, sizeof(LegicPrimeData));
}

void legic_prime_copy(LegicPrimeData* data, const LegicPrimeData* other) {
    furi_check(data);
    furi_check(other);
    *data = *other;
}

bool legic_prime_verify(LegicPrimeData* data, const FuriString* device_type) {
    UNUSED(data);
    furi_check(device_type);
    return furi_string_equal_str(device_type, LEGIC_PRIME_PROTOCOL_NAME);
}

bool legic_prime_load(LegicPrimeData* data, FlipperFormat* ff, uint32_t version) {
    furi_check(data);
    furi_check(ff);

    bool parsed = false;
    FuriString* tmp = furi_string_alloc();

    do {
        if(version < NFC_UNIFIED_FORMAT_VERSION) break;
        if(!flipper_format_read_string(ff, LEGIC_PRIME_TYPE_KEY, tmp)) break;

        bool type_parsed = false;
        for(size_t i = 0; i < LegicPrimeTypeNum; i++) {
            if(furi_string_equal_str(tmp, legic_prime_features[i].type_name)) {
                data->type = (LegicPrimeType)i;
                data->data_size = legic_prime_features[i].mem_size;
                type_parsed = true;
                break;
            }
        }
        if(!type_parsed) break;

        if(!flipper_format_read_hex(ff, LEGIC_PRIME_DATA_KEY, data->data, data->data_size)) break;

        parsed = true;
    } while(false);

    furi_string_free(tmp);
    return parsed;
}

bool legic_prime_save(const LegicPrimeData* data, FlipperFormat* ff) {
    furi_check(data);
    furi_check(ff);
    furi_check(data->type < LegicPrimeTypeNum);

    bool saved = false;

    do {
        if(!flipper_format_write_comment_cstr(ff, LEGIC_PRIME_PROTOCOL_NAME " specific data"))
            break;
        if(!flipper_format_write_string_cstr(
               ff, LEGIC_PRIME_TYPE_KEY, legic_prime_features[data->type].type_name))
            break;
        if(!flipper_format_write_hex(ff, LEGIC_PRIME_DATA_KEY, data->data, data->data_size)) break;

        saved = true;
    } while(false);

    return saved;
}

bool legic_prime_is_equal(const LegicPrimeData* data, const LegicPrimeData* other) {
    furi_check(data);
    furi_check(other);
    return memcmp(data, other, sizeof(LegicPrimeData)) == 0; //-V1103
}

const char* legic_prime_get_device_name(const LegicPrimeData* data, NfcDeviceNameType name_type) {
    furi_check(data);
    furi_check(data->type < LegicPrimeTypeNum);
    return name_type == NfcDeviceNameTypeFull ? legic_prime_features[data->type].full_name :
                                                legic_prime_features[data->type].type_name;
}

const uint8_t* legic_prime_get_uid(const LegicPrimeData* data, size_t* uid_len) {
    furi_check(data);
    if(uid_len) *uid_len = LEGIC_PRIME_UID_SIZE;
    return data->data;
}

bool legic_prime_set_uid(LegicPrimeData* data, const uint8_t* uid, size_t uid_len) {
    furi_check(data);
    furi_check(uid);
    if(uid_len != LEGIC_PRIME_UID_SIZE) return false;
    memcpy(data->data, uid, LEGIC_PRIME_UID_SIZE);
    // Recompute MCC so the saved tag stays internally consistent.
    data->data[4] = legic_prime_crc8(data->data, LEGIC_PRIME_UID_SIZE);
    return true;
}

LegicPrimeData* legic_prime_get_base_data(const LegicPrimeData* data) {
    UNUSED(data);
    furi_crash("No base data");
}

uint16_t legic_prime_get_mem_size(LegicPrimeType type) {
    furi_check(type < LegicPrimeTypeNum);
    return legic_prime_features[type].mem_size;
}

const LegicPrimeProtocolParams* legic_prime_get_protocol_params(LegicPrimeType type) {
    furi_check(type < LegicPrimeTypeNum);
    return &legic_prime_features[type].params;
}

bool legic_prime_verify_mcc(const LegicPrimeData* data) {
    furi_check(data);
    return legic_prime_crc8(data->data, LEGIC_PRIME_UID_SIZE) == data->data[4];
}

uint16_t legic_prime_get_dcf(const LegicPrimeData* data) {
    furi_check(data);
    return (uint16_t)data->data[5] | ((uint16_t)data->data[6] << 8);
}

LegicPrimeTokenType legic_prime_get_token_type(const LegicPrimeData* data) {
    furi_check(data);
    const uint16_t dcf = legic_prime_get_dcf(data);
    if(dcf == 0xFFFF) return LegicPrimeTokenTypeNewMedia;
    if(dcf > 60000) {
        if(data->data[6] == 0xEC) return LegicPrimeTokenTypeXam;
        const uint8_t level = data->data[5] & 0x7F;
        if(level <= 0x2F) return LegicPrimeTokenTypeIam;
        if(level <= 0x6F) return LegicPrimeTokenTypeSam;
        return LegicPrimeTokenTypeGam;
    }
    if(data->data[7] == 0x9F && data->data[8] == 0xFF) return LegicPrimeTokenTypeImSegmented;
    return LegicPrimeTokenTypeIm;
}

const char* legic_prime_get_token_type_name(LegicPrimeTokenType token_type) {
    static const char* const names[LegicPrimeTokenTypeNum] = {
        [LegicPrimeTokenTypeNewMedia] = "NM",
        [LegicPrimeTokenTypeIm] = "IM",
        [LegicPrimeTokenTypeImSegmented] = "IM-S",
        [LegicPrimeTokenTypeXam] = "XAM",
        [LegicPrimeTokenTypeIam] = "IAM",
        [LegicPrimeTokenTypeSam] = "SAM",
        [LegicPrimeTokenTypeGam] = "GAM",
    };
    furi_check(token_type < LegicPrimeTokenTypeNum);
    return names[token_type];
}

#define LEGIC_PRIME_SEGMENT_AREA_START  (22U)
#define LEGIC_PRIME_SEGMENT_HEADER_SIZE (5U)

bool legic_prime_get_segment(const LegicPrimeData* data, size_t index, LegicPrimeSegment* segment) {
    furi_check(data);
    furi_check(segment);
    if(legic_prime_get_token_type(data) != LegicPrimeTokenTypeImSegmented) return false;

    const uint8_t mcc = data->data[4];
    size_t offset = LEGIC_PRIME_SEGMENT_AREA_START;
    for(size_t i = 0; offset + LEGIC_PRIME_SEGMENT_HEADER_SIZE <= data->data_size; i++) {
        uint8_t crc_input[LEGIC_PRIME_UID_SIZE + 4];
        memcpy(crc_input, data->data, LEGIC_PRIME_UID_SIZE);
        for(size_t j = 0; j < 4; j++) {
            crc_input[LEGIC_PRIME_UID_SIZE + j] = data->data[offset + j] ^ mcc;
        }
        const uint8_t* hdr = &crc_input[LEGIC_PRIME_UID_SIZE];

        segment->offset = offset;
        segment->length = ((uint16_t)(hdr[1] & 0x0F) << 8) | hdr[0];
        segment->valid = hdr[1] & 0x40;
        segment->last = hdr[1] & 0x80;
        segment->wrp = hdr[2];
        segment->wrc = (hdr[3] & 0x70) >> 4;
        segment->rd = hdr[3] & 0x80;
        segment->crc_ok = legic_prime_crc8(crc_input, sizeof(crc_input)) ==
                          (data->data[offset + 4] ^ mcc);

        if(segment->length < LEGIC_PRIME_SEGMENT_HEADER_SIZE ||
           offset + segment->length > data->data_size) {
            return false;
        }
        if(i == index) return true;
        if(segment->last) return false;
        offset += segment->length;
    }
    return false;
}
