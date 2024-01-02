#include "gen4_poller_i.h"

#include "bit_buffer.h"
#include "protocols/gen4/gen4_poller.h"
#include <nfc/protocols/iso14443_3a/iso14443_3a_poller.h>
#include <nfc/helpers/nfc_util.h>

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

#define GEM4_RESPONSE_SUCCESS (0x02)

#define CONFIG_SIZE_MAX (32)
#define CONFIG_SIZE_MIN (30)
#define REVISION_SIZE (5)

static Gen4PollerError gen4_poller_process_error(Iso14443_3aError error) {
    Gen4PollerError ret = Gen4PollerErrorNone;

    if(error == Iso14443_3aErrorNone) {
        ret = Gen4PollerErrorNone;
    } else {
        ret = Gen4PollerErrorTimeout;
    }

    return ret;
}

Gen4PollerError gen4_poller_set_shadow_mode(
    Gen4Poller* instance,
    uint32_t password,
    Gen4PollerShadowMode mode) {
    Gen4PollerError ret = Gen4PollerErrorNone;
    bit_buffer_reset(instance->tx_buffer);

    do {
        uint8_t password_arr[4] = {};
        nfc_util_num2bytes(password, COUNT_OF(password_arr), password_arr);
        bit_buffer_append_byte(instance->tx_buffer, GEN4_CMD_PREFIX);
        bit_buffer_append_bytes(instance->tx_buffer, password_arr, COUNT_OF(password_arr));
        bit_buffer_append_byte(instance->tx_buffer, GEN4_CMD_SET_SHD_MODE);
        bit_buffer_append_byte(instance->tx_buffer, mode);

        Iso14443_3aError error = iso14443_3a_poller_send_standard_frame(
            instance->iso3_poller, instance->tx_buffer, instance->rx_buffer, GEN4_POLLER_MAX_FWT);

        if(error != Iso14443_3aErrorNone) {
            ret = gen4_poller_process_error(error);
            break;
        }

        uint16_t response = bit_buffer_get_size_bytes(instance->rx_buffer);

        FURI_LOG_D(TAG, "Card response: 0x%02X, Shadow mode set: 0x%02X", response, mode);

        if(response != GEM4_RESPONSE_SUCCESS) {
            ret = Gen4PollerErrorProtocol;
            break;
        }

    } while(false);

    return ret;
}

Gen4PollerError gen4_poller_set_direct_write_block_0_mode(
    Gen4Poller* instance,
    uint32_t password,
    Gen4PollerDirectWriteBlock0Mode mode) {
    Gen4PollerError ret = Gen4PollerErrorNone;
    bit_buffer_reset(instance->tx_buffer);

    do {
        uint8_t password_arr[4] = {};
        nfc_util_num2bytes(password, COUNT_OF(password_arr), password_arr);
        bit_buffer_append_byte(instance->tx_buffer, GEN4_CMD_PREFIX);
        bit_buffer_append_bytes(instance->tx_buffer, password_arr, COUNT_OF(password_arr));
        bit_buffer_append_byte(instance->tx_buffer, GEN4_CMD_SET_DW_BLOCK_0);
        bit_buffer_append_byte(instance->tx_buffer, mode);

        Iso14443_3aError error = iso14443_3a_poller_send_standard_frame(
            instance->iso3_poller, instance->tx_buffer, instance->rx_buffer, GEN4_POLLER_MAX_FWT);

        if(error != Iso14443_3aErrorNone) {
            ret = gen4_poller_process_error(error);
            break;
        }
        uint16_t response = bit_buffer_get_size_bytes(instance->rx_buffer);

        FURI_LOG_D(
            TAG, "Card response: 0x%02X, Direct write to block 0 mode set: 0x%02X", response, mode);

        if(response != GEM4_RESPONSE_SUCCESS) {
            ret = Gen4PollerErrorProtocol;
            break;
        }

    } while(false);

    return ret;
}

Gen4PollerError
    gen4_poller_get_config(Gen4Poller* instance, uint32_t password, uint8_t* config_result) {
    Gen4PollerError ret = Gen4PollerErrorNone;
    bit_buffer_reset(instance->tx_buffer);

    do {
        uint8_t password_arr[4] = {};
        nfc_util_num2bytes(password, COUNT_OF(password_arr), password_arr);
        bit_buffer_append_byte(instance->tx_buffer, GEN4_CMD_PREFIX);
        bit_buffer_append_bytes(instance->tx_buffer, password_arr, COUNT_OF(password_arr));
        bit_buffer_append_byte(instance->tx_buffer, GEN4_CMD_GET_CFG);

        Iso14443_3aError error = iso14443_3a_poller_send_standard_frame(
            instance->iso3_poller, instance->tx_buffer, instance->rx_buffer, GEN4_POLLER_MAX_FWT);

        if(error != Iso14443_3aErrorNone) {
            ret = gen4_poller_process_error(error);
            break;
        }

        size_t rx_bytes = bit_buffer_get_size_bytes(instance->rx_buffer);

        if(rx_bytes != CONFIG_SIZE_MAX || rx_bytes != CONFIG_SIZE_MIN) {
            ret = Gen4PollerErrorProtocol;
            break;
        }
        bit_buffer_write_bytes(instance->rx_buffer, config_result, CONFIG_SIZE_MAX);
    } while(false);

    return ret;
}

