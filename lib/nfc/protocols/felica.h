#pragma once

#include <furi_hal_nfc.h>

#define NFCF_F_SIG (13560000.0)
#define MRT_T_SIG 302064.89 //ns, 256 * 16 / NFC_F_SIG * 1e9
#define MRT_T_SIG_x4 1208259.56 //ns, MRT_T_SIG * (4 ** 1)
#define MRT_T_SIG_x16 4833038.24 //ns, MRT_T_SIG * (4 ** 2)
#define MRT_T_SIG_x64 19332152.96 //ns, MRT_T_SIG * (4 ** 2)

#define FELICA_BLOCK_SIZE 16

#define SUICA_SYSTEM_CODE 0x0003
#define NDEF_SYSTEM_CODE 0x12fc
#define HCE_F_SYSTEM_CODE 0x4000
#define OCTOPUS_SYSTEM_CODE 0x8008
#define EDY_SYSTEM_CODE 0x811d
#define PASPY_SYSTEM_CODE 0x8592
#define BLACKBOARD_SYSTEM_CODE 0x8620
#define SAPICA_SYSTEM_CODE 0x865e
#define LITE_SYSTEM_CODE 0x88b4
#define RYUTO_SYSTEM_CODE 0x8b5d
#define OKICA_SYSTEM_CODE 0x8fc1
#define SECURE_ID_SYSTEM_CODE 0x957a
#define IRUCA_SYSTEM_CODE 0xde80
#define COMMON_AREA_SYSTEM_CODE 0xfe00
#define PLUG_SYSTEM_CODE 0xfee1

#define REG_LITE_BLOCK 0x0e
#define RC_LITE_BLOCK 0x80
#define MAC_LITE_BLOCK 0x81
#define ID_LITE_BLOCK 0x82
#define DEVICE_ID_LITE_BLOCK 0x83
#define SERVICE_CODE_LITE_BLOCK 0x84
#define SYS_CODE_LITE_BLOCK 0x85
#define CARD_KEY_VER_LITE_BLOCK 0x86
#define CARD_KEY_LITE_BLOCK 0x87
#define MEM_CONFIG_LITE_BLOCK 0x88
#define WRITE_COUNT_LITE_BLOCK 0x90
#define MAC_A_LITE_BLOCK 0x91
#define STATE_LITE_BLOCK 0x92
#define CRC_CHECK_LITE_BLOCK 0xA0

#define RANDOM_TYPE_SERVICE_ATTRIBUTE (0b0010 << 2)
#define CYCLIC_TYPE_SERVICE_ATTRIBUTE (0b0011 << 2)
#define PURSE_TYPE_SERVICE_ATTRIBUTE  (0b010  << 3)

#define AUTH_RW_SERVICE_ATTRIBUTE (0b00)
#define UNAUTH_RW_SERVICE_ATTRIBUTE (0b01)
#define AUTH_RO_SERVICE_ATTRIBUTE (0b10)
#define UNAUTH_RO_SERVICE_ATTRIBUTE (0b11)

#define AUTH_DIRECT_ACCESS_SERVICE_ATTRIBUTE (0b000)
#define UNAUTH_DIRECT_ACCESS_SERVICE_ATTRIBUTE  (0b001)
#define AUTH_CASHBACK_DECREMENT_SERVICE_ATTRIBUTE (0b010)
#define UNAUTH_CASHBACK_DECREMENT_SERVICE_ATTRIBUTE (0b011)
#define AUTH_DECREMENT_SERVICE_ATTRIBUTE (0b100)
#define UNAUTH_DECREMENT_SERVICE_ATTRIBUTE (0b101)
#define AUTH_RO_PURSE_SERVICE_ATTRIBUTE (0b110)
#define UNAUTH_RO_PURSE_SERVICE_ATTRIBUTE (0b111)

#define IS_2_BYTE_BLOCK_LIST_ELEMENT 0x80

#define FELICA_UNENCRYPTED_READ_CMD 0x06
#define FELICA_UNENCRYPTED_WRITE_CMD 0x08

