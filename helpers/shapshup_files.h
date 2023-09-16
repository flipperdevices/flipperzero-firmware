#pragma once
#include <furi.h>
#include <flipper_format_i.h>
#include <flipper_format_stream_i.h>
#include <subghz/types.h>
#include <toolbox/path.h>
#include <subghz/types.h>
#include "m-array.h"

typedef enum {
    ShapShupFileResultOk,
    ShapShupFileResultOpenError,
    ShapShupFileResultIncorrectHeader,
    ShapShupFileResultTypeOfVersionMismatch,
    ShapShupFileResultNotRawFile,
    ShapShupFileResultMissingFrequency,
    ShapShupFileResultMissingPreset,
    ShapShupFileResultMissingProtocol,
    ShapShupFileResultKeyNotFound,
    ShapShupFileResultUnknown,
    ShapShupFileResultTotal
} ShapShupFileResults;

ARRAY_DEF(array_raw, int32_t)

typedef struct {
    ShapShupFileResults result;
    uint32_t frequency;
    uint64_t total_len;
    uint64_t total_count;
    int32_t min_value;
    int32_t max_value;
    uint64_t min_len;
    uint64_t max_len;
    array_raw_t values;
} ShapShupRawFile;

/**
 * @brief load_file_shapshup
 * 
 * @param file_path 
 * @return ShapShupRawFile* 
 */
ShapShupRawFile* load_file_shapshup(const char* file_path);

/**
 * @brief read_int32_shapshup
 * 
 * @param stream 
 * @param _data 
 * @param data_size 
 * @return true 
 * @return false 
 */
bool read_int32_shapshup(Stream* stream, int32_t* _data, uint16_t data_size);

/**
 * @brief shapshup_files_result_description
 * 
 * @param index 
 * @return const char* 
 */
const char* shapshup_files_result_description(ShapShupFileResults index);

/**
 * @brief clean_raw_values
 * 
 * @param raw_file 
 */
void clean_raw_values(ShapShupRawFile* raw_file);
