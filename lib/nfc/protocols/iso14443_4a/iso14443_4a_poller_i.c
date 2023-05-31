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
    const BitBuffer* tx_buffer,
    BitBuffer* rx_buffer,
    uint32_t fwt) {
    furi_assert(instance);
    furi_assert(instance->iso14443_3a_poller);
    furi_assert(instance->buffer);
    furi_assert(tx_buffer);
    furi_assert(rx_buffer);

    const uint8_t pcb = ISO14443_4A_PCB_I | instance->protocol_data.block_number;
    instance->protocol_data.block_number ^= 1;

    bit_buffer_copy_prepend_bytes(instance->tx_buffer, tx_buffer, &pcb, sizeof(pcb));

    Iso14443_4aError ret = Iso14443_4aErrorNone;

    do {
        // TODO: Adapt lower methods to use BitBuffers
        uint16_t rx_size_bits;
        NfcaError error = nfca_poller_send_standart_frame(
            instance->iso14443_3a_poller,
            bit_buffer_get_data(instance->tx_buffer),
            bit_buffer_get_size(instance->tx_buffer),
            bit_buffer_get_data(instance->rx_buffer),
            bit_buffer_get_capacity_bytes(instance->rx_buffer),
            &rx_size_bits,
            fwt);

        if(error != NfcaErrorNone) {
            FURI_LOG_E(TAG, "Iso14443-3 error: %d", error);
            ret = iso14443_4a_poller_process_error(error);
            break;
        } else if(bit_buffer_get_byte(instance->rx_buffer, 0) != pcb) {
            ret = Iso14443_4aErrorProtocol;
            break;
        }

        // TODO: Do not set the size directly
        bit_buffer_set_size(instance->rx_buffer, rx_size_bits);
        bit_buffer_copy_right(rx_buffer, instance->rx_buffer, sizeof(pcb));

        ret = Iso14443_4aErrorNone;
    } while(false);

    return ret;
}
