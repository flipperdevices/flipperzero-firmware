#include "gen4_poller_i.h"

#include "bit_buffer.h"
#include "magic/protocols/gen4/gen4_poller.h"
#include <nfc/protocols/iso14443_3a/iso14443_3a_poller.h>

#define GEN4_CMD_PREFIX (0xCF)

#define GEN4_CMD_SET_SHD_MODE (0x32)
#define GEN4_CMD_GET_CFG (0xC6)
#define GEN4_CMD_GET_REVISION (0xCC)
#define GEN4_CMD_WRITE (0xCD)
#define GEN4_CMD_READ (0xCE)
#define GEN4_CMD_SET_DW_BLOCK_0 (0xCF)
#define GEN4_CMD_SET_CFG (0xF0)
#define GEN4_CMD_FUSE_CFG (0xF1)
#define GEN4_CMD_SET_PWD (0xFE)

#define GEN4_RESPONSE_SUCCESS (0x02)

static Gen4PollerError gen4_poller_process_error(Iso14443_3aError error) {
    Gen4PollerError ret = Gen4PollerErrorNone;

    if(error == Iso14443_3aErrorNone) {
        ret = Gen4PollerErrorNone;
    } else {
        ret = Gen4PollerErrorTimeout;
    }

    return ret;
}

Gen4PollerError
    gen4_poller_set_shadow_mode(Gen4Poller* instance, Gen4Password password, Gen4ShadowMode mode) {
    Gen4PollerError ret = Gen4PollerErrorNone;
    bit_buffer_reset(instance->tx_buffer);

    do {
        bit_buffer_append_byte(instance->tx_buffer, GEN4_CMD_PREFIX);
        bit_buffer_append_bytes(instance->tx_buffer, password.bytes, GEN4_PASSWORD_LEN);
        bit_buffer_append_byte(instance->tx_buffer, GEN4_CMD_SET_SHD_MODE);
        bit_buffer_append_byte(instance->tx_buffer, mode);

        Iso14443_3aError error = iso14443_3a_poller_send_standard_frame(
            instance->iso3_poller, instance->tx_buffer, instance->rx_buffer, GEN4_POLLER_MAX_FWT);

        if(error != Iso14443_3aErrorNone) {
            ret = gen4_poller_process_error(error);
            break;
        }

        size_t response = bit_buffer_get_size_bytes(instance->rx_buffer);

        FURI_LOG_D(TAG, "Card response: 0x%02X, Shadow mode set: 0x%02X", response, mode);

        if(response != GEN4_RESPONSE_SUCCESS) {
            ret = Gen4PollerErrorProtocol;
            break;
        }

    } while(false);

    return ret;
}

Gen4PollerError gen4_poller_set_direct_write_block_0_mode(
    Gen4Poller* instance,
    Gen4Password password,
    Gen4DirectWriteBlock0Mode mode) {
    Gen4PollerError ret = Gen4PollerErrorNone;
    bit_buffer_reset(instance->tx_buffer);

    do {
        bit_buffer_append_byte(instance->tx_buffer, GEN4_CMD_PREFIX);
        bit_buffer_append_bytes(instance->tx_buffer, password.bytes, GEN4_PASSWORD_LEN);
        bit_buffer_append_byte(instance->tx_buffer, GEN4_CMD_SET_DW_BLOCK_0);
        bit_buffer_append_byte(instance->tx_buffer, mode);

        Iso14443_3aError error = iso14443_3a_poller_send_standard_frame(
            instance->iso3_poller, instance->tx_buffer, instance->rx_buffer, GEN4_POLLER_MAX_FWT);

        if(error != Iso14443_3aErrorNone) {
            ret = gen4_poller_process_error(error);
            break;
        }
        size_t response = bit_buffer_get_size_bytes(instance->rx_buffer);

        FURI_LOG_D(
            TAG, "Card response: 0x%02X, Direct write to block 0 mode set: 0x%02X", response, mode);

        if(response != GEN4_RESPONSE_SUCCESS) {
            ret = Gen4PollerErrorProtocol;
            break;
        }

    } while(false);

    return ret;
}

Gen4PollerError
    gen4_poller_get_config(Gen4Poller* instance, Gen4Password password, Gen4Config* config_result) {
    Gen4PollerError ret = Gen4PollerErrorNone;
    bit_buffer_reset(instance->tx_buffer);

    do {
        bit_buffer_append_byte(instance->tx_buffer, GEN4_CMD_PREFIX);
        bit_buffer_append_bytes(instance->tx_buffer, password.bytes, GEN4_PASSWORD_LEN);
        bit_buffer_append_byte(instance->tx_buffer, GEN4_CMD_GET_CFG);

        Iso14443_3aError error = iso14443_3a_poller_send_standard_frame(
            instance->iso3_poller, instance->tx_buffer, instance->rx_buffer, GEN4_POLLER_MAX_FWT);

        if(error != Iso14443_3aErrorNone) {
            ret = gen4_poller_process_error(error);
            break;
        }

        size_t rx_bytes = bit_buffer_get_size_bytes(instance->rx_buffer);

        if(rx_bytes != GEN4_CONFIG_SIZE) {
            ret = Gen4PollerErrorProtocol;
            break;
        }
        bit_buffer_write_bytes(instance->rx_buffer, config_result->data_raw, GEN4_CONFIG_SIZE);
    } while(false);

    return ret;
}

