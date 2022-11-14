#pragma once

#include <furi.h>
#include "spi_mem_chip.h"

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
    SPIMemChipVendorAPMemory = 0x0D
} SPIMemChipVendor;

typedef enum {
    SPIMemChipWriteModeUnknown = 0,
    SPIMemChipWriteModePage256Bytes = 0x01,
    SPIMemChipWriteModeByte = 0x02,
    SPIMemChipWriteModeAAI = 0x04,
    SPIMemChipWriteModeDualBuffer = 0x08
} SPIMemChipWriteMode;

typedef enum {
    SPIMemChipCMDReadJEDECChipID = 0x9F,
    SPIMemChipCMDReadData = 0x03,
    SPIMemChipCMDChipErase = 0xC7,
    SPIMemChipCMDWriteEnable = 0x06,
    SPIMemChipCMDWriteDisable = 0x04,
    SPIMemChipCMDReadStatus = 0x05
} SPIMemChipCMD;

typedef enum {
    SPIMemChipStatusBitBusy = (1 << 0),
    SPIMemChipStatusBitWriteEnabled = (1 << 1),
    SPIMemChipStatusBitSoftwareProtection1 = (1 << 2),
    SPIMemChipStatusBitSoftwareProtection2 = (1 << 3),
    SPIMemChipStatusBitWriteProtect = (1 << 4),
    SPIMemChipStatusBitEraseError = (1 << 5),
    SPIMemChipStatusBitReserved = (1 << 6),
    SPIMemChipStatusBitRegisterProtect = (1 << 7)
} SPIMemChipStatusBit;

typedef struct {
    const char* vendor_name;
    SPIMemChipVendor vendor_id;
} SPIMemChipVendorName;

typedef enum {
    SPIMemChipAddressTypeUnknown,
    SPIMemChipAddressType3byte,
    SPIMemChipAddressType4byte,
    SPIMemChipAddressTypeAll
} SPIMemChipAddressType;

struct SPIMemChip {
    SPIMemChipVendor vendor_id;
    const char* model_name;
    const char* vendor_name;
    size_t size;
    SPIMemChipWriteMode write_mode;
    uint8_t type_id;
    uint8_t capacity_id;
    SPIMemChipAddressType address_type;
};
