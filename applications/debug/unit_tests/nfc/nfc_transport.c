#ifdef APP_UNIT_TESTS

#include <lib/nfc/nfc.h>
#include <lib/nfc/protocols/nfca/nfca.h>

#include <furi/furi.h>

#define NFC_MAX_DATA_SIZE (128)

typedef enum {
    NfcTransportLogLevelWarning,
    NfcTransportLogLevelInfo,
} NfcTransportLogLevel;

FuriMessageQueue* poller_queue = NULL;
FuriMessageQueue* listener_queue = NULL;

typedef enum {
    NfcMessageTypeTx,
    NfcMessageTypeTimeout,
    NfcMessageTypeAbort,
} NfcMessageType;

typedef struct {
    uint16_t data_bits;
    uint8_t data[NFC_MAX_DATA_SIZE];
} NfcMessageData;

typedef struct {
    NfcMessageType type;
    NfcMessageData data;
} NfcMessage;

typedef enum {
    NfcStateIdle,
    NfcStateStarted,
    NfcStateReady,
    NfcStateReset,
} NfcState;

typedef enum {
    NfcaColResStatusIdle,
    NfcaColResStatusInProgress,
    NfcaColResStatusDone,
} NfcaColResStatus;

typedef struct {
    NfcaSensResp sens_resp;
    NfcaSddResp sdd_resp[2];
    NfcaSelResp sel_resp[2];
} NfcaColResData;

struct Nfc {
    NfcState state;

    NfcaColResStatus col_res_status;
    NfcaColResData col_res_data;

    NfcEventCallback callback;
    void* context;

    FuriThread* worker_thread;
};

static void nfc_test_print(
    NfcTransportLogLevel log_level,
    const char* message,
    uint8_t* buffer,
    uint16_t bits) {
    FuriString* str = furi_string_alloc();
    size_t bytes = (bits + 7) / 8;

    for(size_t i = 0; i < bytes; i++) {
        furi_string_cat_printf(str, " %02X", buffer[i]);
    }
    if(log_level == NfcTransportLogLevelWarning) {
        FURI_LOG_W(message, "%s", furi_string_get_cstr(str));
    } else {
        FURI_LOG_I(message, "%s", furi_string_get_cstr(str));
    }

    furi_string_free(str);
}

static void nfc_prepare_col_res_data(
    Nfc* instance,
    uint8_t* uid,
    uint8_t uid_len,
    uint8_t* atqa,
    uint8_t sak) {
    memcpy(instance->col_res_data.sens_resp.sens_resp, atqa, 2);

    if(uid_len == 7) {
        instance->col_res_data.sdd_resp[0].nfcid[0] = 0x88;
        memcpy(&instance->col_res_data.sdd_resp[0].nfcid[1], uid, 3);
        uint8_t bss = 0;
        for(size_t i = 0; i < 4; i++) {
            bss ^= instance->col_res_data.sdd_resp[0].nfcid[i];
        }
        instance->col_res_data.sdd_resp[0].bss = bss;
        instance->col_res_data.sel_resp[0].sak = 0x04;

        memcpy(instance->col_res_data.sdd_resp[1].nfcid, &uid[3], 4);
        bss = 0;
        for(size_t i = 0; i < 4; i++) {
            bss ^= instance->col_res_data.sdd_resp[1].nfcid[i];
        }
        instance->col_res_data.sdd_resp[1].bss = bss;
        instance->col_res_data.sel_resp[1].sak = sak;

    } else {
        furi_crash("Not supporting not 7 bytes");
    }
}

Nfc* nfc_alloc() {
    Nfc* instance = malloc(sizeof(Nfc));

    return instance;
}

void nfc_free(Nfc* instance) {
    furi_assert(instance);

    free(instance);
}

void nfc_config(Nfc* instance, NfcMode mode) {
    UNUSED(instance);
    UNUSED(mode);
}

void nfc_set_fdt_poll_fc(Nfc* instance, uint32_t fdt_poll_fc) {
    UNUSED(instance);
    UNUSED(fdt_poll_fc);
}

