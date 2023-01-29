// TODO: Count total number of nonces
// TODO: Update progress bar
// TODO: Use Flipper dictionary to eliminate already computed keys
// TODO: Run attack using mfkeypaging.c
// TODO: Eliminate functions:
/*
    fclose: storage_file_close
    fgets: Not directly equivalent, but similar functionality can be achieved using storage_file_read and string manipulation
    fopen: storage_file_open
    fputc: Not directly equivalent, but similar functionality can be achieved using storage_file_write and casting a single character to a void*
    fread: storage_file_read
    fseek: storage_file_seek
    ftell: storage_file_tell
    fwrite: storage_file_write
    strtok_r: Not directly equivalent, but similar functionality can be achieved using string manipulation and storage_get_next_filename
*/

#include <furi.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include <input/input.h>
#include <stdlib.h>
#include "mfkey32_icons.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <storage/storage.h>
#include <lib/nfc/helpers/mf_classic_dict.h>
#include <lib/toolbox/args.h>
#include <lib/flipper_format/flipper_format.h>

#define MF_CLASSIC_DICT_FLIPPER_PATH EXT_PATH("nfc/assets/mf_classic_dict.nfc")
#define MF_CLASSIC_DICT_USER_PATH EXT_PATH("nfc/assets/mf_classic_dict_user.nfc")
#define MF_CLASSIC_NONCE_PATH EXT_PATH("nfc/.mfkey32.log")
#define MF_CLASSIC_MEM_FILE_PATH EXT_PATH("nfc/cache/mem_file.bin")
#define TAG "Mfkey32"
#define NFC_MF_CLASSIC_KEY_LEN (13)

#define LF_POLY_ODD (0x29CE5C)
#define LF_POLY_EVEN (0x870804)
#define BIT(x, n) ((x) >> (n)&1)
#define BEBIT(x, n) BIT(x, (n) ^ 24)
#define SWAPENDIAN(x) (x = (x >> 8 & 0xff00ff) | (x & 0xff00ff) << 8, x = x >> 16 | x << 16)
#define SIZEOF(arr) sizeof(arr) / sizeof(*arr)

struct Crypto1State {
    uint32_t odd, even;
};

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} PluginEvent;

typedef enum {
    MissingNonces,
    OutOfMemory,
} MfkeyError;

typedef struct {
    bool running;
    MfkeyError err;
    int cracked;
    int total;
    int dict_count;
} ProgramState;

typedef struct {
    uint32_t uid; // serial number
    uint32_t nt0; // tag challenge first
    uint32_t nt1; // tag challenge second
    uint32_t nr0_enc; // first encrypted reader challenge
    uint32_t ar0_enc; // first encrypted reader response
    uint32_t nr1_enc; // second encrypted reader challenge
    uint32_t ar1_enc; // second encrypted reader response
} MfClassicNonce;

typedef struct {
    Stream* stream;
    uint32_t total_nonces;
} MfClassicNonceArray;

struct MfClassicDict {
    Stream* stream;
    uint32_t total_keys;
};

void* file_calloc(size_t nmemb, size_t size, File* file) {
    /*
    // Seek to the end of the file
    fseek(file, 0, SEEK_END); // XXX FIXME
    // Get the current position, which is the end of the file
    long int offset = ftell(file); // XXX FIXME
    // Write nmemb * size bytes of 0s to the file
    for (uint32_t i = 0; i < nmemb * size; i++) {
        fputc(0, file); // XXX FIXME
    }
    */
    long int offset = 0; // DEBUG
    // Return the starting offset of the allocated memory
    return (void*)offset;
}

void file_write_at_offset(void* ptr, int element, void* src, size_t n, File* file) {
    /*
    // Seek to the specified offset in the file
    fseek(file, (long int) (ptr + (element*n)), SEEK_SET); // XXX FIXME
    // Write n bytes from src to the file
    fwrite(src, n, 1, file); // XXX FIXME
    */
}

void* file_read_at_offset(void* ptr, int element, size_t n, File* file) {
    /*
    // Seek to the specified offset in the file
    fseek(file, (long int) (ptr + (element*n)), SEEK_SET); // XXX FIXME
    // Read n bytes from the file
    void *buf = malloc(n);
    fread(buf, n, 1, file); // XXX FIXME
    */
    void* buf = 0; // DEBUG
    return buf;
}
int file_read_int_at_offset(void* ptr, int element, size_t n, File* file) {
    /*
    int res;
    // Seek to the specified offset in the file
    fseek(file, (long int) (ptr + (element*n)), SEEK_SET); // XXX FIXME
    // Read n bytes from the file
    fread(&res, n, 1, file); // XXX FIXME
    */
    int res = 0;
    return res;
}

