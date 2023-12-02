#pragma once

#include <furi_hal_nfc.h>

#define RFAL_PICOPASS_UID_LEN 8
#define RFAL_PICOPASS_MAX_BLOCK_LEN 8

enum {
    RFAL_PICOPASS_CMD_ACTALL = 0x0A,
    RFAL_PICOPASS_CMD_IDENTIFY = 0x0C,
    RFAL_PICOPASS_CMD_SELECT = 0x81,
    RFAL_PICOPASS_CMD_READCHECK = 0x88,
    RFAL_PICOPASS_CMD_CHECK = 0x05,
    RFAL_PICOPASS_CMD_READ4 = 0x06,
    RFAL_PICOPASS_CMD_READ = 0x0C,
    RFAL_PICOPASS_CMD_WRITE = 0x87,
};

typedef struct {
    uint8_t CSN[RFAL_PICOPASS_UID_LEN]; // Anti-collision CSN
    uint8_t crc[2];
} rfalPicoPassIdentifyRes;

typedef struct {
    uint8_t CSN[RFAL_PICOPASS_UID_LEN]; // Real CSN
    uint8_t crc[2];
} rfalPicoPassSelectRes;

FuriHalNfcReturn rfalPicoPassPollerInitialize(void);
FuriHalNfcReturn rfalPicoPassPollerCheckPresence(void);
FuriHalNfcReturn rfalPicoPassPollerIdentify(rfalPicoPassIdentifyRes* idRes);
FuriHalNfcReturn rfalPicoPassPollerSelect(uint8_t* csn, rfalPicoPassSelectRes* selRes);
