#include <furi_hal.h>
#include <inttypes.h>
#include <toolbox/keys_dict.h>
#include <bit_lib/bit_lib.h>
#include <toolbox/stream/buffered_file_stream.h>
#include <nfc/protocols/mf_classic/mf_classic.h>
#include "mfkey.h"
#include "crypto1.h"
#include "plugin_interface.h"
#include <flipper_application/flipper_application.h>

// TODO: Remove defines that are not needed
#define KEYS_DICT_SYSTEM_PATH EXT_PATH("nfc/assets/mf_classic_dict.nfc")
#define KEYS_DICT_USER_PATH EXT_PATH("nfc/assets/mf_classic_dict_user.nfc")
#define MF_CLASSIC_NONCE_PATH EXT_PATH("nfc/.mfkey32.log")
#define MF_CLASSIC_NESTED_NONCE_PATH EXT_PATH("nfc/.nested")
#define TAG "MFKey"
#define MAX_NAME_LEN 32
#define MAX_PATH_LEN 64

#define LF_POLY_ODD (0x29CE5C)
#define LF_POLY_EVEN (0x870804)
#define CONST_M1_1 (LF_POLY_EVEN << 1 | 1)
#define CONST_M2_1 (LF_POLY_ODD << 1)
#define CONST_M1_2 (LF_POLY_ODD)
#define CONST_M2_2 (LF_POLY_EVEN << 1 | 1)
#define BIT(x, n) ((x) >> (n) & 1)
#define BEBIT(x, n) BIT(x, (n) ^ 24)
#define SWAPENDIAN(x) \
    ((x) = ((x) >> 8 & 0xff00ff) | ((x) & 0xff00ff) << 8, (x) = (x) >> 16 | (x) << 16)

bool key_already_found_for_nonce_in_dict(KeysDict* dict, MfClassicNonce* nonce) {
    bool found = false;
    uint8_t key_bytes[sizeof(MfClassicKey)];
    keys_dict_rewind(dict);
    while(keys_dict_get_next_key(dict, key_bytes, sizeof(MfClassicKey))) {
        uint64_t k = bit_lib_bytes_to_num_be(key_bytes, sizeof(MfClassicKey));
        struct Crypto1State temp = {0, 0};
        for(int i = 0; i < 24; i++) {
            (&temp)->odd |= (BIT(k, 2 * i + 1) << (i ^ 3));
            (&temp)->even |= (BIT(k, 2 * i) << (i ^ 3));
        }
        if(nonce->attack == mfkey32) {
            crypt_word_noret(&temp, nonce->uid_xor_nt1, 0);
            crypt_word_noret(&temp, nonce->nr1_enc, 1);
            if(nonce->ar1_enc == (crypt_word(&temp) ^ nonce->p64b)) {
                found = true;
                break;
            }
        } else if(nonce->attack == static_nested) {
            uint32_t expected_ks1 = crypt_word_ret(&temp, nonce->uid_xor_nt0, 0);
            if(nonce->ks1_1_enc == expected_ks1) {
                found = true;
                break;
            }
        }
    }
    return found;
}

bool napi_mf_classic_mfkey32_nonces_check_presence() {
    Storage* storage = furi_record_open(RECORD_STORAGE);

    bool nonces_present = storage_common_stat(storage, MF_CLASSIC_NONCE_PATH, NULL) == FSE_OK;

    furi_record_close(RECORD_STORAGE);

    return nonces_present;
}

bool distance_in_nonces_file(const char* file_path, const char* file_name) {
    char full_path[MAX_PATH_LEN];
    snprintf(full_path, sizeof(full_path), "%s/%s", file_path, file_name);
    bool distance_present = false;
    Storage* storage = furi_record_open(RECORD_STORAGE);
    Stream* file_stream = buffered_file_stream_alloc(storage);
    FuriString* line_str;
    line_str = furi_string_alloc();

    if(buffered_file_stream_open(file_stream, full_path, FSAM_READ, FSOM_OPEN_EXISTING)) {
        while(true) {
            if(!stream_read_line(file_stream, line_str)) break;
            if(furi_string_search_str(line_str, "distance") != FURI_STRING_FAILURE) {
                distance_present = true;
                break;
            }
        }
    }

    buffered_file_stream_close(file_stream);
    stream_free(file_stream);
    furi_string_free(line_str);
    furi_record_close(RECORD_STORAGE);

    return distance_present;
}

