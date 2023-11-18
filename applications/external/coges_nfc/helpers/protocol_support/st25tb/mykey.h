#pragma once
#include <nfc/protocols/st25tb/st25tb.h>

enum {
    MYKEY_BLOCK_KEY_ID = 0x07,
    MYKEY_BLOCK_PRODUCTION_DATE = 0x08,
    MYKEY_BLOCK_VENDOR_ID_1 = 0x18,
    MYKEY_BLOCK_VENDOR_ID_2 = 0x19,
    MYKEY_BLOCK_CURRENT_CREDIT = 0x21,
    MYKEY_BLOCK_PREVIOUS_CREDIT = 0x23,
    MYKEY_DEFAULT_VENDOR_ID = 0xFEDC0123,
    MYKEY_DEFAULT_VENDOR_ID_1 = 0xFEDC,
    MYKEY_DEFAULT_VENDOR_ID_2 = 0x0123,
};

typedef enum {
    LockIdStatusNone,
    LockIdStatusActive,
} LockIdStatus;

#define get_uid(uid) ((uid)[7] | ((uid)[6] << 8) | ((uid)[5] << 16) | ((uid)[4] << 24))
#define get_count_down_counter(b6) (~((b6)[3] << 24 | (b6)[2] << 16 | (b6)[1] << 8 | (b6)[0]))
#define get_vendor(b1, b2) (get_block(b1) << 16 | (get_block(b2) & 0x0000FFFF))
#define get_master_key(uid, vendor_id) ((uid) * ((vendor_id) + 1))
#define get_is_bound(vendor_id) ((vendor_id) != MYKEY_DEFAULT_VENDOR_ID)
#define get_encryption_key(master_key, count_down_counter)((master_key) * ((count_down_counter) + 1))

uint32_t encode_decode_block(uint32_t input);
uint32_t st25tb_get_block_value(uint32_t block);
uint32_t get_block(uint32_t block);
uint32_t get_xored_block(uint32_t block, uint32_t key);