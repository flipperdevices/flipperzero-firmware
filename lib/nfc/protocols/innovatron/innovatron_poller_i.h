#pragma once

#include "innovatron_poller.h"

#include <nfc/nfc.h>
#include <nfc/protocols/nfc_generic_event.h>
#include <toolbox/bit_buffer.h>

#define INNOVATRON_GUARD_TIME_US       (5000U)
#define INNOVATRON_APGEN_TR0_FC        (106000U)
#define INNOVATRON_RR_TR0_FC           (40700U)
#define INNOVATRON_COM_R_TR0_FC        (2712000U)
#define INNOVATRON_FDT_POLL_FC         (36864U)
#define INNOVATRON_APGEN_REPEAT_MIN_US (9820U)
#define INNOVATRON_POLLER_BUF_SIZE     (256U)

#define INNOVATRON_CMD_RR                         (0x01U)
#define INNOVATRON_CMD_REPGEN                     (0x07U)
#define INNOVATRON_CMD_APGEN                      (0x0BU)
#define INNOVATRON_CMD_ATTRIB                     (0x0FU)
#define INNOVATRON_APGEN_OCCUPAR_DEFAULT          (0x3FU)
#define INNOVATRON_APGEN_CONFIG_ATR_REQUEST_MASK  (1U << 7)
#define INNOVATRON_REPGEN_VERLOG_LONG_FORMAT_MASK (1U << 7)
#define INNOVATRON_REPGEN_VERLOG_FIXED_BITS_MASK  ((1U << 6) | (1U << 5) | (1U << 0))
#define INNOVATRON_REPGEN_CONFIG_ATR_PRESENT_MASK (1U << 6)
#define INNOVATRON_REPGEN_CONFIG_RFU_MASK         (0x3FU)

#define INNOVATRON_CMD_COM_DATA_FAMILY_COM_R (0x00U)
#define INNOVATRON_CMD_COM_DATA_FAMILY_REC   (0x40U)
#define INNOVATRON_CMD_COM_DATA_SEQ_MASK     (0x0EU)
#define INNOVATRON_CMD_COM_DATA_SEQ_SHIFT    (1U)
#define INNOVATRON_COM_SEQUENCE_START        (1U)
#define INNOVATRON_COM_SEQUENCE_END          (7U)
#define INNOVATRON_CMD_COM_R(seq)           \
    (INNOVATRON_CMD_COM_DATA_FAMILY_COM_R | \
     (((seq) << INNOVATRON_CMD_COM_DATA_SEQ_SHIFT) & INNOVATRON_CMD_COM_DATA_SEQ_MASK))
#define INNOVATRON_CMD_REC(seq)           \
    (INNOVATRON_CMD_COM_DATA_FAMILY_REC | \
     (((seq) << INNOVATRON_CMD_COM_DATA_SEQ_SHIFT) & INNOVATRON_CMD_COM_DATA_SEQ_MASK))

struct InnovatronPoller {
    Nfc* nfc;
    BitBuffer* tx_buffer;
    BitBuffer* rx_buffer;

    InnovatronData* data;
    bool activated;
    uint8_t com_sequence;

    NfcGenericCallback callback;
    void* context;

    NfcGenericEvent general_event;
    InnovatronPollerEvent innovatron_event;
    InnovatronPollerEventData innovatron_event_data;
};

InnovatronError innovatron_poller_apgen(InnovatronPoller* instance, InnovatronData* data);
