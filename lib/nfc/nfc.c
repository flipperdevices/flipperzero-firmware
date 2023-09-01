#ifndef APP_UNIT_TESTS

#include "nfc.h"

#include <f_hal_nfc.h>
#include <furi/furi.h>

#define TAG "Nfc"

#define NFC_MAX_BUFFER_SIZE (256)

typedef enum {
    NfcStateIdle,
    NfcStateFieldOn,
    NfcStateListenerStarted,
    NfcStatePollerReady,
    NfcStatePollerReset,
} NfcState;

typedef enum {
    NfcPollerStateIdle,
    NfcPollerStateStart,
    NfcPollerStateReady,
    NfcPollerStateReset,
    NfcPollerStateStop,

    NfcPollerStateNum,
} NfcPollerState;

typedef enum {
    NfcCommStateIdle,
    NfcCommStateWaitBlockTxTimer,
    NfcCommStateReadyTx,
    NfcCommStateWaitTxEnd,
    NfcCommStateWaitRxStart,
    NfcCommStateWaitRxEnd,
    NfcCommStateFailed,
} NfcCommState;

typedef enum {
    NfcConfigurationStateIdle,
    NfcConfigurationStateDone,
} NfcConfigurationState;

struct Nfc {
    NfcState state;
    NfcPollerState poller_state;
    NfcCommState comm_state;
    NfcConfigurationState config_state;
    uint32_t fdt_listen_fc;
    uint32_t mask_rx_time_fc;
    uint32_t fdt_poll_fc;
    uint32_t fdt_poll_poll_us;
    uint32_t guard_time_us;
    NfcEventCallback callback;
    void* context;

    uint8_t tx_buffer[NFC_MAX_BUFFER_SIZE];
    size_t tx_bits;
    uint8_t rx_buffer[NFC_MAX_BUFFER_SIZE];
    size_t rx_bits;

    FuriThread* worker_thread;
};

static NfcError nfc_process_hal_error(FHalNfcError error) {
    NfcError err = NfcErrorNone;

    if(error == FHalNfcErrorNone) {
        err = NfcErrorNone;
    } else if(error == FHalNfcErrorChipCommunication) {
        err = NfcErrorInternal;
    }

    return err;
}

static void nfc_listener_sleep(Nfc* instance) {
    furi_assert(instance);
    furi_assert(instance->state == NfcStateListenerStarted);

    f_hal_nfc_listener_sleep();
}

static int32_t nfc_worker_listener(void* context) {
    furi_assert(context);

    Nfc* instance = context;
    furi_assert(instance->callback);
    furi_assert(instance->config_state == NfcConfigurationStateDone);

    instance->state = NfcStateListenerStarted;

    f_hal_nfc_event_start();

    NfcEventData event_data = {};
    event_data.buffer = bit_buffer_alloc(NFC_MAX_BUFFER_SIZE);
    NfcEvent nfc_event = {.data = event_data};
    NfcCommand command = NfcCommandContinue;

    while(true) {
        FHalNfcEvent event = f_hal_nfc_listener_wait_event(F_HAL_NFC_EVENT_WAIT_FOREVER);
        if(event & FHalNfcEventAbortRequest) {
            nfc_event.type = NfcEventTypeUserAbort;
            instance->callback(nfc_event, instance->context);
            break;
        }
        if(event & FHalNfcEventFieldOn) {
            nfc_event.type = NfcEventTypeFieldOn;
            instance->callback(nfc_event, instance->context);
        }
        if(event & FHalNfcEventFieldOff) {
            nfc_event.type = NfcEventTypeFieldOff;
            instance->callback(nfc_event, instance->context);
            nfc_listener_reset(instance);
        }
        if(event & FHalNfcEventListenerActive) {
            f_hal_nfc_listener_disable_auto_col_res();
            nfc_event.type = NfcEventTypeListenerActivated;
            instance->callback(nfc_event, instance->context);
        }
        if(event & FHalNfcEventRxEnd) {
            nfc_event.type = NfcEventTypeRxEnd;
            f_hal_nfc_listener_rx(
                instance->rx_buffer, sizeof(instance->rx_buffer), &instance->rx_bits);
            bit_buffer_copy_bits(event_data.buffer, instance->rx_buffer, instance->rx_bits);
            command = instance->callback(nfc_event, instance->context);
            if(command == NfcCommandStop) {
                break;
            } else if(command == NfcCommandReset) {
                f_hal_listener_reset(instance);
            } else if(command == NfcCommandSleep) {
                f_hal_nfc_listener_sleep(instance);
            }
        }
    }

    nfc_config(instance, NfcModeIdle);
    bit_buffer_free(event_data.buffer);
    f_hal_nfc_low_power_mode_start();

    return 0;
}

