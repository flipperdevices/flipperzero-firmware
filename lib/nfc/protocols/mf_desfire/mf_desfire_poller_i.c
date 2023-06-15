#include "mf_desfire_poller_i.h"

#include <furi.h>

#include "mf_desfire_i.h"

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

        bit_buffer_reset(instance->tx_buffer);
        bit_buffer_append_byte(instance->tx_buffer, MF_DESFIRE_FLAG_HAS_NEXT);

        if(bit_buffer_get_size_bytes(instance->rx_buffer) > sizeof(uint8_t)) {
            bit_buffer_copy_right(rx_buffer, instance->rx_buffer, sizeof(uint8_t));
        } else {
            bit_buffer_reset(rx_buffer);
        }

        while(bit_buffer_starts_with_byte(instance->rx_buffer, MF_DESFIRE_FLAG_HAS_NEXT)) {
            Iso14443_4aError error = iso14443_4a_poller_send_block(
                instance->iso14443_4a_poller, instance->tx_buffer, instance->rx_buffer, fwt);

            if(error != Iso14443_4aErrorNone) {
                ret = mf_desfire_process_error(error);
                break;
            }

            bit_buffer_append_right(rx_buffer, instance->rx_buffer, sizeof(uint8_t));
        }
    } while(false);

    return ret;
}

MfDesfireError
    mf_desfire_poller_async_read_version(MfDesfirePoller* instance, MfDesfireVersion* data) {
    furi_assert(instance);

    bit_buffer_reset(instance->input_buffer);
    bit_buffer_append_byte(instance->input_buffer, MF_DESFIRE_CMD_GET_VERSION);

    MfDesfireError error = mf_desfire_send_chunks(
        instance,
        instance->input_buffer,
        instance->result_buffer,
        MF_DESFIRE_POLLER_STANDARD_FWT_FC);

    if(error == MfDesfireErrorNone) {
        mf_desfire_version_parse(data, instance->result_buffer);
    }

    return error;
}

MfDesfireError
    mf_desfire_poller_async_read_free_memory(MfDesfirePoller* instance, MfDesfireFreeMemory* data) {
    furi_assert(instance);

    bit_buffer_reset(instance->input_buffer);
    bit_buffer_append_byte(instance->input_buffer, MF_DESFIRE_CMD_GET_FREE_MEMORY);

    MfDesfireError error = mf_desfire_send_chunks(
        instance,
        instance->input_buffer,
        instance->result_buffer,
        MF_DESFIRE_POLLER_STANDARD_FWT_FC);

    if(error == MfDesfireErrorNone) {
        mf_desfire_free_memory_parse(data, instance->result_buffer);
    }

    return error;
}

MfDesfireError mf_desfire_poller_async_read_key_settings(
    MfDesfirePoller* instance,
    MfDesfireKeySettings* data) {
    furi_assert(instance);

    bit_buffer_reset(instance->input_buffer);
    bit_buffer_append_byte(instance->input_buffer, MF_DESFIRE_CMD_GET_KEY_SETTINGS);

    MfDesfireError error = mf_desfire_send_chunks(
        instance,
        instance->input_buffer,
        instance->result_buffer,
        MF_DESFIRE_POLLER_STANDARD_FWT_FC);

    if(error == MfDesfireErrorNone) {
        mf_desfire_key_settings_parse(data, instance->result_buffer);
    }

    return error;
}

