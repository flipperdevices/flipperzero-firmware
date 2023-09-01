#pragma once

#include <nfc/protocols/nfc_device_base_i.h>

#include <toolbox/simple_array.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ISO15693_3_UID_SIZE (8U)

#define ISO15693_3_GUARD_TIME_US (5000U)
#define ISO15693_3_FDT_POLL_FC (4202U)
#define ISO15693_3_FDT_LISTEN_FC (4320U)
#define ISO15693_3_POLL_POLL_MIN_US (1500U)

/* true: modulating releases load, false: modulating adds load resistor to field coil */
#define ISO15693_3_LOAD_MODULATION_POLARITY (false)

#define ISO15693_3_FC (13560000.0f) /* MHz */
#define ISO15693_3_RESP_SUBC1_PULSE_32 (1.0f / (ISO15693_3_FC / 32) / 2.0f) /*  1.1799 µs */
#define ISO15693_3_RESP_SUBC1_UNMOD_256 (256.0f / ISO15693_3_FC) /* 18.8791 µs */
#define ISO15693_3_PULSE_DURATION_NS (128.0f * 1000000000.0f / ISO15693_3_FC)

/* ISO/IEC 15693-3:2019(E) 10.4.12: maximum number of blocks is defined as 256 */
#define ISO15693_3_BLOCKS_MAX 256
/* ISO/IEC 15693-3:2019(E) 10.4.12: maximum size of blocks is defined as 32 */
#define ISO15693_3_BLOCKSIZE_MAX 32
/* the resulting memory size a card can have */
#define ISO15693_3_MEMSIZE_MAX (ISO15693_3_BLOCKS_MAX * ISO15693_3_BLOCKSIZE_MAX)
/* ISO/IEC 15693-3:2019(E) 7.1b: standard allows up to 8192, the maxium frame length that we are expected to receive/send is less */
#define ISO15693_3_FRAMESIZE_MAX (1 + ISO15693_3_MEMSIZE_MAX + ISO15693_3_BLOCKS_MAX)

#define ISO15693_3_REQ_FLAG_SUBCARRIER_1 (0U << 0)
#define ISO15693_3_REQ_FLAG_SUBCARRIER_2 (1U << 0)
#define ISO15693_3_REQ_FLAG_DATA_RATE_LO (0U << 1)
#define ISO15693_3_REQ_FLAG_DATA_RATE_HI (1U << 1)
#define ISO15693_3_REQ_FLAG_INVENTORY_T4 (0U << 2)
#define ISO15693_3_REQ_FLAG_INVENTORY_T5 (1U << 2)
#define ISO15693_3_REQ_FLAG_EXTENSION (1U << 3)

#define ISO15693_3_REQ_FLAG_T4_SELECTED (1U << 4)
#define ISO15693_3_REQ_FLAG_T4_ADDRESSED (1U << 5)
#define ISO15693_3_REQ_FLAG_T4_OPTION (1U << 6)

#define ISO15693_3_REQ_FLAG_T5_AFI_PRESENT (1U << 4)
#define ISO15693_3_REQ_FLAG_T5_N_SLOTS_16 (0U << 5)
#define ISO15693_3_REQ_FLAG_T5_N_SLOTS_1 (1U << 5)
#define ISO15693_3_REQ_FLAG_T5_OPTION (1U << 6)

#define ISO15693_3_RESP_FLAG_NONE (0U)
#define ISO15693_3_RESP_FLAG_ERROR (1U << 0)
#define ISO15693_3_RESP_FLAG_EXTENSION (1U << 3)

#define ISO15693_3_RESP_ERROR_NOT_SUPPORTED (0x01U)
#define ISO15693_3_RESP_ERROR_FORMAT (0x02U)
#define ISO15693_3_RESP_ERROR_OPTION (0x03U)
#define ISO15693_3_RESP_ERROR_UNKNOWN (0x0FU)
#define ISO15693_3_RESP_ERROR_BLOCK_UNAVAILABLE (0x10U)
#define ISO15693_3_RESP_ERROR_BLOCK_ALREADY_LOCKED (0x11U)
#define ISO15693_3_RESP_ERROR_BLOCK_LOCKED (0x12U)
#define ISO15693_3_RESP_ERROR_BLOCK_WRITE (0x13U)
#define ISO15693_3_RESP_ERROR_BLOCK_LOCK (0x14U)
#define ISO15693_3_RESP_ERROR_CUSTOM_START (0xA0U)
#define ISO15693_3_RESP_ERROR_CUSTOM_END (0xDFU)

