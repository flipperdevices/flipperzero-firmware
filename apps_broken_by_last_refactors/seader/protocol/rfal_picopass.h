#pragma once

#include <furi_hal_nfc.h>

#define RFAL_PICOPASS_UID_LEN 8
#define PICOPASS_BLOCK_LEN 8

enum {
    // PicoPass command bytes:
    // Low nibble used for command
    // High nibble used for options and checksum (MSB)
    // The only option we care about in 15693 mode is the key
    // which is only used by READCHECK, so for simplicity we
    // don't bother breaking down the command and flags into parts

    // READ: ADDRESS(1) CRC16(2) -> DATA(8) CRC16(2)
    // IDENTIFY: No args -> ASNB(8) CRC16(2)
    RFAL_PICOPASS_CMD_READ_OR_IDENTIFY = 0x0C,
    // ADDRESS(1) CRC16(2) -> DATA(32) CRC16(2)
    RFAL_PICOPASS_CMD_READ4 = 0x06,
    // ADDRESS(1) DATA(8) SIGN(4)|CRC16(2) -> DATA(8) CRC16(2)
    RFAL_PICOPASS_CMD_UPDATE = 0x87,
    // ADDRESS(1) -> DATA(8)
    RFAL_PICOPASS_CMD_READCHECK_KD = 0x88,
    // ADDRESS(1) -> DATA(8)
    RFAL_PICOPASS_CMD_READCHECK_KC = 0x18,
    // CHALLENGE(4) READERSIGNATURE(4) -> CHIPRESPONSE(4)
    RFAL_PICOPASS_CMD_CHECK = 0x05,
    // No args -> SOF
    RFAL_PICOPASS_CMD_ACTALL = 0x0A,
    // No args -> SOF
    RFAL_PICOPASS_CMD_ACT = 0x8E,
    // ASNB(8)|SERIALNB(8) -> SERIALNB(8) CRC16(2)
    RFAL_PICOPASS_CMD_SELECT = 0x81,
    // No args -> SERIALNB(8) CRC16(2)
    RFAL_PICOPASS_CMD_DETECT = 0x0F,
    // No args -> SOF
    RFAL_PICOPASS_CMD_HALT = 0x00,
    // PAGE(1) CRC16(2) -> BLOCK1(8) CRC16(2)
    RFAL_PICOPASS_CMD_PAGESEL = 0x84,
};

typedef struct {
    uint8_t CSN[RFAL_PICOPASS_UID_LEN]; // Anti-collision CSN
    uint8_t crc[2];
} rfalPicoPassIdentifyRes;

typedef struct {
    uint8_t CSN[RFAL_PICOPASS_UID_LEN]; // Real CSN
    uint8_t crc[2];
} rfalPicoPassSelectRes;

typedef struct {
    uint8_t CCNR[8];
} rfalPicoPassReadCheckRes;

typedef struct {
    uint8_t mac[4];
} rfalPicoPassCheckRes;

typedef struct {
    uint8_t data[PICOPASS_BLOCK_LEN];
    uint8_t crc[2];
} rfalPicoPassReadBlockRes;
