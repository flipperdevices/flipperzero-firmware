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
    NfcPollerBuffer* buff = instance->buffer;
    buff->tx_data[0] = MF_ULTRALIGHT_CMD_AUTH;
    // fill password in lsb
    nfc_util_num2bytes(data->password.pass, MF_ULTRALIGHT_AUTH_PASSWORD_SIZE, &buff->tx_data[1]);
    buff->tx_bits = (MF_ULTRALIGHT_AUTH_PASSWORD_SIZE + 1) * 8;

    MfUltralightError ret = MfUltralightErrorNone;
    NfcaError error = NfcaErrorNone;
    do {
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            buff->tx_data,
            buff->tx_bits,
            buff->rx_data,
            buff->rx_data_size,
            &buff->rx_bits,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(buff->rx_bits != (MF_ULTRALIGHT_AUTH_PACK_SIZE * 8)) {
            ret = MfUltralightErrorAuth;
            break;
        }
        data->pack.data[0] = buff->rx_data[0];
        data->pack.data[1] = buff->rx_data[1];
    } while(false);

    return ret;
}

MfUltralightError mf_ultralight_poller_async_read_page_from_sector(
    MfUltralightPoller* instance,
    uint8_t sector,
    uint8_t tag,
    MfUltralightPageReadCommandData* data) {
    NfcPollerBuffer* buff = instance->buffer;
    MfUltralightError ret = MfUltralightErrorNone;
    NfcaError error = NfcaErrorNone;

    do {
        buff->tx_data[0] = MF_ULTRALIGHT_CMD_SECTOR_SELECT;
        buff->tx_data[1] = 0xff;
        buff->tx_bits = 16;
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            buff->tx_data,
            buff->tx_bits,
            buff->rx_data,
            buff->rx_data_size,
            &buff->rx_bits,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorNone) {
            FURI_LOG_D(TAG, "Failed to issue sector select command");
            ret = mf_ultralight_process_error(error);
            break;
        }

        buff->tx_data[0] = sector;
        buff->tx_data[1] = 0x00;
        buff->tx_data[2] = 0x00;
        buff->tx_data[3] = 0x00;
        buff->tx_bits = 32;
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            buff->tx_data,
            buff->tx_bits,
            buff->rx_data,
            buff->rx_data_size,
            &buff->rx_bits,
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
    NfcPollerBuffer* buff = instance->buffer;
    buff->tx_data[0] = MF_ULTRALIGHT_CMD_READ_PAGE;
    buff->tx_data[1] = start_page;
    buff->tx_bits = 16;

    MfUltralightError ret = MfUltralightErrorNone;
    NfcaError error = NfcaErrorNone;
    do {
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            buff->tx_data,
            buff->tx_bits,
            buff->rx_data,
            buff->rx_data_size,
            &buff->rx_bits,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(buff->rx_bits != sizeof(MfUltralightPageReadCommandData) * 8) {
            ret = MfUltralightErrorProtocol;
            break;
        }
        memcpy(data, buff->rx_data, sizeof(MfUltralightPageReadCommandData));
    } while(false);

    return ret;
}

MfUltralightError mf_ultralight_poller_async_write_page(
    MfUltralightPoller* instance,
    uint8_t page,
    MfUltralightPage* data) {
    NfcPollerBuffer* buff = instance->buffer;
    buff->tx_data[0] = MF_ULTRALIGHT_CMD_WRITE_PAGE;
    buff->tx_data[1] = page;
    memcpy(&buff->tx_data[2], data, MF_ULTRALIGHT_PAGE_SIZE);
    buff->tx_bits = (2 + MF_ULTRALIGHT_PAGE_SIZE) * 8;

    MfUltralightError ret = MfUltralightErrorNone;
    NfcaError error = NfcaErrorNone;
    do {
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            buff->tx_data,
            buff->tx_bits,
            buff->rx_data,
            buff->rx_data_size,
            &buff->rx_bits,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        FURI_LOG_D(TAG, "Rx bits: %d", buff->rx_bits);
        if(error != NfcaErrorNone) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(buff->rx_bits != 4) {
            ret = MfUltralightErrorProtocol;
            break;
        }
        memcpy(&instance->data->page[page], buff->rx_data, MF_ULTRALIGHT_PAGE_SIZE);
    } while(false);

    return ret;
}

