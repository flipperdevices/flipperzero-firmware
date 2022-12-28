#pragma once

#include <furi_hal_nfc.h>
#include <m-array.h>
#include <m-dict.h>

#define NFCF_F_SIG (13560000.0)
#define MRT_T_SIG 302064.89 //ns, 256 * 16 / NFC_F_SIG * 1e9
#define MRT_T_SIG_x4 1208259.56 //ns, MRT_T_SIG * (4 ** 1)
#define MRT_T_SIG_x16 4833038.24 //ns, MRT_T_SIG * (4 ** 2)
#define MRT_T_SIG_x64 19332152.96 //ns, MRT_T_SIG * (4 ** 2)

#define FELICA_VARIABLE_MRT 0
#define FELICA_FIXED_MRT 1
#define FELICA_MUTUAL_AUTH_MRT 2
#define FELICA_READ_MRT 3
#define FELICA_WRITE_MRT 4
#define FELICA_OTHER_MRT 5

#define FELICA_BLOCK_SIZE 16

#define CYBERNET_SYSTEM_CODE 0x0003
#define NDEF_SYSTEM_CODE 0x12fc
#define HCE_F_SYSTEM_CODE 0x4000
#define OCTOPUS_SYSTEM_CODE 0x8008
#define IRUCA_SYSTEM_CODE 0x80de
#define EDY_SYSTEM_CODE 0x811d
#define PASPY_SYSTEM_CODE 0x8592
#define BLACKBOARD_SYSTEM_CODE 0x8620
#define SAPICA_SYSTEM_CODE 0x865e
#define SUICA_SYSTEM_CODE 0x86a7
#define LITE_SYSTEM_CODE 0x88b4
#define RYUTO_SYSTEM_CODE 0x8b5d
#define OKICA_SYSTEM_CODE 0x8fc1
#define SECURE_ID_SYSTEM_CODE 0x957a
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
    uint8_t exponent : 2;
    // Incremented at read
    uint8_t real_a : 4;
    uint8_t real_b : 4;
} FelicaMRTParts;

typedef enum {
    FelicaServiceTypeRandom = (0b0010 << 2),
    FelicaServiceTypeCyclic = (0b0011 << 2),
    FelicaServiceTypePurse = (0b010 << 3),
} FelicaServiceType;
typedef enum {
    FelicaServiceAttributeAuthRW = 0b00,
    FelicaServiceAttributeUnauthRW = 0b01,
    FelicaServiceAttributeAuthRO = 0b10,
    FelicaServiceAttributeUnauthRO = 0b11,

    FelicaServiceAttributeAuthDirectAccess = 0b000,
    FelicaServiceAttributeUnauthDirectAccess = 0b001,
    FelicaServiceAttributeAuthCashbackDecrement = 0b010,
    FelicaServiceAttributeUnauthCashbackDecrement = 0b011,
    FelicaServiceAttributeAuthDecrement = 0b100,
    FelicaServiceAttributeUnauthDecrement = 0b101,
    FelicaServiceAttributeAuthPurseRO = 0b110,
    FelicaServiceAttributeUnauthPurseRO = 0b111,
} FelicaServiceAttribute;

DICT_SET_DEF(
    FelicaServiceAttributeList,
    FelicaServiceAttribute,
    M_ENUM_OPLIST(FelicaServiceAttribute, FelicaServiceAttributeAuthRW))

typedef FelicaMRTParts FelicaMRTParameters[6];

typedef struct {
    uint8_t data[FELICA_BLOCK_SIZE];
} FelicaBlock;

ARRAY_DEF(FelicaBlockList, FelicaBlock*, M_PTR_OPLIST)

typedef struct {
    uint16_t number;
    FelicaServiceAttributeList_t access_control_list; // accounts for overlap services
    bool is_extended_overlap;
    union {
        FelicaBlockList_t blocks;
        struct {
            uint16_t overlap_target;
            uint8_t block_start;
            uint8_t block_count;
        };
    };
} FelicaService;