void nfc_set_fdt_listen_fc(Nfc* instance, uint32_t fdt_listen_fc) {
    UNUSED(instance);
    UNUSED(fdt_listen_fc);
}

void nfc_set_mask_receive_time_fc(Nfc* instance, uint32_t mask_rx_time_fc) {
    UNUSED(instance);
    UNUSED(mask_rx_time_fc);
}

void nfc_set_fdt_poll_poll_us(Nfc* instance, uint32_t fdt_poll_poll_us) {
    UNUSED(instance);
    UNUSED(fdt_poll_poll_us);
}

void nfc_set_guard_time_us(Nfc* instance, uint32_t guard_time_us) {
    UNUSED(instance);
    UNUSED(guard_time_us);
}

NfcError nfc_listener_set_col_res_data(
    Nfc* instance,
    uint8_t* uid,
    uint8_t uid_len,
    uint8_t* atqa,
    uint8_t sak) {
    furi_assert(instance);
    furi_assert(uid);
    furi_assert(atqa);

    nfc_prepare_col_res_data(instance, uid, uid_len, atqa, sak);

    return NfcErrorNone;
}

static int32_t nfc_worker_poller(void* context) {
    Nfc* instance = context;
    furi_assert(instance->callback);

    instance->state = NfcStateStarted;
    NfcCommand command = NfcCommandContinue;
    NfcEvent event = {};

    while(true) {
        if(instance->state == NfcStateStarted) {
            event.type = NfcEventTypeConfigureRequest;
            instance->callback(event, instance->context);
            instance->state = NfcStateReady;
        } else if(instance->state == NfcStateReady) {
            event.type = NfcEventTypePollerReady;
            command = instance->callback(event, instance->context);
            if(command == NfcCommandReset) {
                event.type = NfcEventTypeReset;
                instance->callback(event, instance->context);
                instance->state = NfcStateStarted;
            } else if(command == NfcCommandStop) {
                event.type = NfcEventTypeReset;
                instance->callback(event, instance->context);
                break;
            }
        }
    }

    instance->state = NfcStateIdle;

    return 0;
}

void nfc_start_poller(Nfc* instance, NfcEventCallback callback, void* context) {
    furi_assert(instance);
    furi_assert(instance->worker_thread == NULL);

    furi_assert(poller_queue == NULL);
    // Check that poller is started after listener
    furi_assert(listener_queue);

    instance->callback = callback;
    instance->context = context;

    poller_queue = furi_message_queue_alloc(4, sizeof(NfcMessage));

    instance->worker_thread = furi_thread_alloc();
    furi_thread_set_name(instance->worker_thread, "NfcWorkerPoller");
    furi_thread_set_context(instance->worker_thread, instance);
    furi_thread_set_priority(instance->worker_thread, FuriThreadPriorityHigh);
    furi_thread_set_stack_size(instance->worker_thread, 8 * 1024);
    furi_thread_set_callback(instance->worker_thread, nfc_worker_poller);
    furi_thread_start(instance->worker_thread);
}