typedef bool (*NfcWorkerPollerStateHandler)(Nfc* instance);

bool nfc_worker_poller_idle_handler(Nfc* instance) {
    f_hal_nfc_low_power_mode_stop();
    instance->poller_state = NfcPollerStateStart;

    return false;
}

bool nfc_worker_poller_start_handler(Nfc* instance) {
    f_hal_nfc_poller_field_on();
    if(instance->guard_time_us) {
        f_hal_nfc_timer_block_tx_start_us(instance->guard_time_us);
        FHalNfcEvent event = f_hal_nfc_poller_wait_event(F_HAL_NFC_EVENT_WAIT_FOREVER);
        furi_assert(event & FHalNfcEventTimerBlockTxExpired);
    }
    instance->poller_state = NfcPollerStateReady;

    return false;
}

bool nfc_worker_poller_ready_handler(Nfc* instance) {
    NfcCommand command = NfcCommandContinue;

    NfcEvent event = {.type = NfcEventTypePollerReady};
    command = instance->callback(event, instance->context);
    if(command == NfcCommandReset) {
        instance->poller_state = NfcPollerStateReset;
    } else if(command == NfcCommandStop) {
        instance->poller_state = NfcPollerStateStop;
    }

    return false;
}

bool nfc_worker_poller_reset_handler(Nfc* instance) {
    f_hal_nfc_low_power_mode_start();
    furi_delay_ms(100);
    instance->poller_state = NfcPollerStateIdle;

    return false;
}

bool nfc_worker_poller_stop_handler(Nfc* instance) {
    nfc_config(instance, NfcModeIdle);
    f_hal_nfc_low_power_mode_start();
    instance->poller_state = NfcPollerStateIdle;

    return true;
}

static const NfcWorkerPollerStateHandler nfc_worker_poller_state_handlers[NfcPollerStateNum] = {
    [NfcPollerStateIdle] = nfc_worker_poller_idle_handler,
    [NfcPollerStateStart] = nfc_worker_poller_start_handler,
    [NfcPollerStateReady] = nfc_worker_poller_ready_handler,
    [NfcPollerStateReset] = nfc_worker_poller_reset_handler,
    [NfcPollerStateStop] = nfc_worker_poller_stop_handler,
};

static int32_t nfc_worker_poller(void* context) {
    furi_assert(context);

    Nfc* instance = context;
    furi_assert(instance->callback);
    instance->poller_state = NfcPollerStateIdle;

    f_hal_nfc_event_start();

    bool exit = false;
    while(!exit) {
        exit = nfc_worker_poller_state_handlers[instance->poller_state](instance);
    }

    return 0;
}

Nfc* nfc_alloc() {
    furi_assert(f_hal_nfc_acquire() == FHalNfcErrorNone);

    Nfc* instance = malloc(sizeof(Nfc));
    instance->state = NfcStateIdle;
    instance->comm_state = NfcCommStateIdle;
    instance->config_state = NfcConfigurationStateIdle;
    instance->poller_state = NfcPollerStateIdle;

    instance->worker_thread = furi_thread_alloc();
    furi_thread_set_name(instance->worker_thread, "NfcWorker");
    furi_thread_set_context(instance->worker_thread, instance);
    furi_thread_set_priority(instance->worker_thread, FuriThreadPriorityHighest);
    furi_thread_set_stack_size(instance->worker_thread, 8 * 1024);

    return instance;
}

void nfc_free(Nfc* instance) {
    furi_assert(instance);
    // TODO REWORK!!!
    if(instance->state == NfcStateListenerStarted) {
        f_hal_nfc_abort();
        furi_thread_join(instance->worker_thread);
    }
    furi_thread_free(instance->worker_thread);
    f_hal_nfc_low_power_mode_start();

    free(instance);
    f_hal_nfc_release();
}

