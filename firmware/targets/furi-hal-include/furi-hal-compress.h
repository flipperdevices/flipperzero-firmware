/**
 * @file furi-hal-compress.h
 * LZSS based compression HAL API
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <lib/heatshrink/heatshrink_decoder.h>

#define FURI_HAL_COMPRESS_ICON_BUFF_SIZE (1024)

typedef struct {
    heatshrink_decoder decoder;
    uint8_t decoded_buff[FURI_HAL_COMPRESS_ICON_BUFF_SIZE];
    uint16_t decoded_buff_size;
} FuriHalCompressIcon;

void furi_hal_compress_init();

void furi_hal_compress_encode();

void furi_hal_compress_icon_decode(const uint8_t* icon_data, uint8_t** decoded_buff);
