#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <lib/digital_signal/digital_signal.h>
#include <lib/pulse_reader/pulse_reader.h>
#include "nfc_util.h"
#include <furi_hal_nfc.h>

#define NFCV_FC (13560000.0f) /* MHz */
#define NFCV_RESP_SUBC1_PULSE_32 (1.0f / (NFCV_FC / 32) / 2.0f) /*  1.1799 µs */
#define NFCV_RESP_SUBC1_UNMOD_256 (256.0f / NFCV_FC) /* 18.8791 µs */

#define PULSE_DURATION_NS (128.0f * 1000000000.0f / NFCV_FC) /* ns */

#define DIGITAL_SIGNAL_UNIT_S (100000000000.0f)
#define DIGITAL_SIGNAL_UNIT_US (100000.0f)

#define NFCV_TOTAL_BLOCKS_MAX 256
#define NFCV_BLOCK_SIZE 4
#define NFCV_MAX_DUMP_SIZE (NFCV_BLOCK_SIZE * NFCV_TOTAL_BLOCKS_MAX)

#define NFCV_FRAME_STATE_SOF1 0
#define NFCV_FRAME_STATE_SOF2 1
#define NFCV_FRAME_STATE_CODING_4 2
#define NFCV_FRAME_STATE_CODING_256 3
#define NFCV_FRAME_STATE_EOF 4
#define NFCV_FRAME_STATE_RESET 5

#define NFCV_SIG_SOF 0
#define NFCV_SIG_BIT0 1
#define NFCV_SIG_BIT1 2
#define NFCV_SIG_EOF 3

/*  */
#define ISO15693_INVENTORY 0x01
#define ISO15693_STAYQUIET 0x02
#define ISO15693_READBLOCK 0x20
#define ISO15693_WRITEBLOCK 0x21
#define ISO15693_LOCKBLOCK 0x22
#define ISO15693_READ_MULTI_BLOCK 0x23
#define ISO15693_WRITE_MULTI_BLOCK 0x24
#define ISO15693_SELECT 0x25
#define ISO15693_RESET_TO_READY 0x26
#define ISO15693_WRITE_AFI 0x27
#define ISO15693_LOCK_AFI 0x28
#define ISO15693_WRITE_DSFID 0x29
#define ISO15693_LOCK_DSFID 0x2A
#define ISO15693_GET_SYSTEM_INFO 0x2B
#define ISO15693_READ_MULTI_SECSTATUS 0x2C

// ISO15693 MANUFACTURER CODES
#define ISO15693_MANUFACTURER_NXP 0x04

// ISO15693-3 CUSTOM NXP COMMANDS
#define ISO15693_CMD_NXP_SET_EAS 0xA2
#define ISO15693_CMD_NXP_RESET_EAS 0xA3
#define ISO15693_CMD_NXP_LOCK_EAS 0xA4
#define ISO15693_CMD_NXP_EAS_ALARM 0xA5
#define ISO15693_CMD_NXP_PASSWORD_PROTECT_EAS_AFI 0xA6
#define ISO15693_CMD_NXP_WRITE_EAS_ID 0xA7
#define ISO15693_CMD_NXP_INVENTORY_PAGE_READ 0xB0
#define ISO15693_CMD_NXP_INVENTORY_PAGE_READ_FAST 0xB1
#define ISO15693_CMD_NXP_GET_RANDOM_NUMBER 0xB2
#define ISO15693_CMD_NXP_SET_PASSWORD 0xB3
#define ISO15693_CMD_NXP_WRITE_PASSWORD 0xB4
#define ISO15693_CMD_NXP_DESTROY 0xB9
#define ISO15693_CMD_NXP_ENABLE_PRIVACY 0xBA

