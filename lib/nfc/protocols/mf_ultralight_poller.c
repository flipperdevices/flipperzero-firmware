#include "mf_ultralight_poller.h"

#include <furi.h>
#include "nfca_poller.h"
#include <nfc/helpers/nfc_poller_buffer.h>
#include "nfc_util.h"

#define TAG "MfUltralightPoller"

#define MF_ULTRALIGHT_MAX_BUFF_SIZE (64)

#define MF_ULTRALIGHT_POLLER_STANDART_FWT_FC (12500)

#define MF_ULTRALIGHT_POLLER_COMPLETE_EVENT (1UL << 0)

typedef struct {
    MfUltralightPage page;
    uint8_t page_to_write;
} MfUltralightPollerWritePageCommand;

typedef union {
    uint8_t page_to_read;
    uint8_t counter_to_read;
    uint8_t tearing_flag_to_read;
    MfUltralightPollerWritePageCommand write_cmd;
} MfUltralightPollerContextData;

typedef struct {
    MfUltralightPoller* instance;
    FuriThreadId thread_id;
    MfUltralightError error;
    MfUltralightPollerContextData data;
} MfUltralightPollerContext;

typedef enum {
    MfUltralightPollerStateIdle,
    MfUltralightPollerStateReadVersion,
    MfUltralightPollerStateDetectNtag203,
    MfUltralightPollerStateGetFeatureSet,
    MfUltralightPollerStateReadSignature,
    MfUltralightPollerStateReadCounters,
    MfUltralightPollerStateReadTearingFlags,
    MfUltralightPollerStateAuth,
    MfUltralightPollerStateReadPages,
    MfUltralightPollerStateReadFailed,
    MfUltralightPollerStateReadSuccess,
} MfUltralightPollerState;

struct MfUltralightPoller {
    NfcaPoller* nfca_poller;
    MfUltralightPollerState state;
    NfcPollerBuffer* buffer;
    MfUltralightData* data;
    MfUltralightPollerCallback callback;
    MfUltralightAuthPassword auth_password;
    uint32_t feature_set;
    uint16_t pages_read;
    uint16_t pages_total;
    void* context;
};

static MfUltralightError mf_ultralight_process_error(NfcaError error) {
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

MfUltralightPoller* mf_ultralight_poller_alloc(NfcaPoller* nfca_poller) {
    MfUltralightPoller* instance = malloc(sizeof(MfUltralightPoller));
    instance->nfca_poller = nfca_poller;

    return instance;
}

void mf_ultralight_poller_free(MfUltralightPoller* instance) {
    furi_assert(instance);

    free(instance);
}

static MfUltralightError
    mf_ultralight_poller_async_auth(MfUltralightPoller* instance, MfUltralightAuthPassword* data) {
    NfcPollerBuffer* buff = instance->buffer;
    buff->tx_data[0] = MF_ULTRALIGHT_CMD_AUTH;
    // fill password in lsb
    nfc_util_num2bytes(data->pass, MF_ULTRALIGHT_AUTH_PASSWORD_SIZE, &buff->tx_data[1]);
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
        if(instance->pages_total != 0) {
            memcpy(
                &instance->data->page[instance->pages_total - 2],
                data,
                MF_ULTRALIGHT_AUTH_PASSWORD_SIZE);
            memcpy(
                &instance->data->page[instance->pages_total - 1],
                buff->rx_data,
                MF_ULTRALIGHT_AUTH_PACK_SIZE);
        }
    } while(false);

    return ret;
}

static MfUltralightError
    mf_ultralight_poller_async_read_page(MfUltralightPoller* instance, uint8_t page) {
    NfcPollerBuffer* buff = instance->buffer;
    buff->tx_data[0] = MF_ULTRALIGHT_CMD_READ_PAGE;
    buff->tx_data[1] = page;
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
        if(buff->rx_bits != (MF_ULTRALIGHT_PAGE_SIZE * 4) * 8) {
            ret = MfUltralightErrorProtocol;
            break;
        }
        memcpy(&instance->data->page[page], buff->rx_data, MF_ULTRALIGHT_PAGE_SIZE);
    } while(false);

    return ret;
}

