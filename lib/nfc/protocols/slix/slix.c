#include "slix_i.h"
#include "slix_device_defs.h"

#include <furi.h>
#include <nfc/nfc_common.h>

#define SLIX_PROTOCOL_NAME "SLIX"
#define SLIX_DEVICE_NAME "SLIX"

#define SLIX_TYPE_SLIX_SLIX2 (0x01U)
#define SLIX_TYPE_SLIX_S (0x02U)
#define SLIX_TYPE_SLIX_L (0x03U)

#define SLIX_TYPE_INDICATOR_SLIX (0x02U)
#define SLIX_TYPE_INDICATOR_SLIX2 (0x01U)

#define SLIX_PASSWORD_READ_KEY "Password Read"
#define SLIX_PASSWORD_WRITE_KEY "Password Write"
#define SLIX_PASSWORD_PRIVACY_KEY "Password Privacy"
#define SLIX_PASSWORD_DESTROY_KEY "Password Destroy"
#define SLIX_PASSWORD_EAS_KEY "Password EAS"
#define SLIX_SIGNATURE_KEY "Signature"
#define SLIX_PRIVACY_MODE_KEY "Privacy Mode"
#define SLIX_PROTECTION_POINTER_KEY "Protection Pointer"
#define SLIX_PROTECTION_CONDITION_KEY "Protection Condition"
#define SLIX_LOCK_BITS_KEY "SLIX Lock Bits"

typedef struct {
    uint8_t iso15693_3[2];
    uint8_t icode_type;
    union {
        struct {
            uint8_t unused_1 : 3;
            uint8_t type_indicator : 2;
            uint8_t unused_2 : 3;
        };
        uint8_t serial_num[5];
    };
} SlixUidLayout;

const NfcDeviceBase nfc_device_slix = {
    .protocol_name = SLIX_PROTOCOL_NAME,
    .alloc = (NfcDeviceAlloc)slix_alloc,
    .free = (NfcDeviceFree)slix_free,
    .reset = (NfcDeviceReset)slix_reset,
    .copy = (NfcDeviceCopy)slix_copy,
    .verify = (NfcDeviceVerify)slix_verify,
    .load = (NfcDeviceLoad)slix_load,
    .save = (NfcDeviceSave)slix_save,
    .is_equal = (NfcDeviceEqual)slix_is_equal,
    .get_name = (NfcDeviceGetName)slix_get_device_name,
    .get_uid = (NfcDeviceGetUid)slix_get_uid,
    .set_uid = (NfcDeviceSetUid)slix_set_uid,
    .get_base_data = (NfcDeviceGetBaseData)slix_get_base_data,
};

static const char* slix_nfc_device_name[] = {
    [SlixTypeSlix] = SLIX_DEVICE_NAME,
    [SlixTypeSlixS] = SLIX_DEVICE_NAME "-S",
    [SlixTypeSlixL] = SLIX_DEVICE_NAME "-L",
    [SlixTypeSlix2] = SLIX_DEVICE_NAME "2",
};

static const SlixTypeFeatures slix_type_features[] = {
    [SlixTypeSlix] = SLIX_TYPE_FEATURES_SLIX,
    [SlixTypeSlixS] = SLIX_TYPE_FEATURES_SLIX_S,
    [SlixTypeSlixL] = SLIX_TYPE_FEATURES_SLIX_L,
    [SlixTypeSlix2] = SLIX_TYPE_FEATURES_SLIX2,
};

typedef struct {
    const char* key;
    uint32_t feature_flag;
} SlixPasswordConfig;

static const SlixPasswordConfig slix_password_configs[SlixPasswordTypeCount] = {
    {SLIX_PASSWORD_READ_KEY, SLIX_TYPE_FEATURE_READ},
    {SLIX_PASSWORD_WRITE_KEY, SLIX_TYPE_FEATURE_WRITE},
    {SLIX_PASSWORD_PRIVACY_KEY, SLIX_TYPE_FEATURE_PRIVACY},
    {SLIX_PASSWORD_DESTROY_KEY, SLIX_TYPE_FEATURE_DESTROY},
    {SLIX_PASSWORD_EAS_KEY, SLIX_TYPE_FEATURE_EAS},
};