// ISO15693 RESPONSE ERROR CODES
#define ISO15693_NOERROR 0x00
#define ISO15693_ERROR_CMD_NOT_SUP 0x01 // Command not supported
#define ISO15693_ERROR_CMD_NOT_REC 0x02 // Command not recognized (eg. parameter error)
#define ISO15693_ERROR_CMD_OPTION 0x03 // Command option not supported
#define ISO15693_ERROR_GENERIC 0x0F // No additional Info about this error
#define ISO15693_ERROR_BLOCK_UNAVAILABLE 0x10
#define ISO15693_ERROR_BLOCK_LOCKED_ALREADY 0x11 // cannot lock again
#define ISO15693_ERROR_BLOCK_LOCKED 0x12 // cannot be changed
#define ISO15693_ERROR_BLOCK_WRITE 0x13 // Writing was unsuccessful
#define ISO15693_ERROR_BLOCL_WRITELOCK 0x14 // Locking was unsuccessful

typedef enum {
    NfcVAuthMethodManual,
    NfcVAuthMethodTonieBox,
} NfcVAuthMethod;

typedef enum {
    NfcVTypePlain = 0,
    NfcVTypeSlix = 1,
    NfcVTypeSlixS = 2,
    NfcVTypeSlixL = 3,
    NfcVTypeSlix2 = 4,
} NfcVType;

typedef struct {
    uint8_t key_eas[4];
    uint8_t rand[2];
} NfcVSlixData;

typedef struct {
    uint8_t key_read[4];
    uint8_t key_write[4];
    uint8_t key_privacy[4];
    uint8_t key_destroy[4];
    uint8_t key_eas[4];
    uint8_t rand[2];
    bool privacy;
} NfcVSlix2Data;

typedef struct {
    uint8_t key_read[4];
    uint8_t key_write[4];
    uint8_t key_privacy[4];
    uint8_t key_destroy[4];
    uint8_t key_eas[4];
    uint8_t rand[2];
    bool privacy;
} NfcVSlixSData;

typedef struct {
    uint8_t key_privacy[4];
    uint8_t key_destroy[4];
    uint8_t key_eas[4];
    uint8_t rand[2];
    bool privacy;
} NfcVSlixLData;

typedef union {
    NfcVSlixData slix;
    NfcVSlix2Data slix2;
    NfcVSlixSData slix_s;
    NfcVSlixLData slix_l;
} NfcVSubtypeData;

typedef struct {
    PulseReader* reader_signal;
    DigitalSignal* nfcv_resp_pulse_32;
    DigitalSignal* nfcv_resp_unmod;
    DigitalSignal* nfcv_resp_one;
    DigitalSignal* nfcv_resp_zero;
    DigitalSignal* nfcv_resp_sof;
    DigitalSignal* nfcv_resp_eof;
    DigitalSignal* nfcv_resp_unmod_256;
    DigitalSignal* nfcv_resp_unmod_768;
    DigitalSequence* nfcv_signal;
} NfcVEmuData;

typedef struct {
    /* common ISO15693 fields */
    uint8_t dsfid;
    uint8_t afi;
    uint8_t ic_ref;
    uint16_t block_num;
    uint8_t block_size;
    uint8_t data[NFCV_MAX_DUMP_SIZE];

    /* specfic variant infos */
    NfcVType type;
    NfcVSubtypeData sub_data;
    NfcVEmuData emulation;

    /* runtime data */
    char last_command[128];
    char error[32];
    NfcVAuthMethod auth_method;
    bool auth_success;
} NfcVData;

typedef struct {
    uint16_t blocks_to_read;
    int16_t blocks_read;
} NfcVReader;

ReturnCode nfcv_read_blocks(NfcVReader* reader, NfcVData* data);
ReturnCode nfcv_read_sysinfo(FuriHalNfcDevData* nfc_data, NfcVData* data);
ReturnCode nfcv_inventory(uint8_t* uid);
bool nfcv_read_card(NfcVReader* reader, FuriHalNfcDevData* nfc_data, NfcVData* data);

void nfcv_emu_init(FuriHalNfcDevData* nfc_data, NfcVData* nfcv_data);
void nfcv_emu_deinit(NfcVData* nfcv_data);
bool nfcv_emu_loop(
    FuriHalNfcTxRxContext* tx_rx,
    FuriHalNfcDevData* nfc_data,
    NfcVData* nfcv_data,
    uint32_t timeout_ms);
