#include "iso14443_4b_i.h"

// T0 bits
#define ISO14443_4B_ATS_T0_TA1_FLAG (1U << 4)
#define ISO14443_4B_ATS_T0_TB1_FLAG (1U << 5)
#define ISO14443_4B_ATS_T0_TC1_FLAG (1U << 6)

// TA_1 bits
#define ISO14443_4B_ATS_TA1_SAME_D (1U << 7)

// TB_1 bits
#define ISO14443_4B_ATS_TB1_SFGI (0U << 0)
#define ISO14443_4B_ATS_TB1_FWI (14U << 4)

// TC_1 bits
#define ISO14443_4B_ATS_TC1_NAD_FLAG (1U << 0)
#define ISO14443_4B_ATS_TC1_CID_FLAG (1U << 1)

bool iso14443_4b_ats_parse(SimpleArray* data, const BitBuffer* buf) {
    bool can_parse = false;

    do {
        const size_t buf_size = bit_buffer_get_size_bytes(buf);
        if(buf_size < sizeof(Iso14443_4bAtsData)) break;

        const size_t ats_size = bit_buffer_get_byte(buf, 0);
        if(ats_size != buf_size) break;

        simple_array_init(data, ats_size);
        bit_buffer_write_bytes(buf, simple_array_get_data(data), ats_size);

        can_parse = true;
    } while(false);

    return can_parse;
}

void iso14443_4b_ats_fill_default(SimpleArray* data) {
    simple_array_init(data, sizeof(Iso14443_4bAtsData));

    Iso14443_4bAtsData* ats_data = simple_array_get_data(data);

    ats_data->tl = sizeof(Iso14443_4bAtsData);
    ats_data->t0 = ISO14443_4B_ATS_T0_TA1_FLAG | ISO14443_4B_ATS_T0_TB1_FLAG |
                   ISO14443_4B_ATS_T0_TC1_FLAG | ISO14443_4B_FSCI_256;
    ats_data->ta_1 = ISO14443_4B_ATS_TA1_SAME_D;
    ats_data->tb_1 = ISO14443_4B_ATS_TB1_FWI | ISO14443_4B_ATS_TB1_SFGI;
    ats_data->tc_1 = ISO14443_4B_ATS_TC1_CID_FLAG;
}

Iso14443_4bError iso14443_4b_process_error(Iso14443_3bError error) {
    switch(error) {
    case Iso14443_3bErrorNone:
        return Iso14443_4bErrorNone;
    case Iso14443_3bErrorNotPresent:
        return Iso14443_4bErrorNotPresent;
    case Iso14443_3bErrorColResFailed:
    case Iso14443_3bErrorCommunication:
    case Iso14443_3bErrorWrongCrc:
        return Iso14443_4bErrorProtocol;
    case Iso14443_3bErrorTimeout:
        return Iso14443_4bErrorTimeout;
    default:
        return Iso14443_4bErrorProtocol;
    }
}