// TODO: refactor this function (use 2 parameters for mode?)
void nfc_config(Nfc* instance, NfcMode mode) {
    furi_assert(instance);
    if(mode == NfcModeIdle) {
        f_hal_nfc_reset_mode();
        instance->config_state = NfcConfigurationStateIdle;
    } else if(mode == NfcModeIso14443aPoller) {
        f_hal_nfc_set_mode(FHalNfcModePoller, FHalNfcTechIso14443a);
        instance->config_state = NfcConfigurationStateDone;
    } else if(mode == NfcModeIso14443aListener) {
        f_hal_nfc_low_power_mode_stop();
        f_hal_nfc_set_mode(FHalNfcModeListener, FHalNfcTechIso14443a);
        instance->config_state = NfcConfigurationStateDone;
    } else if(mode == NfcModeIso14443bPoller) {
        f_hal_nfc_set_mode(FHalNfcModePoller, FHalNfcTechIso14443b);
        instance->config_state = NfcConfigurationStateDone;
    } else if(mode == NfcModeIso15693Poller) {
        f_hal_nfc_set_mode(FHalNfcModePoller, FHalNfcTechIso15693);
        instance->config_state = NfcConfigurationStateDone;
    } else if(mode == NfcModeIso15693Listener) {
        f_hal_nfc_low_power_mode_stop();
        f_hal_nfc_set_mode(FHalNfcModeListener, FHalNfcTechIso15693);
        instance->config_state = NfcConfigurationStateDone;
    } else if(mode == NfcModeFelicaPoller) {
        f_hal_nfc_set_mode(FHalNfcModePoller, FHalNfcTechFelica);
        instance->config_state = NfcConfigurationStateDone;
    }
}

NfcError nfc_listener_set_col_res_data(
    Nfc* instance,
    uint8_t* uid,
    uint8_t uid_len,
    uint8_t* atqa,
    uint8_t sak) {
    furi_assert(instance);

    FHalNfcError error = furi_hal_nfca_set_col_res_data(uid, uid_len, atqa, sak);
    instance->comm_state = NfcCommStateIdle;
    return nfc_process_hal_error(error);
}

void nfc_set_fdt_poll_fc(Nfc* instance, uint32_t fdt_poll_fc) {
    furi_assert(instance);
    instance->fdt_poll_fc = fdt_poll_fc;
}

void nfc_set_fdt_listen_fc(Nfc* instance, uint32_t fdt_listen_fc) {
    furi_assert(instance);
    instance->fdt_listen_fc = fdt_listen_fc;
}

void nfc_set_fdt_poll_poll_us(Nfc* instance, uint32_t fdt_poll_poll_us) {
    furi_assert(instance);
    instance->fdt_poll_poll_us = fdt_poll_poll_us;
}

void nfc_set_guard_time_us(Nfc* instance, uint32_t guard_time_us) {
    furi_assert(instance);
    instance->guard_time_us = guard_time_us;
}

void nfc_set_mask_receive_time_fc(Nfc* instance, uint32_t mask_rx_time_fc) {
    furi_assert(instance);
    instance->mask_rx_time_fc = mask_rx_time_fc;
}

void nfc_start_poller(Nfc* instance, NfcEventCallback callback, void* context) {
    furi_assert(instance);
    furi_assert(instance->worker_thread);
    furi_assert(callback);
    furi_assert(instance->config_state == NfcConfigurationStateDone);

    instance->callback = callback;
    instance->context = context;
    furi_thread_set_callback(instance->worker_thread, nfc_worker_poller);
    furi_thread_start(instance->worker_thread);
    instance->comm_state = NfcCommStateIdle;
}

void nfc_start_listener(Nfc* instance, NfcEventCallback callback, void* context) {
    furi_assert(instance);
    furi_assert(instance->worker_thread);
    furi_assert(callback);

    instance->callback = callback;
    instance->context = context;
    furi_thread_set_callback(instance->worker_thread, nfc_worker_listener);
    furi_thread_start(instance->worker_thread);
    instance->comm_state = NfcCommStateIdle;
}

void nfc_listener_abort(Nfc* instance) {
    furi_assert(instance);
    f_hal_nfc_abort();
    furi_thread_join(instance->worker_thread);
}

void nfc_stop(Nfc* instance) {
    furi_assert(instance);
    furi_thread_join(instance->worker_thread);
}

NfcError nfc_listener_tx(Nfc* instance, const BitBuffer* tx_buffer) {
    furi_assert(instance);
    furi_assert(tx_buffer);

    NfcError ret = NfcErrorNone;
    FHalNfcError error =
        f_hal_nfc_listener_tx(bit_buffer_get_data(tx_buffer), bit_buffer_get_size(tx_buffer));
    if(error != FHalNfcErrorNone) {
        FURI_LOG_E(TAG, "Failed in listener TX");
        ret = nfc_process_hal_error(error);
    }

    return ret;
}