static void nfc_worker_listener_pass_col_res(Nfc* instance, uint8_t* rx_data, uint16_t rx_bits) {
    furi_assert(instance->col_res_status != NfcaColResStatusDone);

    bool processed = false;

    if((rx_bits == 7) && (rx_data[0] == 0x52)) {
        instance->col_res_status = NfcaColResStatusInProgress;
        nfc_listener_tx(instance, instance->col_res_data.sens_resp.sens_resp, 16);
        processed = true;
    } else if(rx_bits == 2 * 8) {
        if((rx_data[0] == 0x93) && (rx_data[1] == 0x20)) {
            nfc_listener_tx(
                instance, (uint8_t*)&instance->col_res_data.sdd_resp[0], sizeof(NfcaSddResp) * 8);
            processed = true;
        } else if((rx_data[0] == 0x95) && (rx_data[1] == 0x20)) {
            nfc_listener_tx(
                instance, (uint8_t*)&instance->col_res_data.sdd_resp[1], sizeof(NfcaSddResp) * 8);
            processed = true;
        }
    } else if(rx_bits == 9 * 8) {
        if((rx_data[0] == 0x93) && (rx_data[1] == 0x70)) {
            uint8_t sak_with_crc[3] = {instance->col_res_data.sel_resp[0].sak};
            nfca_append_crc(sak_with_crc, 1);
            nfc_listener_tx(instance, sak_with_crc, 3 * 8);
            processed = true;
        } else if((rx_data[0] == 0x95) && (rx_data[1] == 0x70)) {
            uint8_t sak_with_crc[3] = {instance->col_res_data.sel_resp[1].sak};
            nfca_append_crc(sak_with_crc, 1);
            nfc_listener_tx(instance, sak_with_crc, 3 * 8);

            instance->col_res_status = NfcaColResStatusDone;
            NfcEvent event = {.type = NfcEventTypeListenerActivated};
            instance->callback(event, instance->context);

            processed = true;
        }
    }

    if(!processed) {
        NfcMessage message = {.type = NfcMessageTypeTimeout};
        furi_message_queue_put(poller_queue, &message, FuriWaitForever);
    }
}

static int32_t nfc_worker_listener(void* context) {
    Nfc* instance = context;
    furi_assert(instance->callback);

    instance->state = NfcStateStarted;
    NfcEvent event = {};
    NfcMessage message = {};

    uint8_t* rx_data = malloc(NFC_MAX_DATA_SIZE);

    event.type = NfcEventTypeConfigureRequest;
    instance->callback(event, instance->context);

    while(true) {
        furi_message_queue_get(listener_queue, &message, FuriWaitForever);
        if(message.type == NfcMessageTypeAbort) {
            event.type = NfcEventTypeUserAbort;
            instance->callback(event, instance->context);
            break;
        } else if(message.type == NfcMessageTypeTx) {
            nfc_test_print(
                NfcTransportLogLevelInfo, "RDR", message.data.data, message.data.data_bits);
            if(instance->col_res_status != NfcaColResStatusDone) {
                nfc_worker_listener_pass_col_res(
                    instance, message.data.data, message.data.data_bits);
            } else {
                instance->state = NfcStateReady;
                event.type = NfcEventTypeRxEnd;
                memcpy(rx_data, message.data.data, (message.data.data_bits + 7) / 8);
                event.data.rx_data = rx_data;
                event.data.rx_bits = message.data.data_bits;
                instance->callback(event, instance->context);
            }
        }
    }

    event.type = NfcEventTypeReset;
    instance->callback(event, instance->context);

    instance->state = NfcStateIdle;
    instance->col_res_status = NfcaColResStatusIdle;
    memset(&instance->col_res_data, 0, sizeof(instance->col_res_data));

    free(rx_data);

    return 0;
}

void nfc_start_listener(Nfc* instance, NfcEventCallback callback, void* context) {
    furi_assert(instance);
    furi_assert(instance->worker_thread == NULL);

    furi_assert(listener_queue == NULL);
    // Check that poller didn't start
    furi_assert(poller_queue == NULL);

    instance->callback = callback;
    instance->context = context;

    listener_queue = furi_message_queue_alloc(4, sizeof(NfcMessage));

    instance->worker_thread = furi_thread_alloc();
    furi_thread_set_name(instance->worker_thread, "NfcWorkerListener");
    furi_thread_set_context(instance->worker_thread, instance);
    furi_thread_set_priority(instance->worker_thread, FuriThreadPriorityHigh);
    furi_thread_set_stack_size(instance->worker_thread, 8 * 1024);
    furi_thread_set_callback(instance->worker_thread, nfc_worker_listener);
    furi_thread_start(instance->worker_thread);
}

NfcError nfc_listener_sleep(Nfc* instance) {
    furi_assert(instance);
    furi_assert(poller_queue);

    instance->col_res_status = NfcaColResStatusIdle;
    NfcMessage message = {.type = NfcMessageTypeTimeout};
    furi_message_queue_put(poller_queue, &message, FuriWaitForever);

    return NfcErrorNone;
}

