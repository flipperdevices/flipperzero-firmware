#include "legic_prime_poller.h"
#include "legic_prime_poller_i.h"

#include <nfc/protocols/nfc_poller_base.h>
#include <furi_hal_nfc.h>

#define TAG "LegicPrimePoller"

// Gain for the next detection attempt.  The NFC app's scanner allocates a new
// poller for every detection, so this has to outlive the instance for the
// gain alternation to reach the second gain; the read poller then starts
// with the gain that detected the card.
static bool legic_prime_poller_detect_full_gain = false;

typedef NfcCommand (*LegicPrimePollerStateHandler)(LegicPrimePoller* instance);

const LegicPrimeData* legic_prime_poller_get_data(LegicPrimePoller* instance) {
    furi_assert(instance);
    furi_assert(instance->data);
    return instance->data;
}

static LegicPrimePoller* legic_prime_poller_alloc(Nfc* nfc) {
    furi_assert(nfc);

    LegicPrimePoller* instance = malloc(sizeof(LegicPrimePoller));
    instance->nfc = nfc;
    instance->state = LegicPrimePollerStateSetup;
    instance->tx_buffer = bit_buffer_alloc(LEGIC_PRIME_POLLER_MAX_BUFFER_SIZE);
    instance->rx_buffer = bit_buffer_alloc(LEGIC_PRIME_POLLER_MAX_BUFFER_SIZE);

    nfc_config(instance->nfc, NfcModePoller, NfcTechLegic);
    nfc_set_guard_time_us(instance->nfc, LEGIC_PRIME_GUARD_US);
    nfc_set_fdt_poll_fc(instance->nfc, LEGIC_PRIME_FDT_POLL_FC);
    nfc_set_fdt_poll_poll_us(instance->nfc, LEGIC_PRIME_POLL_POLL_US);

    instance->data = legic_prime_alloc();
    instance->rx_full_gain = legic_prime_poller_detect_full_gain;

    instance->legic_event.data = &instance->legic_event_data;
    instance->general_event.protocol = NfcProtocolLegicPrime;
    instance->general_event.event_data = &instance->legic_event;
    instance->general_event.instance = instance;

    return instance;
}

static void legic_prime_poller_free(LegicPrimePoller* instance) {
    furi_assert(instance);
    bit_buffer_free(instance->tx_buffer);
    bit_buffer_free(instance->rx_buffer);
    legic_prime_free(instance->data);
    free(instance);
}

