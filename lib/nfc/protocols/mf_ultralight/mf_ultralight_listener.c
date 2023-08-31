#include "mf_ultralight_listener_i.h"
#include "mf_ultralight_listener_defs.h"

#include <lib/nfc/protocols/iso14443_3a/iso14443_3a_listener_i.h>

#include <furi.h>

#define TAG "MfUltralightListener"

#define MF_ULTRALIGHT_LISTENER_MAX_TX_BUFF_SIZE (32)

typedef enum {
    MfUltralightListenerAccessTypeRead,
    MfUltralightListenerAccessTypeWrite,
} MfUltralightListenerAccessType;

typedef struct {
    uint8_t cmd;
    size_t cmd_len_bits;
    MfUltralightListenerCommandCallback callback;
} MfUltralightListenerCmdHandler;

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
    furi_assert(instance->tx_buffer);

    bit_buffer_set_size(instance->tx_buffer, 4);
    bit_buffer_set_byte(instance->tx_buffer, 0, data);
    iso14443_3a_listener_tx(instance->iso14443_3a_listener, instance->tx_buffer);
};

static MfUltralightCommand
    mf_ultralight_listener_read_page_handler(MfUltralightListener* instance, BitBuffer* buffer) {
    uint8_t start_page = bit_buffer_get_byte(buffer, 1);
    uint16_t pages_total = instance->data->pages_total;
    MfUltralightPageReadCommandData read_cmd_data = {};
    MfUltralightCommand command = MfUltralightCommandNotProcessedNAK;

    FURI_LOG_D(TAG, "CMD_READ: %d", start_page);

    if(pages_total < start_page) {
        instance->state = MfUltraligthListenerStateIdle;
        instance->auth_state = MfUltralightListenerAuthStateIdle;
    } else if(!mf_ultralight_listener_check_access(
                  instance, start_page, MfUltralightListenerAccessTypeRead)) {
        instance->state = MfUltraligthListenerStateIdle;
        instance->auth_state = MfUltralightListenerAuthStateIdle;
    } else {
        mf_ultralight_mirror_read_prepare(start_page, instance);

        uint16_t config_page = mf_ultralight_get_config_page_num(instance->data->type);
        for(size_t i = 0; i < 4; i++) {
            bool hide_data =
                ((config_page != 0) && ((i == config_page + 1U) || (i == config_page + 2U)));
            if(hide_data) {
                memset(read_cmd_data.page[i].data, 0, sizeof(MfUltralightPage));
            } else {
                uint8_t current_page = start_page + i;
                read_cmd_data.page[i] = instance->data->page[current_page % pages_total];

                mf_ultralight_mirror_read_handler(
                    current_page, read_cmd_data.page[i].data, instance);
            }
        }

        bit_buffer_copy_bytes(
            instance->tx_buffer,
            (uint8_t*)&read_cmd_data,
            sizeof(MfUltralightPageReadCommandData));
        iso14443_3a_listener_send_standard_frame(
            instance->iso14443_3a_listener, instance->tx_buffer);
        command = MfUltralightCommandProcessed;
    }

    return command;
}

static MfUltralightCommand
    mf_ultralight_listener_write_page_handler(MfUltralightListener* instance, BitBuffer* buffer) {
    uint8_t start_page = bit_buffer_get_byte(buffer, 1);
    uint16_t pages_total = instance->data->pages_total;
    MfUltralightCommand command = MfUltralightCommandNotProcessedNAK;

    FURI_LOG_D(TAG, "CMD_WRITE");

    if(pages_total < start_page) {
        instance->state = MfUltraligthListenerStateIdle;
        instance->auth_state = MfUltralightListenerAuthStateIdle;
    } else if(!mf_ultralight_listener_check_access(
                  instance, start_page, MfUltralightListenerAccessTypeWrite)) {
        instance->state = MfUltraligthListenerStateIdle;
        instance->auth_state = MfUltralightListenerAuthStateIdle;
    } else {
        const uint8_t* rx_data = bit_buffer_get_data(buffer);
        memcpy(instance->data->page[start_page].data, &rx_data[2], sizeof(MfUltralightPage));
        mf_ultralight_listener_send_short_resp(instance, MF_ULTRALIGHT_CMD_ACK);
        command = MfUltralightCommandProcessed;
    }

    return command;
}