SlixData* slix_alloc() {
    SlixData* data = malloc(sizeof(SlixData));

    data->iso15693_3_data = iso15693_3_alloc();

    return data;
}

void slix_free(SlixData* data) {
    furi_assert(data);

    iso15693_3_free(data->iso15693_3_data);

    free(data);
}

void slix_reset(SlixData* data) {
    furi_assert(data);

    iso15693_3_reset(data->iso15693_3_data);

    memset(&data->system_info, 0, sizeof(SlixSystemInfo));
    memset(data->passwords, 0, sizeof(SlixPassword) * SlixPasswordTypeCount);
    memset(&data->signature, 0, sizeof(SlixSignature));

    data->is_privacy_mode = false;
}

void slix_copy(SlixData* data, const SlixData* other) {
    furi_assert(data);
    furi_assert(other);

    iso15693_3_copy(data->iso15693_3_data, other->iso15693_3_data);
    memcpy(data->passwords, other->passwords, sizeof(SlixPassword) * SlixPasswordTypeCount);

    data->system_info = other->system_info;
    data->signature = other->signature;
    data->is_privacy_mode = other->is_privacy_mode;
}

bool slix_verify(SlixData* data, const FuriString* device_type) {
    UNUSED(data);
    UNUSED(device_type);
    // No backward compatibility, unified format only
    return false;
}

static bool slix_load_passwords(SlixPassword* passwords, SlixType slix_type, FlipperFormat* ff) {
    bool ret = true;

    for(uint32_t i = 0; i < SlixTypeCount; ++i) {
        SlixPassword* password = &passwords[i];
        const SlixPasswordConfig* password_config = &slix_password_configs[i];

        if(!slix_type_has_features(slix_type, password_config->feature_flag)) continue;
        if(!flipper_format_key_exist(ff, password_config->key)) continue;
        if(!flipper_format_read_hex(
               ff, password_config->key, (uint8_t*)&password->value, sizeof(SlixPasswordValue))) {
            ret = false;
            break;
        }
        password->is_set = true;
    }

    return ret;
}

bool slix_load(SlixData* data, FlipperFormat* ff, uint32_t version) {
    furi_assert(data);

    bool loaded = false;

    do {
        if(!iso15693_3_load(data->iso15693_3_data, ff, version)) break;

        const SlixType slix_type = slix_get_type(data);
        if(slix_type >= SlixTypeCount) break;

        if(!slix_load_passwords(data->passwords, slix_type, ff)) break;

        if(slix_type_has_features(slix_type, SLIX_TYPE_FEATURE_SIGNATURE)) {
            SlixSignature* signature = &data->signature;
            if(flipper_format_key_exist(ff, SLIX_SIGNATURE_KEY)) {
                if(!flipper_format_read_hex(
                       ff, SLIX_SIGNATURE_KEY, signature->data, SLIX_SIGNATURE_SIZE))
                    break;
                signature->is_present = true;
            }
        }

        if(slix_type_has_features(slix_type, SLIX_TYPE_FEATURE_PRIVACY)) {
            if(flipper_format_key_exist(ff, SLIX_PRIVACY_MODE_KEY)) {
                if(!flipper_format_read_bool(ff, SLIX_PRIVACY_MODE_KEY, &data->is_privacy_mode, 1))
                    break;
            }
        }

        if(slix_type_has_features(slix_type, SLIX_TYPE_FEATURE_SIGNATURE)) {
            SlixProtection* protection = &data->system_info.protection;
            if(flipper_format_key_exist(ff, SLIX_PROTECTION_POINTER_KEY) &&
               flipper_format_key_exist(ff, SLIX_PROTECTION_CONDITION_KEY)) {
                if(!flipper_format_read_hex(
                       ff, SLIX_PROTECTION_POINTER_KEY, &protection->pointer, 1))
                    break;
                if(!flipper_format_read_hex(
                       ff, SLIX_PROTECTION_CONDITION_KEY, &protection->condition, 1))
                    break;
                protection->is_present = true;
            }
        }

        if(slix_type_has_features(slix_type, SLIX_TYPE_FEATURE_LOCK_BITS)) {
            SlixLockBits* lock_bits = &data->system_info.lock_bits;
            if(flipper_format_key_exist(ff, SLIX_LOCK_BITS_KEY)) {
                if(!flipper_format_read_hex(ff, SLIX_LOCK_BITS_KEY, &lock_bits->data, 1)) break;
                lock_bits->is_present = true;
            }
        }

        loaded = true;
    } while(false);

    return loaded;
}

