#pragma once

#include <furi.h>

typedef enum {
    SPIMemChipVendorUnknown = 0,
    SPIMemChipVendorCypress = 0x01,
    SPIMemChipVendorFujitsu = 0x04,
    SPIMemChipVendorEon = 0x1C,
    SPIMemChipVendorAtmel = 0x1F,
    SPIMemChipVendorMicron = 0x20,
    SPIMemChipVendorAmic = 0x37,
    SPIMemChipVendorNormem = 0x52,
    SPIMemChipVendorSanyo = 0x62,
    SPIMemChipVendorIntel = 0x89,
    SPIMemChipVendorEsmt = 0x8C,
    SPIMemChipVendorFudan = 0xA1,
    SPIMemChipVendorHyundai = 0xAD,
    SPIMemChipVendorSst = 0xBF,
    SPIMemChipVendorMicronix = 0xC2,
    SPIMemChipVendorGigadevice = 0xC8,
    SPIMemChipVendorIssi = 0xD5,
    SPIMemChipVendorWinbond = 0xEF,
    SPIMemChipVendorBoya = 0x68,
} SPIMemChipVendor;

typedef enum {
    SPIMemChipWriteModeUnknown = 0,
    SPIMemChipWriteModePage256Bytes = 0x01,
    SPIMemChipWriteModeByte = 0x02,
    SPIMemChipWriteModeAAI = 0x04,
    SPIMemChipWriteModeDualBuffer = 0x08
} SPIMemChipWriteMode;

typedef struct {
    const char* vendor_name;
    SPIMemChipVendor vendor_id;
} SPIMemChipVendorName;

typedef struct {
    SPIMemChipVendor vendor_id;
    const char* model_name;
    const char* vendor_name;
    size_t size;
    SPIMemChipWriteMode write_mode;
    uint8_t type_id;
    uint8_t capacity_id;
    size_t erase_gran;
    uint8_t erase_gran_cmd;
} SPIMemChip;

bool spi_mem_get_chip_info(SPIMemChip* chip);
