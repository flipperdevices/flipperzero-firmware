#pragma once

#include "iso14443_4a.h"

#define ISO14443_4A_CMD_READ_ATS (0xE0)

#define ISO14443_4A_FSDI_16 (0x00)
#define ISO14443_4A_FSDI_24 (0x01)
#define ISO14443_4A_FSDI_32 (0x02)
#define ISO14443_4A_FSDI_40 (0x03)
#define ISO14443_4A_FSDI_48 (0x04)
#define ISO14443_4A_FSDI_64 (0x05)
#define ISO14443_4A_FSDI_96 (0x06)
#define ISO14443_4A_FSDI_128 (0x07)
#define ISO14443_4A_FSDI_256 (0x08)

#define ISO14443_4A_FSCI_16 ISO14443_4A_FSDI_16
#define ISO14443_4A_FSCI_24 ISO14443_4A_FSDI_24
#define ISO14443_4A_FSCI_32 ISO14443_4A_FSDI_32
#define ISO14443_4A_FSCI_40 ISO14443_4A_FSDI_40
#define ISO14443_4A_FSCI_48 ISO14443_4A_FSDI_48
#define ISO14443_4A_FSCI_64 ISO14443_4A_FSDI_64
#define ISO14443_4A_FSCI_96 ISO14443_4A_FSDI_96
#define ISO14443_4A_FSCI_128 ISO14443_4A_FSDI_128
#define ISO14443_4A_FSCI_256 ISO14443_4A_FSDI_256

typedef struct {
    uint8_t tl;
    uint8_t t0;
    uint8_t ta_1;
    uint8_t tb_1;
    uint8_t tc_1;
    uint8_t t1_tk[];
} Iso14443_4aAtsData;

bool iso14443_4a_ats_parse(SimpleArray* data, const BitBuffer* buf);

void iso14443_4a_ats_fill_default(SimpleArray* data);

Iso14443_4aError iso14443_4a_process_error(Iso14443_3aError error);