MfUltralightError mf_ultralight_poller_async_read_version(
    MfUltralightPoller* instance,
    MfUltralightVersion* data) {
    NfcPollerBuffer* buff = instance->buffer;
    buff->tx_data[0] = MF_ULTRALIGHT_CMD_GET_VERSION;
    buff->tx_bits = 8;

    MfUltralightError ret = MfUltralightErrorNone;
    NfcaError error = NfcaErrorNone;
    do {
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            buff->tx_data,
            buff->tx_bits,
            buff->rx_data,
            buff->rx_data_size,
            &buff->rx_bits,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(buff->rx_bits != sizeof(MfUltralightVersion) * 8) {
            ret = MfUltralightErrorProtocol;
            break;
        }
        memcpy(data, buff->rx_data, sizeof(MfUltralightVersion));
    } while(false);

    return ret;
}

MfUltralightError mf_ultralight_poller_async_read_signature(
    MfUltralightPoller* instance,
    MfUltralightSignature* data) {
    NfcPollerBuffer* buff = instance->buffer;
    buff->tx_data[0] = MF_ULTRALIGTH_CMD_READ_SIG;
    buff->rx_data[1] = 0x00;
    buff->tx_bits = 16;

    MfUltralightError ret = MfUltralightErrorNone;
    NfcaError error = NfcaErrorNone;
    do {
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            buff->tx_data,
            buff->tx_bits,
            buff->rx_data,
            buff->rx_data_size,
            &buff->rx_bits,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(buff->rx_bits != sizeof(MfUltralightSignature) * 8) {
            ret = MfUltralightErrorProtocol;
            break;
        }
        memcpy(data, buff->rx_data, sizeof(MfUltralightSignature));
    } while(false);

    return ret;
}

MfUltralightError mf_ultralight_poller_async_read_counter(
    MfUltralightPoller* instance,
    uint8_t counter_num,
    MfUltralightCounter* data) {
    NfcPollerBuffer* buff = instance->buffer;
    buff->tx_data[0] = MF_ULTRALIGHT_CMD_READ_CNT;
    buff->tx_data[1] = counter_num;
    buff->tx_bits = 2 * 8;

    MfUltralightError ret = MfUltralightErrorNone;
    NfcaError error = NfcaErrorNone;
    do {
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            buff->tx_data,
            buff->tx_bits,
            buff->rx_data,
            buff->rx_data_size,
            &buff->rx_bits,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(buff->rx_bits != 3 * 8) {
            ret = MfUltralightErrorProtocol;
            break;
        }
        memcpy(data, buff->rx_data, MF_ULTRALIGHT_COUNTER_SIZE);
    } while(false);

    return ret;
}

MfUltralightError mf_ultralight_poller_async_read_tearing_flag(
    MfUltralightPoller* instance,
    uint8_t tearing_falg_num,
    MfUltralightTearingFlag* data) {
    NfcPollerBuffer* buff = instance->buffer;
    buff->tx_data[0] = MF_ULTRALIGHT_CMD_CHECK_TEARING;
    buff->tx_data[1] = tearing_falg_num;
    buff->tx_bits = 2 * 8;

    MfUltralightError ret = MfUltralightErrorNone;
    NfcaError error = NfcaErrorNone;
    do {
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            buff->tx_data,
            buff->tx_bits,
            buff->rx_data,
            buff->rx_data_size,
            &buff->rx_bits,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(buff->rx_bits != sizeof(MfUltralightTearingFlag) * 8) {
            ret = MfUltralightErrorProtocol;
            break;
        }
        memcpy(data, buff->rx_data, MF_ULTRALIGHT_TEARING_FLAG_SIZE);
    } while(false);

    return ret;
}