uint32_t prng_successor(uint32_t x, uint32_t n) {
    SWAPENDIAN(x);
    while(n--) x = x >> 1 | (x >> 16 ^ x >> 18 ^ x >> 19 ^ x >> 21) << 31;
    return SWAPENDIAN(x);
}

static inline int filter(uint32_t const x) {
    uint32_t f;
    f = 0xf22c0 >> (x & 0xf) & 16;
    f |= 0x6c9c0 >> (x >> 4 & 0xf) & 8;
    f |= 0x3c8b0 >> (x >> 8 & 0xf) & 4;
    f |= 0x1e458 >> (x >> 12 & 0xf) & 2;
    f |= 0x0d938 >> (x >> 16 & 0xf) & 1;
    return BIT(0xEC57E80A, f);
}

static inline uint8_t evenparity32(uint32_t x) {
    return (__builtin_parity(x) & 0xFF);
}

int binsearch(void* data_offset, File* mem_file, int start, int stop) {
    int mid, val = file_read_int_at_offset(data_offset, stop, sizeof(int), mem_file) & 0xff000000;
    while(start != stop) {
        mid = (stop - start) >> 1;
        int data_start_mid =
            file_read_int_at_offset(data_offset, start + mid, sizeof(int), mem_file);
        if((data_start_mid ^ 0x80000000) > (val ^ 0x80000000))
            stop = start + mid;
        else
            start += mid + 1;
    }
    return start;
}

void quicksort(void* data_offset, File* mem_file, int low, int high) {
    //if (SIZEOF(array) == 0)
    //    return;
    if(low >= high) return;
    int middle = low + (high - low) / 2;
    int pivot = file_read_int_at_offset(data_offset, middle, sizeof(int), mem_file);
    int i = low, j = high;
    while(i <= j) {
        while(file_read_int_at_offset(data_offset, i, sizeof(int), mem_file) < pivot) {
            i++;
        }
        while(file_read_int_at_offset(data_offset, j, sizeof(int), mem_file) > pivot) {
            j--;
        }
        if(i <= j) { // swap
            int temp = file_read_int_at_offset(data_offset, i, sizeof(int), mem_file);
            int temp_2 = file_read_int_at_offset(data_offset, j, sizeof(int), mem_file);
            file_write_at_offset(data_offset, i, &temp_2, sizeof(int), mem_file);
            file_write_at_offset(data_offset, j, &temp, sizeof(int), mem_file);
            i++;
            j--;
        }
    }
    if(low < j) {
        quicksort(data_offset, mem_file, low, j);
    }
    if(high > i) {
        quicksort(data_offset, mem_file, i, high);
    }
}

void update_contribution(void* data_offset, File* mem_file, int item, int mask1, int mask2) {
    int data_item = file_read_int_at_offset(data_offset, item, sizeof(int), mem_file);
    int p = data_item >> 25;
    p = p << 1 | evenparity32(data_item & mask1);
    p = p << 1 | evenparity32(data_item & mask2);
    data_item = p << 24 | (data_item & 0xffffff);
    file_write_at_offset(data_offset, item, &data_item, sizeof(int), mem_file);
}

int extend_table(void* data_offset, File* mem_file, int tbl, int end, int bit, int m1, int m2) {
    int data_tbl_init = file_read_int_at_offset(data_offset, tbl, sizeof(int), mem_file);
    data_tbl_init <<= 1;
    file_write_at_offset(data_offset, tbl, &data_tbl_init, sizeof(int), mem_file);
    while(tbl <= end) {
        int data_tbl = file_read_int_at_offset(data_offset, tbl, sizeof(int), mem_file);
        if((filter(data_tbl) ^ filter(data_tbl | 1)) != 0) {
            data_tbl |= filter(data_tbl) ^ bit;
            file_write_at_offset(data_offset, tbl, &data_tbl, sizeof(int), mem_file);
            update_contribution(data_offset, mem_file, tbl, m1, m2);
        } else if(filter(data_tbl) == bit) {
            end++;
            int data_tbl_2 = file_read_int_at_offset(data_offset, tbl + 1, sizeof(int), mem_file);
            file_write_at_offset(data_offset, end, &data_tbl_2, sizeof(int), mem_file);
            int data_tbl_3 = file_read_int_at_offset(data_offset, tbl, sizeof(int), mem_file);
            data_tbl = data_tbl_3 | 1;
            file_write_at_offset(data_offset, tbl + 1, &data_tbl, sizeof(int), mem_file);
            update_contribution(data_offset, mem_file, tbl, m1, m2);
            tbl++;
            update_contribution(data_offset, mem_file, tbl, m1, m2);
        } else {
            int data_end = file_read_int_at_offset(data_offset, end, sizeof(int), mem_file);
            file_write_at_offset(data_offset, tbl, &data_end, sizeof(int), mem_file);
            end--;
            tbl--;
        }
        tbl++;
        int data_tbl_update = file_read_int_at_offset(data_offset, tbl, sizeof(int), mem_file);
        data_tbl_update <<= 1;
        file_write_at_offset(data_offset, tbl, &data_tbl_update, sizeof(int), mem_file);
    }
    return end;
}