Gen4PollerError
    gen4_poller_get_revision(Gen4Poller* instance, uint32_t password, uint8_t* revision_result) {
    Gen4PollerError ret = Gen4PollerErrorNone;
    bit_buffer_reset(instance->tx_buffer);

    do {
        uint8_t password_arr[4] = {};
        nfc_util_num2bytes(password, COUNT_OF(password_arr), password_arr);
        bit_buffer_append_byte(instance->tx_buffer, GEN4_CMD_PREFIX);
        bit_buffer_append_bytes(instance->tx_buffer, password_arr, COUNT_OF(password_arr));
        bit_buffer_append_byte(instance->tx_buffer, GEN4_CMD_GET_REVISION);

        Iso14443_3aError error = iso14443_3a_poller_send_standard_frame(
            instance->iso3_poller, instance->tx_buffer, instance->rx_buffer, GEN4_POLLER_MAX_FWT);

        if(error != Iso14443_3aErrorNone) {
            ret = gen4_poller_process_error(error);
            break;
        }

        size_t rx_bytes = bit_buffer_get_size_bytes(instance->rx_buffer);
        if(rx_bytes != REVISION_SIZE) {
            ret = Gen4PollerErrorProtocol;
            break;
        }
        bit_buffer_write_bytes(instance->rx_buffer, revision_result, REVISION_SIZE);
    } while(false);

    return ret;
}

Gen4PollerError gen4_poller_set_config(
    Gen4Poller* instance,
    uint32_t password,
    const uint8_t* config,
    size_t config_size,
    bool fuse) {
    Gen4PollerError ret = Gen4PollerErrorNone;
    bit_buffer_reset(instance->tx_buffer);

    do {
        uint8_t password_arr[4] = {};
        nfc_util_num2bytes(password, COUNT_OF(password_arr), password_arr);
        bit_buffer_append_byte(instance->tx_buffer, GEN4_CMD_PREFIX);
        bit_buffer_append_bytes(instance->tx_buffer, password_arr, COUNT_OF(password_arr));
        uint8_t fuse_config = fuse ? GEN4_CMD_FUSE_CFG : GEN4_CMD_SET_CFG;
        bit_buffer_append_byte(instance->tx_buffer, fuse_config);
        bit_buffer_append_bytes(instance->tx_buffer, config, config_size);

        Iso14443_3aError error = iso14443_3a_poller_send_standard_frame(
            instance->iso3_poller, instance->tx_buffer, instance->rx_buffer, GEN4_POLLER_MAX_FWT);

        if(error != Iso14443_3aErrorNone) {
            ret = gen4_poller_process_error(error);
            break;
        }

        uint16_t response = bit_buffer_get_size_bytes(instance->rx_buffer);

        FURI_LOG_D(TAG, "Card response to set default config command: 0x%02X", response);

        if(response != GEM4_RESPONSE_SUCCESS) {
            ret = Gen4PollerErrorProtocol;
            break;
        }
    } while(false);

    return ret;
}

Gen4PollerError gen4_poller_write_block(
    Gen4Poller* instance,
    uint32_t password,
    uint8_t block_num,
    const uint8_t* data) {
    Gen4PollerError ret = Gen4PollerErrorNone;
    bit_buffer_reset(instance->tx_buffer);

    do {
        uint8_t password_arr[4] = {};
        nfc_util_num2bytes(password, COUNT_OF(password_arr), password_arr);
        bit_buffer_append_byte(instance->tx_buffer, GEN4_CMD_PREFIX);
        bit_buffer_append_bytes(instance->tx_buffer, password_arr, COUNT_OF(password_arr));
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

Gen4PollerError
    gen4_poller_change_password(Gen4Poller* instance, uint32_t pwd_current, uint32_t pwd_new) {
    Gen4PollerError ret = Gen4PollerErrorNone;
    bit_buffer_reset(instance->tx_buffer);

    do {
        uint8_t password_arr[4] = {};
        nfc_util_num2bytes(pwd_current, COUNT_OF(password_arr), password_arr);
        bit_buffer_append_byte(instance->tx_buffer, GEN4_CMD_PREFIX);
        bit_buffer_append_bytes(instance->tx_buffer, password_arr, COUNT_OF(password_arr));

        bit_buffer_append_byte(instance->tx_buffer, GEN4_CMD_SET_PWD);
        nfc_util_num2bytes(pwd_new, COUNT_OF(password_arr), password_arr);
        bit_buffer_append_bytes(instance->tx_buffer, password_arr, COUNT_OF(password_arr));

        Iso14443_3aError error = iso14443_3a_poller_send_standard_frame(
            instance->iso3_poller, instance->tx_buffer, instance->rx_buffer, GEN4_POLLER_MAX_FWT);

        if(error != Iso14443_3aErrorNone) {
            ret = gen4_poller_process_error(error);
            break;
        }

        uint16_t response = bit_buffer_get_size_bytes(instance->rx_buffer);

        FURI_LOG_D(
            TAG,
            "Trying to change password from 0x%08lX to 0x%08lX. Card response: 0x%02X",
            pwd_current,
            pwd_new,
            response);

        if(response != GEM4_RESPONSE_SUCCESS) {
            ret = Gen4PollerErrorProtocol;
            break;
        }
    } while(false);

    return ret;
}