bool napi_mf_classic_nested_nonces_check_presence() {
    Storage* storage = furi_record_open(RECORD_STORAGE);

    if(!(storage_dir_exists(storage, MF_CLASSIC_NESTED_NONCE_PATH))) {
        furi_record_close(RECORD_STORAGE);
        return false;
    }

    bool nonces_present = false;
    File* dir = storage_file_alloc(storage);
    char filename_buffer[MAX_NAME_LEN];
    FileInfo file_info;

    if(storage_dir_open(dir, MF_CLASSIC_NESTED_NONCE_PATH)) {
        while(storage_dir_read(dir, &file_info, filename_buffer, MAX_NAME_LEN)) {
            // We only care about Static Nested files
            if(!(file_info.flags & FSF_DIRECTORY) && strstr(filename_buffer, ".nonces") &&
               !(distance_in_nonces_file(MF_CLASSIC_NESTED_NONCE_PATH, filename_buffer))) {
                nonces_present = true;
                break;
            }
        }
    }

    storage_dir_close(dir);
    storage_file_free(dir);
    furi_record_close(RECORD_STORAGE);

    return nonces_present;
}

int binaryStringToInt(const char* binStr) {
    int result = 0;
    while(*binStr) {
        result <<= 1;
        if(*binStr == '1') {
            result |= 1;
        }
        binStr++;
    }
    return result;
}

bool load_mfkey32_nonces(
    MfClassicNonceArray* nonce_array,
    ProgramState* program_state,
    KeysDict* system_dict,
    bool system_dict_exists,
    KeysDict* user_dict) {
    bool array_loaded = false;

    do {
        // https://github.com/flipperdevices/flipperzero-firmware/blob/5134f44c09d39344a8747655c0d59864bb574b96/applications/services/storage/filesystem_api_defines.h#L8-L22
        if(!buffered_file_stream_open(
               nonce_array->stream, MF_CLASSIC_NONCE_PATH, FSAM_READ_WRITE, FSOM_OPEN_EXISTING)) {
            buffered_file_stream_close(nonce_array->stream);
            break;
        }

        // Check for newline ending
        if(!stream_eof(nonce_array->stream)) {
            if(!stream_seek(nonce_array->stream, -1, StreamOffsetFromEnd)) break;
            uint8_t last_char = 0;
            if(stream_read(nonce_array->stream, &last_char, 1) != 1) break;
            if(last_char != '\n') {
                //FURI_LOG_D(TAG, "Adding new line ending");
                if(stream_write_char(nonce_array->stream, '\n') != 1) break;
            }
            if(!stream_rewind(nonce_array->stream)) break;
        }

        // Read total amount of nonces
        FuriString* next_line;
        next_line = furi_string_alloc();
        while(!(program_state->close_thread_please)) {
            if(!stream_read_line(nonce_array->stream, next_line)) {
                //FURI_LOG_T(TAG, "No nonces left");
                break;
            }
            /*
            FURI_LOG_T(
                TAG,
                "Read line: %s, len: %zu",
                furi_string_get_cstr(next_line),
                furi_string_size(next_line));
            */
            if(!furi_string_start_with_str(next_line, "Sec")) continue;
            const char* next_line_cstr = furi_string_get_cstr(next_line);
            MfClassicNonce res = {0};
            res.attack = mfkey32;
            int i = 0;
            char* endptr;
            for(i = 0; i <= 17; i++) {
                if(i != 0) {
                    next_line_cstr = strchr(next_line_cstr, ' ');
                    if(next_line_cstr) {
                        next_line_cstr++;
                    } else {
                        break;
                    }
                }
                unsigned long value = strtoul(next_line_cstr, &endptr, 16);
                switch(i) {
                case 5:
                    res.uid = value;
                    break;
                case 7:
                    res.nt0 = value;
                    break;
                case 9:
                    res.nr0_enc = value;
                    break;
                case 11:
                    res.ar0_enc = value;
                    break;
                case 13:
                    res.nt1 = value;
                    break;
                case 15:
                    res.nr1_enc = value;
                    break;
                case 17:
                    res.ar1_enc = value;
                    break;
                default:
                    break; // Do nothing
                }
                next_line_cstr = endptr;
            }
            res.p64 = prng_successor(res.nt0, 64);
            res.p64b = prng_successor(res.nt1, 64);
            res.uid_xor_nt0 = res.uid ^ res.nt0;
            res.uid_xor_nt1 = res.uid ^ res.nt1;

            (program_state->total)++;
            if((system_dict_exists && key_already_found_for_nonce_in_dict(system_dict, &res)) ||
               (key_already_found_for_nonce_in_dict(user_dict, &res))) {
                (program_state->cracked)++;
                (program_state->num_completed)++;
                continue;
            }
            //FURI_LOG_I(TAG, "No key found for %8lx %8lx", res.uid, res.ar1_enc);
            // TODO: Refactor
            nonce_array->remaining_nonce_array = realloc( //-V701
                nonce_array->remaining_nonce_array,
                sizeof(MfClassicNonce) * ((nonce_array->remaining_nonces) + 1));
            nonce_array->remaining_nonces++;
            nonce_array->remaining_nonce_array[(nonce_array->remaining_nonces) - 1] = res;
            nonce_array->total_nonces++;
        }
        furi_string_free(next_line);
        buffered_file_stream_close(nonce_array->stream);
        //stream_free(nonce_array->stream);

        array_loaded = true;
        //FURI_LOG_I(TAG, "Loaded %lu Mfkey32 nonces", nonce_array->total_nonces);
    } while(false);

    return array_loaded;
}

