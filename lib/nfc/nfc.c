#include "nfc.h"

#include <f_hal_nfc.h>
#include <furi/furi.h>

#define TAG "Nfc"

typedef enum {
    NfcStateIdle,
    NfcStateConfigured,
    NfcStateChipSleep,
    NfcStateChipActive,
    NfcStateFieldOn,
    NfcStateFieldOff,
} NfcState;

typedef enum {
    NfcCommStateIdle,
    NfcCommStateWaitBlockTxTimer,
    NfcCommStateReadyTx,
    NfcCommStateWaitTxEnd,
    NfcCommStateWaitRxStart,
    NfcCommStateWaitRxEnd,
    NfcCommStateFailed,
} NfcCommState;

struct Nfc {
    NfcState state;
    NfcCommState comm_state;
    uint32_t fdt_listen_fc;
    uint32_t mask_rx_time_fc;
    uint32_t fdt_poll_fc;
    uint32_t fdt_poll_poll_us;
    uint32_t guard_time_us;
    NfcEventCallback callback;
    void* context;
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

static void nfc_hal_event_handler(FHalNfcEvent event, void* context) {
    furi_assert(context);
    Nfc* instance = context;

    if(instance->callback) {
        if(event == FHalNfcEventFieldOn) {
            instance->callback(NfcEventFieldOn, instance->context);
        } else if(event == FHalNfcEventTxStart) {
            instance->callback(NfcEventTxStart, instance->context);
        } else if(event == FHalNfcEventTxEnd) {
            instance->callback(NfcEventTxEnd, instance->context);
        } else if(event == FHalNfcEventRxStart) {
            instance->callback(NfcEventRxStart, instance->context);
        } else if(event == FHalNfcEventRxEnd) {
            instance->callback(NfcEventRxEnd, instance->context);
        }
    }
}

Nfc* nfc_alloc() {
    Nfc* instance = malloc(sizeof(Nfc));
    instance->state = NfcStateIdle;
    f_hal_nfc_set_callback(nfc_hal_event_handler, instance);
    f_hal_nfc_low_power_mode_stop();
    instance->state = NfcStateChipActive;

    return instance;
}

void nfc_free(Nfc* instance) {
    furi_assert(instance);
    f_hal_nfc_low_power_mode_start();
    f_hal_nfc_set_callback(NULL, NULL);

    free(instance);
}

void nfc_set_event_callback(Nfc* instance, NfcEventCallback callback, void* context) {
    furi_assert(instance);
    furi_assert(callback);

    instance->callback = callback;
    instance->context = context;
}

void nfc_config(Nfc* instance, NfcMode mode) {
    furi_assert(instance);
    if(mode == NfcModeNfcaPoller) {
        f_hal_nfc_set_mode(FHalNfcModeNfcaPoller, FHalNfcBitrate106);
        instance->state = NfcStateConfigured;
    } else if(mode == NfcModeNfcaListener) {
        f_hal_nfc_set_mode(FHalNfcModeNfcaListener, FHalNfcBitrate106);
        instance->state = NfcStateConfigured;
    }
}

NfcError nfc_listener_set_col_res_data(
    Nfc* instance,
    uint8_t* uid,
    uint8_t uid_len,
    uint8_t* atqa,
    uint8_t sak) {
    furi_assert(instance);
    furi_assert(instance->state == NfcStateConfigured);

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

static NfcError nfc_poller_trx_state_machine(Nfc* instance, uint32_t fwt_fc) {
    FHalNfcEvent event = 0;
    NfcError error = NfcErrorNone;

    while(true) {
        event = f_hal_nfc_wait_event(F_HAL_NFC_EVENT_WAIT_FOREVER);
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
            instance->comm_state = NfcCommStateWaitBlockTxTimer;
            break;
        }
        if(event & FHalNfcEventTimerFwtExpired) {
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

    return error;
}

static NfcError nfc_poller_prepare_trx(Nfc* instance) {
    furi_assert(instance);
    furi_assert(instance->state == NfcStateIdle);

    FHalNfcError error = FHalNfcErrorNone;
    NfcError ret = NfcErrorNone;
    FHalNfcEvent event = 0;
    do {
        if(instance->state == NfcStateConfigured) {
            error = f_hal_nfc_low_power_mode_stop();
            if(error != FHalNfcErrorNone) break;
            instance->state = NfcStateChipActive;
        }
        if(instance->state == NfcStateChipActive) {
            error = f_hal_nfc_poller_field_on();
            if(error != FHalNfcErrorNone) break;
            instance->state = NfcStateFieldOn;
            if(instance->fdt_poll_poll_us) {
                f_hal_nfc_timer_block_tx_start_us(instance->guard_time_us);
                instance->comm_state = NfcCommStateWaitBlockTxTimer;
                event = f_hal_nfc_wait_event(F_HAL_NFC_EVENT_WAIT_FOREVER);
                if(event & FHalNfcEventTimerBlockTxExpired) {
                    f_hal_nfc_set_mask_receive_timer(instance->mask_rx_time_fc);
                    instance->comm_state = NfcCommStateReadyTx;
                } else {
                    FURI_LOG_D(TAG, "Unexpected event in tx rx prepare %d", event);
                    instance->comm_state = NfcCommStateFailed;
                }
            }
        }
    } while(false);
    ret = nfc_process_hal_error(error);
    // Reset FIFO, prepare TX, setup Mask rx timer
    f_hal_nfc_trx_reset();
    return ret;
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
    furi_assert(rx_data);
    furi_assert(rx_bits);

    furi_assert(instance->state == NfcStateFieldOn);

    NfcError ret = NfcErrorNone;
    FHalNfcError error = FHalNfcErrorNone;
    do {
        ret = nfc_poller_prepare_trx(instance);
        if(ret != NfcErrorNone) {
            FURI_LOG_E(TAG, "Failed in prepare tx rx");
            break;
        }
        error = f_hal_nfc_poller_tx(tx_data, tx_bits);
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
        error = f_hal_nfc_poller_rx(rx_data, rx_data_size, rx_bits);
        if(error != FHalNfcErrorNone) {
            FURI_LOG_E(TAG, "Failed in poller RX");
            ret = nfc_process_hal_error(error);
            break;
        }
    } while(false);

    return ret;
}

NfcError nfc_iso13444a_short_frame(
    Nfc* instance,
    NfcIso14443aShortFrame frame,
    uint8_t* rx_data,
    uint16_t rx_data_size,
    uint16_t* rx_bits,
    uint32_t fwt) {
    furi_assert(instance);
    furi_assert(rx_data);
    furi_assert(rx_bits);

    FHalNfcaShortFrame short_frame = (frame == NfcIso14443aShortFrameAllReqa) ?
                                         FHalNfcaShortFrameAllReq :
                                         FHalNfcaShortFrameSensReq;

    furi_assert(instance->state == NfcStateFieldOn);

    NfcError ret = NfcErrorNone;
    FHalNfcError error = FHalNfcErrorNone;
    do {
        ret = nfc_poller_prepare_trx(instance);
        if(ret != NfcErrorNone) {
            FURI_LOG_E(TAG, "Failed in prepare tx rx");
            break;
        }
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
        error = f_hal_nfc_poller_rx(rx_data, rx_data_size, rx_bits);
        if(error != FHalNfcErrorNone) {
            FURI_LOG_E(TAG, "Failed in poller RX");
            ret = nfc_process_hal_error(error);
            break;
        }
    } while(false);

    return ret;
}

NfcError nfc_iso13444a_sdd_frame(
    Nfc* instance,
    uint8_t* tx_data,
    uint16_t tx_bits,
    uint8_t* rx_data,
    uint16_t rx_data_size,
    uint16_t* rx_bits,
    uint32_t fwt) {
    furi_assert(instance);
    furi_assert(instance->state == NfcStateFieldOn);

    NfcError ret = NfcErrorNone;
    FHalNfcError error = FHalNfcErrorNone;
    do {
        ret = nfc_poller_prepare_trx(instance);
        if(ret != NfcErrorNone) {
            FURI_LOG_E(TAG, "Failed in prepare tx rx");
            break;
        }
        error = f_hal_nfca_send_sdd_frame(tx_data, tx_bits);
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
        error = f_hal_nfca_receive_sdd_frame(rx_data, rx_data_size, rx_bits);
        if(error != FHalNfcErrorNone) {
            FURI_LOG_E(TAG, "Failed in poller RX");
            ret = nfc_process_hal_error(error);
            break;
        }
    } while(false);

    return ret;
}

NfcError nfc_listener_rx(
    Nfc* instance,
    uint8_t* rx_data,
    uint16_t rx_data_size,
    uint16_t* rx_bits,
    uint32_t timeout) {
    furi_assert(instance);
    furi_assert(instance->state == NfcStateConfigured);
    furi_assert(rx_data);
    furi_assert(rx_bits);

    NfcError ret = NfcErrorNone;
    if(instance->comm_state == NfcCommStateIdle) {
        f_hal_nfc_listen_start();
        instance->comm_state = NfcCommStateWaitRxStart;
    }
    do {
        FHalNfcEvent event = f_hal_nfc_wait_event(timeout);
        if(event & FHalNfcEventFieldOn) {
            FURI_LOG_D(TAG, "FieldOn");
        }
        if(event & FHalNfcEventListenerActive) {
            FURI_LOG_D(TAG, "Activated");
            f_hal_nfc_listen_start();
        }
        if(event & FHalNfcEventRxEnd) {
            FURI_LOG_D(TAG, "Received data");
            f_hal_nfc_poller_rx(rx_data, rx_data_size, rx_bits);
            break;
        }
        if(event & FHalNfcEventFieldOff) {
            FURI_LOG_D(TAG, "Field off");
            ret = NfcErrorLinkLoss;
            break;
        }
        if(event & FHalNfcEventTimeout) {
            ret = NfcErrorTimeout;
            FURI_LOG_D(TAG, "Timeout");
            break;
        }
        if(event & FHalNfcEventAbortRequest) {
            ret = NfcErrorAbortRequest;
            FURI_LOG_D(TAG, "Abirt request");
            break;
        }

        // if(event & FHalNfcEventRxStart) {
        //     FURI_LOG_I(TAG, "Listener RxStart");
        //     instance->comm_state = NfcCommStateWaitRxEnd;
        // } else if(event == FHalNfcEventTimeout) {
        //     FURI_LOG_W(TAG, "Listener Timeout");
        //     ret = NfcErrorTimeout;
        //     break;
        // }
        // event = f_hal_nfc_wait_event(timeout);
        // if(event & FHalNfcEventRxEnd) {
        //     FURI_LOG_I(TAG, "Listener Rx end");
        // }
        // FHalNfcError error = f_hal_nfc_poller_rx(rx_data, rx_data_size, rx_bits);
        // if(error != FHalNfcErrorNone) {
        //     ret = nfc_process_hal_error(error);
        //     break;
        // }
        // f_hal_nfc_timer_block_tx_start(instance->fdt_listen_fc);
        // instance->comm_state = NfcCommStateWaitBlockTxTimer;
        // break;
    } while(true);

    return ret;
}

NfcError nfc_listener_tx(Nfc* instance, uint8_t* tx_data, uint16_t tx_bits) {
    furi_assert(instance);
    furi_assert(tx_data);
    UNUSED(tx_bits);

    return NfcErrorNone;
}
