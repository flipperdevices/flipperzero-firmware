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

static NfcPoller* mf_ultralight_poller_alloc_new(NfcPoller* nfca_poller) {
    furi_assert(nfca_poller);

    MfUltralightPoller* instance = malloc(sizeof(MfUltralightPoller));
    instance->nfca_poller = nfca_poller;
    instance->tx_buffer = bit_buffer_alloc(MF_ULTRALIGHT_MAX_BUFF_SIZE);
    instance->rx_buffer = bit_buffer_alloc(MF_ULTRALIGHT_MAX_BUFF_SIZE);
    instance->event = malloc(sizeof(NfcPollerEvent));
    instance->mfu_event = malloc(sizeof(MfUltralightPollerEvent));
    instance->data = mf_ultralight_alloc();

    return instance;
}

static void mf_ultralight_poller_free_new(NfcPoller* mfu_poller) {
    furi_assert(mfu_poller);

    MfUltralightPoller* instance = mfu_poller;
    furi_assert(instance->data);
    furi_assert(instance->tx_buffer);
    furi_assert(instance->rx_buffer);
    furi_assert(instance->mfu_event);
    furi_assert(instance->event);

    bit_buffer_free(instance->tx_buffer);
    bit_buffer_free(instance->rx_buffer);
    mf_ultralight_free(instance->data);
    free(instance->mfu_event);
    free(instance->event);
    free(instance);
}

static void mf_ultralight_poller_set_callback(
    NfcPoller* mfu_poller,
    NfcPollerCallback callback,
    void* context) {
    furi_assert(mfu_poller);
    furi_assert(callback);

    MfUltralightPoller* instance = mfu_poller;
    instance->callback_new = callback;
    instance->context_new = context;
}

static const NfcProtocolData* mf_ultralight_poller_get_data_new(const NfcPoller* mfu_poller) {
    furi_assert(mfu_poller);

    const MfUltralightPoller* instance = mfu_poller;
    furi_assert(instance->data);

    return instance->data;
}

typedef NfcCommand (*MfUltralightPollerReadHandler)(MfUltralightPoller* instance);

static NfcCommand mf_ultralight_poller_handler_idle(MfUltralightPoller* instance) {
    bit_buffer_reset(instance->tx_buffer);
    bit_buffer_reset(instance->rx_buffer);
    nfca_copy(instance->data->nfca_data, nfca_poller_get_data(instance->nfca_poller));
    instance->counters_read = 0;
    instance->counters_total = 3;
    instance->tearing_flag_read = 0;
    instance->tearing_flag_total = 3;
    instance->pages_read = 0;
    instance->state = MfUltralightPollerStateReadVersion;

    return NfcCommandContinue;
}

static NfcCommand mf_ultralight_poller_handler_read_version(MfUltralightPoller* instance) {
    instance->error = mf_ultralight_poller_async_read_version(instance, &instance->data->version);
    if(instance->error == MfUltralightErrorNone) {
        FURI_LOG_D(TAG, "Read version success");
        instance->data->type = mf_ultralight_get_type_by_version(&instance->data->version);
        instance->state = MfUltralightPollerStateGetFeatureSet;
    } else {
        FURI_LOG_D(TAG, "Didn't response. Check NTAG 203");
        nfca_poller_halt(instance->nfca_poller);
        instance->state = MfUltralightPollerStateDetectNtag203;
    }

    return NfcCommandContinue;
}

static NfcCommand mf_ultralight_poller_handler_check_ntag_203(MfUltralightPoller* instance) {
    MfUltralightPageReadCommandData data = {};
    instance->error = mf_ultralight_poller_async_read_page(instance, 41, &data);
    if(instance->error == MfUltralightErrorNone) {
        FURI_LOG_D(TAG, "NTAG203 detected");
        instance->data->type = MfUltralightTypeNTAG203;
    } else {
        FURI_LOG_D(TAG, "Original Ultralight detected");
        nfca_poller_halt(instance->nfca_poller);
        instance->data->type = MfUltralightTypeUnknown;
    }
    instance->state = MfUltralightPollerStateGetFeatureSet;

    return NfcCommandContinue;
}