int extend_table_simple(void* data_offset, File* mem_file, int tbl, int end, int bit) {
    int data_tbl_init = file_read_int_at_offset(data_offset, tbl, sizeof(int), mem_file);
    data_tbl_init <<= 1;
    file_write_at_offset(data_offset, tbl, &data_tbl_init, sizeof(int), mem_file);
    while(tbl <= end) {
        int data_tbl = file_read_int_at_offset(data_offset, tbl, sizeof(int), mem_file);
        if((filter(data_tbl) ^ filter(data_tbl | 1)) != 0) {
            data_tbl |= filter(data_tbl) ^ bit;
            file_write_at_offset(data_offset, tbl, &data_tbl, sizeof(int), mem_file);
        } else if(filter(data_tbl) == bit) {
            end++;
            tbl++;
            int data_tbl_2 = file_read_int_at_offset(data_offset, tbl, sizeof(int), mem_file);
            file_write_at_offset(data_offset, end, &data_tbl_2, sizeof(int), mem_file);
            int data_tbl_3 = file_read_int_at_offset(data_offset, tbl - 1, sizeof(int), mem_file);
            data_tbl = data_tbl_3 | 1;
            file_write_at_offset(data_offset, tbl, &data_tbl, sizeof(int), mem_file);
        } else {
            int data_end = file_read_int_at_offset(data_offset, end, sizeof(int), mem_file);
            file_write_at_offset(data_offset, tbl, &data_end, sizeof(int), mem_file);
            end--;
            tbl--;
        }
        tbl++;
        int data_tbl_update = file_read_int_at_offset(data_offset, tbl, sizeof(int), mem_file);
        data_tbl_update <<= 1;
        file_write_at_offset(data_offset, tbl, &data_tbl_update, sizeof(int), mem_file);
    }
    return end;
}

int recover(
    void* odd_start_offset,
    File* mem_file,
    int o_head,
    int o_tail,
    int oks,
    void* even_start_offset,
    int e_head,
    int e_tail,
    int eks,
    int rem,
    void* statelist_start_offset,
    int s) {
    int o = 0, e = 0, i = 0;
    if(rem == -1) {
        for(e = e_head; e <= e_tail; ++e) {
            int even_e = file_read_int_at_offset(even_start_offset, e, sizeof(int), mem_file);
            even_e = (even_e << 1) ^ evenparity32(even_e & LF_POLY_EVEN);
            file_write_at_offset(even_start_offset, e, &even_e, sizeof(int), mem_file);
            for(o = o_head; o <= o_tail; ++o, ++s) {
                int odd_o = file_read_int_at_offset(odd_start_offset, o, sizeof(int), mem_file);
                int even_e_xor = even_e ^ evenparity32(odd_o & LF_POLY_ODD);
                struct Crypto1State sl_s = {even_e_xor, odd_o};
                file_write_at_offset(
                    statelist_start_offset, s, &sl_s, sizeof(struct Crypto1State), mem_file);
                struct Crypto1State sl_s_1 = {0, 0};
                file_write_at_offset(
                    statelist_start_offset, s + 1, &sl_s_1, sizeof(struct Crypto1State), mem_file);
            }
        }
        return s;
    }
    for(i = 0; (i < 4) && (rem-- != 0); i++) {
        oks >>= 1;
        eks >>= 1;
        o_tail = extend_table(
            odd_start_offset,
            mem_file,
            o_head,
            o_tail,
            oks & 1,
            LF_POLY_EVEN << 1 | 1,
            LF_POLY_ODD << 1);
        if(o_head > o_tail) return s;
        e_tail = extend_table(
            even_start_offset,
            mem_file,
            e_head,
            e_tail,
            eks & 1,
            LF_POLY_ODD,
            LF_POLY_EVEN << 1 | 1);
        if(e_head > e_tail) return s;
    }
    quicksort(odd_start_offset, mem_file, o_head, o_tail);
    quicksort(even_start_offset, mem_file, e_head, e_tail);
    while(o_tail >= o_head && e_tail >= e_head) {
        int odd_o_tail = file_read_int_at_offset(odd_start_offset, o_tail, sizeof(int), mem_file);
        int even_e_tail =
            file_read_int_at_offset(even_start_offset, e_tail, sizeof(int), mem_file);
        if(((odd_o_tail ^ even_e_tail) >> 24) == 0) {
            o_tail = binsearch(odd_start_offset, mem_file, o_head, o = o_tail);
            e_tail = binsearch(even_start_offset, mem_file, e_head, e = e_tail);
            s = recover(
                odd_start_offset,
                mem_file,
                o_tail--,
                o,
                oks,
                even_start_offset,
                e_tail--,
                e,
                eks,
                rem,
                statelist_start_offset,
                s);
        } else if((odd_o_tail ^ 0x80000000) > (even_e_tail ^ 0x80000000)) {
            o_tail = binsearch(odd_start_offset, mem_file, o_head, o_tail) - 1;
        } else {
            e_tail = binsearch(even_start_offset, mem_file, e_head, e_tail) - 1;
        }
    }
    return s;
}

