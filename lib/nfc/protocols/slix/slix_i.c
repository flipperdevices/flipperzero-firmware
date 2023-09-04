#include "slix_i.h"

#include <nfc/protocols/iso15693_3/iso15693_3_i.h>

bool slix_error_response_parse(SlixError* error, const BitBuffer* buf) {
    Iso15693_3Error iso15693_3_error;
    const bool ret = iso15693_3_error_response_parse(&iso15693_3_error, buf);

    if(ret) {
        *error = slix_process_iso15693_3_error(iso15693_3_error);
    }

    return ret;
}

SlixError slix_process_iso15693_3_error(Iso15693_3Error iso15693_3_error) {
    switch(iso15693_3_error) {
    case Iso15693_3ErrorNone:
        return SlixErrorNone;
    case Iso15693_3ErrorTimeout:
        return SlixErrorTimeout;
    case Iso15693_3ErrorFormat:
        return SlixErrorFormat;
    case Iso15693_3ErrorInternal:
        return SlixErrorInternal;
    default:
        return SlixErrorUnknown;
    }
}

SlixError slix_get_nxp_system_info_response_parse(SlixSystemInfo* data, const BitBuffer* buf) {
    furi_assert(data);
    SlixError error = SlixErrorNone;

    do {
        if(slix_error_response_parse(&error, buf)) break;

        typedef struct {
            uint8_t flags;
            uint8_t pp_pointer;
            uint8_t pp_condition;
            uint8_t lock_bits;
            uint32_t feature_flags;
        } SlixGetNxpSystemInfoResponseLayout;

        const size_t size_received = bit_buffer_get_size_bytes(buf);
        const size_t size_required = sizeof(SlixGetNxpSystemInfoResponseLayout);

        if(size_received != size_required) {
            error = SlixErrorFormat;
            break;
        }

        const SlixGetNxpSystemInfoResponseLayout* response =
            (const SlixGetNxpSystemInfoResponseLayout*)bit_buffer_get_data(buf);

        data->protection.pointer = response->pp_pointer;
        data->protection.condition = response->pp_condition;
        data->lock_bits = response->lock_bits;

    } while(false);

    return error;
}

SlixError slix_read_signature_response_parse(SlixSignature data, const BitBuffer* buf) {
    SlixError error = SlixErrorNone;

    do {
        if(slix_error_response_parse(&error, buf)) break;

        typedef struct {
            uint8_t flags;
            uint8_t signature[SLIX_SIGNATURE_SIZE];
        } SlixReadSignatureResponseLayout;

        const size_t size_received = bit_buffer_get_size_bytes(buf);
        const size_t size_required = sizeof(SlixReadSignatureResponseLayout);

        if(size_received != size_required) {
            error = SlixErrorFormat;
            break;
        }

        const SlixReadSignatureResponseLayout* response =
            (const SlixReadSignatureResponseLayout*)bit_buffer_get_data(buf);

        memcpy(data, response->signature, sizeof(SlixSignature));
    } while(false);

    return error;
}
