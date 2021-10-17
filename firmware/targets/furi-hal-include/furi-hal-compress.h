/**
 * @file furi-hal-compress.h
 * LZSS based compression HAL API
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define FURI_HAL_COMPRESS_EXP_BUFF_SIZE_LOG (8)
#define FURI_HAL_COMPRESS_LOOKAHEAD_BUFF_SIZE_LOG (4)

typedef struct FuriHalCompress FuriHalCompress;

void furi_hal_compress_init();

void furi_hal_compress_icon_decode(const uint8_t* icon_data, uint8_t** decoded_buff);

FuriHalCompress* furi_hal_compress_alloc(uint16_t copress_buff_size);

void furi_hal_compress_free(FuriHalCompress* compress);

bool furi_hal_compress_encode(FuriHalCompress* compress, uint8_t* data_in, size_t data_in_size, uint8_t* data_out, size_t data_out_size, size_t* data_res_size);

bool furi_hal_compress_decode(FuriHalCompress* compress, uint8_t* data_in, size_t data_in_size, uint8_t* data_out, size_t data_out_size, size_t* data_res_size);