static NfcCommand mf_ultralight_poller_handler_get_feature_set(MfUltralightPoller* instance) {
    instance->feature_set = mf_ultralight_get_feature_support_set(instance->data->type);
    instance->pages_total = mf_ultralight_get_pages_total(instance->data->type);
    instance->data->pages_total = instance->pages_total;
    FURI_LOG_D(
        TAG,
        "%s detected. Total pages: %d",
        mf_ultralight_get_name(instance->data, NfcProtocolNameTypeFull),
        instance->pages_total);

    instance->state = MfUltralightPollerStateReadSignature;
    return NfcCommandContinue;
}

static NfcCommand mf_ultralight_poller_handler_read_signature(MfUltralightPoller* instance) {
    MfUltralightPollerState next_state = MfUltralightPollerStateAuth;
    if(instance->feature_set & MfUltralightFeatureSupportReadSignature) {
        FURI_LOG_D(TAG, "Reading signature");
        instance->error =
            mf_ultralight_poller_async_read_signature(instance, &instance->data->signature);
        if(instance->error != MfUltralightErrorNone) {
            FURI_LOG_D(TAG, "Read signature failed");
            next_state = MfUltralightPollerStateReadFailed;
        }
    } else {
        FURI_LOG_D(TAG, "Skip reading signature");
    }
    instance->state = next_state;

    return NfcCommandContinue;
}

static NfcCommand mf_ultralight_poller_handler_read_counters(MfUltralightPoller* instance) {
    if(instance->feature_set & MfUltralightFeatureSupportReadCounter) {
        if(mf_ultralight_is_counter_configured(instance->data)) {
            if(instance->feature_set & MfUltralightFeatureSupportSingleCounter) {
                instance->counters_read = 2;
            }
            if(instance->counters_read == instance->counters_total) {
                instance->state = MfUltralightPollerStateReadTearingFlags;
            } else {
                FURI_LOG_D(TAG, "Reading counter %d", instance->counters_read);
                instance->error = mf_ultralight_poller_async_read_counter(
                    instance,
                    instance->counters_read,
                    &instance->data->counter[instance->counters_read]);
                if(instance->error != MfUltralightErrorNone) {
                    FURI_LOG_D(TAG, "Failed to read %d counter", instance->counters_read);
                    instance->state = MfUltralightPollerStateReadTearingFlags;
                } else {
                    instance->counters_read++;
                }
            }
        } else {
            instance->state = MfUltralightPollerStateReadTearingFlags;
        }
    } else {
        FURI_LOG_D(TAG, "Skip reading counters");
        instance->state = MfUltralightPollerStateReadTearingFlags;
    }

    return NfcCommandContinue;
}

static NfcCommand mf_ultralight_poller_handler_read_tearing_flags(MfUltralightPoller* instance) {
    if(instance->feature_set & MfUltralightFeatureSupportCheckTearingFlag) {
        if(instance->tearing_flag_read == instance->tearing_flag_total) {
            instance->state = MfUltralightPollerStateTryDefaultPass;
        } else {
            FURI_LOG_D(TAG, "Reading tearing flag %d", instance->tearing_flag_read);
            instance->error = mf_ultralight_poller_async_read_tearing_flag(
                instance,
                instance->tearing_flag_read,
                &instance->data->tearing_flag[instance->tearing_flag_read]);
            if(instance->error != MfUltralightErrorNone) {
                FURI_LOG_D(TAG, "Reading tearing flag %d failed", instance->tearing_flag_read);
                instance->state = MfUltralightPollerStateReadFailed;
            } else {
                instance->tearing_flag_read++;
            }
        }
    } else {
        FURI_LOG_D(TAG, "Skip reading tearing flags");
        instance->state = MfUltralightPollerStateTryDefaultPass;
    }

    return NfcCommandContinue;
}

