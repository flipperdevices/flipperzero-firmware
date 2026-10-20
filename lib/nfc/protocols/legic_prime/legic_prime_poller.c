#include "legic_prime_poller.h"
#include "legic_prime_poller_i.h"

#include <nfc/protocols/nfc_poller_base.h>

#define TAG "LegicPrimePoller"

// IV value matches Proxmark3's default (cmdhflegic.c). The LEGIC PRNG
// requires IV to be odd; 0x55 is the empirically-known working value.
#define LEGIC_PRIME_POLLER_IV (0x55U)

// Gain for the next detection attempt.  The NFC app's scanner allocates a new
// poller for every detection, so this has to outlive the instance for the
// gain alternation to reach the second gain.  It alternates on purpose while
// no card answers; a detection leaves it at the gain that worked, which the
// read poller allocated next starts with.  CLI reads and writes don't detect
// first: they start from the last detection's gain and alternate after a
// failed handshake.
static bool legic_prime_poller_detect_reduced_gain = true;

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
    instance->rx_reduced_gain = legic_prime_poller_detect_reduced_gain;

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
    free(instance->progress);
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

// The HAL's default (reduced) receiver gain suits strongly coupled tags; a
// weakly coupled one can need full gain.  Alternate the two after a failed
// handshake or a session that made no progress.  The gain is applied with
// the session's first frame (the IV).
static void legic_prime_poller_apply_gain(LegicPrimePoller* instance) {
    nfc_legic_poller_set_rx_reduced_gain(instance->nfc, instance->rx_reduced_gain);
}

// Ends the read or write with its result, reported by the next state.
static void legic_prime_poller_finish(
    LegicPrimePoller* instance,
    LegicPrimeError error,
    uint16_t bytes_done,
    uint16_t bytes_total) {
    LegicPrimePollerEventDataResult* result = &instance->legic_event_data.result;
    result->error = error;
    result->bytes_done = bytes_done;
    result->bytes_total = bytes_total;
    result->write_unacked = false;
    instance->state = error == LegicPrimeErrorNone ? LegicPrimePollerStateSuccess :
                                                     LegicPrimePollerStateFailure;
}

static NfcCommand legic_prime_poller_setup_handler(LegicPrimePoller* instance) {
    legic_prime_poller_apply_gain(instance);
    LegicPrimeError err = legic_prime_poller_setup(instance, LEGIC_PRIME_POLLER_IV);
    NfcCommand command = NfcCommandContinue;
    if(err == LegicPrimeErrorNone) {
        instance->setup_attempts = 0;
        instance->state = LegicPrimePollerStateRequestMode;
    } else if(++instance->setup_attempts < LEGIC_PRIME_SETUP_ATTEMPTS) {
        instance->rx_reduced_gain = !instance->rx_reduced_gain;
        // The tag is waiting for an ACK; start over from a fresh field.
        command = NfcCommandReset;
    } else {
        instance->setup_attempts = 0;
        legic_prime_poller_finish(instance, err, 0, 0);
    }
    return command;
}

static NfcCommand legic_prime_poller_request_mode_handler(LegicPrimePoller* instance) {
    instance->legic_event.type = LegicPrimePollerEventTypeRequestMode;
    LegicPrimePollerEventDataModeRequest* request = &instance->legic_event_data.mode_request;
    memset(request, 0, sizeof(*request));
    request->mode = LegicPrimePollerModeRead;
    NfcCommand cmd = instance->callback(instance->general_event, instance->context);
    // Counts every session of the read or write, swaps and progress included,
    // against legic_prime_poller_max_sessions().
    instance->sessions = 0;

    if(request->mode == LegicPrimePollerModeWrite) {
        instance->write_offset = request->write_offset;
        instance->write_data = request->write_data;
        instance->write_size = request->write_size;
        instance->write_pos = 0;
        instance->write_attempts = 0;
        instance->write_unacked = false;
        // Proxmark3's legicrfsim.c ACKs in window 33; a real tag's window is
        // learned from its first ACK.
        instance->write_ack_window = 33;
        instance->uid_known = false;
        // Only what doesn't depend on the card type: one misread type reply
        // must not refuse a write, so the write session checks the range
        // once its handshake confirmed the type.
        if(!instance->write_data ||
           !legic_prime_is_writable(
               LEGIC_PRIME_MEM_MAX, instance->write_offset, instance->write_size)) {
            legic_prime_poller_finish(instance, LegicPrimeErrorReadOnly, 0, instance->write_size);
            return cmd;
        }
        instance->state = LegicPrimePollerStateWrite;
        return cmd == NfcCommandContinue ? NfcCommandReset : cmd;
    }

    instance->state = LegicPrimePollerStateRead;
    instance->read_offset = request->read_offset;
    instance->read_size = request->read_size;
    if(!instance->progress) instance->progress = malloc(LEGIC_PRIME_MEM_MAX);
    legic_prime_poller_read_reset(instance);
    instance->resume_done = 0;
    instance->resumes = 0;
    instance->swaps = 0;
    // The tag is waiting for the ACK of the detection handshake; start the
    // read session from a fresh field.
    return cmd == NfcCommandContinue ? NfcCommandReset : cmd;
}

// A tag that misses a frame drops its session and waits for a new IV; a
// field reset plus a fresh handshake and UID check lets the read continue
// where it stopped.
#define LEGIC_PRIME_MAX_RESUMES (8U)
// Card swaps a read survives: a card put down by mistake, or a sim restarted
// with another UID.  Every swap gives the new card a fresh resume budget, so
// swaps need a limit of their own for cards taking turns to end the read.
#define LEGIC_PRIME_MAX_SWAPS   (2U)

