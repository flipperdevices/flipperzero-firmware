#pragma once

#include <lib/nfc/protocols/nfca/nfca.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t hw_vendor;
    uint8_t hw_type;
    uint8_t hw_subtype;
    uint8_t hw_major;
    uint8_t hw_minor;
    uint8_t hw_storage;
    uint8_t hw_proto;

    uint8_t sw_vendor;
    uint8_t sw_type;
    uint8_t sw_subtype;
    uint8_t sw_major;
    uint8_t sw_minor;
    uint8_t sw_storage;
    uint8_t sw_proto;

    uint8_t uid[7];
    uint8_t batch[5];
    uint8_t prod_week;
    uint8_t prod_year;
} MfDesfireVersion;

typedef enum {
    MfDesfireErrorNone,
} MfDesfireError;

typedef struct {
    NfcaData nfca_data;
    MfDesfireVersion version;
} MfDesfireData;

bool mf_desfire_detect_protocol(NfcaData* nfca_data);

#ifdef __cplusplus
}
#endif
