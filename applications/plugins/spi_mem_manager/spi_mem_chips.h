#ifndef _SPI_MEM_CHIPS_H_
#define _SPI_MEM_CHIPS_H_

typedef enum {
    SPIMemChipCmdPageProgram = 0x02,
    SPIMemChipCmdReadData = 0x03,
    SPIMemChipCmdWriteDisable = 0x04,
    SPIMemChipCmdReadStatus = 0x05,
    SPIMemChipCmdWriteStatusReg1 = 0x01,
    SPIMemChipCmdWriteStatusReg2 = 0x31, // TODO: check compliancy
    SPIMemChipCmdWriteEnable = 0x06,
    SPIMemChipCmdVolatileSrWriteEnable = 0x50,
    SPIMemChipCmdReadSfdpRegister = 0x5A,
    SPIMemChipCmdJedecId = 0x9F,
    SPIMemChipCmdReleaseDeep = 0xAB,
    SPIMemChipCmdAaiWordProgram = 0xAD,
    SPIMemChipCmdEraseChip = 0xC7,
} SPIMemChipCmd;

enum {
    SPIMemChipStatusRegBusy = (1 << 0), // busy
    SPIMemChipStatusRegWel = (1 << 1), // write enable latch
    SPIMemChipStatusRegSwp0 = (1 << 2), // Software protection status
    SPIMemChipStatusRegSwp1 = (1 << 3), // Software protection status
    SPIMemChipStatusRegWpStatus = (1 << 4), // WRITE PROTECT Pin status
    SPIMemChipStatusRegEpe = (1 << 5), // Erase Program Error
    SPIMemChipStatusRegRes = (1 << 6), // Reserved
    SPIMemChipStatusRegSrp = (1 << 7), // status register protect
};

typedef enum {
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

typedef struct {
    const char* name;
    const uint8_t vendor_id;
} SPIMemChipVendorList;

typedef enum {
    SPIMemChipWriteMode256B = 0x01, // write 1 to 256 bytes per page
    SPIMemChipWriteModeByte = 0x02, // byte write
    SPIMemChipWriteModeAai = 0x04, // auto address increment
    SPIMemChipWriteModeDual = 0x08, // dual-buffer write, like AT45DB series
} SPIMemChipWriteMode;

typedef struct {
    const char* name; // flash chip name
    uint8_t vendor_id; // vendor ID
    uint8_t type_id; // memory type ID
    uint8_t capacity_id; // capacity ID
    uint32_t size; // flash capacity (bytes)
    SPIMemChipWriteMode write_mode; // write mode
    uint32_t erase_gran; // erase granularity (bytes)
    uint8_t erase_gran_cmd; // erase granularity size block command
} SPIMemChip;

extern const SPIMemChipVendorList SPIMemChipVendors[];
const SPIMemChip SPIMemChips[];

const char* spi_mem_chip_get_vendor_name(const uint8_t vendor_id);
const SPIMemChip* spi_mem_chip_get_chip(const uint8_t vendor_id, const uint8_t type_id, const uint8_t capacity_id);

#endif // _SPI_MEM_CHIPS_H_
