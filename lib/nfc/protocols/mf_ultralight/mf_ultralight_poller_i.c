#include "mf_ultralight_poller_i.h"

#include <furi.h>

#define TAG "MfUltralightPoller"

MfUltralightError mf_ultralight_process_error(NfcaError error) {
    MfUltralightError ret = MfUltralightErrorNone;

    switch(error) {
    case NfcaErrorNone:
        ret = MfUltralightErrorNone;
        break;
    case NfcaErrorNotPresent:
        ret = MfUltralightErrorNotPresent;
        break;
    case NfcaErrorColResFailed:
    case NfcaErrorCommunication:
    case NfcaErrorWrongCrc:
        ret = MfUltralightErrorProtocol;
        break;
    case NfcaErrorTimeout:
        ret = MfUltralightErrorTimeout;
        break;
    default:
        ret = MfUltralightErrorProtocol;
        break;
    }

    return ret;
}

static bool mf_ultralight_poller_ntag_i2c_addr_lin_to_tag_ntag_i2c_1k(
    uint16_t lin_addr,
    uint8_t* sector,
    uint8_t* tag,
    uint8_t* pages_left) {
    bool tag_calculated = false;
    // 0 - 226: sector 0
    // 227 - 228: config registers
    // 229 - 230: session registers

    if(lin_addr > 230) {
        *pages_left = 0;
    } else if(lin_addr >= 229) {
        *sector = 3;
        *pages_left = 2 - (lin_addr - 229);
        *tag = lin_addr - 229 + 248;
        tag_calculated = true;
    } else if(lin_addr >= 227) {
        *sector = 0;
        *pages_left = 2 - (lin_addr - 227);
        *tag = lin_addr - 227 + 232;
        tag_calculated = true;
    } else {
        *sector = 0;
        *pages_left = 227 - lin_addr;
        *tag = lin_addr;
        tag_calculated = true;
    }

    return tag_calculated;
}

static bool mf_ultralight_poller_ntag_i2c_addr_lin_to_tag_ntag_i2c_2k(
    uint16_t lin_addr,
    uint8_t* sector,
    uint8_t* tag,
    uint8_t* pages_left) {
    bool tag_calculated = false;
    // 0 - 255: sector 0
    // 256 - 480: sector 1
    // 481 - 482: config registers
    // 483 - 484: session registers

    if(lin_addr > 484) {
        *pages_left = 0;
    } else if(lin_addr >= 483) {
        *sector = 3;
        *pages_left = 2 - (lin_addr - 483);
        *tag = lin_addr - 483 + 248;
        tag_calculated = true;
    } else if(lin_addr >= 481) {
        *sector = 1;
        *pages_left = 2 - (lin_addr - 481);
        *tag = lin_addr - 481 + 232;
        tag_calculated = true;
    } else if(lin_addr >= 256) {
        *sector = 1;
        *pages_left = 225 - (lin_addr - 256);
        *tag = lin_addr - 256;
        tag_calculated = true;
    } else {
        *sector = 0;
        *pages_left = 256 - lin_addr;
        *tag = lin_addr;
        tag_calculated = true;
    }

    return tag_calculated;
}

static bool mf_ultralight_poller_ntag_i2c_addr_lin_to_tag_ntag_i2c_plus_1k(
    uint16_t lin_addr,
    uint8_t* sector,
    uint8_t* tag,
    uint8_t* pages_left) {
    bool tag_calculated = false;
    // 0 - 233: sector 0 + registers
    // 234 - 235: session registers

    if(lin_addr > 235) {
        *pages_left = 0;
    } else if(lin_addr >= 234) {
        *sector = 0;
        *pages_left = 2 - (lin_addr - 234);
        *tag = lin_addr - 234 + 236;
        tag_calculated = true;
    } else {
        *sector = 0;
        *pages_left = 234 - lin_addr;
        *tag = lin_addr;
        tag_calculated = true;
    }

    return tag_calculated;
}

