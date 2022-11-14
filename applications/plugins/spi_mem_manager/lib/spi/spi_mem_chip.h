#pragma once

#include <furi.h>
typedef struct SPIMemChip SPIMemChip;

typedef enum {
    SPIMemChipStatusBusy,
    SPIMemChipStatusIdle,
    SPIMemChipStatusError
} SPIMemChipStatus;

bool spi_mem_chip_complete_info(SPIMemChip* chip_info);
void spi_mem_chip_print_chip_models(FuriString* str);
const char* spi_mem_chip_get_vendor_name(SPIMemChip* chip);
const char* spi_mem_chip_get_model_name(SPIMemChip* chip);
size_t spi_mem_chip_get_size(SPIMemChip* chip);