static void legic_prime_poller_set_callback(
    LegicPrimePoller* instance,
    NfcGenericCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

// Handshakes tried before reporting a failure: one misdecoded card-type
// reply should not end a read.
#define LEGIC_PRIME_SETUP_ATTEMPTS (3U)

// The HAL's default receiver gain suits strongly coupled tags; a weakly
// coupled one can need full gain.  Alternate the two after a failed
// handshake or a read session that made no progress.  The gain is applied
// with the session's first frame (the IV).
static void legic_prime_poller_apply_gain(LegicPrimePoller* instance) {
    furi_hal_nfc_legic_poller_set_rx_reduced_gain(!instance->rx_full_gain);
}

static NfcCommand legic_prime_poller_setup_handler(LegicPrimePoller* instance) {
    // IV value matches Proxmark3's default (cmdhflegic.c). The LEGIC PRNG
    // requires IV to be odd; 0x55 is the empirically-known working value.
    legic_prime_poller_apply_gain(instance);
    LegicPrimeError err = legic_prime_poller_setup(instance, 0x55);
    NfcCommand command = NfcCommandContinue;
    if(err == LegicPrimeErrorNone) {
        instance->setup_attempts = 0;
        instance->state = LegicPrimePollerStateRequestMode;
    } else if(++instance->setup_attempts < LEGIC_PRIME_SETUP_ATTEMPTS) {
        instance->rx_full_gain = !instance->rx_full_gain;
        // The tag is waiting for an ACK; start over from a fresh field.
        command = NfcCommandReset;
    } else {
        instance->setup_attempts = 0;
        instance->state = LegicPrimePollerStateFailure;
        instance->legic_event_data.error = err;
    }
    return command;
}

// Bytes 0-4 (MCD, MSN, MCC) can't be written.
#define LEGIC_PRIME_WRITABLE_FROM (5U)

static NfcCommand legic_prime_poller_request_mode_handler(LegicPrimePoller* instance) {
    instance->legic_event.type = LegicPrimePollerEventTypeRequestMode;
    LegicPrimePollerEventDataModeRequest* request = &instance->legic_event_data.mode_request;
    memset(request, 0, sizeof(*request));
    request->mode = LegicPrimePollerModeRead;
    NfcCommand cmd = instance->callback(instance->general_event, instance->context);

    if(request->mode == LegicPrimePollerModeWrite) {
        instance->write_offset = request->write_offset;
        instance->write_data = request->write_data;
        instance->write_size = request->write_size;
        instance->write_pos = 0;
        instance->write_attempts = 0;
        // Proxmark3's legicrfsim.c ACKs in window 33; a real tag's window is
        // learned from its first ACK.
        instance->write_ack_window = 33;
        instance->write_uid_known = false;
        if(!request->write_data || request->write_offset < LEGIC_PRIME_WRITABLE_FROM ||
           request->write_offset + request->write_size > instance->data->data_size) {
            instance->state = LegicPrimePollerStateFailure;
            instance->legic_event_data.error = LegicPrimeErrorReadOnly;
            return cmd;
        }
        instance->state = LegicPrimePollerStateWrite;
        return cmd == NfcCommandContinue ? NfcCommandReset : cmd;
    }

    instance->state = LegicPrimePollerStateRead;
    memset(instance->done, 0, sizeof(instance->done));
    memset(instance->seen, 0, sizeof(instance->seen));
    instance->done_count = 0;
    instance->resume_done = 0;
    instance->resumes = 0;
    // The tag is waiting for the ACK of the detection handshake; start the
    // read session from a fresh field.
    return cmd == NfcCommandContinue ? NfcCommandReset : cmd;
}

// A tag that misses a frame drops its session and waits for a new IV; a
// field reset plus a fresh handshake lets the read continue where it stopped.
#define LEGIC_PRIME_MAX_RESUMES (8U)

static NfcCommand legic_prime_poller_read_handler(LegicPrimePoller* instance) {
    NfcCommand command = NfcCommandContinue;
    const uint16_t done_before = instance->done_count;
    legic_prime_poller_apply_gain(instance);
    LegicPrimeError err = legic_prime_poller_read(instance);
    if(err != LegicPrimeErrorNone && instance->done_count == done_before) {
        instance->rx_full_gain = !instance->rx_full_gain;
    }
    // Only resumes that make no progress count against the limit.
    if(instance->done_count != instance->resume_done) {
        instance->resume_done = instance->done_count;
        instance->resumes = 0;
    }
    if(err == LegicPrimeErrorNone) {
        instance->state = LegicPrimePollerStateSuccess;
    } else if(instance->resumes < LEGIC_PRIME_MAX_RESUMES) {
        instance->resumes++;
        command = NfcCommandReset;
    } else {
        instance->state = LegicPrimePollerStateFailure;
        instance->legic_event_data.error = err;
    }
    return command;
}

// Write sessions in a row that write nothing before giving up.
#define LEGIC_PRIME_WRITE_ATTEMPTS (3U)

static NfcCommand legic_prime_poller_write_handler(LegicPrimePoller* instance) {
    if(instance->write_pos == instance->write_size) {
        instance->state = LegicPrimePollerStateSuccess;
        return NfcCommandContinue;
    }

    legic_prime_poller_apply_gain(instance);
    const uint16_t pos_before = instance->write_pos;
    LegicPrimeError err = legic_prime_poller_write_session(instance);
    if(instance->write_pos == instance->write_size) {
        instance->state = LegicPrimePollerStateSuccess;
        return NfcCommandContinue;
    }
    if(instance->write_pos != pos_before) {
        // Progress: the session ended early or failed after some bytes.
        instance->write_attempts = 0;
    } else if(++instance->write_attempts >= LEGIC_PRIME_WRITE_ATTEMPTS) {
        instance->state = LegicPrimePollerStateFailure;
        instance->legic_event_data.error = err;
        return NfcCommandContinue;
    } else {
        instance->rx_full_gain = !instance->rx_full_gain;
    }
    // Continue in a new session from a fresh field.
    return NfcCommandReset;
}

static NfcCommand legic_prime_poller_success_handler(LegicPrimePoller* instance) {
    instance->legic_event.type = LegicPrimePollerEventTypeSuccess;
    NfcCommand cmd = instance->callback(instance->general_event, instance->context);
    furi_delay_ms(100);
    instance->state = LegicPrimePollerStateSetup;
    return cmd;
}

static NfcCommand legic_prime_poller_failure_handler(LegicPrimePoller* instance) {
    instance->legic_event.type = LegicPrimePollerEventTypeFailure;
    NfcCommand cmd = instance->callback(instance->general_event, instance->context);
    furi_delay_ms(100);
    instance->state = LegicPrimePollerStateSetup;
    return cmd;
}

static LegicPrimePollerStateHandler legic_prime_poller_state_handlers[LegicPrimePollerStateNum] = {
    [LegicPrimePollerStateSetup] = legic_prime_poller_setup_handler,
    [LegicPrimePollerStateRequestMode] = legic_prime_poller_request_mode_handler,
    [LegicPrimePollerStateRead] = legic_prime_poller_read_handler,
    [LegicPrimePollerStateWrite] = legic_prime_poller_write_handler,
    [LegicPrimePollerStateSuccess] = legic_prime_poller_success_handler,
    [LegicPrimePollerStateFailure] = legic_prime_poller_failure_handler,
};

static NfcCommand legic_prime_poller_run(NfcGenericEvent event, void* context) {
    furi_assert(context);
    furi_assert(event.protocol == NfcProtocolInvalid);
    furi_assert(event.event_data);

    LegicPrimePoller* instance = context;
    NfcEvent* nfc_event = event.event_data;
    NfcCommand command = NfcCommandContinue;

    furi_assert(instance->state < LegicPrimePollerStateNum);
    if(nfc_event->type == NfcEventTypePollerReady) {
        command = legic_prime_poller_state_handlers[instance->state](instance);
    }
    return command;
}

static bool legic_prime_poller_detect(NfcGenericEvent event, void* context) {
    furi_assert(context);
    furi_assert(event.event_data);
    furi_assert(event.instance);
    furi_assert(event.protocol == NfcProtocolInvalid);

    LegicPrimePoller* instance = context;
    NfcEvent* nfc_event = event.event_data;
    furi_assert(instance->state == LegicPrimePollerStateSetup);

    bool detected = false;
    if(nfc_event->type == NfcEventTypePollerReady) {
        instance->rx_full_gain = legic_prime_poller_detect_full_gain;
        legic_prime_poller_apply_gain(instance);
        LegicPrimeError err = legic_prime_poller_setup(instance, 0x55);
        detected = (err == LegicPrimeErrorNone);
        if(!detected) legic_prime_poller_detect_full_gain = !legic_prime_poller_detect_full_gain;
    }
    return detected;
}

const NfcPollerBase nfc_poller_legic_prime = {
    .alloc = (NfcPollerAlloc)legic_prime_poller_alloc,
    .free = (NfcPollerFree)legic_prime_poller_free,
    .set_callback = (NfcPollerSetCallback)legic_prime_poller_set_callback,
    .run = (NfcPollerRun)legic_prime_poller_run,
    .detect = (NfcPollerDetect)legic_prime_poller_detect,
    .get_data = (NfcPollerGetData)legic_prime_poller_get_data,
};