static bool mf_ultralight_poller_ntag_i2c_addr_lin_to_tag_ntag_i2c_plus_2k(
    uint16_t lin_addr,
    uint8_t* sector,
    uint8_t* tag,
    uint8_t* pages_left) {
    bool tag_calculated = false;
    // 0 - 233: sector 0 + registers
    // 234 - 235: session registers
    // 236 - 491: sector 1

    if(lin_addr > 491) {
        *pages_left = 0;
    } else if(lin_addr >= 236) {
        *sector = 1;
        *pages_left = 256 - (lin_addr - 236);
        *tag = lin_addr - 236;
        tag_calculated = true;
    } else if(lin_addr >= 234) {
        *sector = 0;
        *pages_left = 2 - (lin_addr - 234);
        *tag = lin_addr - 234 + 236;
        tag_calculated = true;
    } else {
        *sector = 0;
        *pages_left = 234 - lin_addr;
        *tag = lin_addr;
        tag_calculated = true;
    }

    return tag_calculated;
}

bool mf_ultralight_poller_ntag_i2c_addr_lin_to_tag(
    MfUltralightPoller* instance,
    uint16_t lin_addr,
    uint8_t* sector,
    uint8_t* tag,
    uint8_t* pages_left) {
    furi_assert(instance);
    furi_assert(sector);
    furi_assert(tag);
    furi_assert(pages_left);

    bool tag_calculated = false;

    if(instance->data->type == MfUltralightTypeNTAGI2C1K) {
        tag_calculated = mf_ultralight_poller_ntag_i2c_addr_lin_to_tag_ntag_i2c_1k(
            lin_addr, sector, tag, pages_left);
    } else if(instance->data->type == MfUltralightTypeNTAGI2C2K) {
        tag_calculated = mf_ultralight_poller_ntag_i2c_addr_lin_to_tag_ntag_i2c_2k(
            lin_addr, sector, tag, pages_left);
    } else if(instance->data->type == MfUltralightTypeNTAGI2CPlus1K) {
        tag_calculated = mf_ultralight_poller_ntag_i2c_addr_lin_to_tag_ntag_i2c_plus_1k(
            lin_addr, sector, tag, pages_left);
    } else if(instance->data->type == MfUltralightTypeNTAGI2CPlus2K) {
        tag_calculated = mf_ultralight_poller_ntag_i2c_addr_lin_to_tag_ntag_i2c_plus_2k(
            lin_addr, sector, tag, pages_left);
    }

    return tag_calculated;
}

MfUltralightError mf_ultralight_poller_async_auth(
    MfUltralightPoller* instance,
    MfUltralightPollerAuthContext* data) {
    uint8_t auth_cmd[5] = {MF_ULTRALIGHT_CMD_AUTH};
    nfc_util_num2bytes(data->password.pass, MF_ULTRALIGHT_AUTH_PASSWORD_SIZE, &auth_cmd[1]);
    bit_buffer_copy_bytes(instance->tx_buffer, auth_cmd, sizeof(auth_cmd));

    MfUltralightError ret = MfUltralightErrorNone;
    NfcaError error = NfcaErrorNone;
    do {
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            instance->tx_buffer,
            instance->rx_buffer,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(bit_buffer_get_size_bytes(instance->rx_buffer) != MF_ULTRALIGHT_AUTH_PACK_SIZE) {
            ret = MfUltralightErrorAuth;
            break;
        }
        bit_buffer_write_bytes(instance->rx_buffer, data->pack.data, MF_ULTRALIGHT_AUTH_PACK_SIZE);
    } while(false);

    return ret;
}