static MfUltralightCommand
    mf_ultralight_listener_read_version_handler(MfUltralightListener* instance, BitBuffer* buffer) {
    UNUSED(buffer);
    MfUltralightCommand command = MfUltralightCommandNotProcessedSilent;

    FURI_LOG_D(TAG, "CMD_GET_VERSION");

    if(mf_ultralight_support_feature(instance->features, MfUltralightFeatureSupportReadVersion)) {
        bit_buffer_copy_bytes(
            instance->tx_buffer, (uint8_t*)&instance->data->version, sizeof(MfUltralightVersion));
        iso14443_3a_listener_send_standard_frame(
            instance->iso14443_3a_listener, instance->tx_buffer);
        command = MfUltralightCommandProcessed;
    } else {
        instance->state = MfUltraligthListenerStateIdle;
    }

    return command;
}

static MfUltralightCommand mf_ultralight_listener_read_signature_handler(
    MfUltralightListener* instance,
    BitBuffer* buffer) {
    UNUSED(buffer);
    MfUltralightCommand command = MfUltralightCommandNotProcessedSilent;

    FURI_LOG_D(TAG, "CMD_READ_SIG");

    if(mf_ultralight_support_feature(instance->features, MfUltralightFeatureSupportReadSignature)) {
        bit_buffer_copy_bytes(
            instance->tx_buffer, instance->data->signature.data, sizeof(MfUltralightSignature));
        iso14443_3a_listener_send_standard_frame(
            instance->iso14443_3a_listener, instance->tx_buffer);
        command = MfUltralightCommandProcessed;
    } else {
        instance->state = MfUltraligthListenerStateIdle;
    }

    return command;
}

static MfUltralightCommand
    mf_ultralight_listener_read_counter_handler(MfUltralightListener* instance, BitBuffer* buffer) {
    MfUltralightCommand command = MfUltralightCommandNotProcessedNAK;

    FURI_LOG_D(TAG, "CMD_READ_CNT");

    do {
        uint8_t counter_num = bit_buffer_get_byte(buffer, 1);
        if(!mf_ultralight_support_feature(
               instance->features, MfUltralightFeatureSupportReadCounter))
            break;

        if(mf_ultralight_support_feature(
               instance->features, MfUltralightFeatureSupportSingleCounter)) {
            if(instance->config == NULL) break;

            if(!instance->config->access.nfc_cnt_en || counter_num != 2) break;

            if(instance->config->access.nfc_cnt_pwd_prot) {
                if(instance->auth_state != MfUltralightListenerAuthStateSuccess) {
                    break;
                }
            }
        }

        if(counter_num > 2) break;
        uint8_t cnt_value[3] = {
            (instance->data->counter[counter_num].counter >> 0) & 0xff,
            (instance->data->counter[counter_num].counter >> 8) & 0xff,
            (instance->data->counter[counter_num].counter >> 16) & 0xff,
        };
        bit_buffer_copy_bytes(instance->tx_buffer, cnt_value, sizeof(cnt_value));
        iso14443_3a_listener_send_standard_frame(
            instance->iso14443_3a_listener, instance->tx_buffer);
        command = MfUltralightCommandProcessed;
    } while(false);

    return command;
}