static NfcCommand mf_ultralight_poller_handler_auth(MfUltralightPoller* instance) {
    NfcCommand command = NfcCommandContinue;
    if(instance->feature_set & MfUltralightFeatureSupportAuthentication) {
        MfUltralightPollerEventData event_data = {};
        instance->mfu_event->type = MfUltralightPollerEventTypeAuthRequest;
        instance->mfu_event->data = &event_data;

        command = instance->callback_new(*instance->event, instance->context_new);
        if(!instance->mfu_event->data->auth_context.skip_auth) {
            instance->auth_context.password = instance->mfu_event->data->auth_context.password;
            FURI_LOG_D(
                TAG,
                "Trying to authenticate with password %08lX",
                instance->auth_context.password.pass);
            instance->error = mf_ultralight_poller_async_auth(instance, &instance->auth_context);
            if(instance->error == MfUltralightErrorNone) {
                FURI_LOG_D(TAG, "Auth success");
                instance->auth_context.auth_success = true;
                instance->mfu_event->data->auth_context = instance->auth_context;
                instance->mfu_event->type = MfUltralightPollerEventTypeAuthSuccess;
                command = instance->callback_new(*instance->event, instance->context_new);
            } else {
                FURI_LOG_D(TAG, "Auth failed");
                instance->auth_context.auth_success = false;
                instance->mfu_event->type = MfUltralightPollerEventTypeAuthFailed;
                command = instance->callback_new(*instance->event, instance->context_new);
                nfca_poller_halt(instance->nfca_poller);
            }
        }
    }
    instance->state = MfUltralightPollerStateReadPages;

    return command;
}

static NfcCommand mf_ultralight_poller_handler_read_pages(MfUltralightPoller* instance) {
    MfUltralightPageReadCommandData data = {};
    uint16_t start_page = instance->pages_read;
    if(MF_ULTRALIGHT_IS_NTAG_I2C(instance->data->type)) {
        uint8_t tag = 0;
        uint8_t sector = 0;
        uint8_t pages_left = 0;
        if(mf_ultralight_poller_ntag_i2c_addr_lin_to_tag(
               instance, start_page, &sector, &tag, &pages_left)) {
            instance->error =
                mf_ultralight_poller_async_read_page_from_sector(instance, sector, tag, &data);
        } else {
            FURI_LOG_D(TAG, "Failed to calculate sector and tag from %d page", start_page);
            instance->error = MfUltralightErrorProtocol;
        }
    } else {
        instance->error = mf_ultralight_poller_async_read_page(instance, start_page, &data);
    }

    if(instance->error == MfUltralightErrorNone) {
        for(size_t i = 0; i < 4; i++) {
            if(start_page + i < instance->pages_total) {
                FURI_LOG_D(TAG, "Read page %d success", start_page + i);
                instance->data->page[start_page + i] = data.page[i];
                instance->pages_read++;
                instance->data->pages_read = instance->pages_read;
            }
        }
        if(instance->pages_read == instance->pages_total) {
            instance->state = MfUltralightPollerStateReadCounters;
        }
    } else {
        FURI_LOG_D(TAG, "Read page %d failed", instance->pages_read);
        if(instance->pages_read) {
            instance->state = MfUltralightPollerStateReadCounters;
        } else {
            instance->state = MfUltralightPollerStateReadFailed;
        }
    }

    return NfcCommandContinue;
}

static NfcCommand mf_ultralight_poller_handler_try_default_pass(MfUltralightPoller* instance) {
    if(instance->feature_set & MfUltralightFeatureSupportAuthentication) {
        MfUltralightConfigPages* config = NULL;
        mf_ultralight_get_config_page(instance->data, &config);
        if(instance->auth_context.auth_success) {
            config->password = instance->auth_context.password;
            config->pack = instance->auth_context.pack;
        } else if(config->access.authlim == 0) {
            FURI_LOG_D(TAG, "No limits in authentication. Trying default password");
            instance->auth_context.password.pass = MF_ULTRALIGHT_DEFAULT_PASSWORD;
            instance->error = mf_ultralight_poller_async_auth(instance, &instance->auth_context);
            if(instance->error == MfUltralightErrorNone) {
                FURI_LOG_D(TAG, "Default password detected");
                config->password.pass = MF_ULTRALIGHT_DEFAULT_PASSWORD;
                config->pack = instance->auth_context.pack;
            }
        }

        if(instance->pages_read != instance->pages_total) {
            // Probably password protected, fix AUTH0 and PROT so before AUTH0
            // can be written and since AUTH0 won't be readable, like on the
            // original card
            config->auth0 = instance->pages_read;
            config->access.prot = true;
        }
    }

    instance->state = MfUltralightPollerStateReadSuccess;
    return NfcCommandContinue;
}