MfUltralightError mf_ultralight_poller_async_read_page_from_sector(
    MfUltralightPoller* instance,
    uint8_t sector,
    uint8_t tag,
    MfUltralightPageReadCommandData* data) {
    MfUltralightError ret = MfUltralightErrorNone;
    NfcaError error = NfcaErrorNone;

    do {
        const uint8_t select_sector_cmd[2] = {MF_ULTRALIGHT_CMD_SECTOR_SELECT, 0xff};
        bit_buffer_copy_bytes(instance->tx_buffer, select_sector_cmd, sizeof(select_sector_cmd));
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            instance->tx_buffer,
            instance->rx_buffer,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorWrongCrc) {
            FURI_LOG_D(TAG, "Failed to issue sector select command");
            ret = mf_ultralight_process_error(error);
            break;
        }

        const uint8_t read_sector_cmd[4] = {sector, 0x00, 0x00, 0x00};
        bit_buffer_copy_bytes(instance->tx_buffer, read_sector_cmd, sizeof(read_sector_cmd));
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            instance->tx_buffer,
            instance->rx_buffer,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorTimeout) {
            // This is NOT a typo! The tag ACKs by not sending a response within 1ms.
            FURI_LOG_D(TAG, "Sector %u select NAK'd", sector);
            ret = MfUltralightErrorProtocol;
            break;
        }

        ret = mf_ultralight_poller_async_read_page(instance, tag, data);
    } while(false);

    return ret;
}

MfUltralightError mf_ultralight_poller_async_read_page(
    MfUltralightPoller* instance,
    uint8_t start_page,
    MfUltralightPageReadCommandData* data) {
    MfUltralightError ret = MfUltralightErrorNone;
    NfcaError error = NfcaErrorNone;

    do {
        uint8_t read_page_cmd[2] = {MF_ULTRALIGHT_CMD_READ_PAGE, start_page};
        bit_buffer_copy_bytes(instance->tx_buffer, read_page_cmd, sizeof(read_page_cmd));
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            instance->tx_buffer,
            instance->rx_buffer,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(bit_buffer_get_size_bytes(instance->rx_buffer) !=
           sizeof(MfUltralightPageReadCommandData)) {
            ret = MfUltralightErrorProtocol;
            break;
        }
        bit_buffer_write_bytes(instance->rx_buffer, data, sizeof(MfUltralightPageReadCommandData));
    } while(false);

    return ret;
}

MfUltralightError mf_ultralight_poller_async_write_page(
    MfUltralightPoller* instance,
    uint8_t page,
    MfUltralightPage* data) {
    MfUltralightError ret = MfUltralightErrorNone;
    NfcaError error = NfcaErrorNone;

    do {
        uint8_t write_page_cmd[MF_ULTRALIGHT_PAGE_SIZE + 2] = {MF_ULTRALIGHT_CMD_WRITE_PAGE, page};
        memcpy(&write_page_cmd[2], data->data, MF_ULTRALIGHT_PAGE_SIZE);
        bit_buffer_copy_bytes(instance->tx_buffer, write_page_cmd, sizeof(write_page_cmd));
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            instance->tx_buffer,
            instance->rx_buffer,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorWrongCrc) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(bit_buffer_get_size(instance->rx_buffer) != 4) {
            ret = MfUltralightErrorProtocol;
            break;
        }
        if(!bit_buffer_starts_with_byte(instance->rx_buffer, MF_ULTRALIGHT_CMD_ACK)) {
            ret = MfUltralightErrorProtocol;
            break;
        }
    } while(false);

    return ret;
}

MfUltralightError mf_ultralight_poller_async_read_version(
    MfUltralightPoller* instance,
    MfUltralightVersion* data) {
    MfUltralightError ret = MfUltralightErrorNone;
    NfcaError error = NfcaErrorNone;

    do {
        const uint8_t get_version_cmd = MF_ULTRALIGHT_CMD_GET_VERSION;
        bit_buffer_copy_bytes(instance->tx_buffer, &get_version_cmd, sizeof(get_version_cmd));
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            instance->tx_buffer,
            instance->rx_buffer,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(bit_buffer_get_size_bytes(instance->rx_buffer) != sizeof(MfUltralightVersion)) {
            FURI_LOG_I(
                TAG, "Read Version failed: %d", bit_buffer_get_size_bytes(instance->rx_buffer));
            ret = MfUltralightErrorProtocol;
            break;
        }
        bit_buffer_write_bytes(instance->rx_buffer, data, sizeof(MfUltralightVersion));
    } while(false);

    return ret;
}

