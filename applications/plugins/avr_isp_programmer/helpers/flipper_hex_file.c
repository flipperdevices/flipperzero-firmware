#include "flipper_hex_file.h"
#include <string.h>
#include <flipper_format/flipper_format.h>
#include <flipper_format/flipper_format_i.h>
#include <toolbox/stream/stream.h>

//https://ru.wikipedia.org/wiki/Intel_HEX

#define COUNT_BYTE_PAYLOAD 32 //how much payload will be used

struct FlipperHexFile {
    uint32_t addr;
    Storage* storage;
    FlipperFormat* flipper_format;
    Stream* stream;
    FuriString* str_data;
};

FlipperHexFile* flipper_hex_file_open(const char* name, uint32_t start_addr) {
    furi_assert(name);

    FlipperHexFile* instance = malloc(sizeof(FlipperHexFile));
    instance->addr = start_addr;
    instance->storage = furi_record_open(RECORD_STORAGE);
    instance->flipper_format = flipper_format_file_alloc(instance->storage);
    flipper_format_file_open_always(instance->flipper_format, name);
    instance->stream = flipper_format_get_raw_stream(instance->flipper_format);

    instance->str_data = furi_string_alloc();

    return instance;
}

void flipper_hex_file_close(FlipperHexFile* instance) {
    furi_assert(instance);
    furi_string_free(instance->str_data);
    flipper_format_free(instance->flipper_format);
    furi_record_close(RECORD_STORAGE);
}

void flipper_hex_file_bin_to_hex_add_data(
    FlipperHexFile* instance,
    uint8_t* data,
    uint32_t data_size) {
    furi_assert(instance);

    uint32_t cur_addr = instance->addr;
    uint8_t count_byte = 0;
    uint32_t ind = 0;
    uint8_t crc = 0;

    furi_string_reset(instance->str_data);

    while(ind < data_size) {
        if((ind + COUNT_BYTE_PAYLOAD) > data_size) {
            count_byte = data_size - ind;
        } else {
            count_byte = COUNT_BYTE_PAYLOAD;
        }
        furi_string_cat_printf(
            instance->str_data, ":%02X%04lX00", count_byte, (instance->addr & 0xFFFF));
        crc = count_byte + ((instance->addr >> 8) & 0xFF) + (instance->addr & 0xFF);

        for(uint32_t i = 0; i < count_byte; i++) {
            furi_string_cat_printf(instance->str_data, "%02X", *data);
            crc += *data++;
        }
        crc = 0x01 + ~crc;
        furi_string_cat_printf(instance->str_data, "%02X\r\n", crc);

        ind += count_byte;
        instance->addr += count_byte;

        if((cur_addr & 0xFF0000) > (instance->addr & 0xFF0000)) {
            crc = 0x02 + 0x04 + ((instance->addr >> 24) & 0xFF) + ((instance->addr >> 16) & 0xFF);
            crc = 0x01 + ~crc;
            furi_string_cat_printf(
                instance->str_data, ":02000004%04lX\r\n", (instance->addr >> 16));
            cur_addr = instance->addr;
        }
    }
    stream_write_string(instance->stream, instance->str_data);
}

void flipper_hex_file_bin_to_hex_add_end_line(FlipperHexFile* instance) {
    furi_assert(instance);
    furi_string_reset(instance->str_data);
    furi_string_cat_printf(instance->str_data, ":00000001FF\r\n");
    stream_write_string(instance->stream, instance->str_data);
}

const char* flipper_hex_file_get_string(FlipperHexFile* instance) {
    furi_assert(instance);
    return furi_string_get_cstr(instance->str_data);
}