#define FELICA_UNENCRYPTED_READ_RES 0x07
#define FELICA_UNENCRYPTED_WRITE_RES 0x09

typedef enum {
    FelicaICTypeRC_SA24_10K, // RC-SA24/1x
    FelicaICTypeRC_SA24_6K, // RC-SA24/1x1
    FelicaICTypeSD2_6K, // RC-SA21/2x1
    FelicaICTypeSD2_4K, // RC-SA21/2
    FelicaICTypeSD2WithDES, // RC-SA20/1, RC-SA20/2
    FelicaICTypeRC_SA08, // Certifications exist, prototype?
    FelicaICTypeSD1, // RC-SA01
    FelicaICTypeSD1WithDES, // RC-SA00
    FelicaICTypeFRAM_4K, // RC-S962
    FelicaICTypeFRAM_9K, // RC-S960
    FelicaICTypeEMV_36K, // RC-S954
    FelicaICTypeEMV_16K, // RC-S953
    FelicaICTypeEMV_32K, // RC-S952
    FelicaICType576B, // RC-S919
    FelicaICType4K, // RC-S915
    FelicaICType2K, // RC-S830 series cards, chip name unknown,
    FelicaICTypeMobileIC_V4_1,
    FelicaICTypeMobileIC_V4,
    FelicaICTypeMobileIC_V3,
    FelicaICTypeMobileIC_V2,
    FelicaICTypeMobileIC_V1,
    FelicaICTypeLite, // RC-S965
    FelicaICTypeLiteS, // RC-S966
    FelicaICTypeLink, // RC-S967,
    FelicaICTypePlug, // RC-S926
    FelicaICTypeSuica, // https://www.tuv-nederland.nl/assets/files/cerfiticaten/2019/07/cr-nscib-cc-10-30076-cr.pdf
} FelicaICType;

typedef struct {
    uint8_t exponent: 2;
    // Incremented at read
    uint8_t real_a: 4;
    uint8_t real_b: 4;
} FelicaMRTParts;

typedef enum {
    FelicaMRTCommandTypeVariable = 0,
    FelicaMRTCommandTypeFixed = 1,
    FelicaMRTCommandTypeMutualAuth = 2,
    FelicaMRTCommandTypeDataRead = 3,
    FelicaMRTCommandTypeDataWrite = 4,
    FelicaMRTCommandTypeDataOther = 4,
} FelicaMRTCommandType;

typedef FelicaMRTParts FelicaMRTParameters[6];

typedef struct {
    uint16_t number;
    uint16_t end_service_code;
} FelicaArea;

typedef enum {
    FelicaBlockTypeNormal,
    FelicaBlockTypeOverlap,
    FelicaBlockTypeExtended,
} FelicaBlockType;

typedef struct {
    FelicaBlockType type;
    union {
        uint8_t data[FELICA_BLOCK_SIZE];
    };
} FelicaBlock;

// typedef struct {} FelicaOverlapBlock;

typedef struct _FelicaService_t {
    uint16_t number;
    uint16_t block_count;
    FelicaBlock** blocks;

    struct _FelicaService_t* next;
} FelicaService;

typedef struct _FelicaSystem_t {
    uint8_t number;
    uint16_t code;
    uint8_t idm[8];
    uint8_t pmm[8];
    FelicaMRTParameters maximum_response_times;

    FelicaService* services;
    
    struct _FelicaSystem_t* next;
} FelicaSystem;

typedef struct {
    FelicaICType type;
    uint8_t subtype;
    uint8_t system_count;
    FelicaSystem* systems;
} FelicaData;

typedef struct {
    uint8_t current_idm[8];
    uint8_t current_pmm[8];

    uint8_t status_flags[2];
} FelicaReader;

bool felica_check_ic_type(uint8_t* PMm);
FelicaICType felica_get_ic_type(uint8_t* PMm);
bool felica_read_card(FuriHalNfcTxRxContext* tx_rx, FelicaData* data, uint8_t* polled_idm, uint8_t* polled_pmm);