Gen4PollerError gen4_poller_get_revision(
    Gen4Poller* instance,
    Gen4Password password,
    Gen4Revision* revision_result) {
    Gen4PollerError ret = Gen4PollerErrorNone;
    bit_buffer_reset(instance->tx_buffer);

    do {
        bit_buffer_append_byte(instance->tx_buffer, GEN4_CMD_PREFIX);
        bit_buffer_append_bytes(instance->tx_buffer, password.bytes, GEN4_PASSWORD_LEN);
        bit_buffer_append_byte(instance->tx_buffer, GEN4_CMD_GET_REVISION);

        Iso14443_3aError error = iso14443_3a_poller_send_standard_frame(
            instance->iso3_poller, instance->tx_buffer, instance->rx_buffer, GEN4_POLLER_MAX_FWT);

        if(error != Iso14443_3aErrorNone) {
            ret = gen4_poller_process_error(error);
            break;
        }

        size_t rx_bytes = bit_buffer_get_size_bytes(instance->rx_buffer);
        if(rx_bytes != GEN4_REVISION_SIZE) {
            ret = Gen4PollerErrorProtocol;
            break;
        }
        bit_buffer_write_bytes(instance->rx_buffer, revision_result->data, GEN4_REVISION_SIZE);
    } while(false);

    return ret;
}

Gen4PollerError gen4_poller_set_config(
    Gen4Poller* instance,
    Gen4Password password,
    const Gen4Config* config,
    size_t config_size,
    bool fuse) {
    Gen4PollerError ret = Gen4PollerErrorNone;
    bit_buffer_reset(instance->tx_buffer);

    do {
        bit_buffer_append_byte(instance->tx_buffer, GEN4_CMD_PREFIX);
        bit_buffer_append_bytes(instance->tx_buffer, password.bytes, GEN4_PASSWORD_LEN);
        uint8_t fuse_config = fuse ? GEN4_CMD_FUSE_CFG : GEN4_CMD_SET_CFG;
        bit_buffer_append_byte(instance->tx_buffer, fuse_config);
        bit_buffer_append_bytes(instance->tx_buffer, config->data_raw, config_size);

        Iso14443_3aError error = iso14443_3a_poller_send_standard_frame(
            instance->iso3_poller, instance->tx_buffer, instance->rx_buffer, GEN4_POLLER_MAX_FWT);

        if(error != Iso14443_3aErrorNone) {
            ret = gen4_poller_process_error(error);
            break;
        }

        size_t response = bit_buffer_get_size_bytes(instance->rx_buffer);

        FURI_LOG_D(TAG, "Card response to set default config command: 0x%02X", response);

        if(response != GEN4_RESPONSE_SUCCESS) {
            ret = Gen4PollerErrorProtocol;
            break;
        }
    } while(false);

    return ret;
}

Gen4PollerError gen4_poller_write_block(
    Gen4Poller* instance,
    Gen4Password password,
    uint8_t block_num,
    const uint8_t* data) {
    Gen4PollerError ret = Gen4PollerErrorNone;
    bit_buffer_reset(instance->tx_buffer);

    do {
        bit_buffer_append_byte(instance->tx_buffer, GEN4_CMD_PREFIX);
        bit_buffer_append_bytes(instance->tx_buffer, password.bytes, GEN4_PASSWORD_LEN);
        bit_buffer_append_byte(instance->tx_buffer, GEN4_CMD_WRITE);
        bit_buffer_append_byte(instance->tx_buffer, block_num);
        bit_buffer_append_bytes(instance->tx_buffer, data, GEN4_POLLER_BLOCK_SIZE);

        Iso14443_3aError error = iso14443_3a_poller_send_standard_frame(
            instance->iso3_poller, instance->tx_buffer, instance->rx_buffer, GEN4_POLLER_MAX_FWT);

        if(error != Iso14443_3aErrorNone) {
            ret = gen4_poller_process_error(error);
            break;
        }

        size_t rx_bytes = bit_buffer_get_size_bytes(instance->rx_buffer);
        if(rx_bytes != 2) {
            ret = Gen4PollerErrorProtocol;
            break;
        }
    } while(false);

    return ret;
}

Gen4PollerError gen4_poller_change_password(
    Gen4Poller* instance,
    Gen4Password pwd_current,
    Gen4Password pwd_new) {
    Gen4PollerError ret = Gen4PollerErrorNone;
    bit_buffer_reset(instance->tx_buffer);

    do {
        bit_buffer_append_byte(instance->tx_buffer, GEN4_CMD_PREFIX);
        bit_buffer_append_bytes(instance->tx_buffer, pwd_current.bytes, GEN4_PASSWORD_LEN);

        bit_buffer_append_byte(instance->tx_buffer, GEN4_CMD_SET_PWD);
        bit_buffer_append_bytes(instance->tx_buffer, pwd_new.bytes, GEN4_PASSWORD_LEN);

        Iso14443_3aError error = iso14443_3a_poller_send_standard_frame(
            instance->iso3_poller, instance->tx_buffer, instance->rx_buffer, GEN4_POLLER_MAX_FWT);

        if(error != Iso14443_3aErrorNone) {
            ret = gen4_poller_process_error(error);
            break;
        }

        size_t response = bit_buffer_get_size_bytes(instance->rx_buffer);

        FURI_LOG_D(
            TAG,
            "Trying to change password from 0x%02X %02X %02X %02X to "
            "0x%02X %02X %02X %02X. Card response: 0x%02X",
            pwd_current.bytes[0],
            pwd_current.bytes[1],
            pwd_current.bytes[2],
            pwd_current.bytes[3],
            pwd_new.bytes[0],
            pwd_new.bytes[1],
            pwd_new.bytes[2],
            pwd_new.bytes[3],
            response);

        if(response != GEN4_RESPONSE_SUCCESS) {
            ret = Gen4PollerErrorProtocol;
            break;
        }
    } while(false);

    return ret;
}