static NfcCommand mf_ultralight_poller_handler_read_fail(MfUltralightPoller* instance) {
    FURI_LOG_D(TAG, "Read Failed");
    nfca_poller_halt(instance->nfca_poller);
    instance->mfu_event->data->error = instance->error;
    NfcCommand command = instance->callback_new(*instance->event, instance->context_new);
    instance->state = MfUltralightPollerStateIdle;
    return command;
}

static NfcCommand mf_ultralight_poller_handler_read_success(MfUltralightPoller* instance) {
    FURI_LOG_D(TAG, "Read success.");
    nfca_poller_halt(instance->nfca_poller);
    instance->mfu_event->type = MfUltralightPollerEventTypeReadSuccess;
    NfcCommand command = instance->callback_new(*instance->event, instance->context_new);
    return command;
}

static const MfUltralightPollerReadHandler
    mf_ultralight_poller_read_handler[MfUltralightPollerStateNum] = {
        [MfUltralightPollerStateIdle] = mf_ultralight_poller_handler_idle,
        [MfUltralightPollerStateReadVersion] = mf_ultralight_poller_handler_read_version,
        [MfUltralightPollerStateDetectNtag203] = mf_ultralight_poller_handler_check_ntag_203,
        [MfUltralightPollerStateGetFeatureSet] = mf_ultralight_poller_handler_get_feature_set,
        [MfUltralightPollerStateReadSignature] = mf_ultralight_poller_handler_read_signature,
        [MfUltralightPollerStateReadCounters] = mf_ultralight_poller_handler_read_counters,
        [MfUltralightPollerStateReadTearingFlags] =
            mf_ultralight_poller_handler_read_tearing_flags,
        [MfUltralightPollerStateAuth] = mf_ultralight_poller_handler_auth,
        [MfUltralightPollerStateTryDefaultPass] = mf_ultralight_poller_handler_try_default_pass,
        [MfUltralightPollerStateReadPages] = mf_ultralight_poller_handler_read_pages,
        [MfUltralightPollerStateReadFailed] = mf_ultralight_poller_handler_read_fail,
        [MfUltralightPollerStateReadSuccess] = mf_ultralight_poller_handler_read_success,

};

static NfcCommand mf_ultralight_poller_run(NfcPollerEvent event, void* context) {
    furi_assert(context);
    furi_assert(event.data);
    furi_assert(event.protocol_type == NfcProtocolTypeIso14443_3a);

    MfUltralightPoller* instance = context;
    furi_assert(instance->callback_new);
    NfcaPollerEvent* nfca_event = event.data;

    instance->event->protocol_type = NfcProtocolTypeMfUltralight;
    instance->event->poller = instance;
    instance->event->data = instance->mfu_event;
    NfcCommand command = NfcCommandContinue;

    if(nfca_event->type == NfcaPollerEventTypeReady) {
        command = mf_ultralight_poller_read_handler[instance->state](instance);
    } else if(nfca_event->type == NfcaPollerEventTypeError) {
        instance->mfu_event->type = MfUltralightPollerEventTypeReadFailed;
        command = instance->callback_new(*instance->event, instance->context_new);
    }

    return command;
}

static bool mf_ultralight_poller_detect(NfcPollerEvent event, void* context) {
    furi_assert(context);
    furi_assert(event.data);
    furi_assert(event.protocol_type == NfcProtocolTypeIso14443_3a);

    bool protocol_detected = false;
    MfUltralightPoller* instance = context;
    NfcaPollerEvent* nfca_event = event.data;

    if(nfca_event->type == NfcaPollerEventTypeReady) {
        MfUltralightPageReadCommandData read_page_cmd_data = {};
        MfUltralightError error =
            mf_ultralight_poller_async_read_page(instance, 0, &read_page_cmd_data);
        protocol_detected = (error == MfUltralightErrorNone);
        nfca_poller_halt(instance->nfca_poller);
    }

    return protocol_detected;
}

const NfcPollerBase mf_ultralight_poller = {
    .alloc = mf_ultralight_poller_alloc_new,
    .free = mf_ultralight_poller_free_new,
    .set_callback = mf_ultralight_poller_set_callback,
    .run = mf_ultralight_poller_run,
    .detect = mf_ultralight_poller_detect,
    .get_data = mf_ultralight_poller_get_data_new,
};
