#ifdef FW_CFG_unit_tests

#include <lib/nfc/nfc.h>
#include <lib/nfc/helpers/iso14443_crc.h>
#include <lib/nfc/protocols/iso14443_3a/iso14443_3a.h>
#include <lib/nfc/protocols/felica/felica.h>
#include <lib/nfc/helpers/felica_crc.h>
#include <lib/nfc/protocols/felica/felica_poller_sync.h>

#include <furi/furi.h>

#define NFC_MAX_BUFFER_SIZE (256)

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
    uint8_t data[NFC_MAX_BUFFER_SIZE];
} NfcMessageData;

typedef struct {
    NfcMessageType type;
    NfcMessageData data;
} NfcMessage;

typedef enum {
    NfcStateIdle,
    NfcStateReady,
    NfcStateReset,
} NfcState;

typedef enum {
    Iso14443_3aColResStatusIdle,
    Iso14443_3aColResStatusInProgress,
    Iso14443_3aColResStatusDone,
} Iso14443_3aColResStatus;

typedef struct {
    Iso14443_3aSensResp sens_resp;
    Iso14443_3aSddResp sdd_resp[2];
    Iso14443_3aSelResp sel_resp[2];
} Iso14443_3aColResData;

typedef struct {
    uint8_t length;
    uint8_t polling_cmd;
    uint16_t system_code;
    uint8_t request_code;
    uint8_t time_slot;
} FelicaPollingRequest;

typedef struct {
    uint8_t code;
    FelicaIDm idm;
    FelicaPMm pmm;
} FelicaSensfResData;

typedef struct {
    uint16_t system_code;
    FelicaSensfResData sens_res;
} FelicaPTMemory;

struct Nfc {
    NfcState state;

    Iso14443_3aColResStatus col_res_status;
    Iso14443_3aColResData col_res_data;
    FelicaPTMemory pt_memory;
    bool software_col_res_required;

    NfcEventCallback callback;
    void* context;

    NfcMode mode;
    NfcTech tech;

