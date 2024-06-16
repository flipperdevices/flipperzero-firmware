/**
 * @file compress.h
 * LZSS based compression HAL API
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Compress Icon control structure */
typedef struct CompressIcon CompressIcon;

/** Initialize icon compressor
 *
 * @param[in]  decode_buf_size  The icon buffer size for decoding. Ensure that
 *                              it's big enough for any icons that you are
 *                              planning to decode with it.
 *
 * @return     Compress Icon instance
 */
CompressIcon* compress_icon_alloc(size_t decode_buf_size);

/** Free icon compressor
 *
 * @param      instance  The Compress Icon instance
 */
void compress_icon_free(CompressIcon* instance);

/** Decompress icon
 *
 * @warning    output pointer set by this function is valid till next
 *             `compress_icon_decode` or `compress_icon_free` call
 *
 * @param      instance   The Compress Icon instance
 * @param      icon_data  pointer to icon data.
 * @param[in]  output     pointer to decoded buffer pointer. Data in buffer is
 *                        valid till next call. If icon data was not compressed,
 *                        pointer within icon_data is returned
 */
void compress_icon_decode(CompressIcon* instance, const uint8_t* icon_data, uint8_t** output);

/** Compress control structure */
typedef struct Compress Compress;

typedef enum {
    COMPRESS_TYPE_HEATSHRINK = 0,
} CompressType;

typedef struct {
    uint16_t window_sz2;
    uint16_t lookahead_sz2;
    uint16_t input_buffer_sz;
} CompressConfigHeatshrink;

extern const CompressConfigHeatshrink* CompressConfigHeatshrinkDefault;

/** Allocate encoder and decoder
 *
 * @param      compress_buff_size  size of decoder and encoder buffer to
 *                                 allocate
 *
 * @return     Compress instance
 */
Compress* compress_alloc(CompressType type, const void* config);

/** Free encoder and decoder
 *
 * @param      compress  Compress instance
 */
void compress_free(Compress* compress);

/** Encode data
 *
 * @param      compress       Compress instance
 * @param      data_in        pointer to input data
 * @param      data_in_size   size of input data
 * @param      data_out       maximum size of output data
 * @param[in]  data_out_size  The data out size
 * @param      data_res_size  pointer to result output data size
 *
 * @return     true on success
 */
bool compress_encode(
    Compress* compress,
    uint8_t* data_in,
    size_t data_in_size,
    uint8_t* data_out,
    size_t data_out_size,
    size_t* data_res_size);

/** Decode data
 *
 * @param      compress       Compress instance
 * @param      data_in        pointer to input data
 * @param      data_in_size   size of input data
 * @param      data_out       maximum size of output data
 * @param[in]  data_out_size  The data out size
 * @param      data_res_size  pointer to result output data size
 *
 * @return     true on success
 */
bool compress_decode(
    Compress* compress,
    uint8_t* data_in,
    size_t data_in_size,
    uint8_t* data_out,
    size_t data_out_size,
    size_t* data_res_size);

typedef size_t (*compress_io_cb_t)(uint8_t* buffer, size_t size, void* context);

bool compress_decode_stream(
    Compress* compress,
    compress_io_cb_t read_cb,
    compress_io_cb_t write_cb,
    void* context);

#ifdef __cplusplus
}
#endif
