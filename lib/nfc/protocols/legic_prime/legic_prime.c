#include "legic_prime_i.h"
#include "legic_prime_crc.h"

#include <flipper_format/flipper_format.h>
#include <furi.h>

#include <nfc/nfc_common.h>
#include <lib/bit_lib/bit_lib.h>

#define LEGIC_PRIME_PROTOCOL_NAME "LEGIC Prime"
#define LEGIC_PRIME_TYPE_KEY      "Legic Prime Type"
#define LEGIC_PRIME_DATA_KEY      "Data"

#define LEGIC_PRIME_MCC_OFFSET          (4U)
// Bytes from 22 on (past the fixed header) are stored XORed with the MCC.
// On IM-S cards the segments start there.
#define LEGIC_PRIME_SEGMENT_AREA_START  (22U)
#define LEGIC_PRIME_SEGMENT_HEADER_SIZE (5U)

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

// The type and the memory size always agree: legic_prime_reset() leaves a
// MIM22, and only legic_prime_set_type() and legic_prime_load() set either.
static void legic_prime_check(const LegicPrimeData* data) {
    furi_check(data);
    furi_check(data->type < LegicPrimeTypeNum);
    furi_check(data->data_size == legic_prime_features[data->type].mem_size);
}

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
    legic_prime_set_type(data, LegicPrimeTypeMim22);
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
        for(LegicPrimeType i = 0; i < LegicPrimeTypeNum; i++) {
            if(furi_string_equal_str(tmp, legic_prime_features[i].type_name)) {
                legic_prime_set_type(data, i);
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
    legic_prime_check(data);
    furi_check(ff);

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
    legic_prime_check(data);
    legic_prime_check(other);
    return data->type == other->type && data->data_size == other->data_size &&
           memcmp(data->data, other->data, data->data_size) == 0;
}

const char* legic_prime_get_device_name(const LegicPrimeData* data, NfcDeviceNameType name_type) {
    legic_prime_check(data);
    return name_type == NfcDeviceNameTypeFull ? legic_prime_features[data->type].full_name :
                                                legic_prime_features[data->type].type_name;
}

const uint8_t* legic_prime_get_uid(const LegicPrimeData* data, size_t* uid_len) {
    legic_prime_check(data);
    if(uid_len) *uid_len = LEGIC_PRIME_UID_SIZE;
    return data->data;
}

// Decodes the segment header at `offset` (stored XORed with the MCC).
static void legic_prime_read_segment_header(
    const LegicPrimeData* data,
    size_t offset,
    uint8_t header[LEGIC_PRIME_SEGMENT_HEADER_SIZE]) {
    for(size_t i = 0; i < LEGIC_PRIME_SEGMENT_HEADER_SIZE; i++) {
        header[i] = data->data[offset + i] ^ data->data[LEGIC_PRIME_MCC_OFFSET];
    }
}

// The header's last byte: CRC-8 over the UID and the other header bytes.
static uint8_t legic_prime_segment_crc(const uint8_t* uid, const uint8_t* header) {
    uint8_t crc_input[LEGIC_PRIME_UID_SIZE + LEGIC_PRIME_SEGMENT_HEADER_SIZE - 1];
    memcpy(crc_input, uid, LEGIC_PRIME_UID_SIZE);
    memcpy(&crc_input[LEGIC_PRIME_UID_SIZE], header, LEGIC_PRIME_SEGMENT_HEADER_SIZE - 1);
    return legic_prime_crc8(crc_input, sizeof(crc_input));
}

bool legic_prime_set_uid(LegicPrimeData* data, const uint8_t* uid, size_t uid_len) {
    legic_prime_check(data);
    furi_check(uid);
    if(uid_len != LEGIC_PRIME_UID_SIZE) return false;

    // As Proxmark3 cmdhflegic.c legic_clone_update_segment_crcs(), but each
    // stored CRC moves by the difference between the old and the new UID's
    // CRC, so a header whose CRC didn't match keeps failing.  The walk reads
    // the old encoding, so this goes first.
    LegicPrimeSegment segment = {};
    while(legic_prime_get_next_segment(data, &segment)) {
        uint8_t header[LEGIC_PRIME_SEGMENT_HEADER_SIZE];
        legic_prime_read_segment_header(data, segment.offset, header);
        data->data[segment.offset + LEGIC_PRIME_SEGMENT_HEADER_SIZE - 1] ^=
            legic_prime_segment_crc(data->data, header) ^ legic_prime_segment_crc(uid, header);
    }

    const uint8_t old_mcc = data->data[LEGIC_PRIME_MCC_OFFSET];

    memcpy(data->data, uid, LEGIC_PRIME_UID_SIZE);
    data->data[LEGIC_PRIME_MCC_OFFSET] = legic_prime_crc8(uid, LEGIC_PRIME_UID_SIZE);
    // nfc_device_load() calls this on a reset MIM22, before the memory is
    // loaded: nothing to re-encode then.
    const uint8_t mcc_change = old_mcc ^ data->data[LEGIC_PRIME_MCC_OFFSET];
    for(size_t i = LEGIC_PRIME_SEGMENT_AREA_START; i < data->data_size; i++) {
        data->data[i] ^= mcc_change;
    }
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

void legic_prime_set_type(LegicPrimeData* data, LegicPrimeType type) {
    furi_check(data);
    data->data_size = legic_prime_get_mem_size(type);
    data->type = type;
}

const LegicPrimeProtocolParams* legic_prime_get_protocol_params(LegicPrimeType type) {
    furi_check(type < LegicPrimeTypeNum);
    return &legic_prime_features[type].params;
}

bool legic_prime_verify_mcc(const LegicPrimeData* data) {
    legic_prime_check(data);
    return legic_prime_crc8(data->data, LEGIC_PRIME_UID_SIZE) ==
           data->data[LEGIC_PRIME_MCC_OFFSET];
}

uint16_t legic_prime_get_dcf(const LegicPrimeData* data) {
    legic_prime_check(data);
    return bit_lib_bytes_to_num_le(&data->data[LEGIC_PRIME_DCF_OFFSET], LEGIC_PRIME_DCF_SIZE);
}

LegicPrimeTokenType legic_prime_get_token_type(const LegicPrimeData* data) {
    legic_prime_check(data);
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

bool legic_prime_get_next_segment(const LegicPrimeData* data, LegicPrimeSegment* segment) {
    legic_prime_check(data);
    furi_check(segment);
    if(legic_prime_get_token_type(data) != LegicPrimeTokenTypeImSegmented) return false;

    size_t offset = LEGIC_PRIME_SEGMENT_AREA_START;
    if(segment->offset) {
        if(segment->last) return false;
        offset = segment->offset + segment->length;
    }
    if(offset + LEGIC_PRIME_SEGMENT_HEADER_SIZE > data->data_size) return false;

    uint8_t header[LEGIC_PRIME_SEGMENT_HEADER_SIZE];
    legic_prime_read_segment_header(data, offset, header);
    segment->offset = offset;
    segment->length = ((uint16_t)(header[1] & 0x0F) << 8) | header[0];
    segment->valid = header[1] & 0x40;
    segment->last = header[1] & 0x80;
    segment->wrp = header[2];
    segment->wrc = (header[3] & 0x70) >> 4;
    segment->rd = header[3] & 0x80;
    segment->crc_ok = legic_prime_segment_crc(data->data, header) == header[4];
    if(segment->length < LEGIC_PRIME_SEGMENT_HEADER_SIZE ||
       offset + segment->length > data->data_size) {
        return false;
    }
    segment->payload_offset = offset + LEGIC_PRIME_SEGMENT_HEADER_SIZE;
    segment->payload_length = segment->length - LEGIC_PRIME_SEGMENT_HEADER_SIZE;
    return true;
}

bool legic_prime_is_writable(uint16_t data_size, uint16_t offset, size_t size) {
    return size > 0 && offset >= LEGIC_PRIME_WRITABLE_FROM && offset + size <= data_size;
}

bool legic_prime_is_dcf_write(uint16_t offset, size_t size) {
    return size > 0 && offset < LEGIC_PRIME_DCF_OFFSET + LEGIC_PRIME_DCF_SIZE &&
           offset + size > LEGIC_PRIME_DCF_OFFSET;
}