bool load_nested_nonces(
    MfClassicNonceArray* nonce_array,
    ProgramState* program_state,
    KeysDict* system_dict,
    bool system_dict_exists,
    KeysDict* user_dict) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* dir = storage_file_alloc(storage);
    char filename_buffer[MAX_NAME_LEN];
    FileInfo file_info;
    FuriString* next_line = furi_string_alloc();

    if(!storage_dir_open(dir, MF_CLASSIC_NESTED_NONCE_PATH)) {
        storage_dir_close(dir);
        storage_file_free(dir);
        furi_record_close(RECORD_STORAGE);
        furi_string_free(next_line);
        return false;
    }

    while(storage_dir_read(dir, &file_info, filename_buffer, MAX_NAME_LEN)) {
        if(!(file_info.flags & FSF_DIRECTORY) && strstr(filename_buffer, ".nonces") &&
           !(distance_in_nonces_file(MF_CLASSIC_NESTED_NONCE_PATH, filename_buffer))) {
            char full_path[MAX_PATH_LEN];
            snprintf(
                full_path,
                sizeof(full_path),
                "%s/%s",
                MF_CLASSIC_NESTED_NONCE_PATH,
                filename_buffer);

            // TODO: We should only need READ_WRITE here if we plan on adding a newline to the end of the file if has none
            if(!buffered_file_stream_open(
                   nonce_array->stream, full_path, FSAM_READ_WRITE, FSOM_OPEN_EXISTING)) {
                buffered_file_stream_close(nonce_array->stream);
                continue;
            }

            while(stream_read_line(nonce_array->stream, next_line)) {
                if(furi_string_search_str(next_line, "Nested:") != FURI_STRING_FAILURE) {
                    MfClassicNonce res = {0};
                    res.attack = static_nested;
                    int parsed = sscanf(
                        furi_string_get_cstr(next_line),
                        "Nested: %*s %*s cuid 0x%" PRIx32 " nt0 0x%" PRIx32 " ks0 0x%" PRIx32
                        " par0 %4[01] nt1 0x%" PRIx32 " ks1 0x%" PRIx32 " par1 %4[01]",
                        &res.uid,
                        &res.nt0,
                        &res.ks1_1_enc,
                        res.par_1_str,
                        &res.nt1,
                        &res.ks1_2_enc,
                        res.par_2_str);

                    if(parsed != 7) continue;
                    res.par_1 = binaryStringToInt(res.par_1_str);
                    res.par_2 = binaryStringToInt(res.par_2_str);
                    res.uid_xor_nt0 = res.uid ^ res.nt0;
                    res.uid_xor_nt1 = res.uid ^ res.nt1;

                    (program_state->total)++;
                    if((system_dict_exists &&
                        key_already_found_for_nonce_in_dict(system_dict, &res)) ||
                       (key_already_found_for_nonce_in_dict(user_dict, &res))) {
                        (program_state->cracked)++;
                        (program_state->num_completed)++;
                        continue;
                    }

                    nonce_array->remaining_nonce_array = realloc(
                        nonce_array->remaining_nonce_array,
                        sizeof(MfClassicNonce) * (nonce_array->remaining_nonces + 1));
                    nonce_array->remaining_nonce_array[nonce_array->remaining_nonces] = res;
                    nonce_array->remaining_nonces++;
                    nonce_array->total_nonces++;
                }
            }

            buffered_file_stream_close(nonce_array->stream);
        }
    }

    storage_dir_close(dir);
    storage_file_free(dir);
    furi_record_close(RECORD_STORAGE);
    furi_string_free(next_line);

    //FURI_LOG_I(TAG, "Loaded %lu Static Nested nonces", nonce_array->total_nonces);
    return true;
}

