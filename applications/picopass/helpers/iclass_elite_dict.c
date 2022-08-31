#include "iclass_elite_dict.h"

#include <lib/toolbox/args.h>
#include <lib/flipper_format/flipper_format.h>

#define ICLASS_ELITE_DICT_FLIPPER_PATH EXT_PATH("picopass/assets/iclass_elite_dict.txt")
#define ICLASS_ELITE_DICT_USER_PATH EXT_PATH("picopass/assets/iclass_elite_dict_user.txt")

#define TAG "IclassEliteDict"

#define ICLASS_ELITE_KEY_LINE_LEN (17)
#define ICLASS_ELITE_KEY_LEN (8)

struct IclassEliteDict {
    Stream* stream;
    uint32_t total_keys;
};

bool iclass_elite_dict_check_presence(IclassEliteDictType dict_type) {
    Storage* storage = furi_record_open(RECORD_STORAGE);

    bool dict_present = false;
    if(dict_type == IclassEliteDictTypeFlipper) {
        dict_present = storage_common_stat(storage, ICLASS_ELITE_DICT_FLIPPER_PATH, NULL) ==
                       FSE_OK;
    } else if(dict_type == IclassEliteDictTypeUser) {
        dict_present = storage_common_stat(storage, ICLASS_ELITE_DICT_USER_PATH, NULL) == FSE_OK;
    }

    furi_record_close(RECORD_STORAGE);

    return dict_present;
}

IclassEliteDict* iclass_elite_dict_alloc(IclassEliteDictType dict_type) {
    IclassEliteDict* dict = malloc(sizeof(IclassEliteDict));
    Storage* storage = furi_record_open(RECORD_STORAGE);
    dict->stream = buffered_file_stream_alloc(storage);
    furi_record_close(RECORD_STORAGE);

    bool dict_loaded = false;
    do {
        if(dict_type == IclassEliteDictTypeFlipper) {
            if(!buffered_file_stream_open(
                   dict->stream, ICLASS_ELITE_DICT_FLIPPER_PATH, FSAM_READ, FSOM_OPEN_EXISTING)) {
                buffered_file_stream_close(dict->stream);
                break;
            }
        } else if(dict_type == IclassEliteDictTypeUser) {
            if(!buffered_file_stream_open(
                   dict->stream, ICLASS_ELITE_DICT_USER_PATH, FSAM_READ_WRITE, FSOM_OPEN_ALWAYS)) {
                buffered_file_stream_close(dict->stream);
                break;
            }
        }

        // Read total amount of keys
        string_t next_line;
        string_init(next_line);
        while(true) {
            if(!stream_read_line(dict->stream, next_line)) break;
            if(string_get_char(next_line, 0) == '#') continue;
            if(string_size(next_line) != ICLASS_ELITE_KEY_LINE_LEN) continue;
            dict->total_keys++;
        }
        string_clear(next_line);
        stream_rewind(dict->stream);

        dict_loaded = true;
        FURI_LOG_I(TAG, "Loaded dictionary with %d keys", dict->total_keys);
    } while(false);

    if(!dict_loaded) {
        buffered_file_stream_close(dict->stream);
        free(dict);
        dict = NULL;
    }

    return dict;
}

void iclass_elite_dict_free(IclassEliteDict* dict) {
    furi_assert(dict);
    furi_assert(dict->stream);

    buffered_file_stream_close(dict->stream);
    stream_free(dict->stream);
    free(dict);
}

uint32_t iclass_elite_dict_get_total_keys(IclassEliteDict* dict) {
    furi_assert(dict);

    return dict->total_keys;
}

bool iclass_elite_dict_get_next_key(IclassEliteDict* dict, uint8_t* key) {
    furi_assert(dict);
    furi_assert(dict->stream);

    uint8_t key_byte_tmp = 0;
    string_t next_line;
    string_init(next_line);

    bool key_read = false;
    *key = 0ULL;
    while(!key_read) {
        if(!stream_read_line(dict->stream, next_line)) break;
        if(string_get_char(next_line, 0) == '#') continue;
        if(string_size(next_line) != ICLASS_ELITE_KEY_LINE_LEN) continue;
        for(uint8_t i = 0; i < ICLASS_ELITE_KEY_LEN * 2; i += 2) {
            args_char_to_hex(
                string_get_char(next_line, i), string_get_char(next_line, i + 1), &key_byte_tmp);
            key[i / 2] = key_byte_tmp;
        }
        key_read = true;
    }

    string_clear(next_line);
    return key_read;
}

bool iclass_elite_dict_rewind(IclassEliteDict* dict) {
    furi_assert(dict);
    furi_assert(dict->stream);

    return stream_rewind(dict->stream);
}

bool iclass_elite_dict_add_key(IclassEliteDict* dict, uint8_t* key) {
    furi_assert(dict);
    furi_assert(dict->stream);

    string_t key_str;
    string_init(key_str);
    for(size_t i = 0; i < 6; i++) {
        string_cat_printf(key_str, "%02X", key[i]);
    }
    string_cat_printf(key_str, "\n");

    bool key_added = false;
    do {
        if(!stream_seek(dict->stream, 0, StreamOffsetFromEnd)) break;
        if(!stream_insert_string(dict->stream, key_str)) break;
        key_added = true;
    } while(false);

    string_clear(key_str);
    return key_added;
}
