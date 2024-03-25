#pragma once

#include <stdint.h>

#define GEN4_CONFIG_SIZE (32)
#define GEN4_REVISION_SIZE (5)

#define GEN4_PASSWORD_LEN (4)
#define GEN4_ATS_MAX_LEN (16)
#define GEN4_ATQA_LEN (2)
#define GEN4_CRC_LEN (2)

typedef enum {
    Gen4ProtocolMfClassic = 0x00,
    Gen4ProtocolMfUltralight = 0x01,
} Gen4Protocol;

typedef union {
    uint32_t value;
    uint8_t bytes[GEN4_PASSWORD_LEN];
} Gen4Password;

typedef enum {
    Gen4UIDLengthSingle = 0x00,
    Gen4UIDLengthDouble = 0x01,
    Gen4UIDLengthTriple = 0x02
} Gen4UIDLength;

typedef enum {
    Gen4UltralightModeUL_EV1 = 0x00,
    Gen4UltralightModeNTAG = 0x01,
    Gen4UltralightModeUL_C = 0x02,
    Gen4UltralightModeUL = 0x03
} Gen4UltralightMode;

typedef enum {
    // for writing original (shadow) data
    Gen4ShadowModePreWrite = 0x00,
    // written data can be read once before restored to original
    Gen4ShadowModeRestore = 0x01,
    // shadow mode disabled
    Gen4ShadowModeDisabled = 0x02,
    // apparently for UL?
    Gen4ShadowModeHighSpeedDisabled = 0x03,
    // work with new UMC. With old UMC is untested
    Gen4ShadowModeSplit = 0x04,
} Gen4ShadowMode;

typedef enum {
    // gen2 card behavour
    Gen4DirectWriteBlock0ModeEnabled = 0x00,
    // common card behavour
    Gen4DirectWriteBlock0ModeDisabled = 0x01,
    // default mode. same behavour as Gen4DirectWriteBlock0ModeActivate
    Gen4DirectWriteBlock0ModeDefault = 0x02,
} Gen4DirectWriteBlock0Mode;

typedef union {
    uint8_t data_raw[GEN4_CONFIG_SIZE];
#pragma pack(push, 1)
    struct {
        Gen4Protocol protocol;
        Gen4UIDLength uid_len_code;
        Gen4Password password;
        Gen4ShadowMode gtu_mode;
        uint8_t ats_len;
        uint8_t ats[GEN4_ATS_MAX_LEN]; // mb another class?
        uint8_t atqa[GEN4_ATQA_LEN];
        uint8_t sak;
        Gen4UltralightMode mfu_mode;
        uint8_t total_blocks;
        Gen4DirectWriteBlock0Mode direct_write_mode;
        uint8_t crc[GEN4_CRC_LEN];
    } data_parsed;
#pragma pack(pop)
} Gen4Config;

typedef struct {
    uint8_t data[GEN4_REVISION_SIZE];
} Gen4Revision;

typedef struct {
    Gen4Config config;
    Gen4Revision revision;
} Gen4;

Gen4* gen4_alloc();

void gen4_free(Gen4* instance);

void gen4_reset(Gen4* instance);

void gen4_copy(Gen4* dest, const Gen4* source);

char* gen4_get_shadow_mode_name(Gen4ShadowMode mode);

char* gen4_get_direct_write_mode_name(Gen4DirectWriteBlock0Mode mode);

char* gen4_get_uid_len_num(Gen4UIDLength code);

char* gen4_get_configuration_name(const Gen4Config* config);