static MfUltralightError mf_ultralight_poller_async_write_page(
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
        FURI_LOG_I(TAG, "Rx bits: %d", buff->rx_bits);
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

static MfUltralightError mf_ultralight_poller_async_read_version(MfUltralightPoller* instance) {
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
        if(buff->rx_bits != 8 * 8) {
            ret = MfUltralightErrorProtocol;
            break;
        }
        memcpy(&instance->data->version, buff->rx_data, sizeof(MfUltralightVersion));
    } while(false);

    return ret;
}

static MfUltralightError mf_ultralight_poller_async_read_signature(MfUltralightPoller* instance) {
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
        if(buff->rx_bits != 32 * 8) {
            ret = MfUltralightErrorProtocol;
            break;
        }
        memcpy(&instance->data->signature, buff->rx_data, sizeof(MfUltralightSignature));
    } while(false);

    return ret;
}

static MfUltralightError
    mf_ultralight_poller_async_read_counter(MfUltralightPoller* instance, uint8_t counter_num) {
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
        memcpy(&instance->data->counter[counter_num], buff->rx_data, MF_ULTRALIGHT_COUNTER_SIZE);
    } while(false);

    return ret;
}

static MfUltralightError mf_ultralight_poller_async_read_tearing_flag(
    MfUltralightPoller* instance,
    uint8_t tearing_falg_num) {
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
        if(buff->rx_bits != 8) {
            ret = MfUltralightErrorProtocol;
            break;
        }
        memcpy(
            &instance->data->tearing_flag[tearing_falg_num],
            buff->rx_data,
            MF_ULTRALIGHT_TEARING_FLAG_SIZE);
    } while(false);

    return ret;
}

