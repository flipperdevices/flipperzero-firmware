#pragma once

#include <lib/nfc/protocols/nfca/nfca.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MF_CLASSIC_AUTH_KEY_A_CMD (0x60U)
#define MF_CLASSIC_AUTH_KEY_B_CMD (0x61U)
#define MF_CLASSIC_READ_BLOCK_CMD (0x30U)
#define MF_CLASSIC_HALT_MSB_CMD (0x50)
#define MF_CLASSIC_HALT_LSB_CMD (0x00)

#define MF_CLASSIC_TOTAL_BLOCKS_MAX (256)
#define MF_CLASSIC_BLOCK_SIZE (16)
#define MF_CLASSIC_KEY_SIZE (6)
#define MF_CLASSIC_ACCESS_BYTES_SIZE (4)

#define MF_CLASSIC_NT_SIZE (4)
#define MF_CLASSIC_NR_SIZE (4)
#define MF_CLASSIC_AR_SIZE (4)
#define MF_CLASSIC_AT_SIZE (4)

typedef enum {
    MfClassicErrorNone,
    MfClassicErrorNotPresent,
    MfClassicErrorProtocol,
    MfClassicErrorAuth,
    MfClassicErrorTimeout,
} MfClassicError;

typedef enum {
    MfClassicTypeMini,
    MfClassicType1k,
    MfClassicType4k,

    MfClassicTypeNum,
} MfClassicType;

typedef enum {
    MfClassicActionDataRead,
    MfClassicActionDataWrite,
    MfClassicActionDataInc,
    MfClassicActionDataDec,

    MfClassicActionKeyARead,
    MfClassicActionKeyAWrite,
    MfClassicActionKeyBRead,
    MfClassicActionKeyBWrite,
    MfClassicActionACRead,
    MfClassicActionACWrite,
} MfClassicAction;

typedef struct {
    uint8_t data[MF_CLASSIC_BLOCK_SIZE];
} MfClassicBlock;

typedef enum {
    MfClassicKeyTypeA,
    MfClassicKeyTypeB,
} MfClassicKeyType;

typedef struct {
    uint8_t data[MF_CLASSIC_KEY_SIZE];
} MfClassicKey;

typedef struct {
    uint8_t data[MF_CLASSIC_ACCESS_BYTES_SIZE];
} MfClassicAccessBits;

typedef struct {
    uint8_t data[MF_CLASSIC_NT_SIZE];
} MfClassicNt;

typedef struct {
    uint8_t data[MF_CLASSIC_AT_SIZE];
} MfClassicAt;

typedef struct {
    uint8_t data[MF_CLASSIC_NR_SIZE];
} MfClassicNr;

typedef struct {
    uint8_t data[MF_CLASSIC_AR_SIZE];
} MfClassicAr;

typedef struct {
    uint8_t block_num;
    MfClassicKey key;
    MfClassicKeyType key_type;
    MfClassicNt nt;
    MfClassicNr nr;
    MfClassicAr ar;
    MfClassicAt at;
} MfClassicAuthContext;

typedef struct {
    MfClassicKey key_a;
    MfClassicAccessBits access_bits;
    MfClassicKey key_b;
} MfClassicSectorTrailer;

typedef struct {
    NfcaData nfca_data;
    MfClassicType type;
    uint32_t block_read_mask[MF_CLASSIC_TOTAL_BLOCKS_MAX / 32];
    uint64_t key_a_mask;
    uint64_t key_b_mask;
    MfClassicBlock block[MF_CLASSIC_TOTAL_BLOCKS_MAX];
} MfClassicData;

bool mf_classic_detect_protocol(NfcaData* data, MfClassicType* type);

uint8_t mf_classic_get_total_sectors_num(MfClassicType type);

uint16_t mf_classic_get_total_block_num(MfClassicType type);

uint8_t mf_classic_get_first_block_num_of_sector(uint8_t sector);

const char* mf_classic_get_name(MfClassicType type, bool full_name);

uint8_t mf_classic_get_sector_trailer_num_by_sector(uint8_t sector);

uint8_t mf_classic_get_sector_trailer_num_by_block(uint8_t block);

MfClassicSectorTrailer*
    mf_classic_get_sector_trailer_by_sector(MfClassicData* data, uint8_t sector_num);

bool mf_classic_is_sector_trailer(uint8_t block);

uint8_t mf_classic_get_sector_by_block(uint8_t block);

bool mf_classic_is_key_found(MfClassicData* data, uint8_t sector_num, MfClassicKeyType key_type);

void mf_classic_set_key_found(
    MfClassicData* data,
    uint8_t sector_num,
    MfClassicKeyType key_type,
    uint64_t key);

void mf_classic_set_key_not_found(
    MfClassicData* data,
    uint8_t sector_num,
    MfClassicKeyType key_type);

bool mf_classic_is_block_read(MfClassicData* data, uint8_t block_num);

void mf_classic_set_block_read(MfClassicData* data, uint8_t block_num, MfClassicBlock* block_data);

bool mf_classic_is_sector_read(MfClassicData* data, uint8_t sector_num);

void mf_classic_get_read_sectors_and_keys(
    MfClassicData* data,
    uint8_t* sectors_read,
    uint8_t* keys_found);

#ifdef __cplusplus
}
#endif
