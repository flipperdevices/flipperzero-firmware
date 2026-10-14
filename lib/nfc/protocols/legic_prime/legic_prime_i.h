#pragma once

#include "legic_prime.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Per-type protocol constants, shared by the poller and the listener. */
typedef struct {
    uint8_t cmd_size_bits; /**< Address bits plus the read/write bit. */
    uint8_t type_code; /**< The tag's 6-bit answer to the IV. */
    uint8_t ack_code; /**< The reader's 6-bit ACK: type code | READCMD. */
} LegicPrimeProtocolParams;

const LegicPrimeProtocolParams* legic_prime_get_protocol_params(LegicPrimeType type);

#ifdef __cplusplus
}
#endif