static NfcError nfc_poller_trx_state_machine(Nfc* instance, uint32_t fwt_fc) {
    FHalNfcEvent event = 0;
    NfcError error = NfcErrorNone;

    while(true) {
        event = f_hal_nfc_poller_wait_event(F_HAL_NFC_EVENT_WAIT_FOREVER);
        if(event & FHalNfcEventTimerBlockTxExpired) {
            if(instance->comm_state == NfcCommStateWaitBlockTxTimer) {
                instance->comm_state = NfcCommStateReadyTx;
            }
        }
        if(event & FHalNfcEventTxEnd) {
            if(instance->comm_state == NfcCommStateWaitTxEnd) {
                if(fwt_fc) {
                    f_hal_nfc_timer_fwt_start(fwt_fc + F_HAL_NFC_TIMER_OFFSET_FC);
                }
                f_hal_nfc_timer_block_tx_start_us(instance->fdt_poll_poll_us);
                instance->comm_state = NfcCommStateWaitRxStart;
            }
        }
        if(event & FHalNfcEventRxStart) {
            if(instance->comm_state == NfcCommStateWaitRxStart) {
                f_hal_nfc_timer_block_tx_stop();
                f_hal_nfc_timer_fwt_stop();
                instance->comm_state = NfcCommStateWaitRxEnd;
            }
        }
        if(event & FHalNfcEventRxEnd) {
            f_hal_nfc_timer_block_tx_start(instance->fdt_poll_fc);
            f_hal_nfc_timer_fwt_stop();
            instance->comm_state = NfcCommStateWaitBlockTxTimer;
            break;
        }
        if(event & FHalNfcEventTimerFwtExpired) {
            if(instance->comm_state == NfcCommStateWaitRxStart) {
                error = NfcErrorTimeout;
                FURI_LOG_W(TAG, "FWT Timeout");
                if(f_hal_nfc_timer_block_tx_is_running()) {
                    instance->comm_state = NfcCommStateWaitBlockTxTimer;
                } else {
                    instance->comm_state = NfcCommStateReadyTx;
                }
                break;
            }
        }
    }

    return error;
}

NfcError nfc_trx_custom_parity(
    Nfc* instance,
    const BitBuffer* tx_buffer,
    BitBuffer* rx_buffer,
    uint32_t fwt) {
    furi_assert(instance);
    furi_assert(tx_buffer);
    furi_assert(rx_buffer);

    furi_assert(instance->poller_state == NfcPollerStateReady);

    NfcError ret = NfcErrorNone;
    FHalNfcError error = FHalNfcErrorNone;
    do {
        f_hal_nfc_trx_reset();
        bit_buffer_write_bytes_with_parity(
            tx_buffer, instance->tx_buffer, sizeof(instance->tx_buffer), &instance->tx_bits);
        error = f_hal_nfc_poller_tx_custom_parity(instance->tx_buffer, instance->tx_bits);
        if(error != FHalNfcErrorNone) {
            FURI_LOG_E(TAG, "Failed in poller TX");
            ret = nfc_process_hal_error(error);
            break;
        }
        instance->comm_state = NfcCommStateWaitTxEnd;
        ret = nfc_poller_trx_state_machine(instance, fwt);
        if(ret != NfcErrorNone) {
            FURI_LOG_E(TAG, "Failed TRX state machine");
            break;
        }

        error = f_hal_nfc_poller_rx(
            instance->rx_buffer, sizeof(instance->rx_buffer), &instance->rx_bits);
        if(error != FHalNfcErrorNone) {
            FURI_LOG_E(TAG, "Failed in poller RX");
            ret = nfc_process_hal_error(error);
            break;
        }

        bit_buffer_copy_bytes_with_parity(rx_buffer, instance->rx_buffer, instance->rx_bits);
    } while(false);

    return ret;
}

NfcError nfc_trx(Nfc* instance, const BitBuffer* tx_buffer, BitBuffer* rx_buffer, uint32_t fwt) {
    furi_assert(instance);
    furi_assert(tx_buffer);
    furi_assert(rx_buffer);

    furi_assert(instance->poller_state == NfcPollerStateReady);

    NfcError ret = NfcErrorNone;
    FHalNfcError error = FHalNfcErrorNone;
    do {
        f_hal_nfc_trx_reset();
        error =
            f_hal_nfc_poller_tx(bit_buffer_get_data(tx_buffer), bit_buffer_get_size(tx_buffer));
        if(error != FHalNfcErrorNone) {
            FURI_LOG_E(TAG, "Failed in poller TX");
            ret = nfc_process_hal_error(error);
            break;
        }
        instance->comm_state = NfcCommStateWaitTxEnd;
        ret = nfc_poller_trx_state_machine(instance, fwt);
        if(ret != NfcErrorNone) {
            FURI_LOG_E(TAG, "Failed TRX state machine");
            break;
        }

        error = f_hal_nfc_poller_rx(
            instance->rx_buffer, sizeof(instance->rx_buffer), &instance->rx_bits);
        if(error != FHalNfcErrorNone) {
            FURI_LOG_E(TAG, "Failed in poller RX");
            ret = nfc_process_hal_error(error);
            break;
        }

        bit_buffer_copy_bits(rx_buffer, instance->rx_buffer, instance->rx_bits);
    } while(false);

    return ret;
}

