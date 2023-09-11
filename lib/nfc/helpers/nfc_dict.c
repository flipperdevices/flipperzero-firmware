#include "nfc_dict.h"

#include <toolbox/args.h>
#include <flipper_format/flipper_format.h>

#include <nfc/helpers/nfc_util.h>

#define MF_CLASSIC_DICT_USER_PATH EXT_PATH("nfc/assets/mf_classic_dict_user.nfc")
#define MF_CLASSIC_DICT_SYSTEM_PATH EXT_PATH("nfc/assets/mf_classic_dict.nfc")
#define MF_CLASSIC_DICT_UNIT_TEST_PATH EXT_PATH("unit_tests/mf_dict.nfc")

#define TAG "NfcDict"

#define NFC_MF_CLASSIC_KEY_LEN (13)

struct NfcDict {
    Stream* stream;
    uint32_t total_keys;
};

typedef struct {
    const char* path;
    FS_OpenMode open_mode;
} NfcDictFile;

static const NfcDictFile nfc_dict_file[NfcDictTypeNum] = {
    [NfcDictTypeUser] =
        {
            .path = MF_CLASSIC_DICT_USER_PATH,
            .open_mode = FSOM_OPEN_ALWAYS,
        },
    [NfcDictTypeSystem] =
        {
            .path = MF_CLASSIC_DICT_SYSTEM_PATH,
            .open_mode = FSOM_OPEN_EXISTING,
        },
    [NfcDictTypeUnitTest] =
        {
            .path = MF_CLASSIC_DICT_UNIT_TEST_PATH,
            .open_mode = FSOM_OPEN_ALWAYS,
        },
};

bool nfc_dict_check_presence(NfcDictType dict_type) {
    furi_assert(dict_type < NfcDictTypeNum);

    Storage* storage = furi_record_open(RECORD_STORAGE);

    const char* path = nfc_dict_file[dict_type].path;
    bool dict_present = storage_common_stat(storage, path, NULL) == FSE_OK;

    furi_record_close(RECORD_STORAGE);

    return dict_present;
}

NfcDict* nfc_dict_alloc(NfcDictType dict_type) {
    furi_assert(dict_type < NfcDictTypeNum);

    NfcDict* dict = malloc(sizeof(NfcDict));
    Storage* storage = furi_record_open(RECORD_STORAGE);
    dict->stream = buffered_file_stream_alloc(storage);
    furi_record_close(RECORD_STORAGE);

    bool dict_loaded = false;
    do {
        if(!buffered_file_stream_open(
               dict->stream,
               nfc_dict_file[dict_type].path,
               FSAM_READ_WRITE,
               nfc_dict_file[dict_type].open_mode)) {
            buffered_file_stream_close(dict->stream);
            break;
        }

        // Check for new line ending
        if(!stream_eof(dict->stream)) {
            if(!stream_seek(dict->stream, -1, StreamOffsetFromEnd)) break;
            uint8_t last_char = 0;
            if(stream_read(dict->stream, &last_char, 1) != 1) break;
            if(last_char != '\n') {
                FURI_LOG_D(TAG, "Adding new line ending");
                if(stream_write_char(dict->stream, '\n') != 1) break;
            }
            if(!stream_rewind(dict->stream)) break;
        }

        // Read total amount of keys
        FuriString* next_line;
        next_line = furi_string_alloc();
        while(true) {
            if(!stream_read_line(dict->stream, next_line)) {
                FURI_LOG_T(TAG, "No keys left in dict");
                break;
            }
            FURI_LOG_T(
                TAG,
                "Read line: %s, len: %zu",
                furi_string_get_cstr(next_line),
                furi_string_size(next_line));
            if(furi_string_get_char(next_line, 0) == '#') continue;
            if(furi_string_size(next_line) != NFC_MF_CLASSIC_KEY_LEN) continue;
            dict->total_keys++;
        }
        furi_string_free(next_line);
        stream_rewind(dict->stream);

        dict_loaded = true;
        FURI_LOG_I(TAG, "Loaded dictionary with %lu keys", dict->total_keys);
    } while(false);

    if(!dict_loaded) {
        buffered_file_stream_close(dict->stream);
        free(dict);
        dict = NULL;
    }

    return dict;
}

void nfc_dict_free(NfcDict* dict) {
    furi_assert(dict);
    furi_assert(dict->stream);

    buffered_file_stream_close(dict->stream);
    stream_free(dict->stream);
    free(dict);
}

static void nfc_dict_int_to_str(const uint8_t* key_int, FuriString* key_str) {
    furi_string_reset(key_str);
    for(size_t i = 0; i < 6; i++) {
        furi_string_cat_printf(key_str, "%02X", key_int[i]);
    }
}

