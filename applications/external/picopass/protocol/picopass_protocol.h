#pragma once

#include "../picopass_device.h"

#define PICOPASS_BLOCK_LEN 8
#define PICOPASS_MAX_APP_LIMIT 32
#define PICOPASS_UID_LEN 8
#define PICOPASS_READ_CHECK_RESP_LEN 8
#define PICOPASS_CHECK_RESP_LEN 4
#define PICOPASS_MAC_LEN 4
#define PICOPASS_KEY_LEN 8

#define PICOPASS_FDT_LISTEN_FC (1000)

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PicopassErrorNone,
    PicopassErrorTimeout,
    PicopassErrorIncorrectCrc,
    PicopassErrorProtocol,
} PicopassError;

typedef struct {
    uint8_t data[RFAL_PICOPASS_UID_LEN];
} PicopassColResSerialNum;

typedef struct {
    uint8_t data[RFAL_PICOPASS_UID_LEN];
} PicopassSerialNum;

typedef struct {
    uint8_t data[PICOPASS_READ_CHECK_RESP_LEN];
} PicopassReadCheckResp;

typedef struct {
    uint8_t data[PICOPASS_CHECK_RESP_LEN];
} PicopassCheckResp;

typedef struct {
    uint8_t data[PICOPASS_MAC_LEN];
} PicopassMac;

#ifdef __cplusplus
}
#endif