NfcError nfc_iso14443_3a_short_frame(
    Nfc* instance,
    NfcIso14443aShortFrame frame,
    BitBuffer* rx_buffer,
    uint32_t fwt) {
    furi_assert(instance);
    furi_assert(rx_buffer);

    FHalNfcaShortFrame short_frame = (frame == NfcIso14443aShortFrameAllReqa) ?
                                         FHalNfcaShortFrameAllReq :
                                         FHalNfcaShortFrameSensReq;

    furi_assert(instance->poller_state == NfcPollerStateReady);

    NfcError ret = NfcErrorNone;
    FHalNfcError error = FHalNfcErrorNone;
    do {
        f_hal_nfc_trx_reset();
        error = f_hal_nfca_send_short_frame(short_frame);
        if(error != FHalNfcErrorNone) {
            FURI_LOG_E(TAG, "Failed in poller TX");
            ret = nfc_process_hal_error(error);
            break;
        }
        instance->comm_state = NfcCommStateWaitTxEnd;
        ret = nfc_poller_trx_state_machine(instance, fwt);
        if(ret != NfcErrorNone) {
            FURI_LOG_E(TAG, "Failed TRX state machine");
            break;
        }

        error = f_hal_nfc_poller_rx(
            instance->rx_buffer, sizeof(instance->rx_buffer), &instance->rx_bits);
        if(error != FHalNfcErrorNone) {
            FURI_LOG_E(TAG, "Failed in poller RX");
            ret = nfc_process_hal_error(error);
            break;
        }

        bit_buffer_copy_bits(rx_buffer, instance->rx_buffer, instance->rx_bits);
    } while(false);

    return ret;
}

NfcError nfc_iso14443_3a_sdd_frame(
    Nfc* instance,
    const BitBuffer* tx_buffer,
    BitBuffer* rx_buffer,
    uint32_t fwt) {
    furi_assert(instance);
    furi_assert(tx_buffer);
    furi_assert(rx_buffer);

    furi_assert(instance->poller_state == NfcPollerStateReady);

    NfcError ret = NfcErrorNone;
    FHalNfcError error = FHalNfcErrorNone;
    do {
        f_hal_nfc_trx_reset();
        error = f_hal_nfca_send_sdd_frame(
            bit_buffer_get_data(tx_buffer), bit_buffer_get_size(tx_buffer));
        if(error != FHalNfcErrorNone) {
            FURI_LOG_E(TAG, "Failed in poller TX");
            ret = nfc_process_hal_error(error);
            break;
        }
        instance->comm_state = NfcCommStateWaitTxEnd;
        ret = nfc_poller_trx_state_machine(instance, fwt);
        if(ret != NfcErrorNone) {
            FURI_LOG_E(TAG, "Failed TRX state machine");
            break;
        }

        error = f_hal_nfc_poller_rx(
            instance->rx_buffer, sizeof(instance->rx_buffer), &instance->rx_bits);
        if(error != FHalNfcErrorNone) {
            FURI_LOG_E(TAG, "Failed in poller RX");
            ret = nfc_process_hal_error(error);
            break;
        }

        bit_buffer_copy_bits(rx_buffer, instance->rx_buffer, instance->rx_bits);
    } while(false);

    return ret;
}

NfcError nfc_iso14443_3a_listener_tx_custom_parity(Nfc* instance, const BitBuffer* tx_buffer) {
    furi_assert(instance);
    furi_assert(tx_buffer);

    NfcError ret = NfcErrorNone;
    FHalNfcError error = FHalNfcErrorNone;

    const uint8_t* tx_data = bit_buffer_get_data(tx_buffer);
    const uint8_t* tx_parity = bit_buffer_get_parity(tx_buffer);
    size_t tx_bits = bit_buffer_get_size(tx_buffer);

    error = f_hal_nfca_listener_tx_custom_parity(tx_data, tx_parity, tx_bits);
    ret = nfc_process_hal_error(error);

    return ret;
}

#endif // APP_UNIT_TESTS
