#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <storage/storage.h>
#include <dialogs/dialogs.h>
#include <mbedtls/des.h>

#include "rfal_picopass.h"
#include "loclass_writer.h"
#include <optimized_ikeys.h>
#include <optimized_cipher.h>
#include "helpers/iclass_elite_dict.h"

#define PICOPASS_DEV_NAME_MAX_LEN 22
#define PICOPASS_READER_DATA_MAX_SIZE 64
#define PICOPASS_BLOCK_LEN 8
#define PICOPASS_MAX_APP_LIMIT 32

#define PICOPASS_CSN_BLOCK_INDEX 0
#define PICOPASS_CONFIG_BLOCK_INDEX 1
#define PICOPASS_EPURSE_BLOCK_INDEX 2
#define PICOPASS_KD_BLOCK_INDEX 3
#define PICOPASS_KC_BLOCK_INDEX 4
#define PICOPASS_AIA_BLOCK_INDEX 5
#define PICOPASS_PACS_CFG_BLOCK_INDEX 6

// Personalization Mode
#define PICOPASS_FUSE_PERS 0x80
// Crypt1 // 1+1 (crypt1+crypt0) means secured and keys changable
#define PICOPASS_FUSE_CRYPT1 0x10
// Crypt0 // 1+0 means secure and keys locked, 0+1 means not secured, 0+0 means disable auth entirely
#define PICOPASS_FUSE_CRTPT0 0x08
#define PICOPASS_FUSE_CRYPT10 (PICOPASS_FUSE_CRYPT1 | PICOPASS_FUSE_CRTPT0)
// Read Access, 1 meanns anonymous read enabled, 0 means must auth to read applicaion
#define PICOPASS_FUSE_RA 0x01

// PicoPass command bytes:
// Low nibble used for command
// High nibble used for options and checksum (MSB)
// The only option we care about in 15693 mode is the key
// which is only used by READCHECK, so for simplicity we
// don't bother breaking down the command and flags into parts
// READ: ADDRESS(1) CRC16(2) -> DATA(8) CRC16(2)
// IDENTIFY: No args -> ASNB(8) CRC16(2)
#define PICOPASS_CMD_READ_OR_IDENTIFY 0x0C
// ADDRESS(1) CRC16(2) -> DATA(32) CRC16(2)
#define PICOPASS_CMD_READ4 0x06
// ADDRESS(1) DATA(8) SIGN(4)|CRC16(2) -> DATA(8) CRC16(2)
#define PICOPASS_CMD_UPDATE 0x87
// ADDRESS(1) -> DATA(8)
#define PICOPASS_CMD_READCHECK_KD 0x88
// ADDRESS(1) -> DATA(8)
#define PICOPASS_CMD_READCHECK_KC 0x18
// CHALLENGE(4) READERSIGNATURE(4) -> CHIPRESPONSE(4)
#define PICOPASS_CMD_CHECK 0x05
// No args -> SOF
#define PICOPASS_CMD_ACTALL 0x0A
// No args -> SOF
#define PICOPASS_CMD_ACT 0x8E
// ASNB(8)|SERIALNB(8) -> SERIALNB(8) CRC16(2)
#define PICOPASS_CMD_SELECT 0x81
// No args -> SERIALNB(8) CRC16(2)
#define PICOPASS_CMD_DETECT 0x0F
// No args -> SOF
#define PICOPASS_CMD_HALT 0x00
// PAGE(1) CRC16(2) -> BLOCK1(8) CRC16(2)
#define PICOPASS_CMD_PAGESEL 0x84

#define PICOPASS_APP_FOLDER ANY_PATH("picopass")
#define PICOPASS_APP_EXTENSION ".picopass"
#define PICOPASS_APP_SHADOW_EXTENSION ".pas"

#define PICOPASS_DICT_KEY_BATCH_SIZE 10

typedef void (*PicopassLoadingCallback)(void* context, bool state);

typedef struct {
    IclassEliteDict* dict;
    IclassEliteDictType type;
    uint8_t current_sector;
} IclassEliteDictAttackData;

typedef enum {
    PicopassDeviceEncryptionUnknown = 0,
    PicopassDeviceEncryptionNone = 0x14,
    PicopassDeviceEncryptionDES = 0x15,
    PicopassDeviceEncryption3DES = 0x17,
} PicopassEncryption;

typedef enum {
    PicopassDeviceSaveFormatHF,
    PicopassDeviceSaveFormatLF,
} PicopassDeviceSaveFormat;

typedef enum {
    PicopassEmulatorStateHalt,
    PicopassEmulatorStateIdle,
    PicopassEmulatorStateActive,
    PicopassEmulatorStateSelected,
} PicopassEmulatorState;

typedef struct {
    bool valid;
    uint8_t bitLength;
    uint8_t FacilityCode;
    uint16_t CardNumber;
} PicopassWiegandRecord;

typedef struct {
    bool legacy;
    bool se_enabled;
    bool sio;
    bool biometrics;
    uint8_t key[8];
    uint8_t pin_length;
    PicopassEncryption encryption;
    uint8_t credential[8];
    uint8_t pin0[8];
    uint8_t pin1[8];
    PicopassWiegandRecord record;
} PicopassPacs;

typedef struct {
    uint8_t data[PICOPASS_BLOCK_LEN];
} PicopassBlock;

typedef struct {
    PicopassBlock AA1[PICOPASS_MAX_APP_LIMIT];
    PicopassPacs pacs;
    IclassEliteDictAttackData iclass_elite_dict_attack_data;
} PicopassDeviceData;

typedef struct {
    PicopassEmulatorState state;
    LoclassState_t cipher_state;
    uint8_t key_block_num; // in loclass mode used to store csn#
    bool loclass_mode;
    bool loclass_got_std_key;
    LoclassWriter* loclass_writer;
} PicopassEmulatorCtx;

typedef struct {
    Storage* storage;
    DialogsApp* dialogs;
    PicopassDeviceData dev_data;
    char dev_name[PICOPASS_DEV_NAME_MAX_LEN + 1];
    FuriString* load_path;
    PicopassDeviceSaveFormat format;
    PicopassLoadingCallback loading_cb;
    void* loading_cb_ctx;
} PicopassDevice;

PicopassDevice* picopass_device_alloc();

void picopass_device_free(PicopassDevice* picopass_dev);

void picopass_device_set_name(PicopassDevice* dev, const char* name);

bool picopass_device_save(PicopassDevice* dev, const char* dev_name);

bool picopass_file_select(PicopassDevice* dev);

void picopass_device_data_clear(PicopassDeviceData* dev_data);

void picopass_device_clear(PicopassDevice* dev);

bool picopass_device_delete(PicopassDevice* dev, bool use_load_path);

void picopass_device_set_loading_callback(
    PicopassDevice* dev,
    PicopassLoadingCallback callback,
    void* context);

ReturnCode picopass_device_parse_credential(PicopassBlock* AA1, PicopassPacs* pacs);
ReturnCode picopass_device_parse_wiegand(uint8_t* data, PicopassWiegandRecord* record);