struct Crypto1State* lfsr_recovery32(int ks2, File* mem_file) {
    int odd_head = 0, odd_tail = -1, oks = 0;
    int even_head = 0, even_tail = -1, eks = 0;
    void* odd_start_offset = file_calloc(1, 5 << 19, mem_file);
    void* even_start_offset = file_calloc(1, 5 << 19, mem_file);
    void* statelist_start_offset = file_calloc(1, sizeof(struct Crypto1State) << 18, mem_file);
    int i;
    for(i = 31; i >= 0; i -= 2) oks = oks << 1 | BEBIT(ks2, i);
    for(i = 30; i >= 0; i -= 2) eks = eks << 1 | BEBIT(ks2, i);
    for(i = 1 << 20; i >= 0; --i) {
        if(filter(i) == (oks & 1))
            //odd[++odd_tail] = i;
            file_write_at_offset(odd_start_offset, ++odd_tail, &i, sizeof(int), mem_file);
        if(filter(i) == (eks & 1))
            //even[++even_tail] = i;
            file_write_at_offset(even_start_offset, ++even_tail, &i, sizeof(int), mem_file);
    }
    for(i = 0; i < 4; i++) {
        odd_tail =
            extend_table_simple(odd_start_offset, mem_file, odd_head, odd_tail, ((oks >>= 1) & 1));
        even_tail = extend_table_simple(
            even_start_offset, mem_file, even_head, even_tail, ((eks >>= 1) & 1));
    }
    recover(
        odd_start_offset,
        mem_file,
        odd_head,
        odd_tail,
        oks,
        even_start_offset,
        even_head,
        even_tail,
        eks,
        11,
        statelist_start_offset,
        0);
    return statelist_start_offset;
}

void crypto1_get_lfsr(struct Crypto1State* state, uint64_t* lfsr) {
    int i;
    for(*lfsr = 0, i = 23; i >= 0; --i) {
        *lfsr = *lfsr << 1 | BIT(state->odd, i ^ 3);
        *lfsr = *lfsr << 1 | BIT(state->even, i ^ 3);
    }
}

uint8_t crypt_or_rollback_bit(struct Crypto1State* s, uint32_t in, int x, int is_crypt) {
    uint8_t ret;
    uint32_t feedin, t;
    if(is_crypt == 0) {
        s->odd &= 0xffffff;
        t = s->odd, s->odd = s->even, s->even = t;
    }
    ret = filter(s->odd);
    feedin = ret & (!!x);
    if(is_crypt == 0) {
        feedin ^= s->even & 1;
        feedin ^= LF_POLY_EVEN & (s->even >>= 1);
    } else {
        feedin ^= LF_POLY_EVEN & s->even;
    }
    feedin ^= LF_POLY_ODD & s->odd;
    feedin ^= !!in;
    if(is_crypt == 0) {
        s->even |= (evenparity32(feedin)) << 23;
    } else {
        s->even = s->even << 1 | (evenparity32(feedin));
        t = s->odd, s->odd = s->even, s->even = t;
    }
    return ret;
}

