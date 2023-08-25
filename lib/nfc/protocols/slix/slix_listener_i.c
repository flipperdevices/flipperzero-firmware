#include "slix_listener_i.h"

#include <nfc/protocols/iso15693_3/iso15693_3_listener_i.h>

#include <furi_hal_random.h>

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

    const SlixData* slix_data = instance->data;

    const SlixProtection* protection = &slix_data->system_info.protection;
    bit_buffer_append_byte(instance->tx_buffer, protection->pointer);
    bit_buffer_append_byte(instance->tx_buffer, protection->condition);

    const SlixLockBits* lock_bits = &slix_data->system_info.lock_bits;
    bit_buffer_append_byte(instance->tx_buffer, lock_bits->data);

    const uint32_t feature_flags = SLIX2_FEATURE_FLAGS;
    bit_buffer_append_bytes(
        instance->tx_buffer, (const uint8_t*)&feature_flags, sizeof(feature_flags));

    return SlixErrorNone;
}

static SlixError slix_get_random_number_handler(
    SlixListener* instance,
    const uint8_t* data,
    size_t data_size,
    uint8_t flags) {
    UNUSED(data);
    UNUSED(data_size);
    UNUSED(flags);

    union {
        uint16_t value;
        uint8_t bytes[sizeof(uint16_t)];
    } random_value;

    random_value.bytes[0] = furi_hal_random_get();
    random_value.bytes[1] = furi_hal_random_get();

    bit_buffer_append_bytes(instance->tx_buffer, random_value.bytes, sizeof(random_value));
    instance->session_state.random = random_value.value;

    return SlixErrorNone;
}

static SlixError slix_read_signature_handler(
    SlixListener* instance,
    const uint8_t* data,
    size_t data_size,
    uint8_t flags) {
    UNUSED(data);
    UNUSED(data_size);
    UNUSED(flags);

    const SlixData* slix_data = instance->data;
    bit_buffer_append_bytes(instance->tx_buffer, slix_data->signature.data, SLIX_SIGNATURE_SIZE);

    return SlixErrorNone;
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
        case SLIX_CMD_GET_NXP_SYSTEM_INFORMATION:
            handler = slix_get_nxp_system_info_handler;
            break;
        case SLIX_CMD_GET_RANDOM_NUMBER:
            handler = slix_get_random_number_handler;
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
