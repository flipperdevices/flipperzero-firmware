#pragma once

#include <lib/nfc/protocols/nfca/nfca.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MF_CLASSIC_BLOCK_SIZE (16)
#define MF_CLASSIC_TOTAL_BLOCKS_MAX (256)
#define MF_CLASSIC_KEY_SIZE (6)
#define MF_CLASSIC_ACCESS_BYTES_SIZE (4)

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
    MfClassicKeyA,
    MfClassicKeyB,
} MfClassicKey;

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
    uint8_t value[MF_CLASSIC_BLOCK_SIZE];
} MfClassicBlock;

typedef struct {
    uint8_t key_a[MF_CLASSIC_KEY_SIZE];
    uint8_t access_bits[MF_CLASSIC_ACCESS_BYTES_SIZE];
    uint8_t key_b[MF_CLASSIC_KEY_SIZE];
} MfClassicSectorTrailer;

typedef struct {
    NfcaData nfca_data;
    MfClassicType type;
    uint32_t block_read_mask[MF_CLASSIC_TOTAL_BLOCKS_MAX / 32];
    uint64_t key_a_mask;
    uint64_t key_b_mask;
    MfClassicBlock block[MF_CLASSIC_TOTAL_BLOCKS_MAX];
} MfClassicData;

uint8_t mf_classic_get_total_sectors_num(MfClassicType type);

uint16_t mf_classic_get_total_block_num(MfClassicType type);

uint8_t mf_classic_get_sector_trailer_block_num_by_sector(uint8_t sector);

uint8_t mf_classic_get_sector_trailer_num_by_block(uint8_t block);

bool mf_classic_is_sector_trailer(uint8_t block);

uint8_t mf_classic_get_sector_by_block(uint8_t block);

bool mf_classic_is_key_found(MfClassicData* data, uint8_t sector_num, MfClassicKey key_type);

void mf_classic_set_key_found(
    MfClassicData* data,
    uint8_t sector_num,
    MfClassicKey key_type,
    uint64_t key);

void mf_classic_set_key_not_found(MfClassicData* data, uint8_t sector_num, MfClassicKey key_type);

bool mf_classic_is_block_read(MfClassicData* data, uint8_t block_num);

void mf_classic_set_block_read(MfClassicData* data, uint8_t block_num, MfClassicBlock* block_data);

#ifdef __cplusplus
}
#endif
