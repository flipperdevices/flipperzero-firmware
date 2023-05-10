#include "mf_ultralight_listener.h"

#include <furi.h>

#define TAG "MfUltralightListener"

#define MF_ULTRALIGHT_LISTENER_MAX_TX_BUFF_SIZE (32)

typedef enum {
    MfUltralightListenerAccessTypeRead,
    MfUltralightListenerAccessTypeWrite,
} MfUltralightListenerAccessType;

typedef enum {
    MfUltralightListenerAuthStateIdle,
    MfUltralightListenerAuthStateSuccess,
} MfUltralightListenerAuthState;

typedef enum {
    MfUltraligthListenerStateIdle,
} MfUltraligthListenerState;

struct MfUltralightListener {
    NfcaListener* nfca_listener;
    MfUltralightListenerAuthState auth_state;
    MfUltraligthListenerState state;
    MfUltralightData* data;
    uint8_t tx_data[MF_ULTRALIGHT_LISTENER_MAX_TX_BUFF_SIZE];
    uint16_t tx_bits;
    MfUltralightFeatureSupport features;
    MfUltralightConfigPages* config;

    MfUltralightListenerEventCallback callback;
    void* context;
};

typedef bool (*MfUltralightListenerCommandCallback)(
    MfUltralightListener* instance,
    uint8_t* rx_data,
    uint16_t rx_bits);

typedef struct {
    uint8_t cmd;
    uint16_t cmd_len_bits;
    MfUltralightListenerCommandCallback callback;
} MfUltralightListenerCmdHandler;

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

static bool mf_ultralight_listener_check_access(
    MfUltralightListener* instance,
    uint8_t start_page,
    MfUltralightListenerAccessType access_type) {
    bool access_success = false;
    bool is_write_op = (access_type == MfUltralightListenerAccessTypeWrite);

    do {
        if((instance->features & MfUltralightFeatureSupportAuthentication) == 0) {
            access_success = true;
            break;
        }
        if(instance->auth_state != MfUltralightListenerAuthStateSuccess) {
            if((instance->config->auth0 <= start_page) &&
               (instance->config->access.prot || is_write_op)) {
                break;
            }
        }
        if(instance->config->access.cfglck && is_write_op) {
            uint16_t config_page_start = instance->data->pages_total - 4;
            if((start_page == config_page_start) || (start_page == config_page_start + 1)) {
                break;
            }
        }

        access_success = true;
    } while(false);

    return access_success;
}

static void mf_ultralight_listener_send_short_resp(MfUltralightListener* instance, uint8_t data) {
    nfca_listener_tx(instance->nfca_listener, &data, 4);
};

static bool mf_ultralight_listener_read_page_handler(
    MfUltralightListener* instance,
    uint8_t* rx_data,
    uint16_t rx_bits) {
    UNUSED(rx_bits);
    bool command_processed = false;
    uint8_t start_page = rx_data[1];
    uint16_t pages_total = instance->data->pages_total;
    MfUltralightPageReadCommandData read_cmd_data = {};

    if(pages_total < start_page) {
        mf_ultralight_listener_send_short_resp(instance, MF_ULTRALIGHT_CMD_NACK);
        instance->state = MfUltraligthListenerStateIdle;
        instance->auth_state = MfUltralightListenerAuthStateIdle;
    } else if(!mf_ultralight_listener_check_access(
                  instance, start_page, MfUltralightListenerAccessTypeRead)) {
        mf_ultralight_listener_send_short_resp(instance, MF_ULTRALIGHT_CMD_NACK);
        instance->state = MfUltraligthListenerStateIdle;
        instance->auth_state = MfUltralightListenerAuthStateIdle;
    } else {
        uint16_t config_page = mf_ultralight_get_config_page_num(instance->data->type);
        for(size_t i = 0; i < 4; i++) {
            bool hide_data =
                ((config_page != 0) && ((i == config_page + 1U) || (i == config_page + 2U)));
            if(hide_data) {
                memset(read_cmd_data.page[i].data, 0, sizeof(MfUltralightPage));
            } else {
                read_cmd_data.page[i] = instance->data->page[(start_page + i) % pages_total];
            }
        }
        instance->tx_bits = sizeof(MfUltralightPageReadCommandData) * 8;
        nfca_listener_send_standart_frame(
            instance->nfca_listener, (uint8_t*)&read_cmd_data, instance->tx_bits);
    }
    command_processed = true;

    return command_processed;
}