static void mf_ultralight_poller_read_callback(NfcaPollerEvent event, void* context) {
    furi_assert(context);

    MfUltralightPoller* instance = context;
    MfUltralightPollerEventData event_data = {};
    MfUltralightPollerEvent mf_ul_poller_event = {.data = &event_data};
    MfUltralightError error = MfUltralightErrorNone;

    if(event.type == NfcaPollerEventTypeReady) {
        NfcPollerBuffer* buff = instance->buffer;
        nfc_poller_buffer_reset(buff);
        nfca_poller_get_data(instance->nfca_poller, &instance->data->nfca_data);

        if(instance->state == MfUltralightPollerStateReadVersion) {
            error = mf_ultralight_poller_async_read_version(instance);
            if(error == MfUltralightErrorNone) {
                FURI_LOG_I(TAG, "Read version success");
                instance->data->type = mf_ultralight_get_type_by_version(&instance->data->version);
                instance->state = MfUltralightPollerStateGetFeatureSet;
            } else {
                FURI_LOG_W(TAG, "Didn't response. Check NTAG 203");
                nfca_poller_halt(instance->nfca_poller);
                instance->state = MfUltralightPollerStateDetectNtag203;
            }
        } else if(instance->state == MfUltralightPollerStateDetectNtag203) {
            error = mf_ultralight_poller_async_read_page(instance, 41);
            if(error == MfUltralightErrorNone) {
                FURI_LOG_I(TAG, "NTAG203 detected");
                instance->data->type = MfUltralightTypeNTAG203;
                instance->state = MfUltralightPollerStateGetFeatureSet;
            } else {
                FURI_LOG_I(TAG, "Original Ultralight detected");
                nfca_poller_halt(instance->nfca_poller);
                instance->data->type = MfUltralightTypeUnknown;
                instance->state = MfUltralightPollerStateGetFeatureSet;
            }
        } else if(instance->state == MfUltralightPollerStateGetFeatureSet) {
            instance->feature_set = mf_ultralight_get_feature_support_set(instance->data->type);
            instance->pages_total = mf_ultralight_get_pages_total(instance->data->type);
            instance->pages_read = 0;
            instance->data->pages_total = instance->pages_total;
            FURI_LOG_I(
                TAG,
                "%s detected. Total pages: %d",
                mf_ultralight_get_name(instance->data->type, true),
                instance->pages_total);
            if((instance->callback) &&
               (instance->feature_set & MfUltralightFeatureSupportAuthentication)) {
                mf_ul_poller_event.type = MfUltralightPollerEventTypeAuthRequest;
                instance->callback(mf_ul_poller_event, instance->context);
                if(!mf_ul_poller_event.data->auth_context.skip_auth) {
                    instance->auth_password = mf_ul_poller_event.data->auth_context.password;
                    instance->state = MfUltralightPollerStateAuth;
                } else {
                    instance->state = MfUltralightPollerStateReadPages;
                }
            } else {
                instance->state = MfUltralightPollerStateReadPages;
            }
        } else if(instance->state == MfUltralightPollerStateAuth) {
            error = mf_ultralight_poller_async_auth(instance, &instance->auth_password);
            if(error == MfUltralightErrorNone) {
                FURI_LOG_I(TAG, "Auth success");
                if(instance->callback) {
                    mf_ul_poller_event.type = MfUltralightPollerEventTypeAuthSuccess;
                    memcpy(
                        &mf_ul_poller_event.data->pack,
                        &instance->data->page[instance->data->pages_read - 1],
                        MF_ULTRALIGHT_AUTH_PACK_SIZE);
                    instance->callback(mf_ul_poller_event, instance->context);
                }
            } else {
                FURI_LOG_W(TAG, "Auth failed");
                if(instance->callback) {
                    mf_ul_poller_event.type = MfUltralightPollerEventTypeAuthFailed;
                    instance->callback(mf_ul_poller_event, instance->context);
                }
                nfca_poller_halt(instance->nfca_poller);
            }
            instance->state = MfUltralightPollerStateReadPages;
        } else if(instance->state == MfUltralightPollerStateReadPages) {
            error = mf_ultralight_poller_async_read_page(instance, instance->pages_read);
            if(error == MfUltralightErrorNone) {
                FURI_LOG_I(TAG, "Read page %d success", instance->pages_read);
                instance->pages_read++;
                instance->data->pages_read = instance->pages_read;
                if(instance->pages_read == instance->pages_total - 2) {
                    instance->state = MfUltralightPollerStateReadSuccess;
                }
            } else {
                FURI_LOG_E(TAG, "Read page %d failed", instance->pages_read);
                if(instance->pages_read) {
                    instance->state = MfUltralightPollerStateReadSuccess;
                } else {
                    instance->state = MfUltralightPollerStateReadFailed;
                }
            }
        } else if(instance->state == MfUltralightPollerStateReadSignature) {
            error = mf_ultralight_poller_async_read_signature(instance);
            if(error == MfUltralightErrorNone) {
                FURI_LOG_I(TAG, "Read signature success");
                if(instance->feature_set & MfUltralightFeatureSupportReadCounter) {
                    instance->state = MfUltralightPollerStateReadCounters;
                }
            } else {
                FURI_LOG_E(TAG, "Read signature failed");
                instance->state = MfUltralightPollerStateReadFailed;
            }
        } else if(instance->state == MfUltralightPollerStateReadCounters) {
        } else if(instance->state == MfUltralightPollerStateReadSuccess) {
            if(instance->callback) {
                mf_ul_poller_event.type = MfUltralightPollerEventTypeReadSuccess;
                instance->callback(mf_ul_poller_event, instance->context);
            }
        } else if(instance->state == MfUltralightPollerStateReadFailed) {
            if(instance->callback) {
                mf_ul_poller_event.type = MfUltralightPollerEventTypeReadFailed;
                instance->callback(mf_ul_poller_event, instance->context);
            }
        }
    } else if(event.type == NfcaPollerEventTypeError) {
        if(instance->callback) {
            mf_ul_poller_event.type = MfUltralightPollerEventTypeReadFailed;
            instance->callback(mf_ul_poller_event, instance->context);
        }
    }
}

