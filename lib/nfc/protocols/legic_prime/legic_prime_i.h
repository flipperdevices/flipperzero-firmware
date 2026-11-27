#pragma once

#include "legic_prime.h"

#ifdef __cplusplus
extern "C" {
#endif

// Bytes 0-4 (MCD, MSN, MCC) can't be written.
#define LEGIC_PRIME_WRITABLE_FROM (5U)
// The DCF (bytes 5-6, little endian), which a real tag only lets decrease.
#define LEGIC_PRIME_DCF_OFFSET    (5U)
#define LEGIC_PRIME_DCF_SIZE      (2U)

// Frame widths in bits.  READ and WRITE frames start with the command
// (address << 1 | read bit), cmd_size_bits wide.
#define LEGIC_PRIME_IV_BITS               (7U)
#define LEGIC_PRIME_TYPE_BITS             (6U) // the tag's answer to the IV
#define LEGIC_PRIME_TYPE_MASK             ((1U << LEGIC_PRIME_TYPE_BITS) - 1U)
#define LEGIC_PRIME_ACK_BITS              (6U) // the reader's answer to the type
#define LEGIC_PRIME_CRC4_BITS             (4U)
#define LEGIC_PRIME_READ_REPLY_BITS       (8U + LEGIC_PRIME_CRC4_BITS) // data byte, CRC-4
#define LEGIC_PRIME_WRITE_FRAME_BITS(cmd) ((cmd) + 8U + LEGIC_PRIME_CRC4_BITS)
// The tag steps its keystream twice between the reader's frame and its reply.
#define LEGIC_PRIME_REPLY_STEPS           (2U)

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
