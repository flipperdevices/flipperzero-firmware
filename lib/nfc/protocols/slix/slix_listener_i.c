#include "slix_listener_i.h"

#include <nfc/protocols/iso15693_3/iso15693_3_listener_i.h>

#include <furi_hal_random.h>

#define TAG "SlixListener"

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
    bit_buffer_append_bytes(instance->tx_buffer, (uint8_t*)&feature_flags, sizeof(uint32_t));

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

    SlixListenerSessionState* session_state = &instance->session_state;
    session_state->random = furi_hal_random_get();
    bit_buffer_append_bytes(
        instance->tx_buffer, (uint8_t*)&session_state->random, sizeof(uint16_t));

    return SlixErrorNone;
}

static SlixPasswordType slix_get_password_type_by_id(uint8_t id) {
    uint32_t type;

    for(type = 0; type < SlixPasswordTypeCount; ++type) {
        if(id >> type == 0x01U) break;
    }

    return type;
}

static SlixPassword slix_unxor_password(const SlixPassword password_xored, uint16_t random) {
    return password_xored ^ ((SlixPassword)random << 16 | random);
}

static SlixError slix_listener_set_password(
    SlixListener* instance,
    SlixPasswordType password_type,
    SlixPassword password) {
    SlixError error = SlixErrorNone;

    do {
        if(password_type >= SlixPasswordTypeCount) {
            error = SlixErrorInternal;
            break;
        }

        SlixData* slix_data = instance->data;

        if(!slix_type_supports_password(slix_get_type(slix_data), password_type)) {
            error = SlixErrorNotSupported;
            break;
        }

        SlixListenerSessionState* session_state = &instance->session_state;
        session_state->password_match[password_type] =
            (password == slix_get_password(slix_data, password_type));

        if(!session_state->password_match[password_type]) {
            error = SlixErrorWrongPassword;
            break;
        }
    } while(false);

    return error;
}

static SlixError slix_set_password_handler(
    SlixListener* instance,
    const uint8_t* data,
    size_t data_size,
    uint8_t flags) {
    UNUSED(flags);
    SlixError error = SlixErrorNone;

    do {
#pragma pack(push, 1)
        typedef struct {
            uint8_t password_id;
            SlixPassword password_xored;
        } SlixSetPasswordRequestLayout;
#pragma pack(pop)

        if(data_size != sizeof(SlixSetPasswordRequestLayout)) {
            error = SlixErrorFormat;
            break;
        }

        const SlixSetPasswordRequestLayout* request = (const SlixSetPasswordRequestLayout*)data;
        const SlixPasswordType password_type = slix_get_password_type_by_id(request->password_id);
        const SlixPassword password_received =
            slix_unxor_password(request->password_xored, instance->session_state.random);

        error = slix_listener_set_password(instance, password_type, password_received);
        if(error != SlixErrorNone) break;

        if(password_type == SlixPasswordTypePrivacy) {
            slix_set_privacy_mode(instance->data, false);
        }
    } while(false);

    return error;
}

static SlixError slix_listener_write_password(
    SlixListener* instance,
    SlixPasswordType password_type,
    SlixPassword password) {
    SlixError error = SlixErrorNone;

    do {
        if(password_type >= SlixPasswordTypeCount) {
            error = SlixErrorInternal;
            break;
        }

        SlixData* slix_data = instance->data;

        if(!slix_type_supports_password(slix_get_type(slix_data), password_type)) {
            error = SlixErrorNotSupported;
            break;
        }

        SlixListenerSessionState* session_state = &instance->session_state;

        if(session_state->password_match[password_type]) {
            // TODO: check for password lock
            slix_set_password(slix_data, password_type, password);
            // Require another SET_PASSWORD command with the new password
            session_state->password_match[password_type] = false;
        } else {
            error = SlixErrorWrongPassword;
            break;
        }
    } while(false);

    return error;
}

static SlixError slix_write_password_handler(
    SlixListener* instance,
    const uint8_t* data,
    size_t data_size,
    uint8_t flags) {
    UNUSED(flags);
    SlixError error = SlixErrorNone;

    do {
#pragma pack(push, 1)
        typedef struct {
            uint8_t password_id;
            SlixPassword password;
        } SlixWritePasswordRequestLayout;
#pragma pack(pop)

        if(data_size != sizeof(SlixWritePasswordRequestLayout)) {
            error = SlixErrorFormat;
            break;
        }

        const SlixWritePasswordRequestLayout* request =
            (const SlixWritePasswordRequestLayout*)data;
        const SlixPasswordType password_type = slix_get_password_type_by_id(request->password_id);

        error = slix_listener_write_password(instance, password_type, request->password);
        if(error != SlixErrorNone) break;

    } while(false);

    return error;
}

static SlixError slix_enable_privacy_handler(
    SlixListener* instance,
    const uint8_t* data,
    size_t data_size,
    uint8_t flags) {
    UNUSED(flags);
    SlixError error = SlixErrorNone;

    do {
        typedef struct {
            SlixPassword password_xored;
        } SlixEnablePrivacyRequestLayout;

        if(data_size != sizeof(SlixEnablePrivacyRequestLayout)) {
            error = SlixErrorFormat;
            break;
        }

        const SlixEnablePrivacyRequestLayout* request =
            (const SlixEnablePrivacyRequestLayout*)data;

        const SlixPassword password_received =
            slix_unxor_password(request->password_xored, instance->session_state.random);

        error = slix_listener_set_password(instance, SlixPasswordTypePrivacy, password_received);
        if(error != SlixErrorNone) break;

        slix_set_privacy_mode(instance->data, true);
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

    const SlixSignature* signature = &instance->data->signature;
    bit_buffer_append_bytes(instance->tx_buffer, signature->data, SLIX_SIGNATURE_SIZE);

    return SlixErrorNone;
}

bool slix_listener_is_password_valid(SlixListener* instance, SlixPasswordType password_type) {
    return instance->session_state.password_match[password_type];
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

        if(buf_size < sizeof(SlixRequestLayout)) {
            error = SlixErrorFormat;
            break;
        }

        const SlixRequestLayout* request =
            (const SlixRequestLayout*)bit_buffer_get_data(rx_buffer);

        const bool addressed_mode = request->flags & ISO15693_3_REQ_FLAG_T4_ADDRESSED;

        const size_t uid_field_size = addressed_mode ? ISO15693_3_UID_SIZE : 0;
        const size_t buf_size_min = sizeof(SlixRequestLayout) + uid_field_size;

        if(buf_size < buf_size_min) {
            error = SlixErrorFormat;
            break;
        }

        // TODO: compare UID in addressed mode

        const uint8_t* request_data = &request->data[uid_field_size];
        const size_t request_data_size = buf_size - buf_size_min;

        SlixRequestHandler handler;

        switch(request->command) {
        case SLIX_CMD_GET_NXP_SYSTEM_INFORMATION:
            handler = slix_get_nxp_system_info_handler;
            break;
        case SLIX_CMD_GET_RANDOM_NUMBER:
            handler = slix_get_random_number_handler;
            break;
        case SLIX_CMD_SET_PASSWORD:
            handler = slix_set_password_handler;
            break;
        case SLIX_CMD_WRITE_PASSWORD:
            handler = slix_write_password_handler;
            break;
        case SLIX_CMD_ENABLE_PRIVACY:
            handler = slix_enable_privacy_handler;
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

        error = handler(instance, request_data, request_data_size, request->flags);

        // It's a trick! Send no reply.
        if(error == SlixErrorFormat || error == SlixErrorWrongPassword)
            break;

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
