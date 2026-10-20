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
    LegicPrimeErrorWrongCrc, /**< Replies kept failing their CRC: bytes could not be confirmed. */
    LegicPrimeErrorTimeout,
    LegicPrimeErrorCommunication,
    LegicPrimeErrorUnknownType,
    LegicPrimeErrorNoAck, /**< The tag did not acknowledge a write. */
    LegicPrimeErrorReadOnly, /**< Address can't be written (MCD/MSN/MCC, or past the end). */
    LegicPrimeErrorWrongCard, /**< A session met a card with another UID than the first. */
    LegicPrimeErrorUidUnconfirmed, /**< A session could not read the UID reliably. */
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
    uint16_t payload_offset; /**< Card offset of the payload, after the header. */
    uint16_t payload_length; /**< Payload length, without the header. */
    bool valid; /**< Header flag: segment valid. */
    bool last; /**< Header flag: last segment. */
    uint8_t wrp; /**< Write-protected payload bytes (including the WRC bytes). */
    uint8_t wrc; /**< Write-protected bytes that carry the stamp (WRC). */
    bool rd; /**< Header flag: read-protected. */
    bool crc_ok; /**< Header CRC-8 (over MCD+MSN and the header) matches. */
} LegicPrimeSegment;

/**
 * The memory size follows the type (legic_prime_get_mem_size()).  A
 * zero-initialised instance is a blank MIM22, as legic_prime_reset() leaves.
 */
typedef struct {
    LegicPrimeType type;
    uint8_t data[LEGIC_PRIME_MEM_MAX];
} LegicPrimeData;

extern const NfcDeviceBase nfc_device_legic_prime;

LegicPrimeData* legic_prime_alloc(void);

void legic_prime_free(LegicPrimeData* data);

/** Clears the memory and sets the smallest type (MIM22). */
void legic_prime_reset(LegicPrimeData* data);

void legic_prime_copy(LegicPrimeData* data, const LegicPrimeData* other);

bool legic_prime_verify(LegicPrimeData* data, const FuriString* device_type);

bool legic_prime_load(LegicPrimeData* data, FlipperFormat* ff, uint32_t version);

bool legic_prime_save(const LegicPrimeData* data, FlipperFormat* ff);

bool legic_prime_is_equal(const LegicPrimeData* data, const LegicPrimeData* other);

const char* legic_prime_get_device_name(const LegicPrimeData* data, NfcDeviceNameType name_type);

const uint8_t* legic_prime_get_uid(const LegicPrimeData* data, size_t* uid_len);

/**
 * Sets the UID (MCD + MSN) and the MCC, and re-encodes the memory for them
 * as Proxmark3's `hf legic` clone does: bytes from 22 on are XORed with the
 * new MCC, and every IM-S segment header CRC is moved to the new UID (a CRC
 * that was wrong stays wrong).  CRCs inside segment payloads (e.g. KGH) are
 * left alone.
 */
bool legic_prime_set_uid(LegicPrimeData* data, const uint8_t* uid, size_t uid_len);

LegicPrimeData* legic_prime_get_base_data(const LegicPrimeData* data);

uint16_t legic_prime_get_mem_size(LegicPrimeType type);

/** Sets the type; the memory size follows it. */
void legic_prime_set_type(LegicPrimeData* data, LegicPrimeType type);

/** Returns true if the MCC byte equals CRC8 over MCD+MSN. */
bool legic_prime_verify_mcc(const LegicPrimeData* data);

/** Returns the DCF (bytes 5-6, little endian). */
uint16_t legic_prime_get_dcf(const LegicPrimeData* data);

/** Classifies the card by its DCF (and bytes 7-8 for data cards). */
LegicPrimeTokenType legic_prime_get_token_type(const LegicPrimeData* data);

/** Returns a short name for a token type ("NM", "IM", "IM-S", "XAM", ...). */
const char* legic_prime_get_token_type_name(LegicPrimeTokenType token_type);

/**
 * Gets the next segment of an IM-S card.  Start from a zeroed segment; each
 * call replaces it with the one that follows.
 *
 * @return false if the card is not IM-S, the previous segment was the last
 * one, or the next header is malformed or outside the memory.
 */
bool legic_prime_get_next_segment(const LegicPrimeData* data, LegicPrimeSegment* segment);

/**
 * Checks a write of `size` bytes at `offset` on a card of `mem_size` bytes:
 * not empty, clear of bytes 0-4 and within the memory.
 */
bool legic_prime_is_writable(uint16_t mem_size, uint16_t offset, size_t size);

/**
 * Checks whether a write of `size` bytes at `offset` changes the DCF
 * (bytes 5-6), which is irreversible on a real tag.
 */
bool legic_prime_is_dcf_write(uint16_t offset, size_t size);

#ifdef __cplusplus
}
#endif
