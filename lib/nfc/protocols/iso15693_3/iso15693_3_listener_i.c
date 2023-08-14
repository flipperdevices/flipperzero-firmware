#include "iso15693_3_listener_i.h"

#include <nfc/helpers/iso13239_crc.h>

#define TAG "Iso15693_3Listener"

typedef Iso15693_3Error (
    *Iso15693_3ListenerHandler)(Iso15693_3Listener* instance, const BitBuffer* rx_buffer);

static Iso15693_3Error iso15693_3_listener_inventory_handler(
    Iso15693_3Listener* instance,
    const BitBuffer* rx_buffer) {
    Iso15693_3Error error = Iso15693_3ErrorNone;

    do {
        if(bit_buffer_get_byte(rx_buffer, 1) != ISO15693_3_CMD_INVENTORY) {
            error = Iso15693_3ErrorNotSupported;
            break;
        }

        bit_buffer_reset(instance->tx_buffer);

        bit_buffer_append_byte(instance->tx_buffer, ISO15693_3_RESP_FLAG_NONE); // Flags
        bit_buffer_append_byte(instance->tx_buffer, instance->data->system_info.dsfid); // DSFID

        for(size_t i = 0; i < ISO15693_3_UID_SIZE; ++i) {
            // Reverse the UID
            bit_buffer_append_byte(
                instance->tx_buffer, instance->data->uid[ISO15693_3_UID_SIZE - i - 1]);
        }

        error = iso15693_3_listener_send_frame(instance, instance->tx_buffer);

    } while(false);

    return error;
}

static Iso15693_3Error iso15693_3_listener_get_system_info_handler(
    Iso15693_3Listener* instance,
    const BitBuffer* rx_buffer) {
    Iso15693_3Error error = Iso15693_3ErrorNone;

    do {
        if(bit_buffer_get_byte(rx_buffer, 1) != ISO15693_3_CMD_GET_SYS_INFO) {
            error = Iso15693_3ErrorNotSupported;
            break;
        }

        // TODO: If UID is present, do something with it?

        bit_buffer_reset(instance->tx_buffer);

        bit_buffer_append_byte(instance->tx_buffer, ISO15693_3_RESP_FLAG_NONE); // Flags
        bit_buffer_append_byte(instance->tx_buffer, instance->data->system_info.flags);

        for(size_t i = 0; i < ISO15693_3_UID_SIZE; ++i) {
            // Reverse the UID
            bit_buffer_append_byte(
                instance->tx_buffer, instance->data->uid[ISO15693_3_UID_SIZE - i - 1]);
        }

        const uint8_t flags = instance->data->system_info.flags;

        if(flags & ISO15693_3_SYSINFO_FLAG_DSFID) {
            bit_buffer_append_byte(instance->tx_buffer, instance->data->system_info.dsfid);
        }
        if(flags & ISO15693_3_SYSINFO_FLAG_AFI) {
            bit_buffer_append_byte(instance->tx_buffer, instance->data->system_info.afi);
        }
        if(flags & ISO15693_3_SYSINFO_FLAG_MEMORY) {
            const uint8_t memory_info[2] = {
                instance->data->system_info.block_count - 1,
                instance->data->system_info.block_size - 1,
            };
            bit_buffer_append_bytes(instance->tx_buffer, memory_info, COUNT_OF(memory_info));
        }
        if(flags & ISO15693_3_SYSINFO_FLAG_IC_REF) {
            bit_buffer_append_byte(instance->tx_buffer, instance->data->system_info.ic_ref);
        }

        error = iso15693_3_listener_send_frame(instance, instance->tx_buffer);

    } while(false);

    return error;
}

static const Iso15693_3ListenerHandler iso15693_listener_handlers[] = {
    iso15693_3_listener_inventory_handler,
    iso15693_3_listener_get_system_info_handler,
};

static Iso15693_3Error iso15693_3_listener_process_nfc_error(NfcError error) {
    Iso15693_3Error ret = Iso15693_3ErrorNone;

    if(error == NfcErrorNone) {
        ret = Iso15693_3ErrorNone;
    } else if(error == NfcErrorTimeout) {
        ret = Iso15693_3ErrorTimeout;
    } else {
        ret = Iso15693_3ErrorFieldOff;
    }

    return ret;
}

Iso15693_3Error iso15693_3_listener_sleep(Iso15693_3Listener* instance) {
    furi_assert(instance);
    UNUSED(instance);

    return Iso15693_3ErrorNone;
}

Iso15693_3Error
    iso15693_3_listener_send_frame(Iso15693_3Listener* instance, const BitBuffer* tx_buffer) {
    furi_assert(instance);
    furi_assert(tx_buffer);

    bit_buffer_copy(instance->tx_buffer, tx_buffer);
    iso13239_crc_append(Iso13239CrcTypeDefault, instance->tx_buffer);

    NfcError error = nfc_listener_tx(instance->nfc, instance->tx_buffer);
    return iso15693_3_listener_process_nfc_error(error);
}

Iso15693_3Error
    iso15693_3_listener_process_request(Iso15693_3Listener* instance, const BitBuffer* rx_buffer) {
    Iso15693_3Error error = Iso15693_3ErrorNone;

    // TODO: Preliminary checks
    for(uint32_t i = 0; i < COUNT_OF(iso15693_listener_handlers); ++i) {
        error = iso15693_listener_handlers[i](instance, rx_buffer);
        if(error == Iso15693_3ErrorNone) break;
    }

    return error;
}