MfDesfireError mf_desfire_poller_async_read_key_versions(
    MfDesfirePoller* instance,
    SimpleArray* data,
    uint32_t count) {
    furi_assert(instance);
    furi_assert(count > 0);

    simple_array_init(data, count);

    bit_buffer_set_size_bytes(instance->input_buffer, sizeof(uint8_t) * 2);
    bit_buffer_set_byte(instance->input_buffer, 0, MF_DESFIRE_CMD_GET_KEY_VERSION);

    MfDesfireError error = MfDesfireErrorNone;

    for(uint32_t i = 0; i < count; ++i) {
        bit_buffer_set_byte(instance->input_buffer, 1, i);

        error = mf_desfire_send_chunks(
            instance,
            instance->input_buffer,
            instance->result_buffer,
            MF_DESFIRE_POLLER_STANDARD_FWT_FC);

        if(error != MfDesfireErrorNone) break;

        mf_desfire_key_version_parse(simple_array_get(data, i), instance->result_buffer);
    }

    return error;
}

MfDesfireError mf_desfire_poller_async_read_key_configuration(
    MfDesfirePoller* instance,
    MfDesfireKeyConfiguration* data) {
    furi_assert(instance);
    furi_assert(data);

    MfDesfireError error;

    do {
        error = mf_desfire_poller_async_read_key_settings(instance, &data->key_settings);
        if(error != MfDesfireErrorNone) break;
        error = mf_desfire_poller_async_read_key_versions(
            instance, data->key_versions, data->key_settings.max_keys);
    } while(false);

    return error;
}

MfDesfireError
    mf_desfire_poller_async_read_application_ids(MfDesfirePoller* instance, SimpleArray* data) {
    furi_assert(instance);

    bit_buffer_reset(instance->input_buffer);
    bit_buffer_append_byte(instance->input_buffer, MF_DESFIRE_CMD_GET_APPLICATION_IDS);

    MfDesfireError error;

    do {
        error = mf_desfire_send_chunks(
            instance,
            instance->input_buffer,
            instance->result_buffer,
            MF_DESFIRE_POLLER_STANDARD_FWT_FC);

        if(error != MfDesfireErrorNone) break;

        const uint32_t ids_count =
            bit_buffer_get_size_bytes(instance->result_buffer) / sizeof(MfDesfireApplicationId);
        if(ids_count == 0) break;

        simple_array_init(data, ids_count);

        for(uint32_t i = 0; i < ids_count; ++i) {
            mf_desfire_application_id_parse(simple_array_get(data, i), i, instance->result_buffer);
        }
    } while(false);

    return error;
}

MfDesfireError mf_desfire_poller_async_select_application(
    MfDesfirePoller* instance,
    const MfDesfireApplicationId id) {
    furi_assert(instance);

    bit_buffer_reset(instance->input_buffer);
    bit_buffer_append_byte(instance->input_buffer, MF_DESFIRE_CMD_SELECT_APPLICATION);
    bit_buffer_append_bytes(instance->input_buffer, id, sizeof(MfDesfireApplicationId));

    MfDesfireError error = mf_desfire_send_chunks(
        instance,
        instance->input_buffer,
        instance->result_buffer,
        MF_DESFIRE_POLLER_STANDARD_FWT_FC);

    return error;
}

MfDesfireError mf_desfire_poller_async_read_application(
    MfDesfirePoller* instance,
    MfDesfireApplication* data) {
    furi_assert(instance);
    UNUSED(instance);
    UNUSED(data);

    MfDesfireError error = MfDesfireErrorNone;

    return error;
}

MfDesfireError mf_desfire_poller_async_read_applications(
    MfDesfirePoller* instance,
    const SimpleArray* app_ids,
    SimpleArray* data) {
    furi_assert(instance);

    MfDesfireError error = MfDesfireErrorNone;

    const uint32_t app_id_count = simple_array_get_count(app_ids);
    if(app_id_count > 0) {
        simple_array_init(data, app_id_count);
    }

    for(uint32_t i = 0; i < app_id_count; ++i) {
        do {
            error = mf_desfire_poller_async_select_application(
                instance, simple_array_cget(app_ids, i));
            if(error != MfDesfireErrorNone) break;

            MfDesfireApplication* current_app = simple_array_get(data, i);
            error = mf_desfire_poller_async_read_application(instance, current_app);

        } while(false);
    }

    return error;
}