MfUltralightError mf_ultralight_poller_start(
    MfUltralightPoller* instance,
    NfcaPollerEventCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(instance->state == MfUltralightPollerStateIdle);
    furi_assert(instance->nfca_poller);
    furi_assert(callback);

    instance->data = malloc(sizeof(MfUltralightData));
    instance->buffer =
        nfc_poller_buffer_alloc(MF_ULTRALIGHT_MAX_BUFF_SIZE, MF_ULTRALIGHT_MAX_BUFF_SIZE);

    nfca_poller_start(instance->nfca_poller, callback, context);

    return MfUltralightErrorNone;
}

MfUltralightError mf_ultralight_poller_read(
    MfUltralightPoller* instance,
    MfUltralightPollerCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(instance->state == MfUltralightPollerStateIdle);
    furi_assert(instance->nfca_poller);
    furi_assert(callback);

    instance->callback = callback;
    instance->context = context;

    instance->state = MfUltralightPollerStateReadVersion;
    return mf_ultralight_poller_start(instance, mf_ultralight_poller_read_callback, instance);
}

MfUltralightError
    mf_ultralight_poller_get_data(MfUltralightPoller* instance, MfUltralightData* data) {
    furi_assert(instance);
    furi_assert(instance->data);
    furi_assert(data);

    *data = *instance->data;

    return MfUltralightErrorNone;
}

MfUltralightError mf_ultralight_poller_reset(MfUltralightPoller* instance) {
    furi_assert(instance);
    furi_assert(instance->data);
    furi_assert(instance->buffer);
    furi_assert(instance->nfca_poller);

    NfcaError error = nfca_poller_reset(instance->nfca_poller);

    nfc_poller_buffer_free(instance->buffer);
    instance->callback = NULL;
    instance->context = NULL;
    free(instance->data);
    instance->state = MfUltralightPollerStateIdle;

    return mf_ultralight_process_error(error);
}

// Called from NfcWorker thread

MfUltralightError mf_ultralight_poller_stop(MfUltralightPoller* instance) {
    furi_assert(instance);
    furi_assert(instance->nfca_poller);

    nfca_poller_stop(instance->nfca_poller);

    return MfUltralightErrorNone;
}

static void mf_ultraight_read_page_callback(NfcaPollerEvent event, void* context) {
    furi_assert(context);

    MfUltralightPollerContext* poller_context = context;
    if(event.type == NfcaPollerEventTypeReady) {
        poller_context->error = mf_ultralight_poller_async_read_page(
            poller_context->instance, poller_context->data.page_to_read);
        nfca_poller_halt(poller_context->instance->nfca_poller);
    } else if(event.type == NfcaPollerEventTypeError) {
        poller_context->error = mf_ultralight_process_error(event.data.error);
    }
    mf_ultralight_poller_stop(poller_context->instance);
    furi_thread_flags_set(poller_context->thread_id, MF_ULTRALIGHT_POLLER_COMPLETE_EVENT);
}

MfUltralightError mf_ultralight_poller_read_page(
    MfUltralightPoller* instance,
    uint16_t page,
    MfUltralightPage* data) {
    furi_assert(instance);
    furi_assert(data);

    MfUltralightPollerContext poller_context = {};
    poller_context.data.page_to_read = page;
    poller_context.instance = instance;
    poller_context.thread_id = furi_thread_get_current_id();

    mf_ultralight_poller_start(instance, mf_ultraight_read_page_callback, &poller_context);
    furi_thread_flags_wait(MF_ULTRALIGHT_POLLER_COMPLETE_EVENT, FuriFlagWaitAny, FuriWaitForever);
    *data = instance->data->page[page];
    mf_ultralight_poller_reset(instance);

    return poller_context.error;
}

