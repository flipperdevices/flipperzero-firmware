#pragma once

#include <nfc/protocols/nfc_device_base_i.h>

#include <toolbox/simple_array.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ISO15693_3_UID_SIZE (8U)

// TODO: Adjust these values
#define ISO15693_3_GUARD_TIME_US (5000U)
#define ISO15693_3_FDT_POLL_FC (9000U)
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

typedef enum {
    Iso15693_3ErrorNone,
    Iso15693_3ErrorNotPresent,
    Iso15693_3ErrorColResFailed,
    Iso15693_3ErrorBufferOverflow,
    Iso15693_3ErrorCommunication,
    Iso15693_3ErrorFieldOff,
    Iso15693_3ErrorWrongCrc,
    Iso15693_3ErrorTimeout,
} Iso15693_3Error;

typedef struct {
    uint8_t uid[ISO15693_3_UID_SIZE];
    uint8_t dsfid;
    uint8_t afi;
    uint8_t ic_ref;
    uint16_t block_count;
    uint8_t block_size;
    SimpleArray* block_data;
    SimpleArray* security_status;
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

const Iso15693_3Data* iso15693_3_get_base_data(const Iso15693_3Data* data);

extern const NfcDeviceBase nfc_device_iso15693_3;

#ifdef __cplusplus
}
#endif
