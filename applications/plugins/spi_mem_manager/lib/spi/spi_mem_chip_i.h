#pragma once

#include <furi.h>
#include "spi_mem_chip.h"

typedef enum {
    SPIMemChipVendorUnknown = 0,
    SPIMemChipVendorCypress = 0x01,
    SPIMemChipVendorFujitsu = 0x04,
    SPIMemChipVendorEon = 0x1C,
    SPIMemChipVendorAtmel = 0x1F,
    SPIMemChipVendorMicron1 = 0x20,
    SPIMemChipVendorMicron2 = 0x2C,
    SPIMemChipVendorAmic = 0x37,
    SPIMemChipVendorNormem = 0x52,
    SPIMemChipVendorSanyo = 0x62,
    SPIMemChipVendorIntel = 0x89,
    SPIMemChipVendorEsmt = 0x8C,
    SPIMemChipVendorFudan1 = 0xA1,
    SPIMemChipVendorFudan2 = 0xF8,
    SPIMemChipVendorHyundai = 0xAD,
    SPIMemChipVendorSst = 0xBF,
    SPIMemChipVendorMicronix = 0xC2,
    SPIMemChipVendorGigadevice1 = 0xC8,
    SPIMemChipVendorGigadevice2 = 0x51,
    SPIMemChipVendorIssi = 0xD5,
    SPIMemChipVendorWinbond = 0xEF,
    SPIMemChipVendorBoya = 0x68,
    SPIMemChipVendorAPMemory = 0x0D,
    SPIMemChipVendorPMCSierra1 = 0x9D,
    SPIMemChipVendorPMCSierra2 = 0x7F,
    SPIMemChipVendorExcelSemi = 0x4A,
    SPIMemChipVendorZbitSemi = 0x5E,
    SPIMemChipVendorBergMicro = 0xE0,
    SPIMemChipVendorATO = 0x9B,
    SPIMemChipVendorDOUQI = 0x54,
    SPIMemChipVendorFremont = 0x0E,
} SPIMemChipVendor;

typedef enum {
    SPIMemChipCMDReadJEDECChipID = 0x9F,
    SPIMemChipCMDReadData = 0x03,
    SPIMemChipCMDChipErase = 0xC7,
    SPIMemChipCMDWriteEnable = 0x06,
    SPIMemChipCMDWriteDisable = 0x04,
    SPIMemChipCMDReadStatus = 0x05,
    SPIMemChipCMDWriteData = 0x02
} SPIMemChipCMD;

enum SPIMemChipStatusBit {
    SPIMemChipStatusBitBusy = (0x01 << 0),
    SPIMemChipStatusBitWriteEnabled = (0x01 << 1),
    SPIMemChipStatusBitBitProtection1 = (0x01 << 2),
    SPIMemChipStatusBitBitProtection2 = (0x01 << 3),
    SPIMemChipStatusBitBitProtection3 = (0x01 << 4),
    SPIMemChipStatusBitTopBottomProtection = (0x01 << 5),
    SPIMemChipStatusBitSectorProtect = (0x01 << 6),
    SPIMemChipStatusBitRegisterProtect = (0x01 << 7)
};

typedef struct {
    const char* vendor_name;
    SPIMemChipVendor vendor_id;
} SPIMemChipVendorName;

struct SPIMemChip {
    SPIMemChipVendor vendor_id;
    const char* model_name;
    const char* vendor_name;
    size_t size;
    SPIMemChipWriteMode write_mode;
    uint8_t type_id;
    uint8_t capacity_id;
    size_t page_size;
};

extern const SPIMemChip SPIMemChips[];
