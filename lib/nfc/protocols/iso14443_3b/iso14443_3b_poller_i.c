#include "iso14443_3b_poller_i.h"

#define TAG "Iso14443_3bPoller"

Iso14443_3bError
    iso14443_3b_poller_async_activate(Iso14443_3bPoller* instance, Iso14443_3bData* data) {
    furi_assert(instance);
    furi_assert(instance->nfc);
    UNUSED(data);

    iso14443_3b_reset(instance->data);
    bit_buffer_reset(instance->tx_buffer);
    bit_buffer_reset(instance->rx_buffer);

    // Halt if necessary
    if(instance->state != Iso14443_3bPollerStateIdle) {
        iso14443_3b_poller_halt(instance);
    }

    NfcError error = NfcErrorNone;
    Iso14443_3bError ret = Iso14443_3bErrorNone;

    // bool activated = false;

    bit_buffer_append_byte(instance->tx_buffer, 0x05);
    bit_buffer_append_byte(instance->tx_buffer, 0x00);
    bit_buffer_append_byte(instance->tx_buffer, 0x08);

    iso14443_3b_append_crc(instance->tx_buffer);

    do {
        error = nfc_trx(
            instance->nfc, instance->tx_buffer, instance->rx_buffer, ISO14443_3B_FDT_POLL_FC);
        if(error != NfcErrorNone) {
            FURI_LOG_D(TAG, "Failed to send/receive REQB: %d", error);
            break;
        }

        if(!iso14443_3b_check_crc(instance->rx_buffer)) {
            FURI_LOG_D(TAG, "Wrong ATQB CRC");
            break;
        }

        iso14443_3b_trim_crc(instance->rx_buffer);

        if(bit_buffer_get_size_bytes(instance->rx_buffer) != sizeof(Iso14443_3bAtqB)) {
            FURI_LOG_D(TAG, "Wrong ATQB size");
            break;
        }

        const Iso14443_3bAtqB* atqb =
            (const Iso14443_3bAtqB*)bit_buffer_get_data(instance->rx_buffer);

        memcpy(instance->data->uid, atqb->uid, ISO14443_3B_UID_SIZE);
        memcpy(instance->data->app_data, atqb->app_data, ISO14443_3B_APP_DATA_SIZE);
        memcpy(instance->data->protocol_data, atqb->app_data, ISO14443_3B_PROTOCOL_DATA_SIZE);
    } while(false);

    return ret;
}

Iso14443_3bError iso14443_3b_poller_halt(Iso14443_3bPoller* instance) {
    furi_assert(instance);

    instance->state = Iso14443_3bPollerStateIdle;
    return Iso14443_3bErrorNone;
}