#define ISO15693_3_CMD_MANDATORY_START (0x01U)
#define ISO15693_3_CMD_INVENTORY (0x01U)
#define ISO15693_3_CMD_STAY_QUIET (0x02U)
#define ISO15693_3_CMD_MANDATORY_RFU (0x03U)
#define ISO15693_3_CMD_OPTIONAL_START (0x20U)
#define ISO15693_3_CMD_READ_BLOCK (0x20U)
#define ISO15693_3_CMD_WRITE_BLOCK (0x21U)
#define ISO15693_3_CMD_LOCK_BLOCK (0x22U)
#define ISO15693_3_CMD_READ_MULTI_BLOCKS (0x23U)
#define ISO15693_3_CMD_WRITE_MULTI_BLOCKS (0x24U)
#define ISO15693_3_CMD_SELECT (0x25U)
#define ISO15693_3_CMD_RESET_TO_READY (0x26U)
#define ISO15693_3_CMD_WRITE_AFI (0x27U)
#define ISO15693_3_CMD_LOCK_AFI (0x28U)
#define ISO15693_3_CMD_WRITE_DSFID (0x29U)
#define ISO15693_3_CMD_LOCK_DSFID (0x2AU)
#define ISO15693_3_CMD_GET_SYS_INFO (0x2BU)
#define ISO15693_3_CMD_GET_BLOCKS_SECURITY (0x2CU)
#define ISO15693_3_CMD_OPTIONAL_RFU (0x2DU)

#define ISO15693_3_SYSINFO_FLAG_DSFID (1U << 0)
#define ISO15693_3_SYSINFO_FLAG_AFI (1U << 1)
#define ISO15693_3_SYSINFO_FLAG_MEMORY (1U << 2)
#define ISO15693_3_SYSINFO_FLAG_IC_REF (1U << 3)

#define ISO15693_3_SYSINFO_LOCK_DSFID (1U << 0)
#define ISO15693_3_SYSINFO_LOCK_AFI (1U << 1)

typedef enum {
    Iso15693_3ErrorNone,
    Iso15693_3ErrorNotPresent,
    Iso15693_3ErrorBufferEmpty,
    Iso15693_3ErrorBufferOverflow,
    Iso15693_3ErrorFraming,
    Iso15693_3ErrorFieldOff,
    Iso15693_3ErrorWrongCrc,
    Iso15693_3ErrorTimeout,
    Iso15693_3ErrorFormat,
    Iso15693_3ErrorNotSupported,
    Iso15693_3ErrorUnexpectedResponse,
    Iso15693_3ErrorInternal,
    Iso15693_3ErrorCustom,
    Iso15693_3ErrorUnknown,
} Iso15693_3Error;

typedef struct {
    uint8_t flags;
    uint8_t dsfid;
    uint8_t afi;
    uint8_t ic_ref;
    uint16_t block_count;
    uint8_t block_size;
} Iso15693_3SystemInfo;

typedef struct {
    uint8_t lock_bits;
} Iso15693_3Settings;

typedef struct {
    uint8_t uid[ISO15693_3_UID_SIZE];
    Iso15693_3SystemInfo system_info;
    Iso15693_3Settings settings;
    SimpleArray* block_data;
    SimpleArray* block_security;
} Iso15693_3Data;

Iso15693_3Data* iso15693_3_alloc();

void iso15693_3_free(Iso15693_3Data* data);

void iso15693_3_reset(Iso15693_3Data* data);

void iso15693_3_copy(Iso15693_3Data* data, const Iso15693_3Data* other);

bool iso15693_3_verify(Iso15693_3Data* data, const FuriString* device_type);

bool iso15693_3_load(Iso15693_3Data* data, FlipperFormat* ff, uint32_t version);

bool iso15693_3_save(const Iso15693_3Data* data, FlipperFormat* ff);

bool iso15693_3_is_equal(const Iso15693_3Data* data, const Iso15693_3Data* other);

const char* iso15693_3_get_device_name(const Iso15693_3Data* data, NfcDeviceNameType name_type);

const uint8_t* iso15693_3_get_uid(const Iso15693_3Data* data, size_t* uid_len);

bool iso15693_3_set_uid(Iso15693_3Data* data, const uint8_t* uid, size_t uid_len);

Iso15693_3Data* iso15693_3_get_base_data(const Iso15693_3Data* data);

// Getters and tests

bool iso15693_3_is_block_locked(const Iso15693_3Data* data, uint8_t block_num);

// Setters

void iso15693_3_set_block_locked(Iso15693_3Data* data, uint8_t block_num, bool locked);

extern const NfcDeviceBase nfc_device_iso15693_3;

#ifdef __cplusplus
}
#endif
