#pragma once

#include <furi.h>
typedef struct SPIMemChip SPIMemChip;

typedef enum {
    SPIMemChipStatusBusy,
    SPIMemChipStatusIdle,
    SPIMemChipStatusError
} SPIMemChipStatus;

typedef enum {
    SPIMemChipWriteModeUnknown = 0,
    SPIMemChipWriteModeOneOrPage256Bytes = (0x01 << 0),
    SPIMemChipWriteModeByte = (0x01 << 1),
    SPIMemChipWriteModeAAI = (0x01 << 2),
    SPIMemChipWriteModeDualBuffer = (0x01 << 3)
} SPIMemChipWriteMode;

bool spi_mem_chip_complete_info(SPIMemChip* chip_info);
void spi_mem_chip_print_chip_models(FuriString* str);
const char* spi_mem_chip_get_vendor_name(SPIMemChip* chip);
const char* spi_mem_chip_get_model_name(SPIMemChip* chip);
size_t spi_mem_chip_get_size(SPIMemChip* chip);
uint8_t spi_mem_chip_get_vendor_id(SPIMemChip* chip);
uint8_t spi_mem_chip_get_type_id(SPIMemChip* chip);
uint8_t spi_mem_chip_get_capacity_id(SPIMemChip* chip);
SPIMemChipWriteMode spi_mem_chip_get_write_mode(SPIMemChip* chip);