static bool mf_ultralight_listener_write_page_handler(
    MfUltralightListener* instance,
    uint8_t* rx_data,
    uint16_t rx_bits) {
    UNUSED(rx_bits);
    bool command_processed = false;
    uint8_t start_page = rx_data[1];
    uint16_t pages_total = instance->data->pages_total;

    if(pages_total < start_page) {
        mf_ultralight_listener_send_short_resp(instance, MF_ULTRALIGHT_CMD_NACK);
        instance->state = MfUltraligthListenerStateIdle;
        instance->auth_state = MfUltralightListenerAuthStateIdle;
    } else if(!mf_ultralight_listener_check_access(
                  instance, start_page, MfUltralightListenerAccessTypeWrite)) {
        mf_ultralight_listener_send_short_resp(instance, MF_ULTRALIGHT_CMD_NACK);
        instance->state = MfUltraligthListenerStateIdle;
        instance->auth_state = MfUltralightListenerAuthStateIdle;
    } else {
        memcpy(instance->data->page[start_page].data, &rx_data[2], sizeof(MfUltralightPage));
        mf_ultralight_listener_send_short_resp(instance, MF_ULTRALIGHT_CMD_ACK);
    }
    command_processed = true;

    return command_processed;
}

static bool mf_ultralight_listener_read_version_handler(
    MfUltralightListener* instance,
    uint8_t* rx_data,
    uint16_t rx_bits) {
    UNUSED(rx_bits);
    UNUSED(rx_data);
    bool command_processed = false;
    if((instance->features & MfUltralightFeatureSupportReadVersion)) {
        memcpy(instance->tx_data, &instance->data->version, sizeof(instance->data->version));
        instance->tx_bits = sizeof(instance->data->version) * 8;
        nfca_listener_send_standart_frame(
            instance->nfca_listener, instance->tx_data, instance->tx_bits);
    } else {
        nfca_listener_sleep(instance->nfca_listener);
        instance->state = MfUltraligthListenerStateIdle;
    }
    command_processed = true;

    return command_processed;
}

static bool mf_ultralight_listener_read_signature_handler(
    MfUltralightListener* instance,
    uint8_t* rx_data,
    uint16_t rx_bits) {
    UNUSED(rx_bits);
    UNUSED(rx_data);

    bool command_processed = false;
    if((instance->features & MfUltralightFeatureSupportReadSignature)) {
        memcpy(instance->tx_data, &instance->data->signature, sizeof(instance->data->signature));
        instance->tx_bits = sizeof(instance->data->signature) * 8;
        nfca_listener_send_standart_frame(
            instance->nfca_listener, instance->tx_data, instance->tx_bits);
    } else {
        nfca_listener_sleep(instance->nfca_listener);
        instance->state = MfUltraligthListenerStateIdle;
    }
    command_processed = true;

    return command_processed;
}

static bool mf_ultralight_listener_read_counter_handler(
    MfUltralightListener* instance,
    uint8_t* rx_data,
    uint16_t rx_bits) {
    UNUSED(rx_bits);

    bool command_processed = false;

    do {
        uint8_t counter_num = rx_data[1];
        if((instance->features & MfUltralightFeatureSupportReadCounter) == 0) break;
        if(instance->features & MfUltralightFeatureSupportSingleCounter) {
            if(counter_num != 2) {
                break;
            }
        }
        if(instance->config) {
            if(!instance->config->access.nfc_cnt_en) {
                break;
            }
            if(instance->config->access.nfc_cnt_pwd_prot) {
                if(instance->auth_state != MfUltralightListenerAuthStateSuccess) {
                    break;
                }
            }
        }
        if(counter_num > 2) break;
        instance->tx_data[0] = (instance->data->counter[counter_num].counter >> 0) & 0xff;
        instance->tx_data[1] = (instance->data->counter[counter_num].counter >> 8) & 0xff;
        instance->tx_data[2] = (instance->data->counter[counter_num].counter >> 16) & 0xff;
        instance->tx_bits = 3 * 8;
        nfca_listener_send_standart_frame(
            instance->nfca_listener, instance->tx_data, instance->tx_bits);
        command_processed = true;
    } while(false);

    return command_processed;
}

static bool mf_ultralight_listener_check_tearing_handler(
    MfUltralightListener* instance,
    uint8_t* rx_data,
    uint16_t rx_bits) {
    UNUSED(rx_bits);

    bool command_processed = false;

    do {
        uint8_t tearing_flag_num = rx_data[1];
        if((instance->features & MfUltralightFeatureSupportCheckTearingFlag) == 0) break;
        if(tearing_flag_num > 2) break;
        instance->tx_data[0] = instance->data->tearing_flag->data[0];
        instance->tx_bits = 8;
        nfca_listener_send_standart_frame(
            instance->nfca_listener, instance->tx_data, instance->tx_bits);
        command_processed = true;
    } while(false);

    return command_processed;
}

static bool mf_ultralight_listener_auth_handler(
    MfUltralightListener* instance,
    uint8_t* rx_data,
    uint16_t rx_bits) {
    UNUSED(rx_bits);

    bool command_processed = false;

    do {
        if((instance->features & MfUltralightFeatureSupportAuthentication) == 0) break;
        MfUltralightAuthPassword password = {};
        memcpy(password.data, &rx_data[1], sizeof(MfUltralightAuthPassword));
        if(instance->callback) {
            MfUltralightListenerEventData data = {.password = password};
            MfUltralightListenerEvent event = {
                .type = MfUltralightListenerEventTypeAuth,
                .data = &data,
            };
            instance->callback(event, instance->context);
        }
        if(password.pass != instance->config->password.pass) break;
        memcpy(instance->tx_data, instance->config->pack.data, sizeof(MfUltralightAuthPack));
        instance->tx_bits = sizeof(MfUltralightAuthPack) * 8;
        instance->auth_state = MfUltralightListenerAuthStateSuccess;
        nfca_listener_send_standart_frame(
            instance->nfca_listener, instance->tx_data, instance->tx_bits);

        command_processed = true;
    } while(false);

    return command_processed;
}

