#include "lfrfid_dict_file.h"
#include <storage/storage.h>
#include <flipper_format/flipper_format.h>

#define LFRFID_DICT_FILETYPE "Flipper RFID key"

bool lfrfid_dict_file_save(ProtocolDict* dict, ProtocolId protocol, const char* filename) {
    furi_check(protocol != PROTOCOL_NO);
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* file = flipper_format_file_alloc(storage);
    size_t data_size = protocol_dict_get_data_size(dict, protocol);
    uint8_t* data = malloc(data_size);
    bool result = false;

    do {
        if(!flipper_format_file_open_always(file, filename)) break;
        if(!flipper_format_write_header_cstr(file, LFRFID_DICT_FILETYPE, 1)) break;

        // TODO: write comment about protocol types into file

        if(!flipper_format_write_string_cstr(
               file, "Key type", protocol_dict_get_name(dict, protocol)))
            break;

        // TODO: write comment about protocol sizes into file

        protocol_dict_get_data(dict, protocol, data, data_size);

        if(!flipper_format_write_hex(file, "Data", data, data_size)) break;
        result = true;
    } while(0);

    flipper_format_free(file);
    furi_record_close(RECORD_STORAGE);
    free(data);

    return result;
}

ProtocolId lfrfid_dict_file_load(ProtocolDict* dict, const char* filename) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* file = flipper_format_file_alloc(storage);
    ProtocolId result = PROTOCOL_NO;
    uint8_t* data = malloc(protocol_dict_get_max_data_size(dict));
    string_t str_result;
    string_init(str_result);

    do {
        if(!flipper_format_file_open_existing(file, filename)) break;

        // header
        uint32_t version;
        if(!flipper_format_read_header(file, str_result, &version)) break;
        if(string_cmp_str(str_result, LFRFID_DICT_FILETYPE) != 0) break;
        if(version != 1) break;

        // type
        if(!flipper_format_read_string(file, "Key type", str_result)) break;
        ProtocolId protocol;
        protocol = protocol_dict_get_protocol_by_name(dict, string_get_cstr(str_result));

        // TODO: protocols fallback
        if(protocol == PROTOCOL_NO) break;

        // data
        size_t data_size = protocol_dict_get_data_size(dict, protocol);
        if(!flipper_format_read_hex(file, "Data", data, data_size)) break;
        protocol_dict_set_data(dict, protocol, data, data_size);

        result = protocol;
    } while(false);

    free(data);
    string_clear(str_result);
    flipper_format_free(file);
    furi_record_close(RECORD_STORAGE);

    return result;
}