#include "mf_desfire_poller_i.h"

#include <furi.h>

#define TAG "MfDesfirePoller"

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
    const BitBuffer* tx_buffer,
    BitBuffer* rx_buffer,
    uint32_t fwt) {
    furi_assert(instance);
    furi_assert(instance->iso14443_4a_poller);
    furi_assert(instance->tx_buffer);
    furi_assert(instance->rx_buffer);
    furi_assert(tx_buffer);
    furi_assert(rx_buffer);

    MfDesfireError ret = MfDesfireErrorNone;
    bool is_first_chunk = true;
    const uint8_t flag_has_next = MF_DESFIRE_FLAG_HAS_NEXT;

    bit_buffer_copy(instance->tx_buffer, tx_buffer);

    do {
        Iso14443_4aError error = iso14443_4a_poller_send_block(
            instance->iso14443_4a_poller, instance->tx_buffer, instance->rx_buffer, fwt);

        if(error != Iso14443_4aErrorNone) {
            ret = mf_desfire_process_error(error);
            break;
        }

        bit_buffer_append_right(rx_buffer, instance->rx_buffer, sizeof(flag_has_next));

        if(is_first_chunk) {
            bit_buffer_copy_bytes(instance->tx_buffer, &flag_has_next, sizeof(flag_has_next));
            is_first_chunk = false;
        }

    } while(bit_buffer_starts_with_byte(instance->rx_buffer, flag_has_next));

    return ret;
}

MfDesfireError
    mf_desfire_poller_async_read_version(MfDesfirePoller* instance, MfDesfireVersion* data) {
    furi_assert(instance);

    const uint8_t cmd = MF_DESFIRE_CMD_GET_VERSION;
    bit_buffer_copy_bytes(instance->input_buffer, &cmd, sizeof(cmd));
    bit_buffer_reset(instance->result_buffer);

    MfDesfireError error = mf_desfire_send_chunks(
        instance,
        instance->input_buffer,
        instance->result_buffer,
        MF_DESFIRE_POLLER_STANDARD_FWT_FC);

    if(error == MfDesfireErrorNone) {
        bit_buffer_write_bytes(instance->result_buffer, data, sizeof(*data));
    }

    return error;
}

MfDesfireError
    mf_desfire_poller_async_read_free_memory(MfDesfirePoller* instance, MfDesfireFreeMemory* data) {
    furi_assert(instance);

    const uint8_t cmd = MF_DESFIRE_CMD_GET_FREE_MEMORY;
    bit_buffer_copy_bytes(instance->input_buffer, &cmd, sizeof(cmd));
    bit_buffer_reset(instance->result_buffer);

    MfDesfireError error = mf_desfire_send_chunks(
        instance,
        instance->input_buffer,
        instance->result_buffer,
        MF_DESFIRE_POLLER_STANDARD_FWT_FC);

    if(error == MfDesfireErrorNone) {
        bit_buffer_write_bytes(instance->result_buffer, data, sizeof(*data));
    }

    return error;
}