typedef enum {
    FelicaNodeTypeArea,
    FelicaNodeTypeService,
} FelicaNodeType;

struct _FelicaArea_t;
typedef struct {
    FelicaNodeType type;
    union {
        struct _FelicaArea_t* area;
        FelicaService* service;
    };
} FelicaNode;

ARRAY_DEF(FelicaNodeList, FelicaNode*, M_PTR_OPLIST)

typedef struct _FelicaArea_t {
    uint16_t number;
    bool can_create_subareas;
    uint16_t end_service_code;

    FelicaNodeList_t nodes;
} FelicaArea;

typedef struct {
    uint8_t* S_PAD[14];
    uint8_t* REG;
    // MACs of all zero bytes (read from CK)
    uint8_t MAC[8];
    uint16_t data_format_code;
    uint8_t ID_value[6];
    uint8_t* card_key_1;
    uint8_t* card_key_2;
    uint16_t card_key_version;
    uint8_t memory_config[FELICA_BLOCK_SIZE];

    // Lite-S only
    uint8_t MAC_A[8];
    uint32_t write_count;
    bool crc_valid;
} FelicaLiteInfo;

typedef struct _FelicaSystem_t {
    uint8_t number;
    uint16_t code;
    uint8_t idm[8];
    uint8_t pmm[8];
    FelicaMRTParameters maximum_response_times;

    union {
        FelicaLiteInfo lite_info;
        FelicaArea root_area;
    };
} FelicaSystem;

ARRAY_DEF(FelicaSystemList, FelicaSystem*, M_PTR_OPLIST)

typedef struct {
    FelicaICType type;
    uint8_t subtype;
    FelicaSystemList_t systems;
} FelicaData;

typedef struct {
    uint8_t current_idm[8];
    uint8_t current_pmm[8];

    uint8_t status_flags[2];
} FelicaReader;

bool felica_check_ic_type(uint8_t* PMm);
FelicaICType felica_get_ic_type(uint8_t* PMm);

uint8_t felica_prepare_unencrypted_read(
    uint8_t* dest,
    const FelicaReader* reader,
    const uint16_t* service_code_list,
    uint8_t service_count,
    const uint32_t* block_list,
    uint8_t block_count);
uint8_t felica_lite_prepare_unencrypted_read(
    uint8_t* dest,
    const FelicaReader* reader,
    bool is_read_only,
    const uint8_t* block_list,
    uint8_t block_count);
uint16_t felica_parse_unencrypted_read(
    uint8_t* buf,
    uint8_t len,
    FelicaReader* reader,
    uint8_t* out,
    uint16_t out_len);

uint8_t felica_prepare_unencrypted_write(
    uint8_t* dest,
    FelicaReader* reader,
    const uint16_t* service_code_list,
    uint8_t service_count,
    const uint32_t* block_list,
    uint8_t block_count,
    const uint8_t* block_data);
uint8_t felica_lite_prepare_unencrypted_write(
    uint8_t* dest,
    const FelicaReader* reader,
    const uint8_t* block_list,
    uint8_t block_count,
    const uint8_t* block_data);
bool felica_parse_unencrypted_write(uint8_t* buf, uint8_t len, FelicaReader* reader);

bool felica_lite_can_read_without_mac(uint8_t* mc_r_restr, uint8_t block_number);

void felica_define_normal_block(FelicaService* service, uint16_t number, uint8_t* data);

bool felica_read_lite_system(
    FuriHalNfcTxRxContext* tx_rx,
    FelicaReader* reader,
    FelicaData* data,
    FelicaSystem* system);

bool felica_read_card(
    FuriHalNfcTxRxContext* tx_rx,
    FelicaData* data,
    uint8_t* polled_idm,
    uint8_t* polled_pmm);
void felica_clear(FelicaData* data);