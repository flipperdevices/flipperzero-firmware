#include "slix_listener_i.h"

#include <nfc/protocols/iso15693_3/iso15693_3_listener_i.h>

typedef SlixError (*SlixRequestHandler)(
    SlixListener* instance,
    const uint8_t* data,
    size_t data_size,
    uint8_t flags);

static SlixError slix_get_nxp_system_info_handler(
    SlixListener* instance,
    const uint8_t* data,
    size_t data_size,
    uint8_t flags) {
    UNUSED(data);
    UNUSED(data_size);
    UNUSED(flags);
    SlixError error = SlixErrorNone;

    do {
        // TODO: Do not hardcode the response
        const SlixData* slix_data = instance->data;
        bit_buffer_append_byte(instance->tx_buffer, slix_data->protection.pointer);
        bit_buffer_append_byte(instance->tx_buffer, slix_data->protection.condition);
        bit_buffer_append_byte(instance->tx_buffer, 0x00);
        bit_buffer_append_byte(instance->tx_buffer, 0x7F);
        bit_buffer_append_byte(instance->tx_buffer, 0x35);
        bit_buffer_append_byte(instance->tx_buffer, 0x00);
        bit_buffer_append_byte(instance->tx_buffer, 0x00);
    } while(false);

    return error;
}

static SlixError slix_read_signature_handler(
    SlixListener* instance,
    const uint8_t* data,
    size_t data_size,
    uint8_t flags) {
    UNUSED(data);
    UNUSED(data_size);
    UNUSED(flags);
    SlixError error = SlixErrorNone;

    do {
        const SlixData* slix_data = instance->data;
        bit_buffer_append_bytes(
            instance->tx_buffer, slix_data->signature.data, SLIX_SIGNATURE_SIZE);
    } while(false);

    return error;
}

static SlixError slix_process_iso15693_3_error(Iso15693_3Error iso15693_3_error) {
    // TODO: Handle more error types?
    switch(iso15693_3_error) {
    case Iso15693_3ErrorNone:
        return SlixErrorNone;
    default:
        return SlixErrorUnknown;
    }
}

SlixError slix_listener_process_request(SlixListener* instance, const BitBuffer* rx_buffer) {
    SlixError error = SlixErrorNone;

    do {
        typedef struct {
            uint8_t flags;
            uint8_t command;
            uint8_t manufacturer;
            uint8_t data[];
        } SlixRequestLayout;

        const size_t buf_size = bit_buffer_get_size_bytes(rx_buffer);
        const size_t buf_size_min = sizeof(SlixRequestLayout);

        if(buf_size < buf_size_min) {
            error = SlixErrorFormat;
            break;
        }

        const SlixRequestLayout* request =
            (const SlixRequestLayout*)bit_buffer_get_data(rx_buffer);

        SlixRequestHandler handler;

        switch(request->command) {
        case SLIX_CMD_GET_NXP_SYSINFO:
            handler = slix_get_nxp_system_info_handler;
            break;
        case SLIX_CMD_READ_SIGNATURE:
            handler = slix_read_signature_handler;
            break;
        default:
            handler = NULL;
            break;
        }

        if(handler == NULL) {
            error = SlixErrorNotSupported;
            break;
        }

        bit_buffer_reset(instance->tx_buffer);
        bit_buffer_append_byte(instance->tx_buffer, ISO15693_3_RESP_FLAG_NONE);

        error = handler(instance, request->data, buf_size - buf_size_min, request->flags);

        if(error != SlixErrorNone) {
            bit_buffer_reset(instance->tx_buffer);
            bit_buffer_append_byte(instance->tx_buffer, ISO15693_3_RESP_FLAG_ERROR);
            bit_buffer_append_byte(instance->tx_buffer, ISO15693_3_RESP_ERROR_UNKNOWN);
        }

        const Iso15693_3Error iso15693_error =
            iso15693_3_listener_send_frame(instance->iso15693_3_listener, instance->tx_buffer);
        error = slix_process_iso15693_3_error(iso15693_error);
    } while(false);

    return error;
}
