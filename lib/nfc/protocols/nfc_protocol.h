/**
 * @file nfc_protocol.h
 * @brief Top-level NFC protocol definitions.
 *
 * This file is to be modified upon adding a new protocol (see below).
 *
 */
#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Numeric identifiers for all available NFC protocols.
 *
 * When implementing a new protocol, add its identifier before the
 * NfcProtocolNum entry.
 */
typedef enum {
    NfcProtocolIso14443_3a,
    NfcProtocolIso14443_3b,
    NfcProtocolIso14443_4a,
    NfcProtocolIso14443_4b,
    NfcProtocolIso15693_3,
    NfcProtocolFelica,
    NfcProtocolMfUltralight,
    NfcProtocolMfClassic,
    NfcProtocolMfDesfire,
    NfcProtocolSlix,
    /* Add new protocols here */

    NfcProtocolNum, /**< Special value representing the number of available protocols. */

    NfcProtocolInvalid, /**< Special value representing an invalid state. */
} NfcProtocol;

/**
 * @brief Get the immediate parent of a specific protocol.
 *
 * @param[in] protocol identifier of the protocol in question.
 * @returns parent protocol identifier if it has one, or NfcProtocolInvalid otherwise.
 */
NfcProtocol nfc_protocol_get_parent(NfcProtocol protocol);

/**
 * @brief Determine if a specific protocol has a parent on an arbitrary level.
 *
 * Unlike nfc_protocol_get_parent(), this function will traverse the full protocol hierarchy
 * and check each parent node for the matching protocol type.
 *
 * @param[in] protocol identifier of the protocol in question.
 * @param[in] parent_protocol identifier of the parent protocol in question.
 * @returns true if the parent of given type exists, false otherwise.
 */
bool nfc_protocol_has_parent(NfcProtocol protocol, NfcProtocol parent_protocol);

#ifdef __cplusplus
}
#endif