MfClassicNonceArray* napi_mf_classic_nonce_array_alloc(
    KeysDict* system_dict,
    bool system_dict_exists,
    KeysDict* user_dict,
    ProgramState* program_state) {
    MfClassicNonceArray* nonce_array = malloc(sizeof(MfClassicNonceArray));
    MfClassicNonce* remaining_nonce_array_init = malloc(sizeof(MfClassicNonce) * 1);
    nonce_array->remaining_nonce_array = remaining_nonce_array_init;
    Storage* storage = furi_record_open(RECORD_STORAGE);
    nonce_array->stream = buffered_file_stream_alloc(storage);
    furi_record_close(RECORD_STORAGE);

    bool array_loaded = false;

    if(program_state->mfkey32_present) {
        array_loaded = load_mfkey32_nonces(
            nonce_array, program_state, system_dict, system_dict_exists, user_dict);
    }

    if(program_state->nested_present) {
        array_loaded |= load_nested_nonces(
            nonce_array, program_state, system_dict, system_dict_exists, user_dict);
    }

    if(!array_loaded) {
        free(nonce_array);
        nonce_array = NULL;
    }

    return nonce_array;
}

void napi_mf_classic_nonce_array_free(MfClassicNonceArray* nonce_array) {
    // TODO: Track free state at the time this is called to ensure double free does not happen
    furi_assert(nonce_array);
    furi_assert(nonce_array->stream);

    buffered_file_stream_close(nonce_array->stream);
    stream_free(nonce_array->stream);
    free(nonce_array);
}

/* Actual implementation of app<>plugin interface */
static const MfkeyPlugin init_plugin = {
    .name = "Initialization Plugin",
    .napi_mf_classic_mfkey32_nonces_check_presence =
        &napi_mf_classic_mfkey32_nonces_check_presence,
    .napi_mf_classic_nested_nonces_check_presence = &napi_mf_classic_nested_nonces_check_presence,
    .napi_mf_classic_nonce_array_alloc = &napi_mf_classic_nonce_array_alloc,
    .napi_mf_classic_nonce_array_free = &napi_mf_classic_nonce_array_free,
};

/* Plugin descriptor to comply with basic plugin specification */
static const FlipperAppPluginDescriptor init_plugin_descriptor = {
    .appid = PLUGIN_APP_ID,
    .ep_api_version = PLUGIN_API_VERSION,
    .entry_point = &init_plugin,
};

/* Plugin entry point - must return a pointer to const descriptor  */
const FlipperAppPluginDescriptor* init_plugin_ep() {
    return &init_plugin_descriptor;
}