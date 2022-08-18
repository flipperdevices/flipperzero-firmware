#pragma once
#include <furi.h>
#include <storage/storage.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct LFRFIDRawFile LFRFIDRawFile;

LFRFIDRawFile* lfrfid_raw_file_alloc(Storage* storage);

void lfrfid_raw_file_free(LFRFIDRawFile* file);

bool lfrfid_raw_file_open_write(LFRFIDRawFile* file, const char* file_path);

bool lfrfid_raw_file_open_read(LFRFIDRawFile* file, const char* file_path);

bool lfrfid_raw_file_write_header(
    LFRFIDRawFile* file,
    float frequency,
    float duty_cycle,
    uint32_t max_buffer_size);

bool lfrfid_raw_file_write_buffer(LFRFIDRawFile* file, uint8_t* buffer_data, size_t buffer_size);

bool lfrfid_raw_file_read_header(LFRFIDRawFile* file, float* frequency, float* duty_cycle);

bool lfrfid_raw_file_read_pair(
    LFRFIDRawFile* file,
    uint32_t* duration,
    uint32_t* pulse,
    bool* pass_end);

#ifdef __cplusplus
}
#endif
