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

    do {
        Iso14443_4aError error = iso14443_4a_poller_send_block(
            instance->iso14443_4a_poller, tx_buffer, instance->rx_buffer, fwt);

        if(error != Iso14443_4aErrorNone) {
            ret = mf_desfire_process_error(error);
            break;
        }

        const uint8_t flag_has_next = MF_DESFIRE_FLAG_HAS_NEXT;

        bit_buffer_copy_bytes(instance->tx_buffer, &flag_has_next, sizeof(flag_has_next));
        bit_buffer_copy_right(rx_buffer, instance->rx_buffer, sizeof(flag_has_next));

        while(bit_buffer_starts_with_byte(instance->rx_buffer, flag_has_next)) {
            Iso14443_4aError error = iso14443_4a_poller_send_block(
                instance->iso14443_4a_poller, instance->tx_buffer, instance->rx_buffer, fwt);

            if(error != Iso14443_4aErrorNone) {
                ret = mf_desfire_process_error(error);
                break;
            }

            bit_buffer_append_right(rx_buffer, instance->rx_buffer, sizeof(flag_has_next));
        }
    } while(false);

    return ret;
}

MfDesfireError
    mf_desfire_poller_async_read_version(MfDesfirePoller* instance, MfDesfireVersion* data) {
    furi_assert(instance);

    const uint8_t cmd = MF_DESFIRE_CMD_GET_VERSION;
    bit_buffer_copy_bytes(instance->input_buffer, &cmd, sizeof(cmd));

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

    MfDesfireError error = mf_desfire_send_chunks(
        instance,
        instance->input_buffer,
        instance->result_buffer,
        MF_DESFIRE_POLLER_STANDARD_FWT_FC);

    if(error == MfDesfireErrorNone) {
        bit_buffer_write_bytes(
            instance->result_buffer, &data->bytes_free, sizeof(data->bytes_free) - 1);
        data->bytes_free &= 0x00ffffff;
        data->is_present = true;
    } else {
        data->is_present = false;
    }

    return error;
}

MfDesfireError mf_desfire_poller_async_read_key_settings(
    MfDesfirePoller* instance,
    MfDesfireKeySettings* data) {
    furi_assert(instance);
    furi_assert(data);

    bit_buffer_set_size_bytes(instance->input_buffer, sizeof(uint8_t));
    bit_buffer_set_byte(instance->input_buffer, 0, MF_DESFIRE_CMD_GET_KEY_SETTINGS);

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

MfDesfireError mf_desfire_poller_async_read_key_version(
    MfDesfirePoller* instance,
    MfDesfireKeyVersion* data,
    size_t key_count) {
    furi_assert(instance);
    furi_assert(data);

    MfDesfireError error = MfDesfireErrorNone;

    bit_buffer_set_size_bytes(instance->input_buffer, sizeof(uint8_t) * 2);
    bit_buffer_set_byte(instance->input_buffer, 0, MF_DESFIRE_CMD_GET_KEY_VERSION);

    for(size_t key_id = 0; key_id < key_count; ++key_id) {
        bit_buffer_set_byte(instance->input_buffer, 1, key_id);

        error = mf_desfire_send_chunks(
            instance,
            instance->input_buffer,
            instance->result_buffer,
            MF_DESFIRE_POLLER_STANDARD_FWT_FC);

        if(error == MfDesfireErrorNone) {
            data[key_id] = bit_buffer_get_byte(instance->result_buffer, 0);
        } else {
            break;
        }
    }

    return error;
}

MfDesfireError mf_desfire_poller_async_read_key_configuration(
    MfDesfirePoller* instance,
    MfDesfireKeyConfiguration* data) {
    furi_assert(instance);
    furi_assert(data);
    furi_assert(data->key_versions == NULL);

    MfDesfireError error = MfDesfireErrorNone;

    do {
        error = mf_desfire_poller_async_read_key_settings(instance, &data->key_settings);
        if(error != MfDesfireErrorNone) break;

        const uint8_t key_count = data->key_settings.max_keys;
        data->key_versions = malloc(sizeof(MfDesfireKeyVersion) * key_count);

        error = mf_desfire_poller_async_read_key_version(instance, data->key_versions, key_count);
    } while(false);

    return error;
}