static bool
    slix_save_passwords(const SlixPassword* passwords, SlixType slix_type, FlipperFormat* ff) {
    bool ret = true;

    for(uint32_t i = 0; i < SlixTypeCount; ++i) {
        const SlixPassword* password = &passwords[i];
        const SlixPasswordConfig* password_config = &slix_password_configs[i];

        if(!slix_type_has_features(slix_type, password_config->feature_flag)) continue;
        if(!password->is_set) continue;
        if(!flipper_format_write_hex(
               ff, password_config->key, (uint8_t*)&password->value, sizeof(SlixPasswordValue))) {
            ret = false;
            break;
        }
    }

    return ret;
}

bool slix_save(const SlixData* data, FlipperFormat* ff) {
    furi_assert(data);

    bool saved = false;

    do {
        const SlixType slix_type = slix_get_type(data);
        if(slix_type >= SlixTypeCount) break;

        if(!iso15693_3_save(data->iso15693_3_data, ff)) break;
        if(!flipper_format_write_comment_cstr(ff, SLIX_PROTOCOL_NAME " specific data")) break;

        if(!flipper_format_write_comment_cstr(
               ff, "Passwords are optional. If a password is omitted, any password is accepted"))
            break;

        if(!slix_save_passwords(data->passwords, slix_type, ff)) break;

        if(slix_type_has_features(slix_type, SLIX_TYPE_FEATURE_SIGNATURE)) {
            const SlixSignature* signature = &data->signature;
            if(signature->is_present) {
                if(!flipper_format_write_comment_cstr(
                       ff,
                       "This is the card's secp128r1 elliptic curve signature. It can not be calculated without knowing NXP's private key."))
                    break;
                if(!flipper_format_write_hex(
                       ff, SLIX_SIGNATURE_KEY, signature->data, SLIX_SIGNATURE_SIZE))
                    break;
            }
        }

        if(slix_type_has_features(slix_type, SLIX_TYPE_FEATURE_PRIVACY)) {
            if(!flipper_format_write_bool(ff, SLIX_PRIVACY_MODE_KEY, &data->is_privacy_mode, 1))
                break;
        }

        if(slix_type_has_features(slix_type, SLIX_TYPE_FEATURE_PROTECTION)) {
            const SlixProtection* protection = &data->system_info.protection;
            if(protection->is_present) {
                if(!flipper_format_write_comment_cstr(ff, "Protection pointer configuration"))
                    break;
                if(!flipper_format_write_hex(
                       ff, SLIX_PROTECTION_POINTER_KEY, &protection->pointer, sizeof(uint8_t)))
                    break;
                if(!flipper_format_write_hex(
                       ff, SLIX_PROTECTION_CONDITION_KEY, &protection->condition, sizeof(uint8_t)))
                    break;
            }
        }

        if(slix_type_has_features(slix_type, SLIX_TYPE_FEATURE_LOCK_BITS)) {
            const SlixLockBits* lock_bits = &data->system_info.lock_bits;
            if(lock_bits->is_present) {
                if(!flipper_format_write_comment_cstr(
                       ff, "SLIX Lock Bits: AFI = 0x01, EAS = 0x02, DSFID = 0x04, PPL = 0x08"))
                    break;
                if(!flipper_format_write_hex(ff, SLIX_LOCK_BITS_KEY, &lock_bits->data, 1)) break;
            }
        }

        saved = true;
    } while(false);

    return saved;
}

