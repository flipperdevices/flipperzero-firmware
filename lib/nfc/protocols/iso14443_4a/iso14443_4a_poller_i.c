#include "iso14443_4a_poller_i.h"

#include <furi.h>

#define TAG "Iso14443_4aPoller"

#define BITS_IN_BYTE (8)

#define ISO14443_4A_ATS_BIT (1 << 5)
#define ISO14443_4A_PCB_I (0x02)

Iso14443_4aError iso14443_4a_poller_process_error(NfcaError error) {
    switch(error) {
    case NfcaErrorNone:
        return Iso14443_4aErrorNone;
    case NfcaErrorNotPresent:
        return Iso14443_4aErrorNotPresent;
    case NfcaErrorColResFailed:
    case NfcaErrorCommunication:
    case NfcaErrorWrongCrc:
        return Iso14443_4aErrorProtocol;
    case NfcaErrorTimeout:
        return Iso14443_4aErrorTimeout;
    default:
        return Iso14443_4aErrorProtocol;
    }
}

Iso14443_4aError iso14443_4a_poller_halt(Iso14443_4aPoller* instance) {
    furi_assert(instance);
    furi_assert(instance->iso14443_3a_poller);

    nfca_poller_halt(instance->iso14443_3a_poller);
    instance->state = Iso14443_4aPollerStateIdle;

    return Iso14443_4aErrorNone;
}

Iso14443_4aError iso14443_4a_poller_async_read_ats(Iso14443_4aPoller* instance) {
    furi_assert(instance);
    furi_assert(instance->iso14443_3a_poller);

    Iso14443_4aError ret = Iso14443_4aErrorProtocol;

    do {
        // Check whether ATS is available
        if(!(instance->data->iso14443_3a_data.sak & ISO14443_4A_ATS_BIT)) {
            FURI_LOG_E(TAG, "Ats not supported: not an ISO14443-4 card");
            break;
        }

        // Send RATS & receive ATS
        instance->protocol_data.ats_request.cmd = 0xe0;
        instance->protocol_data.ats_request.param = 0x80;

        uint16_t rx_bits;

        NfcaError error = nfca_poller_send_standart_frame(
            instance->iso14443_3a_poller,
            (uint8_t*)&instance->protocol_data.ats_request,
            BITS_IN_BYTE * sizeof(instance->protocol_data.ats_request),
            (uint8_t*)&instance->protocol_data.ats_response,
            BITS_IN_BYTE * sizeof(instance->protocol_data.ats_response),
            &rx_bits,
            NFCA_FDT_LISTEN_FC);

        if(error != NfcaErrorNone) {
            FURI_LOG_E(TAG, "Ats request failed: %d", error);
            break;
        } else if(rx_bits != BITS_IN_BYTE * sizeof(instance->protocol_data.ats_response)) {
            FURI_LOG_E(TAG, "Ats response wrong length: %d bits", rx_bits);
            break;
        }

        ret = Iso14443_4aErrorNone;
    } while(false);

    return ret;
}

Iso14443_4aError iso14443_4a_poller_send_block(
    Iso14443_4aPoller* instance,
    uint8_t* tx_data,
    uint16_t tx_bits,
    uint8_t* rx_data,
    uint16_t rx_data_size,
    uint16_t* rx_bits,
    uint32_t fwt) {
    furi_assert(instance);
    furi_assert(instance->iso14443_3a_poller);
    furi_assert(instance->buffer);
    furi_assert(tx_data);
    furi_assert(rx_data);
    furi_assert(rx_bits);
    furi_assert(tx_bits >= BITS_IN_BYTE);

    NfcPollerBuffer* buf = instance->buffer;

    const uint16_t tx_data_size = tx_bits / BITS_IN_BYTE;
    furi_assert(tx_data_size <= buf->tx_data_size - 1);

    const uint8_t pcb = ISO14443_4A_PCB_I | instance->protocol_data.block_number;
    instance->protocol_data.block_number ^= 1;

    buf->tx_data[0] = pcb;
    buf->tx_bits = tx_bits + BITS_IN_BYTE;

    memcpy(&buf->tx_data[1], tx_data, tx_data_size);

    Iso14443_4aError ret = Iso14443_4aErrorNone;

    do {
        NfcaError error = nfca_poller_send_standart_frame(
            instance->iso14443_3a_poller,
            buf->tx_data,
            buf->tx_bits,
            buf->rx_data,
            buf->rx_data_size,
            &buf->rx_bits,
            fwt);

        if(error != NfcaErrorNone) {
            FURI_LOG_E(TAG, "Iso14443-3 error: %d", error);
            ret = iso14443_4a_poller_process_error(error);
            break;
        } else if((buf->rx_bits < BITS_IN_BYTE) || (buf->rx_data[0] != pcb)) {
            ret = Iso14443_4aErrorProtocol;
            break;
        }

        const uint16_t rx_data_size_actual = buf->rx_bits / BITS_IN_BYTE - 1;

        if(rx_data_size < rx_data_size_actual) {
            ret = Iso14443_4aErrorProtocol;
            break;
        }

        memcpy(rx_data, &buf->rx_data[1], rx_data_size_actual);
        *rx_bits = rx_data_size_actual * BITS_IN_BYTE;

        ret = Iso14443_4aErrorNone;
    } while(false);

    return ret;
}
