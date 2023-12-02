#include "gen4_poller_i.h"

#include <nfc/protocols/iso14443_3a/iso14443_3a_poller.h>
#include <nfc/helpers/nfc_util.h>

#define GEN4_CMD_PREFIX (0xCF)

#define GEN4_CMD_GET_CFG (0xC6)
#define GEN4_CMD_WRITE (0xCD)
#define GEN4_CMD_READ (0xCE)
#define GEN4_CMD_SET_CFG (0xF0)
#define GEN4_CMD_FUSE_CFG (0xF1)
#define GEN4_CMD_SET_PWD (0xFE)

static Gen4PollerError gen4_poller_process_error(Iso14443_3aError error) {
    Gen4PollerError ret = Gen4PollerErrorNone;

    if(error == Iso14443_3aErrorNone) {
        ret = Gen4PollerErrorNone;
    } else {
        ret = Gen4PollerErrorTimeout;
    }

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

        size_t rx_bytes = bit_buffer_get_size_bytes(instance->rx_buffer);
        if(rx_bytes != 2) {
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

        size_t rx_bytes = bit_buffer_get_size_bytes(instance->rx_buffer);
        if(rx_bytes != 2) {
            ret = Gen4PollerErrorProtocol;
            break;
        }
    } while(false);

    return ret;
}
