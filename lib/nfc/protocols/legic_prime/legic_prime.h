/**
 * @file legic_prime.h
 * @brief LEGIC Prime protocol data structures.
 *
 * LEGIC Prime is a 13.56 MHz proprietary protocol from LEGIC Identsystems AG.
 * Tags come in three memory sizes (MIM22 / MIM256 / MIM1024). The first 22
 * bytes of every tag are a fixed header containing the manufacturer block:
 * MCD (1B) + MSN (3B) + MCC (1B CRC-8) + DCF (2B) + CDF (15B).
 *
 * The layout decoding (token types, segments) follows the Proxmark3 client
 * (client/src/cmdhflegic.c, `hf legic info`).
 */
#pragma once

#include <nfc/protocols/nfc_device_base_i.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LEGIC_PRIME_UID_SIZE         (4U) // MCD + MSN
#define LEGIC_PRIME_MEM_SIZE_MIM22   (22U)
#define LEGIC_PRIME_MEM_SIZE_MIM256  (256U)
#define LEGIC_PRIME_MEM_SIZE_MIM1024 (1024U)
#define LEGIC_PRIME_MEM_MAX          LEGIC_PRIME_MEM_SIZE_MIM1024

typedef enum {
    LegicPrimeErrorNone,
    LegicPrimeErrorNotPresent,
    LegicPrimeErrorWrongCrc,
    LegicPrimeErrorTimeout,
    LegicPrimeErrorCommunication,
    LegicPrimeErrorUnknownType,
    LegicPrimeErrorNoAck, /**< The tag did not acknowledge a write. */
    LegicPrimeErrorReadOnly, /**< Address can't be written (MCD/MSN/MCC, or past the end). */
    LegicPrimeErrorWrongCard, /**< A write session met a card with another UID. */
} LegicPrimeError;

typedef enum {
    LegicPrimeTypeMim22,
    LegicPrimeTypeMim256,
    LegicPrimeTypeMim1024,

    LegicPrimeTypeNum,
} LegicPrimeType;

/** Token type, from the DCF (bytes 5-6) and, for data cards, bytes 7-8. */
typedef enum {
    LegicPrimeTokenTypeNewMedia, /**< DCF FFFF: unwritten media. */
    LegicPrimeTokenTypeIm, /**< Unsegmented data card. */
    LegicPrimeTokenTypeImSegmented, /**< Segmented data card (IM-S). */
    LegicPrimeTokenTypeXam, /**< Master token types (DCF > 60000). */
    LegicPrimeTokenTypeIam,
    LegicPrimeTokenTypeSam,
    LegicPrimeTokenTypeGam,

    LegicPrimeTokenTypeNum,
} LegicPrimeTokenType;

/**
 * One segment of an IM-S card.  Segments start at byte 22 and follow each
 * other; every byte from 22 on is stored XORed with the MCC (byte 4).
 */
typedef struct {
    uint16_t offset; /**< Card offset of the 5-byte segment header. */
    uint16_t length; /**< Segment length including the header. */
    bool valid; /**< Header flag: segment valid. */
    bool last; /**< Header flag: last segment. */
    uint8_t wrp; /**< Write-protected payload bytes (including the WRC bytes). */
    uint8_t wrc; /**< Write-protected bytes that carry the stamp (WRC). */
    bool rd; /**< Header flag: read-protected. */
    bool crc_ok; /**< Header CRC-8 (over MCD+MSN and the header) matches. */
} LegicPrimeSegment;

typedef struct {
    LegicPrimeType type;
    uint8_t iv; // IV used on the last successful read (kept for debugging)
    uint8_t data[LEGIC_PRIME_MEM_MAX];
    uint16_t data_size; // populated bytes count (22 / 256 / 1024)
} LegicPrimeData;

extern const NfcDeviceBase nfc_device_legic_prime;

LegicPrimeData* legic_prime_alloc(void);

void legic_prime_free(LegicPrimeData* data);

void legic_prime_reset(LegicPrimeData* data);

void legic_prime_copy(LegicPrimeData* data, const LegicPrimeData* other);

bool legic_prime_verify(LegicPrimeData* data, const FuriString* device_type);

bool legic_prime_load(LegicPrimeData* data, FlipperFormat* ff, uint32_t version);

bool legic_prime_save(const LegicPrimeData* data, FlipperFormat* ff);

bool legic_prime_is_equal(const LegicPrimeData* data, const LegicPrimeData* other);

const char* legic_prime_get_device_name(const LegicPrimeData* data, NfcDeviceNameType name_type);

const uint8_t* legic_prime_get_uid(const LegicPrimeData* data, size_t* uid_len);

bool legic_prime_set_uid(LegicPrimeData* data, const uint8_t* uid, size_t uid_len);

LegicPrimeData* legic_prime_get_base_data(const LegicPrimeData* data);

uint16_t legic_prime_get_mem_size(LegicPrimeType type);

/** Returns true if the MCC byte equals CRC8 over MCD+MSN. */
bool legic_prime_verify_mcc(const LegicPrimeData* data);

/** Returns the DCF (bytes 5-6, little endian). */
uint16_t legic_prime_get_dcf(const LegicPrimeData* data);

/** Classifies the card by its DCF (and bytes 7-8 for data cards). */
LegicPrimeTokenType legic_prime_get_token_type(const LegicPrimeData* data);

/** Returns a short name for a token type ("NM", "IM", "IM-S", "XAM", ...). */
const char* legic_prime_get_token_type_name(LegicPrimeTokenType token_type);

/**
 * Gets segment `index` (0-based) of an IM-S card.
 *
 * @return false if the card is not IM-S, the segment chain ended before
 * `index` (last flag), or a header is malformed or outside the memory.
 */
bool legic_prime_get_segment(const LegicPrimeData* data, size_t index, LegicPrimeSegment* segment);

#ifdef __cplusplus
}
#endif