bool slix_is_equal(const SlixData* data, const SlixData* other) {
    return iso15693_3_is_equal(data->iso15693_3_data, other->iso15693_3_data) &&
           memcmp(&data->system_info, &other->system_info, sizeof(SlixSystemInfo)) == 0 &&
           memcmp(
               data->passwords, other->passwords, sizeof(SlixPassword) * SlixPasswordTypeCount) ==
               0 &&
           memcmp(&data->signature, &other->signature, sizeof(SlixSignature)) == 0 &&
           data->is_privacy_mode == other->is_privacy_mode;
}

const char* slix_get_device_name(const SlixData* data, NfcDeviceNameType name_type) {
    UNUSED(name_type);

    const SlixType slix_type = slix_get_type(data);
    furi_assert(slix_type < SlixTypeCount);

    return slix_nfc_device_name[slix_type];
}

const uint8_t* slix_get_uid(const SlixData* data, size_t* uid_len) {
    return iso15693_3_get_uid(data->iso15693_3_data, uid_len);
}

bool slix_set_uid(SlixData* data, const uint8_t* uid, size_t uid_len) {
    furi_assert(data);

    return iso15693_3_set_uid(data->iso15693_3_data, uid, uid_len);
}

const Iso15693_3Data* slix_get_base_data(const SlixData* data) {
    furi_assert(data);

    return data->iso15693_3_data;
}

SlixType slix_get_type(const SlixData* data) {
    SlixType type = SlixTypeCount;

    do {
        if(iso15693_3_get_manufacturer_id(data->iso15693_3_data) != SLIX_NXP_MANUFACTURER_CODE)
            break;

        const SlixUidLayout* uid = (const SlixUidLayout*)data->iso15693_3_data->uid;

        if(uid->icode_type == SLIX_TYPE_SLIX_SLIX2) {
            if(uid->type_indicator == SLIX_TYPE_INDICATOR_SLIX) {
                type = SlixTypeSlix;
            } else if(uid->type_indicator == SLIX_TYPE_INDICATOR_SLIX2) {
                type = SlixTypeSlix2;
            }
        } else if(uid->icode_type == SLIX_TYPE_SLIX_S) {
            type = SlixTypeSlixS;
        } else if(uid->icode_type == SLIX_TYPE_SLIX_L) {
            type = SlixTypeSlixL;
        }

    } while(false);

    return type;
}

bool slix_is_password_set(const SlixData* data, SlixPasswordType password_type) {
    furi_assert(data);
    furi_assert(password_type < SlixPasswordTypeCount);

    return data->passwords[password_type].is_set;
}

SlixPasswordValue slix_get_password(const SlixData* data, SlixPasswordType password_type) {
    furi_assert(data);
    furi_assert(password_type < SlixPasswordTypeCount);
    furi_assert(data->passwords[password_type].is_set);

    return data->passwords[password_type].value;
}

bool slix_is_privacy_mode(const SlixData* data) {
    furi_assert(data);

    return data->is_privacy_mode;
}

void slix_set_password(SlixData* data, SlixPasswordType password_type, SlixPasswordValue password) {
    furi_assert(data);
    furi_assert(password_type < SlixPasswordTypeCount);

    data->passwords[password_type].is_set = true;
    data->passwords[password_type].value = password;
}

void slix_set_privacy_mode(SlixData* data, bool set) {
    furi_assert(data);

    data->is_privacy_mode = set;
}

bool slix_type_has_features(SlixType slix_type, SlixTypeFeatures features) {
    furi_assert(slix_type < SlixTypeCount);
    return (slix_type_features[slix_type] & features) == features;
}

bool slix_type_supports_password(SlixType slix_type, SlixPasswordType password_type) {
    furi_assert(slix_type < SlixTypeCount);
    furi_assert(password_type < SlixPasswordTypeCount);
    return slix_type_features[slix_type] & slix_password_configs[password_type].feature_flag;
}