static NfcCommand legic_prime_poller_read_handler(LegicPrimePoller* instance) {
    NfcCommand command = NfcCommandContinue;
    const uint16_t done_before = instance->done_count;
    legic_prime_poller_apply_gain(instance);
    LegicPrimeError err = legic_prime_poller_read(instance);
    instance->sessions++;
    if(err == LegicPrimeErrorWrongCard) {
        // read() started over on the card that answered now.
        instance->swaps++;
        FURI_LOG_W(TAG, "Another card answered, reading it from the start");
    } else if(err != LegicPrimeErrorNone && instance->done_count == done_before) {
        instance->rx_reduced_gain = !instance->rx_reduced_gain;
    }
    // Only resumes that make no progress count against the limit.  A swap
    // starts it over for the new card.
    if(instance->done_count != instance->resume_done) {
        instance->resume_done = instance->done_count;
        instance->resumes = 0;
    }
    const uint16_t size = instance->read_end - instance->read_start;
    // The swap counted above is included: a read survives
    // LEGIC_PRIME_MAX_SWAPS swaps and the next one ends it.
    const bool budget_left =
        instance->resumes < LEGIC_PRIME_MAX_RESUMES && instance->swaps <= LEGIC_PRIME_MAX_SWAPS &&
        instance->sessions < legic_prime_poller_max_sessions(LegicPrimePollerModeRead, size);
    // An empty range (past the end of the card) is read once a session
    // confirmed the UID, and the card's type and size with it.
    if(instance->uid_known && instance->done_count == size) {
        legic_prime_poller_finish(instance, LegicPrimeErrorNone, size, size);
    } else if(budget_left) {
        instance->resumes++;
        command = NfcCommandReset;
    } else {
        // A read that only ran out of budget (resumes or sessions) did not
        // confirm the range either.
        if(err == LegicPrimeErrorNone) err = LegicPrimeErrorWrongCrc;
        legic_prime_poller_finish(instance, err, instance->done_count, size);
    }
    return command;
}

// Write sessions in a row that write nothing before giving up.
#define LEGIC_PRIME_WRITE_ATTEMPTS (3U)

static NfcCommand legic_prime_poller_write_handler(LegicPrimePoller* instance) {
    LegicPrimeError err = LegicPrimeErrorNone;
    if(instance->write_pos < instance->write_size) {
        legic_prime_poller_apply_gain(instance);
        const uint16_t pos_before = instance->write_pos;
        err = legic_prime_poller_write_session(instance);
        instance->sessions++;
        // NoAck only comes after a WRITE went out.  Sessions that fail before
        // their first WRITE leave that byte in doubt.
        if(instance->write_pos != pos_before) instance->write_unacked = false;
        if(err == LegicPrimeErrorNoAck) instance->write_unacked = true;
        if(instance->write_pos == instance->write_size) {
            err = LegicPrimeErrorNone;
        } else if(
            instance->sessions >=
            legic_prime_poller_max_sessions(LegicPrimePollerModeWrite, instance->write_size)) {
            // Still progressing, but too slowly: report what was written.
            // Timeout here means the session budget ran out, as WrongCrc does
            // for a read; the bytes done are exact, so the write can be re-run.
            if(err == LegicPrimeErrorNone) err = LegicPrimeErrorTimeout;
        } else if(instance->write_pos != pos_before) {
            // Progress: the session ended early or failed after some bytes.
            // Continue in a new session from a fresh field.
            instance->write_attempts = 0;
            return NfcCommandReset;
        } else if(++instance->write_attempts < LEGIC_PRIME_WRITE_ATTEMPTS) {
            instance->rx_reduced_gain = !instance->rx_reduced_gain;
            return NfcCommandReset;
        }
    }
    legic_prime_poller_finish(instance, err, instance->write_pos, instance->write_size);
    instance->legic_event_data.result.write_unacked = instance->write_unacked;
    return NfcCommandContinue;
}

static NfcCommand
    legic_prime_poller_report(LegicPrimePoller* instance, LegicPrimePollerEventType type) {
    instance->legic_event.type = type;
    NfcCommand cmd = instance->callback(instance->general_event, instance->context);
    furi_delay_ms(100);
    instance->state = LegicPrimePollerStateSetup;
    return cmd;
}

static NfcCommand legic_prime_poller_success_handler(LegicPrimePoller* instance) {
    return legic_prime_poller_report(instance, LegicPrimePollerEventTypeSuccess);
}

static NfcCommand legic_prime_poller_failure_handler(LegicPrimePoller* instance) {
    const LegicPrimePollerEventDataResult* result = &instance->legic_event_data.result;
    FURI_LOG_W(
        TAG,
        "Failed: error %d, %u of %u bytes, %u sessions, %u resumes, %u swaps, %u write attempts",
        result->error,
        (unsigned)result->bytes_done,
        (unsigned)result->bytes_total,
        (unsigned)instance->sessions,
        (unsigned)instance->resumes,
        (unsigned)instance->swaps,
        (unsigned)instance->write_attempts);
    return legic_prime_poller_report(instance, LegicPrimePollerEventTypeFailure);
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
        instance->rx_reduced_gain = legic_prime_poller_detect_reduced_gain;
        legic_prime_poller_apply_gain(instance);
        LegicPrimeError err = legic_prime_poller_setup(instance, LEGIC_PRIME_POLLER_IV);
        detected = (err == LegicPrimeErrorNone);
        if(!detected) {
            legic_prime_poller_detect_reduced_gain = !legic_prime_poller_detect_reduced_gain;
        }
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