uint32_t crypt_or_rollback_word(struct Crypto1State* s, uint32_t in, int x, int is_crypt) {
    uint32_t ret = 0;
    int i;
    if(is_crypt == 0) {
        for(i = 31; i >= 0; i--) {
            ret |= crypt_or_rollback_bit(s, BEBIT(in, i), x, 0) << (24 ^ i);
        }
    } else {
        for(i = 0; i <= 31; i++) {
            ret |= crypt_or_rollback_bit(s, BEBIT(in, i), x, 1) << (24 ^ i);
        }
    }
    return ret;
}

int napi_key_already_found_for_nonce(
    MfClassicDict* dict,
    uint32_t uid_xor_nt1,
    uint32_t nr1_enc,
    uint32_t p64b,
    uint32_t ar1_enc) {
    uint32_t found = 0;
    // TODO
    /*
    uint32_t k = 0, found = 0;
    for(k = 0; k < keyarray_size; k++) {
        struct Crypto1State temp = {0, 0};
        int i;
        for (i = 0; i < 24; i++) {
            (&temp)->odd |= (BIT(keyarray[k], 2*i+1) << (i ^ 3));
            (&temp)->even |= (BIT(keyarray[k], 2*i) << (i ^ 3));
        }
        crypt_or_rollback_word(&temp, uid_xor_nt1, 0, 1);
        crypt_or_rollback_word(&temp, nr1_enc, 1, 1);
        if (ar1_enc == (crypt_or_rollback_word(&temp, 0, 0, 1) ^ p64b)) {
            found = 1;
            break;
        }
    }
    */
    return found;
}

bool napi_mf_classic_nonces_check_presence() {
    Storage* storage = furi_record_open(RECORD_STORAGE);

    bool nonces_present = storage_common_stat(storage, MF_CLASSIC_NONCE_PATH, NULL) == FSE_OK;

    furi_record_close(RECORD_STORAGE);

    return nonces_present;
}

MfClassicNonceArray*
    napi_mf_classic_nonce_array_alloc(MfClassicDict* system_dict, MfClassicDict* user_dict) {
    MfClassicNonceArray* nonce_array = malloc(sizeof(MfClassicNonceArray));
    Storage* storage = furi_record_open(RECORD_STORAGE);
    nonce_array->stream = buffered_file_stream_alloc(storage);
    furi_record_close(RECORD_STORAGE);

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
                FURI_LOG_D(TAG, "Adding new line ending");
                if(stream_write_char(nonce_array->stream, '\n') != 1) break;
            }
            if(!stream_rewind(nonce_array->stream)) break;
        }

        // Read total amount of nonces
        FuriString* next_line;
        next_line = furi_string_alloc();
        while(true) {
            if(!stream_read_line(nonce_array->stream, next_line)) {
                FURI_LOG_T(TAG, "No nonces left");
                break;
            }
            FURI_LOG_T(
                TAG,
                "Read line: %s, len: %zu",
                furi_string_get_cstr(next_line),
                furi_string_size(next_line));
            if(!furi_string_start_with_str(next_line, "Sec")) continue;
            const char* next_line_cstr = furi_string_get_cstr(next_line);
            MfClassicNonce res = {0};
            char token[20];
            int i = 0;
            const char* ptr = next_line_cstr;
            while(sscanf(ptr, "%s", token) == 1) {
                switch(i) {
                case 5:
                    sscanf(token, "%lx", &res.uid);
                    break;
                case 7:
                    sscanf(token, "%lx", &res.nt0);
                    break;
                case 9:
                    sscanf(token, "%lx", &res.nr0_enc);
                    break;
                case 11:
                    sscanf(token, "%lx", &res.ar0_enc);
                    break;
                case 13:
                    sscanf(token, "%lx", &res.nt1);
                    break;
                case 15:
                    sscanf(token, "%lx", &res.nr1_enc);
                    break;
                case 17:
                    sscanf(token, "%lx", &res.ar1_enc);
                    break;
                default:
                    break; // Do nothing
                }
                i++;
                ptr = strchr(ptr, ' ');
                if(!ptr) {
                    break;
                }
                ptr++;
            }
            // TODO: Scan line and check if key for nonce is already known
            uint32_t p64b = prng_successor(res.nt1, 64);
            if(napi_key_already_found_for_nonce(
                   system_dict, res.uid ^ res.nt1, res.nr1_enc, p64b, res.ar1_enc) ||
               napi_key_already_found_for_nonce(
                   user_dict, res.uid ^ res.nt1, res.nr1_enc, p64b, res.ar1_enc)) {
                continue;
            }
            //FURI_LOG_I(TAG, "Nonce uid: %lx, ar1: %lx", res.uid, res.ar1_enc);
            // Store what nonces need to be cracked in an array
            nonce_array->total_nonces++;
        }
        furi_string_free(next_line);
        stream_rewind(nonce_array->stream);

        array_loaded = true;
        FURI_LOG_I(TAG, "Loaded %lu nonces", nonce_array->total_nonces);
    } while(false);

    if(!array_loaded) {
        buffered_file_stream_close(nonce_array->stream);
        free(nonce_array);
        nonce_array = NULL;
    }

    return nonce_array;
}

