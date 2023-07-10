#include "iso14443_4a_i.h"

// T0 bits
#define ISO14443_4A_ATS_T0_TA1_FLAG (1U << 4)
#define ISO14443_4A_ATS_T0_TB1_FLAG (1U << 5)
#define ISO14443_4A_ATS_T0_TC1_FLAG (1U << 5)
#define ISO14443_4A_ATS_T0_FSCI_32 (2U << 0)
#define ISO14443_4A_ATS_T0_FSCI_64 (5U << 0)

// TA_1 bits
#define ISO14443_4A_ATS_TA1_SAME_D (1U << 7)

// TB_1 bits
#define ISO14443_4A_ATS_TB1_SFGI (0U << 0)
#define ISO14443_4A_ATS_TB1_FWI (14U << 4)

// TC_1 bits
#define ISO14443_4A_ATS_TC1_NAD_FLAG (1U << 0)
#define ISO14443_4A_ATS_TC1_CID_FLAG (1U << 1)

bool iso14443_4a_ats_parse(Iso14443_4aAtsData* data, const BitBuffer* buf) {
    // TODO better check
    const bool can_parse = bit_buffer_get_size_bytes(buf) == sizeof(Iso14443_4aAtsData);
    if(can_parse) {
        bit_buffer_write_bytes(buf, data, sizeof(Iso14443_4aAtsData));
    }
    return can_parse;
}

void iso14443_4a_ats_fill_default(Iso14443_4aAtsData* data) {
    data->tl = sizeof(Iso14443_4aAtsData);
    data->t0 = ISO14443_4A_ATS_T0_TA1_FLAG | ISO14443_4A_ATS_T0_TB1_FLAG |
               ISO14443_4A_ATS_T0_TC1_FLAG | ISO14443_4A_ATS_T0_FSCI_64;
    data->ta_1 = ISO14443_4A_ATS_TA1_SAME_D;
    data->tb_1 = ISO14443_4A_ATS_TB1_FWI | ISO14443_4A_ATS_TB1_SFGI;
    data->tc_1 = ISO14443_4A_ATS_TC1_CID_FLAG;
    data->t1 = 0;
}

Iso14443_4aError iso14443_4a_process_error(Iso14443_3aError error) {
    switch(error) {
    case Iso14443_3aErrorNone:
        return Iso14443_4aErrorNone;
    case Iso14443_3aErrorNotPresent:
        return Iso14443_4aErrorNotPresent;
    case Iso14443_3aErrorColResFailed:
    case Iso14443_3aErrorCommunication:
    case Iso14443_3aErrorWrongCrc:
        return Iso14443_4aErrorProtocol;
    case Iso14443_3aErrorTimeout:
        return Iso14443_4aErrorTimeout;
    default:
        return Iso14443_4aErrorProtocol;
    }
}