static void nfc_dict_str_to_int(FuriString* key_str, uint64_t* key_int) {
    uint8_t key_byte_tmp;

    *key_int = 0ULL;
    for(uint8_t i = 0; i < 12; i += 2) {
        args_char_to_hex(
            furi_string_get_char(key_str, i), furi_string_get_char(key_str, i + 1), &key_byte_tmp);
        *key_int |= (uint64_t)key_byte_tmp << (8 * (5 - i / 2));
    }
}

uint32_t nfc_dict_get_total_keys(NfcDict* dict) {
    furi_assert(dict);

    return dict->total_keys;
}

bool nfc_dict_rewind(NfcDict* dict) {
    furi_assert(dict);
    furi_assert(dict->stream);

    return stream_rewind(dict->stream);
}

static bool nfc_dict_get_next_key_str(NfcDict* dict, FuriString* key) {
    furi_assert(dict);
    furi_assert(dict->stream);

    bool key_read = false;
    furi_string_reset(key);
    while(!key_read) {
        if(!stream_read_line(dict->stream, key)) break;
        if(furi_string_get_char(key, 0) == '#') continue;
        if(furi_string_size(key) != NFC_MF_CLASSIC_KEY_LEN) continue;
        furi_string_left(key, 12);
        key_read = true;
    }

    return key_read;
}

bool nfc_dict_get_next_key(NfcDict* dict, MfClassicKey* key) {
    furi_assert(dict);
    furi_assert(dict->stream);

    FuriString* temp_key;
    uint64_t key_int = 0;
    temp_key = furi_string_alloc();
    bool key_read = nfc_dict_get_next_key_str(dict, temp_key);
    if(key_read) {
        nfc_dict_str_to_int(temp_key, &key_int);
        nfc_util_num2bytes(key_int, 6, key->data);
    }
    furi_string_free(temp_key);
    return key_read;
}

static bool nfc_dict_is_key_present_str(NfcDict* dict, FuriString* key) {
    furi_assert(dict);
    furi_assert(dict->stream);

    FuriString* next_line;
    next_line = furi_string_alloc();

    bool key_found = false;
    stream_rewind(dict->stream);
    while(!key_found) { //-V654
        if(!stream_read_line(dict->stream, next_line)) break;
        if(furi_string_get_char(next_line, 0) == '#') continue;
        if(furi_string_size(next_line) != NFC_MF_CLASSIC_KEY_LEN) continue;
        furi_string_left(next_line, 12);
        if(!furi_string_equal(key, next_line)) continue;
        key_found = true;
    }

    furi_string_free(next_line);
    return key_found;
}

bool nfc_dict_is_key_present(NfcDict* dict, const MfClassicKey* key) {
    FuriString* temp_key;

    temp_key = furi_string_alloc();
    nfc_dict_int_to_str(key->data, temp_key);
    bool key_found = nfc_dict_is_key_present_str(dict, temp_key);
    furi_string_free(temp_key);
    return key_found;
}

static bool nfc_dict_add_key_str(NfcDict* dict, FuriString* key) {
    furi_assert(dict);
    furi_assert(dict->stream);

    furi_string_cat_printf(key, "\n");

    bool key_added = false;
    do {
        if(!stream_seek(dict->stream, 0, StreamOffsetFromEnd)) break;
        if(!stream_insert_string(dict->stream, key)) break;
        dict->total_keys++;
        key_added = true;
    } while(false);

    furi_string_left(key, 12);
    return key_added;
}

bool nfc_dict_add_key(NfcDict* dict, const MfClassicKey* key) {
    furi_assert(dict);
    furi_assert(dict->stream);

    FuriString* temp_key;
    temp_key = furi_string_alloc();
    nfc_dict_int_to_str(key->data, temp_key);
    bool key_added = nfc_dict_add_key_str(dict, temp_key);

    furi_string_free(temp_key);
    return key_added;
}

bool nfc_dict_delete_key(NfcDict* dict, const MfClassicKey* key) {
    furi_assert(dict);
    furi_assert(dict->stream);

    bool key_removed = false;
    MfClassicKey temp_key = {};

    nfc_dict_rewind(dict);
    while(!key_removed) {
        if(!nfc_dict_get_next_key(dict, &temp_key)) break;
        if(memcmp(temp_key.data, key->data, sizeof(MfClassicKey)) == 0) {
            stream_seek(dict->stream, -NFC_MF_CLASSIC_KEY_LEN, StreamOffsetFromCurrent);
            if(!stream_delete(dict->stream, NFC_MF_CLASSIC_KEY_LEN)) break;
            dict->total_keys--;
            key_removed = true;
        }
    }
    nfc_dict_rewind(dict);

    return key_removed;
}