static MfUltralightCommand mf_ultralight_listener_increase_counter_handler(
    MfUltralightListener* instance,
    BitBuffer* buffer) {
    MfUltralightCommand command = MfUltralightCommandNotProcessedNAK;

    FURI_LOG_D(TAG, "CMD_INCR_CNT");

    do {
        if(!mf_ultralight_support_feature(
               instance->features, MfUltralightFeatureSupportIncCounter)) {
            command = MfUltralightCommandNotProcessedSilent;
            break;
        }

        uint8_t counter_num = bit_buffer_get_byte(buffer, 1);
        if(counter_num > 2) break;

        if(instance->data->counter[counter_num].counter == MF_ULTRALIGHT_MAX_CNTR_VAL) {
            command = MfUltralightCommandProcessed;
            break;
        }

        MfUltralightCounter buf_counter = {};
        bit_buffer_write_bytes_mid(buffer, buf_counter.data, 2, sizeof(buf_counter.data));
        uint32_t incr_value = buf_counter.counter;

        if(instance->data->counter[counter_num].counter + incr_value > MF_ULTRALIGHT_MAX_CNTR_VAL)
            break;

        instance->data->counter[counter_num].counter += incr_value;
        mf_ultralight_listener_send_short_resp(instance, MF_ULTRALIGHT_CMD_ACK);
        command = MfUltralightCommandProcessed;
    } while(false);

    return command;
}

static MfUltralightCommand mf_ultralight_listener_check_tearing_handler(
    MfUltralightListener* instance,
    BitBuffer* buffer) {
    MfUltralightCommand command = MfUltralightCommandNotProcessedNAK;

    FURI_LOG_D(TAG, "CMD_CHECK_TEARING");

    do {
        uint8_t tearing_flag_num = bit_buffer_get_byte(buffer, 1);
        if(!mf_ultralight_support_feature(
               instance->features,
               MfUltralightFeatureSupportCheckTearingFlag |
                   MfUltralightFeatureSupportSingleCounter)) {
            break;
        }
        if(mf_ultralight_support_feature(
               instance->features, MfUltralightFeatureSupportSingleCounter) &&
           (tearing_flag_num != 2)) {
            break;
        }

        bit_buffer_set_size_bytes(instance->tx_buffer, 1);
        bit_buffer_set_byte(
            instance->tx_buffer, 0, instance->data->tearing_flag->data[tearing_flag_num]);
        iso14443_3a_listener_send_standard_frame(
            instance->iso14443_3a_listener, instance->tx_buffer);
        command = MfUltralightCommandProcessed;

    } while(false);

    return command;
}

static MfUltralightCommand
    mf_ultralight_listener_auth_handler(MfUltralightListener* instance, BitBuffer* buffer) {
    MfUltralightCommand command = MfUltralightCommandNotProcessedSilent;

    FURI_LOG_D(TAG, "CMD_AUTH");

    do {
        if(!mf_ultralight_support_feature(
               instance->features, MfUltralightFeatureSupportAuthentication))
            break;

        const uint8_t* rx_data = bit_buffer_get_data(buffer);
        MfUltralightAuthPassword password = {};
        memcpy(password.data, &rx_data[1], sizeof(MfUltralightAuthPassword));
        if(instance->callback) {
            instance->mfu_event_data.password = password;
            instance->mfu_event.type = MfUltralightListenerEventTypeAuth;
            instance->callback(instance->generic_event, instance->context);
        }
        if(password.pass != instance->config->password.pass) break;

        bit_buffer_copy_bytes(
            instance->tx_buffer, instance->config->pack.data, sizeof(MfUltralightAuthPack));
        instance->auth_state = MfUltralightListenerAuthStateSuccess;
        iso14443_3a_listener_send_standard_frame(
            instance->iso14443_3a_listener, instance->tx_buffer);

        command = MfUltralightCommandProcessed;
    } while(false);

    return command;
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
    },
    {
        .cmd = MF_ULTRALIGHT_CMD_INCR_CNT,
        .cmd_len_bits = 6 * 8,
        .callback = mf_ultralight_listener_increase_counter_handler,
    },
};

static void mf_ultralight_listener_prepare_emulation(MfUltralightListener* instance) {
    MfUltralightData* data = instance->data;
    instance->features = mf_ultralight_get_feature_support_set(data->type);
    mf_ultralight_get_config_page(data, &instance->config);
    mf_ultraligt_mirror_prepare_emulation(instance);
}