void nfc_listener_abort(Nfc* instance) {
    furi_assert(instance);

    NfcMessage message = {.type = NfcMessageTypeAbort};
    furi_message_queue_put(listener_queue, &message, FuriWaitForever);

    furi_thread_join(instance->worker_thread);

    furi_message_queue_free(listener_queue);
    listener_queue = NULL;

    furi_thread_free(instance->worker_thread);
    instance->worker_thread = NULL;
}

void nfc_stop(Nfc* instance) {
    furi_assert(instance);
    furi_assert(instance->worker_thread);

    furi_thread_join(instance->worker_thread);

    furi_message_queue_free(poller_queue);
    poller_queue = NULL;

    furi_thread_free(instance->worker_thread);
    instance->worker_thread = NULL;
}

// Called from worker thread

NfcError nfc_listener_tx(Nfc* instance, uint8_t* tx_data, uint16_t tx_bits) {
    furi_assert(instance);
    furi_assert(poller_queue);
    furi_assert(listener_queue);
    furi_assert(tx_data);
    furi_assert(tx_bits / 8 < NFC_MAX_DATA_SIZE);

    NfcMessage message = {};
    message.type = NfcMessageTypeTx;
    message.data.data_bits = tx_bits;
    memcpy(message.data.data, tx_data, (tx_bits + 7) / 8);

    furi_message_queue_put(poller_queue, &message, FuriWaitForever);

    return NfcErrorNone;
}

NfcError nfc_trx(
    Nfc* instance,
    uint8_t* tx_data,
    uint16_t tx_bits,
    uint8_t* rx_data,
    uint16_t rx_data_size,
    uint16_t* rx_bits,
    uint32_t fwt) {
    furi_assert(instance);
    furi_assert(tx_data);
    furi_assert(rx_data);
    furi_assert(rx_bits);
    furi_assert(poller_queue);
    furi_assert(listener_queue);
    furi_assert(tx_bits / 8 < NFC_MAX_DATA_SIZE);
    UNUSED(fwt);

    NfcError error = NfcErrorNone;

    NfcMessage message = {};
    message.type = NfcMessageTypeTx;
    message.data.data_bits = tx_bits;
    memcpy(message.data.data, tx_data, (tx_bits + 7) / 8);
    // Tx
    furi_assert(furi_message_queue_put(listener_queue, &message, FuriWaitForever) == FuriStatusOk);
    // Rx
    furi_assert(furi_message_queue_get(poller_queue, &message, FuriWaitForever) == FuriStatusOk);

    if(message.type == NfcMessageTypeTx) {
        furi_assert(message.data.data_bits / 8 <= rx_data_size);
        *rx_bits = message.data.data_bits;
        memcpy(rx_data, message.data.data, (message.data.data_bits + 7) / 8);
        nfc_test_print(NfcTransportLogLevelWarning, "TAG", rx_data, *rx_bits);
    } else if(message.type == NfcMessageTypeTimeout) {
        error = NfcErrorTimeout;
    }

    return error;
}

// Technology specific API

NfcError nfc_iso13444a_short_frame(
    Nfc* instance,
    NfcIso14443aShortFrame frame,
    uint8_t* rx_data,
    uint16_t rx_data_size,
    uint16_t* rx_bits,
    uint32_t fwt) {
    UNUSED(frame);

    uint8_t tx_data[1] = {0x52};
    uint16_t tx_bits = 7;

    return nfc_trx(instance, tx_data, tx_bits, rx_data, rx_data_size, rx_bits, fwt);
}

NfcError nfc_iso13444a_sdd_frame(
    Nfc* instance,
    uint8_t* tx_data,
    uint16_t tx_bits,
    uint8_t* rx_data,
    uint16_t rx_data_size,
    uint16_t* rx_bits,
    uint32_t fwt) {
    return nfc_trx(instance, tx_data, tx_bits, rx_data, rx_data_size, rx_bits, fwt);
}

#endif