static void mf_ultraight_write_page_callback(NfcaPollerEvent event, void* context) {
    furi_assert(context);

    MfUltralightPollerContext* poller_context = context;
    if(event.type == NfcaPollerEventTypeReady) {
        poller_context->error = mf_ultralight_poller_async_write_page(
            poller_context->instance,
            poller_context->data.write_cmd.page_to_write,
            &poller_context->data.write_cmd.page);
        nfca_poller_halt(poller_context->instance->nfca_poller);
    } else if(event.type == NfcaPollerEventTypeError) {
        poller_context->error = mf_ultralight_process_error(event.data.error);
    }
    mf_ultralight_poller_stop(poller_context->instance);
    furi_thread_flags_set(poller_context->thread_id, MF_ULTRALIGHT_POLLER_COMPLETE_EVENT);
}

MfUltralightError mf_ultralight_poller_write_page(
    MfUltralightPoller* instance,
    uint16_t page,
    MfUltralightPage* data) {
    furi_assert(instance);
    furi_assert(data);

    MfUltralightPollerContext poller_context = {};
    poller_context.data.write_cmd.page_to_write = page;
    poller_context.data.write_cmd.page = *data;
    poller_context.instance = instance;
    poller_context.thread_id = furi_thread_get_current_id();

    mf_ultralight_poller_start(instance, mf_ultraight_write_page_callback, &poller_context);
    furi_thread_flags_wait(MF_ULTRALIGHT_POLLER_COMPLETE_EVENT, FuriFlagWaitAny, FuriWaitForever);
    mf_ultralight_poller_reset(instance);

    return poller_context.error;
}

static void mf_ultraight_read_version_callback(NfcaPollerEvent event, void* context) {
    furi_assert(context);

    MfUltralightPollerContext* poller_context = context;
    if(event.type == NfcaPollerEventTypeReady) {
        poller_context->error = mf_ultralight_poller_async_read_version(poller_context->instance);
        nfca_poller_halt(poller_context->instance->nfca_poller);
    } else if(event.type == NfcaPollerEventTypeError) {
        poller_context->error = mf_ultralight_process_error(event.data.error);
    }
    mf_ultralight_poller_stop(poller_context->instance);
    furi_thread_flags_set(poller_context->thread_id, MF_ULTRALIGHT_POLLER_COMPLETE_EVENT);
}

MfUltralightError
    mf_ultralight_poller_read_version(MfUltralightPoller* instance, MfUltralightVersion* data) {
    furi_assert(instance);
    furi_assert(data);
    MfUltralightPollerContext poller_context = {};
    poller_context.instance = instance;
    poller_context.thread_id = furi_thread_get_current_id();

    mf_ultralight_poller_start(instance, mf_ultraight_read_version_callback, &poller_context);
    furi_thread_flags_wait(MF_ULTRALIGHT_POLLER_COMPLETE_EVENT, FuriFlagWaitAny, FuriWaitForever);
    *data = instance->data->version;
    mf_ultralight_poller_reset(instance);

    return poller_context.error;
}

static void mf_ultraight_read_signature_callback(NfcaPollerEvent event, void* context) {
    furi_assert(context);

    MfUltralightPollerContext* poller_context = context;
    if(event.type == NfcaPollerEventTypeReady) {
        poller_context->error =
            mf_ultralight_poller_async_read_signature(poller_context->instance);
        nfca_poller_halt(poller_context->instance->nfca_poller);
    } else if(event.type == NfcaPollerEventTypeError) {
        poller_context->error = mf_ultralight_process_error(event.data.error);
    }
    mf_ultralight_poller_stop(poller_context->instance);
    furi_thread_flags_set(poller_context->thread_id, MF_ULTRALIGHT_POLLER_COMPLETE_EVENT);
}