void napi_mf_classic_nonce_array_free(MfClassicNonceArray* nonce_array) {
    furi_assert(nonce_array);
    furi_assert(nonce_array->stream);

    buffered_file_stream_close(nonce_array->stream);
    stream_free(nonce_array->stream);
    free(nonce_array);
}

bool napi_mf_classic_dict_check_presence(MfClassicDictType dict_type) {
    Storage* storage = furi_record_open(RECORD_STORAGE);

    bool dict_present = false;
    if(dict_type == MfClassicDictTypeSystem) {
        dict_present = storage_common_stat(storage, MF_CLASSIC_DICT_FLIPPER_PATH, NULL) == FSE_OK;
    } else if(dict_type == MfClassicDictTypeUser) {
        dict_present = storage_common_stat(storage, MF_CLASSIC_DICT_USER_PATH, NULL) == FSE_OK;
    }

    furi_record_close(RECORD_STORAGE);

    return dict_present;
}

MfClassicDict* napi_mf_classic_dict_alloc(MfClassicDictType dict_type) {
    MfClassicDict* dict = malloc(sizeof(MfClassicDict));
    Storage* storage = furi_record_open(RECORD_STORAGE);
    dict->stream = buffered_file_stream_alloc(storage);
    furi_record_close(RECORD_STORAGE);

    bool dict_loaded = false;
    do {
        if(dict_type == MfClassicDictTypeSystem) {
            if(!buffered_file_stream_open(
                   dict->stream,
                   MF_CLASSIC_DICT_FLIPPER_PATH,
                   FSAM_READ_WRITE,
                   FSOM_OPEN_EXISTING)) {
                buffered_file_stream_close(dict->stream);
                break;
            }
        } else if(dict_type == MfClassicDictTypeUser) {
            if(!buffered_file_stream_open(
                   dict->stream, MF_CLASSIC_DICT_USER_PATH, FSAM_READ_WRITE, FSOM_OPEN_ALWAYS)) {
                buffered_file_stream_close(dict->stream);
                break;
            }
        }

        // Check for newline ending
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

void napi_mf_classic_dict_free(MfClassicDict* dict) {
    furi_assert(dict);
    furi_assert(dict->stream);

    buffered_file_stream_close(dict->stream);
    stream_free(dict->stream);
    free(dict);
}

static void napi_mf_classic_dict_int_to_str(uint8_t* key_int, FuriString* key_str) {
    furi_string_reset(key_str);
    for(size_t i = 0; i < 6; i++) {
        furi_string_cat_printf(key_str, "%02X", key_int[i]);
    }
}

static void napi_mf_classic_dict_str_to_int(FuriString* key_str, uint64_t* key_int) {
    uint8_t key_byte_tmp;

    *key_int = 0ULL;
    for(uint8_t i = 0; i < 12; i += 2) {
        args_char_to_hex(
            furi_string_get_char(key_str, i), furi_string_get_char(key_str, i + 1), &key_byte_tmp);
        *key_int |= (uint64_t)key_byte_tmp << (8 * (5 - i / 2));
    }
}

uint32_t napi_mf_classic_dict_get_total_keys(MfClassicDict* dict) {
    furi_assert(dict);

    return dict->total_keys;
}

bool napi_mf_classic_dict_rewind(MfClassicDict* dict) {
    furi_assert(dict);
    furi_assert(dict->stream);

    return stream_rewind(dict->stream);
}

bool napi_mf_classic_dict_get_next_key_str(MfClassicDict* dict, FuriString* key) {
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

bool napi_mf_classic_dict_get_next_key(MfClassicDict* dict, uint64_t* key) {
    furi_assert(dict);
    furi_assert(dict->stream);

    FuriString* temp_key;
    temp_key = furi_string_alloc();
    bool key_read = napi_mf_classic_dict_get_next_key_str(dict, temp_key);
    if(key_read) {
        napi_mf_classic_dict_str_to_int(temp_key, key);
    }
    furi_string_free(temp_key);
    return key_read;
}

bool napi_mf_classic_dict_is_key_present_str(MfClassicDict* dict, FuriString* key) {
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

bool napi_mf_classic_dict_is_key_present(MfClassicDict* dict, uint8_t* key) {
    FuriString* temp_key;

    temp_key = furi_string_alloc();
    napi_mf_classic_dict_int_to_str(key, temp_key);
    bool key_found = napi_mf_classic_dict_is_key_present_str(dict, temp_key);
    furi_string_free(temp_key);
    return key_found;
}

// TODO: Do we manually need to render here if the main thread is blocked?
void mfkey32(ProgramState* const program_state) {
    uint64_t key; // recovered key
    uint32_t uid; // serial number
    uint32_t nt0; // tag challenge first
    uint32_t nt1; // tag challenge second
    uint32_t nr0_enc; // first encrypted reader challenge
    uint32_t ar0_enc; // first encrypted reader response
    uint32_t nr1_enc; // second encrypted reader challenge
    uint32_t ar1_enc; // second encrypted reader response
    uint32_t i;
    char* rest;
    char* token;
    size_t keyarray_size;
    uint64_t* keyarray = malloc(sizeof(uint64_t) * 1);
    struct Crypto1State* temp;
    // Check for nonces
    if(!napi_mf_classic_nonces_check_presence()) {
        program_state->err = MissingNonces;
        return;
    }
    // Read dictionaries (optional)
    MfClassicDict* system_dict = {0};
    bool system_dict_exists = napi_mf_classic_dict_check_presence(MfClassicDictTypeSystem);
    MfClassicDict* user_dict = {0};
    bool user_dict_exists = napi_mf_classic_dict_check_presence(MfClassicDictTypeUser);
    uint32_t total_dict_keys = 0;
    if(system_dict_exists) {
        system_dict = napi_mf_classic_dict_alloc(MfClassicDictTypeSystem);
        total_dict_keys += napi_mf_classic_dict_get_total_keys(system_dict);
    }
    if(user_dict_exists) {
        user_dict = napi_mf_classic_dict_alloc(MfClassicDictTypeUser);
        total_dict_keys += napi_mf_classic_dict_get_total_keys(user_dict);
    }
    program_state->dict_count = total_dict_keys;
    // Read nonces
    MfClassicNonceArray* nonce_arr;
    nonce_arr = napi_mf_classic_nonce_array_alloc(system_dict, user_dict);
    Storage* storage = furi_record_open(RECORD_STORAGE);
    storage_simply_remove(storage, MF_CLASSIC_MEM_FILE_PATH);
    /*
    keyarray_size = 0;
    while(fgets(buffer, bufferLength, filePointer)) { // XXX FIXME
        rest = buffer;
        for (i = 0; i <= 17; i++) {
            token = strtok_r(rest, " ", &rest); // XXX FIXME
            switch(i){
                case 5: sscanf(token, "%lx", &uid); break;
                case 7: sscanf(token, "%lx", &nt0); break;
                case 9: sscanf(token, "%lx", &nr0_enc); break;
                case 11: sscanf(token, "%lx", &ar0_enc); break;
                case 13: sscanf(token, "%lx", &nt1); break;
                case 15: sscanf(token, "%lx", &nr1_enc); break;
                case 17: sscanf(token, "%lx", &ar1_enc); break;
                default: break; // Do nothing
            }
        }
        uint32_t p64 = prng_successor(nt0, 64);
        uint32_t p64b = prng_successor(nt1, 64);
        if (key_already_found_for_nonce(keyarray, keyarray_size, uid ^ nt1, nr1_enc, p64b, ar1_enc)) {
            continue;
        }
        File *mem_file = fopen(file_path, "wb+"); // XXX FIXME
        void *s_offset = lfsr_recovery32(ar0_enc ^ p64, mem_file);
        int ti = 0;
        while (1) {
            temp = (struct Crypto1State *) file_read_at_offset(s_offset, ti, sizeof(struct Crypto1State), mem_file);
            if (!temp->odd && !temp->even) break;
            crypt_or_rollback_word(temp, 0, 0, 0);
            crypt_or_rollback_word(temp, nr0_enc, 1, 0);
            crypt_or_rollback_word(temp, uid ^ nt0, 0, 0);
            crypto1_get_lfsr(temp, &key);
            crypt_or_rollback_word(temp, uid ^ nt1, 0, 1);
            crypt_or_rollback_word(temp, nr1_enc, 1, 1);
            if (ar1_enc == (crypt_or_rollback_word(temp, 0, 0, 1) ^ p64b)) {
                int found = 0;
                for(i = 0; i < keyarray_size; i++) {
                    if (keyarray[i] == key) {
                        found = 1;
                        break;
                    }
                }
                if (found == 0) {
                    keyarray = realloc(keyarray, sizeof(uint64_t)*(keyarray_size+1));
                    keyarray_size += 1;
                    keyarray[keyarray_size-1] = key;
                }
                break;
            }
            //file_write_at_offset(s_offset, t, &temp, sizeof(struct Crypto1State), mem_file);
            free(temp);
            ti++;
        }
    }

    // TODO: Update display to show all keys were found
    fclose(filePointer); // XXX FIXME
    printf("Unique keys found:\n");
    for(i = 0; i < keyarray_size; i++) {
        printf("%012" PRIx64 , keyarray[i]);
        printf("\n");
    }
    */
    // TODO: Prepend key to user dictionary file
    napi_mf_classic_nonce_array_free(nonce_arr);
    if(system_dict_exists) {
        napi_mf_classic_dict_free(system_dict);
    }
    if(user_dict_exists) {
        napi_mf_classic_dict_free(user_dict);
    }
    furi_record_close(RECORD_STORAGE);
    FURI_LOG_I(TAG, "mfkey32 function completed"); // DEBUG
    return;
}

// Screen is 128x64 px
static void render_callback(Canvas* const canvas, void* ctx) {
    UNUSED(ctx);
    const ProgramState* program_state = acquire_mutex((ValueMutex*)ctx, 25);
    if(program_state == NULL) {
        return;
    }
    char draw_str[32] = {};
    float dict_progress = (float)program_state->cracked / (float)program_state->total;
    canvas_clear(canvas);
    canvas_draw_frame(canvas, 0, 0, 128, 64);
    canvas_draw_frame(canvas, 0, 15, 128, 64);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 5, 4, AlignLeft, AlignTop, "Mfkey32");
    canvas_draw_icon(canvas, 114, 4, &I_mfkey);
    if(program_state->running) {
        elements_progress_bar_with_text(canvas, 5, 18, 118, dict_progress, draw_str);
        canvas_set_font(canvas, FontSecondary);
        snprintf(
            draw_str,
            sizeof(draw_str),
            "Keys found: %d/%d",
            program_state->cracked,
            program_state->total);
        canvas_draw_str_aligned(canvas, 26, 31, AlignLeft, AlignTop, draw_str);
        snprintf(draw_str, sizeof(draw_str), "Keys in dict: %d", program_state->dict_count);
        canvas_draw_str_aligned(canvas, 26, 41, AlignLeft, AlignTop, draw_str);
        elements_button_center(canvas, "Stop");
    } else {
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(canvas, 50, 30, AlignLeft, AlignTop, "Ready");
        elements_button_center(canvas, "Start");
    }
    release_mutex((ValueMutex*)ctx, program_state);
}

static void input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);

    PluginEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void mfkey32_state_init(ProgramState* const program_state) {
    program_state->running = false;
    program_state->cracked = 0;
    program_state->total = 10; // Simulated
    program_state->dict_count = 0;
}

int32_t mfkey32_main() {
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(PluginEvent));

    ProgramState* program_state = malloc(sizeof(ProgramState));

    mfkey32_state_init(program_state);

    ValueMutex state_mutex;
    if(!init_mutex(&state_mutex, program_state, sizeof(ProgramState))) {
        FURI_LOG_E(TAG, "cannot create mutex\r\n");
        free(program_state);
        return 255;
    }

    // Set system callbacks
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, &state_mutex);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    // Open GUI and register view_port
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    PluginEvent event;
    for(bool main_loop = true; main_loop;) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 100);

        ProgramState* program_state = (ProgramState*)acquire_mutex_block(&state_mutex);

        if(event_status == FuriStatusOk) {
            // press events
            if(event.type == EventTypeKey) {
                if(event.input.type == InputTypePress) {
                    switch(event.input.key) {
                    case InputKeyUp:
                        break;
                    case InputKeyDown:
                        break;
                    case InputKeyRight:
                        break;
                    case InputKeyLeft:
                        break;
                    case InputKeyOk:
                        program_state->running = !program_state->running;
                        if(program_state->running) {
                            mfkey32(program_state);
                        }
                        break;
                    default:
                        main_loop = false;
                        break;
                    }
                }
            }
        } else {
            FURI_LOG_D(TAG, "FuriMessageQueue: event timeout");
            // event timeout
        }

        view_port_update(view_port);
        release_mutex(&state_mutex, program_state);
    }

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close("gui");
    view_port_free(view_port);
    furi_message_queue_free(event_queue);
    delete_mutex(&state_mutex);

    return 0;
}