MfUltralightListener* mf_ultralight_listener_alloc(
    Iso14443_3aListener* iso14443_3a_listener,
    const MfUltralightData* data) {
    furi_assert(iso14443_3a_listener);

    MfUltralightListener* instance = malloc(sizeof(MfUltralightListener));
    instance->mirror.ascii_mirror_data = furi_string_alloc();
    instance->iso14443_3a_listener = iso14443_3a_listener;
    instance->data = mf_ultralight_alloc();
    mf_ultralight_copy(instance->data, data);
    mf_ultralight_listener_prepare_emulation(instance);
    instance->tx_buffer = bit_buffer_alloc(MF_ULTRALIGHT_LISTENER_MAX_TX_BUFF_SIZE);

    instance->mfu_event.data = &instance->mfu_event_data;
    instance->generic_event.protocol = NfcProtocolMfUltralight;
    instance->generic_event.instance = instance;
    instance->generic_event.data = &instance->mfu_event;

    return instance;
}

void mf_ultralight_listener_free(MfUltralightListener* instance) {
    furi_assert(instance);
    furi_assert(instance->data);
    furi_assert(instance->tx_buffer);

    bit_buffer_free(instance->tx_buffer);
    furi_string_free(instance->mirror.ascii_mirror_data);
    mf_ultralight_free(instance->data);
    free(instance);
}

const MfUltralightData* mf_ultralight_listener_get_data(MfUltralightListener* instance) {
    furi_assert(instance);
    furi_assert(instance->data);

    return instance->data;
}

void mf_ultralight_listener_set_callback(
    MfUltralightListener* instance,
    NfcGenericCallback callback,
    void* context) {
    furi_assert(instance);

    instance->callback = callback;
    instance->context = context;
}

NfcCommand mf_ultralight_listener_run(NfcGenericEvent event, void* context) {
    furi_assert(context);
    furi_assert(event.protocol == NfcProtocolIso14443_3a);
    furi_assert(event.data);

    MfUltralightListener* instance = context;
    Iso14443_3aListenerEvent* iso14443_3a_event = event.data;
    BitBuffer* rx_buffer = iso14443_3a_event->data->buffer;
    NfcCommand command = NfcCommandContinue;

    if(iso14443_3a_event->type == Iso14443_3aListenerEventTypeReceivedStandardFrame) {
        MfUltralightCommand mfu_command = MfUltralightCommandNotFound;
        size_t size = bit_buffer_get_size(rx_buffer);
        uint8_t cmd = bit_buffer_get_byte(rx_buffer, 0);

        for(size_t i = 0; i < COUNT_OF(mf_ultralight_command); i++) {
            if(size != mf_ultralight_command[i].cmd_len_bits) continue;
            if(cmd != mf_ultralight_command[i].cmd) continue;
            mfu_command = mf_ultralight_command[i].callback(instance, rx_buffer);

            if(mfu_command != MfUltralightCommandNotFound) break;
        }

        if(mfu_command != MfUltralightCommandProcessed) {
            instance->state = MfUltraligthListenerStateIdle;
            instance->auth_state = MfUltralightListenerAuthStateIdle;
            command = NfcCommandReset;

            if(mfu_command == MfUltralightCommandNotProcessedNAK) {
                mf_ultralight_listener_send_short_resp(instance, MF_ULTRALIGHT_CMD_NACK);
            }
        }
    } else if(iso14443_3a_event->type == Iso14443_3aListenerEventTypeReceivedData) {
        command = NfcCommandReset;
    } else if(iso14443_3a_event->type == Iso14443_3aListenerEventTypeFieldOff) {
        command = NfcCommandReset;
    } else if(iso14443_3a_event->type == Iso14443_3aListenerEventTypeHalted) {
        command = NfcCommandReset;
    }

    return command;
}

const NfcListenerBase mf_ultralight_listener = {
    .alloc = (NfcListenerAlloc)mf_ultralight_listener_alloc,
    .free = (NfcListenerFree)mf_ultralight_listener_free,
    .get_data = (NfcListenerGetData)mf_ultralight_listener_get_data,
    .set_callback = (NfcListenerSetCallback)mf_ultralight_listener_set_callback,
    .run = (NfcListenerRun)mf_ultralight_listener_run,
};