MfUltralightError mf_ultralight_poller_async_read_signature(
    MfUltralightPoller* instance,
    MfUltralightSignature* data) {
    MfUltralightError ret = MfUltralightErrorNone;
    NfcaError error = NfcaErrorNone;

    do {
        const uint8_t read_signature_cmd[2] = {MF_ULTRALIGTH_CMD_READ_SIG, 0x00};
        bit_buffer_copy_bytes(instance->tx_buffer, read_signature_cmd, sizeof(read_signature_cmd));
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            instance->tx_buffer,
            instance->rx_buffer,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(bit_buffer_get_size_bytes(instance->rx_buffer) != sizeof(MfUltralightSignature)) {
            ret = MfUltralightErrorProtocol;
            break;
        }
        bit_buffer_write_bytes(instance->rx_buffer, data, sizeof(MfUltralightSignature));
    } while(false);

    return ret;
}

MfUltralightError mf_ultralight_poller_async_read_counter(
    MfUltralightPoller* instance,
    uint8_t counter_num,
    MfUltralightCounter* data) {
    MfUltralightError ret = MfUltralightErrorNone;
    NfcaError error = NfcaErrorNone;

    do {
        uint8_t read_counter_cmd[2] = {MF_ULTRALIGHT_CMD_READ_CNT, counter_num};
        bit_buffer_copy_bytes(instance->tx_buffer, read_counter_cmd, sizeof(read_counter_cmd));
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            instance->tx_buffer,
            instance->rx_buffer,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(bit_buffer_get_size_bytes(instance->rx_buffer) != MF_ULTRALIGHT_COUNTER_SIZE) {
            ret = MfUltralightErrorProtocol;
            break;
        }
        bit_buffer_write_bytes(instance->rx_buffer, data->data, MF_ULTRALIGHT_COUNTER_SIZE);
    } while(false);

    return ret;
}

MfUltralightError mf_ultralight_poller_async_read_tearing_flag(
    MfUltralightPoller* instance,
    uint8_t tearing_falg_num,
    MfUltralightTearingFlag* data) {
    MfUltralightError ret = MfUltralightErrorNone;
    NfcaError error = NfcaErrorNone;

    do {
        uint8_t check_tearing_cmd[2] = {MF_ULTRALIGHT_CMD_CHECK_TEARING, tearing_falg_num};
        bit_buffer_copy_bytes(instance->tx_buffer, check_tearing_cmd, sizeof(check_tearing_cmd));
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            instance->tx_buffer,
            instance->rx_buffer,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(bit_buffer_get_size_bytes(instance->rx_buffer) != sizeof(MfUltralightTearingFlag)) {
            ret = MfUltralightErrorProtocol;
            break;
        }
        bit_buffer_write_bytes(instance->rx_buffer, data, sizeof(MfUltralightTearingFlag));
    } while(false);

    return ret;
}

NfcPoller* mf_ultralight_poller_alloc_new(NfcPoller* nfca_poller) {
    furi_assert(nfca_poller);

    MfUltralightPoller* instance = malloc(sizeof(MfUltralightPoller));
    instance->nfca_poller = nfca_poller;
    instance->data = mf_ultralight_alloc();
    instance->tx_buffer = bit_buffer_alloc(MF_ULTRALIGHT_MAX_BUFF_SIZE);
    instance->rx_buffer = bit_buffer_alloc(MF_ULTRALIGHT_MAX_BUFF_SIZE);

    return instance;
}

void mf_ultralight_poller_free_new(NfcPoller* mfu_poller) {
    furi_assert(mfu_poller);

    MfUltralightPoller* instance = mfu_poller;
    mf_ultralight_free(instance->data);
    bit_buffer_free(instance->tx_buffer);
    bit_buffer_free(instance->rx_buffer);
    free(mfu_poller);
}

const NfcPollerBase mf_ultralight_poller = {
    .alloc = mf_ultralight_poller_alloc_new,
    .free = mf_ultralight_poller_free_new,
};
