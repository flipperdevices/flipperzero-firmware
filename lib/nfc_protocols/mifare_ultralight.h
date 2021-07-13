#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MF_UL_MAX_DUMP_SIZE 255

#define MF_UL_GET_VERSION_CMD (0x60)
#define MF_UL_READ_CMD (0x30)
#define MF_UL_FAST_READ_CMD (0x3A)

typedef enum {
    MfUltralightTypeUnknown,
    MfUltralightTypeUL11,
    MfUltralightTypeUL21,
    MfUltralightTypeNTAG213,
    MfUltralightTypeNTAG215,
    MfUltralightTypeNTAG216,
} MfUltralightType;

typedef struct {
    uint8_t header;
    uint8_t vendor_id;
    uint8_t prod_type;
    uint8_t prod_subtype;
    uint8_t prod_ver_major;
    uint8_t prod_ver_minor;
    uint8_t storage_size;
    uint8_t protocol_type;
} MfUltralightVersion;

typedef struct {
    uint8_t  sn0[3];
    uint8_t  btBCC0;
    uint8_t  sn1[4];
    uint8_t  btBCC1;
    uint8_t  internal;
    uint8_t  lock[2];
    uint8_t  otp[4];
} MfUltralightManufacturerBlock;

typedef struct {
    MfUltralightVersion version;
    uint8_t signature[32];
    uint8_t counter0[3];
    uint8_t tearing0;
    uint8_t counter1[3];
    uint8_t tearing1;
    uint8_t counter2[3];
    uint8_t tearing2;
    uint8_t dump[MF_UL_MAX_DUMP_SIZE];
} MifareUlData;

typedef struct {
    MfUltralightType type;
    uint8_t pages_to_read;
    uint8_t pages_readed;
    bool support_fast_read;
    MifareUlData data;
} MifareUlDevice;

bool mf_ul_check_card_type(uint8_t ATQA0, uint8_t ATQA1, uint8_t SAK);

uint16_t mf_ul_prepare_get_version(uint8_t* dest);
void mf_ul_parse_get_version_response(uint8_t* buff, MifareUlDevice* mf_ul_read);
void mf_ul_set_default_version(MifareUlDevice* mf_ul_read);

uint16_t mf_ul_prepare_get_signature(uint8_t* dest);
void mf_ul_parse_get_signature_response(uint8_t* buff, MifareUlDevice* mf_ul_read);

uint16_t mf_ul_prepare_read(uint8_t* dest, uint8_t start_page);
uint16_t mf_ul_prepare_fast_read(uint8_t* dest, uint8_t start_page, uint8_t end_page);
