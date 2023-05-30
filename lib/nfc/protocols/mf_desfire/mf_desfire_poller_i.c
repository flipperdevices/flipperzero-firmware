#include "mf_desfire_poller_i.h"

#include <furi.h>

#define TAG "MfDesfirePoller"

#define BITS_IN_BYTE (8)

MfDesfireError mf_desfire_process_error(Iso14443_4aError error) {
    switch(error) {
    case Iso14443_4aErrorNone:
        return MfDesfireErrorNone;
    case Iso14443_4aErrorNotPresent:
        return MfDesfireErrorNotPresent;
    case Iso14443_4aErrorTimeout:
        return MfDesfireErrorTimeout;
    default:
        return MfDesfireErrorProtocol;
    }
}

MfDesfireError mf_desfire_send_chunks(
    MfDesfirePoller* instance,
    const uint8_t* tx_data,
    size_t tx_data_size,
    uint8_t* rx_data,
    size_t rx_data_size,
    uint32_t fwt) {
    furi_assert(instance);
    furi_assert(instance->iso14443_4a_poller);
    furi_assert(instance->buffer);
    furi_assert(tx_data);
    furi_assert(rx_data);

    MfDesfireError ret = MfDesfireErrorNone;
    NfcPollerBuffer* buf = instance->buffer;

    memcpy(buf->tx_data, tx_data, tx_data_size);
    buf->tx_bits = tx_data_size * BITS_IN_BYTE;

    size_t rx_data_size_total = 0;

    do {
        Iso14443_4aError error = iso14443_4a_poller_send_block(
            instance->iso14443_4a_poller,
            buf->tx_data,
            buf->tx_bits,
            buf->rx_data,
            buf->rx_data_size,
            &buf->rx_bits,
            fwt);

        if(error != Iso14443_4aErrorNone) {
            ret = mf_desfire_process_error(error);
            break;
        }

        const size_t rx_data_size_current = buf->rx_bits / BITS_IN_BYTE - 1;

        if(rx_data_size_total + rx_data_size_current > rx_data_size) {
            ret = MfDesfireErrorProtocol;
            break;
        }

        memcpy(&rx_data[rx_data_size_total], &buf->rx_data[1], rx_data_size_current);
        rx_data_size_total += rx_data_size_current;

        buf->tx_data[0] = MF_DESFIRE_FLAG_HAS_NEXT;
        buf->tx_bits = BITS_IN_BYTE;

    } while(buf->rx_data[0] == MF_DESFIRE_FLAG_HAS_NEXT);

    return ret;
}

MfDesfireError
    mf_desfire_poller_async_read_version(MfDesfirePoller* instance, MfDesfireVersion* data) {
    const uint8_t cmd = MF_DESFIRE_CMD_GET_VERSION;
    return mf_desfire_send_chunks(
        instance,
        &cmd,
        sizeof(cmd),
        (uint8_t*)data,
        sizeof(MfDesfireVersion),
        MF_DESFIRE_POLLER_STANDARD_FWT_FC);
}

MfDesfireError
    mf_desfire_poller_async_read_free_memory(MfDesfirePoller* instance, MfDesfireFreeMemory* data) {
    const uint8_t cmd = MF_DESFIRE_CMD_GET_FREE_MEMORY;
    return mf_desfire_send_chunks(
        instance,
        &cmd,
        sizeof(cmd),
        (uint8_t*)data,
        sizeof(MfDesfireFreeMemory),
        MF_DESFIRE_POLLER_STANDARD_FWT_FC);
}