MfUltralightError
    mf_ultralight_poller_read_signature(MfUltralightPoller* instance, MfUltralightSignature* data) {
    furi_assert(instance);
    furi_assert(data);

    MfUltralightPollerContext poller_context = {};
    poller_context.instance = instance;
    poller_context.thread_id = furi_thread_get_current_id();

    mf_ultralight_poller_start(instance, mf_ultraight_read_signature_callback, &poller_context);
    furi_thread_flags_wait(MF_ULTRALIGHT_POLLER_COMPLETE_EVENT, FuriFlagWaitAny, FuriWaitForever);
    *data = instance->data->signature;
    mf_ultralight_poller_reset(instance);

    return poller_context.error;
}

static void mf_ultraight_read_counter_callback(NfcaPollerEvent event, void* context) {
    furi_assert(context);

    MfUltralightPollerContext* poller_context = context;
    if(event.type == NfcaPollerEventTypeReady) {
        poller_context->error = mf_ultralight_poller_async_read_counter(
            poller_context->instance, poller_context->data.counter_to_read);
        nfca_poller_halt(poller_context->instance->nfca_poller);
    } else if(event.type == NfcaPollerEventTypeError) {
        poller_context->error = mf_ultralight_process_error(event.data.error);
    }
    mf_ultralight_poller_stop(poller_context->instance);
    furi_thread_flags_set(poller_context->thread_id, MF_ULTRALIGHT_POLLER_COMPLETE_EVENT);
}

MfUltralightError mf_ultralight_poller_read_counter(
    MfUltralightPoller* instance,
    uint8_t counter_num,
    MfUltralightCounter* data) {
    furi_assert(instance);
    furi_assert(data);

    MfUltralightPollerContext poller_context = {};
    poller_context.data.counter_to_read = counter_num;
    poller_context.instance = instance;
    poller_context.thread_id = furi_thread_get_current_id();

    mf_ultralight_poller_start(instance, mf_ultraight_read_counter_callback, &poller_context);
    furi_thread_flags_wait(MF_ULTRALIGHT_POLLER_COMPLETE_EVENT, FuriFlagWaitAny, FuriWaitForever);
    *data = instance->data->counter[counter_num];
    mf_ultralight_poller_reset(instance);

    return poller_context.error;
}

static void mf_ultraight_read_tering_flag_callback(NfcaPollerEvent event, void* context) {
    furi_assert(context);

    MfUltralightPollerContext* poller_context = context;
    if(event.type == NfcaPollerEventTypeReady) {
        poller_context->error = mf_ultralight_poller_async_read_tearing_flag(
            poller_context->instance, poller_context->data.tearing_flag_to_read);
        nfca_poller_halt(poller_context->instance->nfca_poller);
    } else if(event.type == NfcaPollerEventTypeError) {
        poller_context->error = mf_ultralight_process_error(event.data.error);
    }
    mf_ultralight_poller_stop(poller_context->instance);
    furi_thread_flags_set(poller_context->thread_id, MF_ULTRALIGHT_POLLER_COMPLETE_EVENT);
}

MfUltralightError mf_ultralight_poller_read_tearing_flag(
    MfUltralightPoller* instance,
    uint8_t flag_num,
    MfUltralightTearingFlag* data) {
    furi_assert(instance);
    furi_assert(data);

    MfUltralightPollerContext poller_context = {};
    poller_context.data.tearing_flag_to_read = flag_num;
    poller_context.instance = instance;
    poller_context.thread_id = furi_thread_get_current_id();

    mf_ultralight_poller_start(instance, mf_ultraight_read_tering_flag_callback, &poller_context);
    furi_thread_flags_wait(MF_ULTRALIGHT_POLLER_COMPLETE_EVENT, FuriFlagWaitAny, FuriWaitForever);
    *data = instance->data->tearing_flag[flag_num];
    mf_ultralight_poller_reset(instance);

    return poller_context.error;
}
