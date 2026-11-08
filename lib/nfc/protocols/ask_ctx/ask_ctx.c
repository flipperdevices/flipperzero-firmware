#include "ask_ctx_i.h"

#include <furi.h>
#include <nfc/nfc_common.h>

#define ASK_CTX_PROTOCOL_NAME "ASK CTx"
#define ASK_CTX_PRODUCT_KEY   "Product Code"
#define ASK_CTX_FAB_KEY       "Fab Code"
#define ASK_CTX_TYPE_KEY      "ASK CTx Type"
#define ASK_CTX_BLOCK_KEY     "Block %u"

const AskCtxFeatures ask_ctx_features[AskCtxTypeNum] = {
    [AskCtxTypeUnknown] =
        {
            .blocks_total = 0,
            .read_command = ASK_CTX_CMD_READ_CTX512B,
            .full_name = "ASK CTx",
            .type_name = "Unknown",
        },
    [AskCtxTypeCts256B] =
        {
            .blocks_total = ASK_CTX_CTS256B_BLOCK_COUNT,
            .read_command = ASK_CTX_CMD_READ_CTX256B,
            .full_name = "ASK CTS256B",
            .type_name = "CTS256B",
        },
    [AskCtxTypeCts512B] =
        {
            .blocks_total = ASK_CTX_CTX512B_BLOCK_COUNT,
            .read_command = ASK_CTX_CMD_READ_CTX512B,
            .full_name = "ASK CTS512B",
            .type_name = "CTS512B",
        },
    [AskCtxTypeCtm512B] =
        {
            .blocks_total = ASK_CTX_CTX512B_BLOCK_COUNT,
            .read_command = ASK_CTX_CMD_READ_CTX512B,
            .full_name = "ASK CTM512B",
            .type_name = "CTM512B",
        },
};

static AskCtxData* ask_ctx_alloc(void) {
    return malloc(sizeof(AskCtxData));
}

static void ask_ctx_free(AskCtxData* data) {
    furi_check(data);
    free(data);
}

static void ask_ctx_reset(AskCtxData* data) {
    furi_check(data);
    memset(data, 0, sizeof(AskCtxData));
}

static void ask_ctx_copy(AskCtxData* data, const AskCtxData* other) {
    furi_check(data);
    furi_check(other);
    *data = *other;
}

static bool ask_ctx_verify(AskCtxData* data, const FuriString* device_type) {
    UNUSED(data);
    furi_check(device_type);
    return furi_string_equal_str(device_type, ASK_CTX_PROTOCOL_NAME);
}

static bool ask_ctx_load(AskCtxData* data, FlipperFormat* ff, uint32_t version) {
    furi_check(data);
    furi_check(ff);

    bool parsed = false;
    FuriString* key = furi_string_alloc();

    do {
        if(version < NFC_UNIFIED_FORMAT_VERSION) break;
        if(!flipper_format_read_string(ff, ASK_CTX_TYPE_KEY, key)) break;
        bool type_parsed = false;
        for(size_t i = 0; i < AskCtxTypeNum; i++) {
            if(furi_string_equal_str(key, ask_ctx_features[i].type_name)) {
                data->type = i;
                type_parsed = true;
                break;
            }
        }
        if(!type_parsed) break;
        if(!flipper_format_read_hex(ff, ASK_CTX_PRODUCT_KEY, &data->product_code, 1)) break;
        if(!flipper_format_read_hex(ff, ASK_CTX_FAB_KEY, &data->fab_code, 1)) break;
        data->block_count = ask_ctx_features[data->type].blocks_total;

        bool blocks_parsed = true;
        for(uint8_t block = 0; block < data->block_count; block++) {
            furi_string_printf(key, ASK_CTX_BLOCK_KEY, block);
            if(!flipper_format_read_hex(
                   ff, furi_string_get_cstr(key), data->blocks[block], ASK_CTX_BLOCK_SIZE)) {
                blocks_parsed = false;
                break;
            }
        }
        if(!blocks_parsed) break;

        parsed = true;
    } while(false);

    furi_string_free(key);
    return parsed;
}

static bool ask_ctx_save(const AskCtxData* data, FlipperFormat* ff) {
    furi_check(data);
    furi_check(ff);

    bool saved = false;
    FuriString* key = furi_string_alloc();

    do {
        if(!flipper_format_write_comment_cstr(ff, ASK_CTX_PROTOCOL_NAME " specific data")) break;
        if(!flipper_format_write_string_cstr(
               ff, ASK_CTX_TYPE_KEY, ask_ctx_features[data->type].type_name))
            break;
        if(!flipper_format_write_hex(ff, ASK_CTX_PRODUCT_KEY, &data->product_code, 1)) break;
        if(!flipper_format_write_hex(ff, ASK_CTX_FAB_KEY, &data->fab_code, 1)) break;

        bool blocks_saved = true;
        for(uint8_t block = 0; block < ask_ctx_features[data->type].blocks_total; block++) {
            furi_string_printf(key, ASK_CTX_BLOCK_KEY, block);
            if(!flipper_format_write_hex(
                   ff, furi_string_get_cstr(key), data->blocks[block], ASK_CTX_BLOCK_SIZE)) {
                blocks_saved = false;
                break;
            }
        }
        if(!blocks_saved) break;

        saved = true;
    } while(false);

    furi_string_free(key);
    return saved;
}

static bool ask_ctx_is_equal(const AskCtxData* data, const AskCtxData* other) {
    furi_check(data);
    furi_check(other);
    return memcmp(data, other, sizeof(AskCtxData)) == 0;
}

static const char* ask_ctx_get_device_name(const AskCtxData* data, NfcDeviceNameType name_type) {
    furi_check(data);

    if(name_type != NfcDeviceNameTypeFull) return "ASK CTx";
    furi_check(data->type < AskCtxTypeNum);
    return ask_ctx_features[data->type].full_name;
}

static const uint8_t* ask_ctx_get_uid(const AskCtxData* data, size_t* uid_len) {
    furi_check(data);
    if(uid_len) *uid_len = ASK_CTX_UID_SIZE;
    return data->uid;
}

static bool ask_ctx_set_uid(AskCtxData* data, const uint8_t* uid, size_t uid_len) {
    furi_check(data);
    furi_check(uid);

    if(uid_len != ASK_CTX_UID_SIZE) return false;
    memcpy(data->uid, uid, uid_len);
    return true;
}

static AskCtxData* ask_ctx_get_base_data(const AskCtxData* data) {
    UNUSED(data);
    furi_crash("No base data");
}

const NfcDeviceBase nfc_device_ask_ctx = {
    .protocol_name = ASK_CTX_PROTOCOL_NAME,
    .alloc = (NfcDeviceAlloc)ask_ctx_alloc,
    .free = (NfcDeviceFree)ask_ctx_free,
    .reset = (NfcDeviceReset)ask_ctx_reset,
    .copy = (NfcDeviceCopy)ask_ctx_copy,
    .verify = (NfcDeviceVerify)ask_ctx_verify,
    .load = (NfcDeviceLoad)ask_ctx_load,
    .save = (NfcDeviceSave)ask_ctx_save,
    .is_equal = (NfcDeviceEqual)ask_ctx_is_equal,
    .get_name = (NfcDeviceGetName)ask_ctx_get_device_name,
    .get_uid = (NfcDeviceGetUid)ask_ctx_get_uid,
    .set_uid = (NfcDeviceSetUid)ask_ctx_set_uid,
    .get_base_data = (NfcDeviceGetBaseData)ask_ctx_get_base_data,
};
