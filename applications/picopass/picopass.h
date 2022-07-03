#pragma once

#include <rfal_picopass.h>
#include <loclass/optimized_ikeys.h>
#include <loclass/optimized_cipher.h>
#include <mbedtls/des.h>

#define PP_MAX_DUMP_SIZE 1024
#define FURI_HAL_PICOPASS_UID_MAX_LEN 10

#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>
#include <st25r3916.h>
#include <rfal_analogConfig.h>
#include <rfal_rf.h>
#include <rfal_nfc.h>

#include <storage/storage.h>
#include <lib/toolbox/path.h>

#define TAG "PicoPass"

#define PICOPASS_APP_ICLASS_KEY_PATH "/any/picopass/iclass_key.bin"
#define PICOPASS_APP_ICLASS_DECRYPT_KEY_PATH "/any/picopass/iclass_decryptionkey.bin"

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} PluginEvent;

typedef struct {
    bool valid;
    uint8_t bitLength;
    uint8_t FacilityCode;
    uint16_t CardNumber;
} WiegandRecord;

typedef struct {
    bool biometrics;
    uint8_t encryption;
    uint8_t credential[8];
    uint8_t pin0[8];
    uint8_t pin1[8];
    WiegandRecord record;
} PACS;

enum State { INIT, KEYS_MISSING, READY, RESULT };
typedef struct {
    enum State state;
    PACS pacs;
} PluginState;


