#pragma once
#include <furi.h>
#include "m-array.h"
#include <toolbox/path.h>
#include <flipper_format_i.h>
#include <flipper_format_stream_i.h>

/**
 * @brief file operation result response
 * 
 */
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

/**
 * @brief Construct a new array def object
 * 
 */
ARRAY_DEF(array_raw, int32_t)

/**
 * @brief contains info about .sub file
 * 
 */
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
 * @param stream stream to read
 * @param _data return readed data
 * @param data_size size of data to read
 * @return true/false
 */
bool read_int32_shapshup(Stream* stream, int32_t* _data, uint16_t data_size);

/**
 * @brief get description about file operation
 * 
 * @param result_code struct @see ShapShupFileResults
 * @return human readable description about result code
 */
const char* shapshup_files_result_description(ShapShupFileResults result_code);

/**
 * @brief make clean of array to read new values
 * 
 * @param raw_file @see ShapShupRawFile
 */
void clean_raw_values(ShapShupRawFile* raw_file);
