#include "iso14443_4a_poller_i.h"

#include <furi.h>

#define TAG "Iso14443_4aPoller"

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
        if(!(instance->data->iso14443_3a_data.sak & (1U << 5))) {
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
            8 * sizeof(instance->protocol_data.ats_request),
            (uint8_t*)&instance->protocol_data.ats_response,
            8 * sizeof(instance->protocol_data.ats_response),
            &rx_bits,
            NFCA_FDT_LISTEN_FC);

        if(error != NfcaErrorNone) {
            FURI_LOG_E(TAG, "Ats request failed: %d", error);
            break;
        } else if(rx_bits != 8 * sizeof(instance->protocol_data.ats_response)) {
            FURI_LOG_E(TAG, "Ats response wrong length: %d bits", rx_bits);
            break;
        }

        ret = Iso14443_4aErrorNone;
    } while(false);

    return ret;
}

Iso14443_4aError iso14443_4a_poller_send_frame(
    Iso14443_4aPoller* instance,
    uint8_t* tx_data,
    uint16_t tx_bits,
    uint8_t* rx_data,
    uint16_t rx_data_size,
    uint16_t* rx_bits,
    uint32_t fwt) {
    furi_assert(instance);
    furi_assert(instance->iso14443_3a_poller);
    furi_assert(tx_data);
    furi_assert(rx_data);
    furi_assert(rx_bits);
    furi_assert(tx_bits >= 8);

    *rx_bits = rx_data_size * 8;
    UNUSED(fwt);

    Iso14443_4aError ret = Iso14443_4aErrorNone;

    return ret;
}