static const MfUltralightListenerCmdHandler mf_ultralight_command[] = {
    {
        .cmd = MF_ULTRALIGHT_CMD_READ_PAGE,
        .cmd_len_bits = 2 * 8,
        .callback = mf_ultralight_listener_read_page_handler,
    },
    {
        .cmd = MF_ULTRALIGHT_CMD_WRITE_PAGE,
        .cmd_len_bits = 6 * 8,
        .callback = mf_ultralight_listener_write_page_handler,
    },
    {
        .cmd = MF_ULTRALIGHT_CMD_GET_VERSION,
        .cmd_len_bits = 8,
        .callback = mf_ultralight_listener_read_version_handler,
    },
    {
        .cmd = MF_ULTRALIGTH_CMD_READ_SIG,
        .cmd_len_bits = 2 * 8,
        .callback = mf_ultralight_listener_read_signature_handler,
    },
    {
        .cmd = MF_ULTRALIGHT_CMD_READ_CNT,
        .cmd_len_bits = 2 * 8,
        .callback = mf_ultralight_listener_read_counter_handler,
    },
    {
        .cmd = MF_ULTRALIGHT_CMD_CHECK_TEARING,
        .cmd_len_bits = 2 * 8,
        .callback = mf_ultralight_listener_check_tearing_handler,
    },
    {
        .cmd = MF_ULTRALIGHT_CMD_AUTH,
        .cmd_len_bits = 5 * 8,
        .callback = mf_ultralight_listener_auth_handler,
    }};

static NfcaListenerCommand
    mf_ultralight_listener_event_handler(NfcaListenerEvent event, void* context) {
    furi_assert(context);

    MfUltralightListener* instance = context;
    uint8_t* rx_data = event.data.rx_data;
    uint16_t rx_bits = event.data.rx_bits;

    NfcaListenerCommand command = NfcaListenerCommandContinue;
    if(event.type == NfcaListenerEventTypeReceivedStandartFrame) {
        bool cmd_processed = false;
        for(size_t i = 0; i < COUNT_OF(mf_ultralight_command); i++) {
            if(rx_bits != mf_ultralight_command[i].cmd_len_bits) continue;
            if(rx_data[0] != mf_ultralight_command[i].cmd) continue;
            cmd_processed = mf_ultralight_command[i].callback(instance, rx_data, rx_bits);
            if(cmd_processed) break;
        }
        if(!cmd_processed) {
            mf_ultralight_listener_send_short_resp(instance, MF_ULTRALIGHT_CMD_NACK);
            instance->state = MfUltraligthListenerStateIdle;
            instance->auth_state = MfUltralightListenerAuthStateIdle;
        }
    }

    return command;
}

static void mf_ultralight_listener_prepare_emulation(MfUltralightListener* instance) {
    MfUltralightData* data = instance->data;
    instance->features = mf_ultralight_get_feature_support_set(data->type);
    mf_ultralight_get_config_page(data, &instance->config);
}

MfUltralightListener* mf_ultralight_listener_alloc(NfcaListener* nfca_listener) {
    furi_assert(nfca_listener);

    MfUltralightListener* instance = malloc(sizeof(MfUltralightListener));
    instance->nfca_listener = nfca_listener;

    return instance;
}

MfUltralightError mf_ultralight_listener_start(
    MfUltralightListener* instance,
    MfUltralightData* data,
    MfUltralightListenerEventCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(data);

    instance->data = malloc(sizeof(MfUltralightData));
    *instance->data = *data;
    mf_ultralight_listener_prepare_emulation(instance);

    instance->callback = callback;
    instance->context = context;

    NfcaError error = nfca_listener_start(
        instance->nfca_listener,
        &instance->data->nfca_data,
        mf_ultralight_listener_event_handler,
        instance);

    return mf_ultralight_process_error(error);
}

void mf_ultralight_listener_free(MfUltralightListener* instance) {
    furi_assert(instance);

    free(instance);
}

MfUltralightError
    mf_ultralight_listener_get_data(MfUltralightListener* instance, MfUltralightData* data) {
    furi_assert(instance);
    furi_assert(instance->data);
    furi_assert(data);

    *data = *instance->data;

    return MfUltralightErrorNone;
}

MfUltralightError mf_ultralight_listener_stop(MfUltralightListener* instance) {
    furi_assert(instance);
    furi_assert(instance->data);

    NfcaError error = nfca_listener_stop(instance->nfca_listener);
    instance->state = MfUltraligthListenerStateIdle;
    free(instance->data);

    return mf_ultralight_process_error(error);
}
