#include <furi.h>
// infrared
#include <infrared.h>
#include <infrared/encoder_decoder/infrared.h>
#include <infrared/worker/infrared_transmit.h>
#include <infrared/worker/infrared_worker.h>

#include <flipper_format/flipper_format.h>

#define INFRARED_FILE_TYPE "IR signals file"
#define INFRARED_FILE_VERSION 1

typedef struct {
    size_t timings_size; /**< Number of elements in the timings array. */
    uint32_t* timings; /**< Pointer to an array of timings describing the signal. */
    uint32_t frequency; /**< Carrier frequency of the signal. */
    float duty_cycle; /**< Duty cycle of the signal. */
} InfraredRawSignal;

typedef struct InfraredSignal {
    bool is_raw;
    union {
        InfraredMessage message; // protocol, address, command, repeat
        InfraredRawSignal raw;
    } payload;
} InfraredSignal;

InfraredSignal* infrared_utils_signal_alloc();

void infrared_utils_signal_free(InfraredSignal* signal);

bool infrared_utils_read_signal_at_index(
    FlipperFormat* fffile,
    uint32_t index,
    InfraredSignal* signal,
    FuriString* name);

bool infrared_utils_write_signal(FlipperFormat* fffile, InfraredSignal* signal, FuriString* name);
