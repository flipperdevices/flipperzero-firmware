#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <lib/digital_signal/digital_signal.h>
#include "nfc_util.h"
#include <furi_hal_nfc.h>


#define NFCV_TOTAL_BLOCKS_MAX 32
#define NFCV_BLOCK_SIZE 4
#define NFCV_MAX_DUMP_SIZE (NFCV_BLOCK_SIZE*NFCV_TOTAL_BLOCKS_MAX)

typedef enum {
    NfcVAuthMethodManual,
    NfcVAuthMethodTonieBox,
} NfcVAuthMethod;

typedef enum {
    NfcVTypeSlix,
    NfcVTypeSlixS,
    NfcVTypeSlixL,
    NfcVTypeSlix2,
} NfcVType;

typedef struct {
    NfcVType type;
    NfcVAuthMethod auth_method;
    bool auth_success;

    uint8_t key_privacy[4];
    uint8_t key_destroy[4];
    uint8_t key_eas[4];

    uint8_t dsfid;
    uint8_t afi;
    uint8_t ic_ref;
    uint8_t block_num;
    uint8_t block_size;
    uint8_t data[NFCV_MAX_DUMP_SIZE];

    char error[32];
} NfcVData;

typedef struct {
    uint16_t blocks_to_read;
    int16_t blocks_read;
} NfcVReader;

ReturnCode nfcv_read_blocks(NfcVReader* reader, NfcVData* data);    
ReturnCode nfcv_read_sysinfo(NfcVData* data);
ReturnCode nfcv_inventory(uint8_t* uid);

