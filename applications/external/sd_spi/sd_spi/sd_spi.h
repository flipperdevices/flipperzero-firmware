#pragma once
#include <stdint.h>
#include <stdbool.h>

#define __IO volatile

#define SD_TIMEOUT_MS 500//(1000)
#define SD_BLOCK_SIZE 512

#define VERSION_APP "0.2"
#define DEVELOPED " "
#define GITHUB "github.com/Gl1tchub/Flipperzero-SD-SPI"


typedef enum {
    SdSpiStatusOK,
    SdSpiStatusError,
    SdSpiStatusTimeout,
} SdSpiStatus;

/** R1 answer value */
typedef enum {
    SdSpi_R1_NO_ERROR = 0x00,
    SdSpi_R1_IN_IDLE_STATE = 0x01,
    SdSpi_R1_ERASE_RESET = 0x02,
    SdSpi_R1_ILLEGAL_COMMAND = 0x04,
    SdSpi_R1_COM_CRC_ERROR = 0x08,
    SdSpi_R1_ERASE_SEQUENCE_ERROR = 0x10,
    SdSpi_R1_ADDRESS_ERROR = 0x20,
    SdSpi_R1_PARAMETER_ERROR = 0x40,
} SdSpiR1;

/** R2 answer value */
typedef enum {
    /* R2 answer value */
    SdSpi_R2_NO_ERROR = 0x00,
    SdSpi_R2_CARD_LOCKED = 0x01,
    SdSpi_R2_LOCKUNLOCK_ERROR = 0x02,
    SdSpi_R2_ERROR = 0x04,
    SdSpi_R2_CC_ERROR = 0x08,
    SdSpi_R2_CARD_ECC_FAILED = 0x10,
    SdSpi_R2_WP_VIOLATION = 0x20,
    SdSpi_R2_ERASE_PARAM = 0x40,
    SdSpi_R2_OUTOFRANGE = 0x80,
} SdSpiR2;

/**
 * @brief Card Specific Data: CSD Register
 */
typedef struct {
    /* Header part */
    uint8_t CSDStruct : 2; /* CSD structure */
    uint8_t Reserved1 : 6; /* Reserved */
    uint8_t TAAC : 8; /* Data read access-time 1 */
    uint8_t NSAC : 8; /* Data read access-time 2 in CLK cycles */
    uint8_t MaxBusClkFrec : 8; /* Max. bus clock frequency */
    uint16_t CardComdClasses : 12; /* Card command classes */
    uint8_t RdBlockLen : 4; /* Max. read data block length */
    uint8_t PartBlockRead : 1; /* Partial blocks for read allowed */
    uint8_t WrBlockMisalign : 1; /* Write block misalignment */
    uint8_t RdBlockMisalign : 1; /* Read block misalignment */
    uint8_t DSRImpl : 1; /* DSR implemented */

    /* v1 or v2 struct */
    union csd_version {
        struct {
            uint8_t Reserved1 : 2; /* Reserved */
            uint16_t DeviceSize : 12; /* Device Size */
            uint8_t MaxRdCurrentVDDMin : 3; /* Max. read current @ VDD min */
            uint8_t MaxRdCurrentVDDMax : 3; /* Max. read current @ VDD max */
            uint8_t MaxWrCurrentVDDMin : 3; /* Max. write current @ VDD min */
            uint8_t MaxWrCurrentVDDMax : 3; /* Max. write current @ VDD max */
            uint8_t DeviceSizeMul : 3; /* Device size multiplier */
        } v1;
        struct {
            uint8_t Reserved1 : 6; /* Reserved */
            uint32_t DeviceSize : 22; /* Device Size */
            uint8_t Reserved2 : 1; /* Reserved */
        } v2;
    } version;

    uint8_t EraseSingleBlockEnable : 1; /* Erase single block enable */
    uint8_t EraseSectorSize : 7; /* Erase group size multiplier */
    uint8_t WrProtectGrSize : 7; /* Write protect group size */
    uint8_t WrProtectGrEnable : 1; /* Write protect group enable */
    uint8_t Reserved2 : 2; /* Reserved */
    uint8_t WrSpeedFact : 3; /* Write speed factor */
    uint8_t MaxWrBlockLen : 4; /* Max. write data block length */
    uint8_t WriteBlockPartial : 1; /* Partial blocks for write allowed */
    uint8_t Reserved3 : 5; /* Reserved */
    uint8_t FileFormatGrouop : 1; /* File format group */
    uint8_t CopyFlag : 1; /* Copy flag (OTP) */
    uint8_t PermWrProtect : 1; /* Permanent write protection */
    uint8_t TempWrProtect : 1; /* Temporary write protection */
    uint8_t FileFormat : 2; /* File Format */
    uint8_t Reserved4 : 2; /* Reserved */
    uint8_t crc : 7; /* Reserved */
    uint8_t Reserved5 : 1; /* always 1*/

} SD_CSD;