    // LEGIC poller: the HAL's queued-frame state (furi_hal_nfc_legic.c), see
    // nfc_legic_poller_trx().  Frames queued with set_next(), not sent yet.
    NfcMessageData legic_next[2];
    uint8_t legic_next_count;
    // The queued frames went out behind the last reply; legic_next_answer is
    // what the listener answered to the last of them, for the claim.
    bool legic_next_sent;
    NfcMessage legic_next_answer;
    bool legic_claim_requested;
    int32_t legic_rx_ack_window; // set_rx_ack() for the next exchange, -1: data reply
    // LEGIC listener: the callback answered the frame it was given.
    bool legic_replied;

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

Nfc* nfc_alloc(void) {
    Nfc* instance = malloc(sizeof(Nfc));

    return instance;
}

void nfc_free(Nfc* instance) {
    furi_check(instance);

    free(instance);
}

void nfc_config(Nfc* instance, NfcMode mode, NfcTech tech) {
    UNUSED(instance);

    instance->mode = mode;
    instance->tech = tech;
    instance->legic_rx_ack_window = -1;
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

NfcError nfc_iso14443a_listener_set_col_res_data(
    Nfc* instance,
    uint8_t* uid,
    uint8_t uid_len,
    uint8_t* atqa,
    uint8_t sak) {
    furi_check(instance);
    furi_check(uid);
    furi_check(atqa);

    nfc_prepare_col_res_data(instance, uid, uid_len, atqa, sak);
    instance->software_col_res_required = true;

    return NfcErrorNone;
}

static int32_t nfc_worker_poller(void* context) {
    Nfc* instance = context;
    furi_check(instance->callback);

    instance->state = NfcStateReady;
    NfcCommand command = NfcCommandContinue;
    NfcEvent event = {};

    while(true) {
        event.type = NfcEventTypePollerReady;
        command = instance->callback(event, instance->context);
        if(command == NfcCommandStop) {
            break;
        }
    }

    instance->state = NfcStateIdle;

    return 0;
}

static void nfc_worker_listener_pass_col_res(Nfc* instance, uint8_t* rx_data, uint16_t rx_bits) {
    furi_check(instance->col_res_status != Iso14443_3aColResStatusDone);
    BitBuffer* tx_buffer = bit_buffer_alloc(NFC_MAX_BUFFER_SIZE);

    bool processed = false;

    if((rx_bits == 7) && (rx_data[0] == 0x52)) {
        instance->col_res_status = Iso14443_3aColResStatusInProgress;
        bit_buffer_copy_bytes(
            tx_buffer,
            instance->col_res_data.sens_resp.sens_resp,
            sizeof(instance->col_res_data.sens_resp.sens_resp));
        nfc_listener_tx(instance, tx_buffer);
        processed = true;
    } else if(rx_bits == 2 * 8) {
        if((rx_data[0] == 0x93) && (rx_data[1] == 0x20)) {
            bit_buffer_copy_bytes(
                tx_buffer,
                (const uint8_t*)&instance->col_res_data.sdd_resp[0],
                sizeof(Iso14443_3aSddResp));
            nfc_listener_tx(instance, tx_buffer);
            processed = true;
        } else if((rx_data[0] == 0x95) && (rx_data[1] == 0x20)) {
            bit_buffer_copy_bytes(
                tx_buffer,
                (const uint8_t*)&instance->col_res_data.sdd_resp[1],
                sizeof(Iso14443_3aSddResp));
            nfc_listener_tx(instance, tx_buffer);
            processed = true;
        }
    } else if(rx_bits == 9 * 8) {
        if((rx_data[0] == 0x93) && (rx_data[1] == 0x70)) {
            bit_buffer_set_size_bytes(tx_buffer, 1);
            bit_buffer_set_byte(tx_buffer, 0, instance->col_res_data.sel_resp[0].sak);
            iso14443_crc_append(Iso14443CrcTypeA, tx_buffer);
            nfc_listener_tx(instance, tx_buffer);
            processed = true;
        } else if((rx_data[0] == 0x95) && (rx_data[1] == 0x70)) {
            bit_buffer_set_size_bytes(tx_buffer, 1);
            bit_buffer_set_byte(tx_buffer, 0, instance->col_res_data.sel_resp[1].sak);
            iso14443_crc_append(Iso14443CrcTypeA, tx_buffer);
            nfc_listener_tx(instance, tx_buffer);
            instance->col_res_status = Iso14443_3aColResStatusDone;
            NfcEvent event = {.type = NfcEventTypeListenerActivated};
            instance->callback(event, instance->context);

            processed = true;
        }
    } else if(rx_bits == 8 * 8) {
        FelicaPollingRequest* request = (FelicaPollingRequest*)rx_data;
        if(request->system_code == 0xFFFF ||
           request->system_code == instance->pt_memory.system_code) {
            uint8_t response_size = sizeof(FelicaSensfResData) + 1;
            bit_buffer_reset(tx_buffer);
            bit_buffer_append_byte(tx_buffer, response_size);
            bit_buffer_append_bytes(
                tx_buffer, (uint8_t*)&instance->pt_memory.sens_res, sizeof(FelicaSensfResData));
            felica_crc_append(tx_buffer);
            nfc_listener_tx(instance, tx_buffer);
            instance->col_res_status = Iso14443_3aColResStatusDone;
            NfcEvent event = {.type = NfcEventTypeListenerActivated};
            instance->callback(event, instance->context);
            processed = true;
        }
    }

    if(!processed) {
        NfcMessage message = {.type = NfcMessageTypeTimeout};
        furi_message_queue_put(poller_queue, &message, FuriWaitForever);
    }

    bit_buffer_free(tx_buffer);
}

static int32_t nfc_worker_listener(void* context) {
    Nfc* instance = context;
    furi_check(instance->callback);

    NfcMessage message = {};

    NfcEventData event_data = {};
    event_data.buffer = bit_buffer_alloc(NFC_MAX_BUFFER_SIZE);
    NfcEvent nfc_event = {.data = event_data};

    while(true) {
        furi_message_queue_get(listener_queue, &message, FuriWaitForever);
        bit_buffer_copy_bits(event_data.buffer, message.data.data, message.data.data_bits);
        if((message.data.data[0] == 0x52) && (message.data.data_bits == 7)) {
            instance->col_res_status = Iso14443_3aColResStatusIdle;
        }

        if(message.type == NfcMessageTypeAbort) {
            break;
        } else if(message.type == NfcMessageTypeTx) {
            nfc_test_print(
                NfcTransportLogLevelInfo, "RDR", message.data.data, message.data.data_bits);
            if(instance->software_col_res_required &&
               (instance->col_res_status != Iso14443_3aColResStatusDone)) {
                nfc_worker_listener_pass_col_res(
                    instance, message.data.data, message.data.data_bits);
            } else {
                instance->state = NfcStateReady;
                nfc_event.type = NfcEventTypeRxEnd;
                instance->legic_replied = false;
                instance->callback(nfc_event, instance->context);
                // A LEGIC poller takes one answer per frame (nfc_legic_poller_send()),
                // so a frame left without a reply says so at once.
                if(instance->tech == NfcTechLegic && !instance->legic_replied) {
                    NfcMessage no_reply = {.type = NfcMessageTypeTimeout};
                    furi_message_queue_put(poller_queue, &no_reply, FuriWaitForever);
                }
            }
        }
    }

    instance->state = NfcStateIdle;
    instance->col_res_status = Iso14443_3aColResStatusIdle;
    memset(&instance->col_res_data, 0, sizeof(instance->col_res_data));
    bit_buffer_free(nfc_event.data.buffer);

    return 0;
}

void nfc_start(Nfc* instance, NfcEventCallback callback, void* context) {
    furi_check(instance);
    furi_check(instance->worker_thread == NULL);

    if(instance->mode == NfcModeListener) {
        furi_check(listener_queue == NULL);
        // Check that poller didn't start
        furi_check(poller_queue == NULL);
    } else {
        furi_check(poller_queue == NULL);
        // Check that poller is started after listener
        furi_check(listener_queue);
    }

    instance->callback = callback;
    instance->context = context;

    if(instance->mode == NfcModeListener) {
        listener_queue = furi_message_queue_alloc(4, sizeof(NfcMessage));
    } else {
        poller_queue = furi_message_queue_alloc(4, sizeof(NfcMessage));
    }

    instance->worker_thread = furi_thread_alloc();
    furi_thread_set_context(instance->worker_thread, instance);
    furi_thread_set_priority(instance->worker_thread, FuriThreadPriorityHigh);
    furi_thread_set_stack_size(instance->worker_thread, 8 * 1024);

    if(instance->mode == NfcModeListener) {
        furi_thread_set_name(instance->worker_thread, "NfcWorkerListener");
        furi_thread_set_callback(instance->worker_thread, nfc_worker_listener);
    } else {
        furi_thread_set_name(instance->worker_thread, "NfcWorkerPoller");
        furi_thread_set_callback(instance->worker_thread, nfc_worker_poller);
    }

    furi_thread_start(instance->worker_thread);
}

void nfc_stop(Nfc* instance) {
    furi_check(instance);
    furi_check(instance->worker_thread);

    if(instance->mode == NfcModeListener) {
        NfcMessage message = {.type = NfcMessageTypeAbort};
        furi_message_queue_put(listener_queue, &message, FuriWaitForever);
        furi_thread_join(instance->worker_thread);

        furi_message_queue_free(listener_queue);
        listener_queue = NULL;

        furi_thread_free(instance->worker_thread);
        instance->worker_thread = NULL;
    } else {
        furi_thread_join(instance->worker_thread);

        furi_message_queue_free(poller_queue);
        poller_queue = NULL;

        furi_thread_free(instance->worker_thread);
        instance->worker_thread = NULL;
    }
}

// Called from worker thread

NfcError nfc_listener_tx(Nfc* instance, const BitBuffer* tx_buffer) {
    furi_check(instance);
    furi_check(poller_queue);
    furi_check(listener_queue);
    furi_check(tx_buffer);

    NfcMessage message = {};
    message.type = NfcMessageTypeTx;
    message.data.data_bits = bit_buffer_get_size(tx_buffer);
    bit_buffer_write_bytes(tx_buffer, message.data.data, bit_buffer_get_size_bytes(tx_buffer));

    instance->legic_replied = true;
    furi_message_queue_put(poller_queue, &message, FuriWaitForever);

    return NfcErrorNone;
}

NfcError nfc_iso14443a_listener_tx_custom_parity(Nfc* instance, const BitBuffer* tx_buffer) {
    return nfc_listener_tx(instance, tx_buffer);
}

/*
 * LEGIC poller exchanges with the HAL's queued frames (furi_hal_nfc_legic.c).
 * The HAL sends a frame queued with nfc_legic_poller_set_next() from the
 * receive path of the exchange before it, behind the tag's reply, and the
 * poller's next exchange only claims it.  Here the frame goes to the listener
 * at that point and its answer is kept for the claim.  The mock's LEGIC
 * listener answers every frame, with its reply or with NfcMessageTypeTimeout,
 * so an answer always belongs to the frame just sent, and a sent frame that
 * nobody claims leaves nothing behind for the next exchange.  The HAL's
 * timing vetoes (a launch too late, a late wake-up) are not modelled.
 */
// Only bounds a listener that never answers.
#define NFC_LEGIC_ANSWER_TIMEOUT_MS (1000)

static void nfc_legic_frame_set(NfcMessageData* frame, const uint8_t* data, size_t bits) {
    furi_check(bits <= sizeof(frame->data) * 8);
    frame->data_bits = bits;
    memcpy(frame->data, data, (bits + 7) / 8);
}

// One frame to the listener; true if its answer is a reply.
static bool nfc_legic_poller_send(const NfcMessageData* frame, NfcMessage* answer) {
    NfcMessage message = {.type = NfcMessageTypeTx, .data = *frame};
    furi_check(furi_message_queue_put(listener_queue, &message, FuriWaitForever) == FuriStatusOk);
    if(furi_message_queue_get(poller_queue, answer, NFC_LEGIC_ANSWER_TIMEOUT_MS) != FuriStatusOk ||
       answer->type != NfcMessageTypeTx) {
        answer->type = NfcMessageTypeTimeout;
        return false;
    }
    nfc_test_print(NfcTransportLogLevelWarning, "TAG", answer->data.data, answer->data.data_bits);
    return true;
}

static NfcError
    nfc_legic_poller_trx(Nfc* instance, const BitBuffer* tx_buffer, BitBuffer* rx_buffer) {
    // As the HAL's tx(): a claim takes the frame sent behind the previous
    // reply, and a sent frame nobody claims is forgotten.
    const bool claims = instance->legic_claim_requested && instance->legic_next_sent;
    instance->legic_claim_requested = false;
    instance->legic_next_sent = false;
    const int32_t ack_window = instance->legic_rx_ack_window;
    instance->legic_rx_ack_window = -1;

    NfcMessage answer;
    bool replied;
    if(claims) {
        answer = instance->legic_next_answer;
        replied = answer.type == NfcMessageTypeTx;
    } else {
        NfcMessageData frame;
        nfc_legic_frame_set(
            &frame, bit_buffer_get_data(tx_buffer), bit_buffer_get_size(tx_buffer));
        replied = nfc_legic_poller_send(&frame, &answer);
    }
    // Nothing received: what set_next() queued stays queued, as in the HAL.
    if(!replied) return NfcErrorTimeout;
    bit_buffer_copy_bits(rx_buffer, answer.data.data, answer.data.data_bits);

    if(instance->legic_next_count) {
        // The HAL sends the queued frames behind a data reply, or behind a
        // write ACK in the bit window the poller predicted (set_rx_ack()): in
        // another one they would be out of step with the tag's keystream.  An
        // ACK's window is the index of its '1' bit.
        bool in_step = ack_window < 0;
        if(!in_step) {
            size_t first_one = 0;
            while(first_one < answer.data.data_bits &&
                  !((answer.data.data[first_one / 8] >> (first_one % 8)) & 1u)) {
                first_one++;
            }
            in_step = first_one == (size_t)ack_window;
        }
        if(in_step) {
            // The last one is the frame the poller claims (the READ behind
            // the handshake's ACK, which gets no reply).
            for(uint8_t i = 0; i < instance->legic_next_count; i++) {
                nfc_legic_poller_send(&instance->legic_next[i], &instance->legic_next_answer);
            }
            instance->legic_next_sent = true;
        }
        instance->legic_next_count = 0;
    }
    return NfcErrorNone;
}

NfcError
    nfc_poller_trx(Nfc* instance, const BitBuffer* tx_buffer, BitBuffer* rx_buffer, uint32_t fwt) {
    furi_check(instance);
    furi_check(tx_buffer);
    furi_check(rx_buffer);
    furi_check(poller_queue);
    furi_check(listener_queue);
    UNUSED(fwt);

    if(instance->tech == NfcTechLegic) {
        return nfc_legic_poller_trx(instance, tx_buffer, rx_buffer);
    }

    NfcError error = NfcErrorNone;

    NfcMessage message = {};
    message.type = NfcMessageTypeTx;
    message.data.data_bits = bit_buffer_get_size(tx_buffer);
    bit_buffer_write_bytes(tx_buffer, message.data.data, bit_buffer_get_size_bytes(tx_buffer));
    // Tx
    furi_check(furi_message_queue_put(listener_queue, &message, FuriWaitForever) == FuriStatusOk);
    // Rx
    FuriStatus status = furi_message_queue_get(poller_queue, &message, 50);

    if(status == FuriStatusErrorTimeout) {
        error = NfcErrorTimeout;
    } else if(message.type == NfcMessageTypeTx) {
        bit_buffer_copy_bits(rx_buffer, message.data.data, message.data.data_bits);
        nfc_test_print(
            NfcTransportLogLevelWarning, "TAG", message.data.data, message.data.data_bits);
    } else if(message.type == NfcMessageTypeTimeout) {
        error = NfcErrorTimeout;
    }

    return error;
}

NfcError nfc_iso14443a_poller_trx_custom_parity(
    Nfc* instance,
    const BitBuffer* tx_buffer,
    BitBuffer* rx_buffer,
    uint32_t fwt) {
    return nfc_poller_trx(instance, tx_buffer, rx_buffer, fwt);
}

// Technology specific API

NfcError nfc_iso14443a_poller_trx_short_frame(
    Nfc* instance,
    NfcIso14443aShortFrame frame,
    BitBuffer* rx_buffer,
    uint32_t fwt) {
    UNUSED(frame);

    BitBuffer* tx_buffer = bit_buffer_alloc(32);
    bit_buffer_set_size(tx_buffer, 7);
    bit_buffer_set_byte(tx_buffer, 0, 0x52);

    NfcError error = nfc_poller_trx(instance, tx_buffer, rx_buffer, fwt);

    bit_buffer_free(tx_buffer);

    return error;
}

NfcError nfc_iso14443a_poller_trx_sdd_frame(
    Nfc* instance,
    const BitBuffer* tx_buffer,
    BitBuffer* rx_buffer,
    uint32_t fwt) {
    return nfc_poller_trx(instance, tx_buffer, rx_buffer, fwt);
}

NfcError nfc_iso15693_listener_tx_sof(Nfc* instance) {
    UNUSED(instance);

    return NfcErrorNone;
}

NfcError nfc_felica_listener_set_sensf_res_data(
    Nfc* instance,
    const uint8_t* idm,
    const uint8_t idm_len,
    const uint8_t* pmm,
    const uint8_t pmm_len,
    const uint16_t sys_code) {
    furi_assert(instance);
    furi_assert(idm);
    furi_assert(pmm);
    furi_assert(idm_len == 8);
    furi_assert(pmm_len == 8);

    instance->pt_memory.system_code = sys_code;
    instance->pt_memory.sens_res.code = 0x01;
    instance->software_col_res_required = true;
    memcpy(instance->pt_memory.sens_res.idm.data, idm, idm_len);
    memcpy(instance->pt_memory.sens_res.pmm.data, pmm, pmm_len);
    return NfcErrorNone;
}

void nfc_felica_listener_timer_anticol_start(Nfc* instance, uint8_t target_time_slot) {
    furi_check(instance);

    UNUSED(target_time_slot);
}

void nfc_felica_listener_timer_anticol_stop(Nfc* instance) {
    furi_check(instance);
}

void nfc_legic_poller_set_rx_bits(Nfc* instance, size_t bits) {
    furi_check(instance);
    furi_check(instance->mode == NfcModePoller);

    UNUSED(bits);
}

void nfc_legic_poller_set_rx_ack(Nfc* instance, size_t window) {
    furi_check(instance);
    furi_check(instance->mode == NfcModePoller);

    instance->legic_rx_ack_window = window;
}

void nfc_legic_poller_set_rx_reduced_gain(Nfc* instance, bool reduced) {
    furi_check(instance);
    furi_check(instance->mode == NfcModePoller);

    UNUSED(reduced);
}

void nfc_legic_poller_set_next(
    Nfc* instance,
    const uint8_t* first,
    size_t first_bits,
    const uint8_t* second,
    size_t second_bits) {
    furi_check(instance);
    furi_check(instance->mode == NfcModePoller);
    furi_check(first);

    nfc_legic_frame_set(&instance->legic_next[0], first, first_bits);
    instance->legic_next_count = 1;
    if(second && second_bits) {
        nfc_legic_frame_set(&instance->legic_next[1], second, second_bits);
        instance->legic_next_count = 2;
    }
    // legic_next_sent is about the frame queued before, which may still wait
    // for its claim: left alone, as in the HAL.
}

bool nfc_legic_poller_next_sent(Nfc* instance) {
    furi_check(instance);
    furi_check(instance->mode == NfcModePoller);

    return instance->legic_next_sent;
}

void nfc_legic_poller_claim_next(Nfc* instance) {
    furi_check(instance);
    furi_check(instance->mode == NfcModePoller);

    instance->legic_claim_requested = true;
}

void nfc_legic_poller_cancel_next(Nfc* instance) {
    furi_check(instance);
    furi_check(instance->mode == NfcModePoller);

    instance->legic_next_count = 0;
    instance->legic_next_sent = false;
    instance->legic_claim_requested = false;
}

void nfc_legic_listener_set_ref_to_frame_end(Nfc* instance, uint32_t extra_fc) {
    furi_check(instance);
    furi_check(instance->mode == NfcModeListener);

    UNUSED(extra_fc);
}

void nfc_legic_listener_set_ref_to_tx_end(Nfc* instance) {
    furi_check(instance);
    furi_check(instance->mode == NfcModeListener);
}

uint32_t nfc_legic_listener_get_elapsed_fc(Nfc* instance) {
    furi_check(instance);
    furi_check(instance->mode == NfcModeListener);

    return 0;
}

void nfc_legic_listener_set_tx_delay(Nfc* instance, uint32_t chips) {
    furi_check(instance);
    furi_check(instance->mode == NfcModeListener);

    UNUSED(chips);
}

#endif