/**
 * @brief Card Identification Data: CID Register
 */
typedef struct {
    uint8_t ManufacturerID; /* ManufacturerID */
    char OEM_AppliID[2]; /* OEM/Application ID */
    char ProdName[5]; /* Product Name */
    uint8_t ProdRev; /* Product Revision */
    uint32_t ProdSN; /* Product Serial Number */
    uint8_t Reserved1; /* Reserved1 */
    uint8_t ManufactYear; /* Manufacturing Year */
    uint8_t ManufactMonth; /* Manufacturing Month */
    uint8_t CID_CRC; /* CID CRC */
    uint8_t Reserved2; /* always 1 */
} SD_CID;

/**
 * @brief SD Card information structure
 */
 typedef struct {
     SD_CSD Csd;
     SD_CID Cid;
     uint64_t CardCapacity; /*!< Card Capacity */
     uint32_t CardBlockSize; /*!< Card Block Size */
     uint32_t LogBlockNbr; /*!< Specifies the Card logical Capacity in blocks   */
     uint32_t LogBlockSize; /*!< Specifies logical block size in bytes           */
 } SD_CardInfo;

 typedef struct {
     uint8_t r1;
     uint8_t r2;
     uint8_t r3;
     uint8_t r4;
     uint8_t r5;
 } SdSpiCmdAnswer;

 extern SdSpiCmdAnswer cmd_answer;


/**
 * @brief SD card max mount retry count
 *
 * @return uint8_t
 */
uint8_t sd_max_mount_retry_count();

/**
 * @brief Init sd card
 *
 * @param power_reset reset card power
 * @return SdSpiStatus
 */
SdSpiStatus sd_init(bool power_reset);

/**
 * @brief Get card state
 *
 * @return SdSpiStatus
 */
SdSpiStatus sd_get_card_state(void);

/**
 * @brief Get card info
 *
 * @param card_info
 * @return SdSpiStatus
 */
SdSpiStatus sd_get_card_info(SD_CardInfo* card_info);

/**
 * @brief Read blocks
 *
 * @param data
 * @param address
 * @param blocks
 * @param timeout_ms
 * @return SdSpiStatus
 */
SdSpiStatus sd_read_blocks(uint32_t* data, uint32_t address, uint32_t blocks, uint32_t timeout_ms);

/**
 * @brief Write blocks
 *
 * @param data
 * @param address
 * @param blocks
 * @param timeout_ms
 * @return SdSpiStatus
 */
SdSpiStatus
    sd_write_blocks(uint32_t* data, uint32_t address, uint32_t blocks, uint32_t timeout_ms);

/**
 * @brief Get card CSD register
 *
 * @param Cid
 * @return SdSpiStatus
 */
SdSpiStatus sd_get_cid(SD_CID* cid);

SdSpiStatus sd_set_pwd(char* pwd);
SdSpiStatus sd_clr_pwd(char* pwd);
SdSpiStatus sd_force_erase(void);
