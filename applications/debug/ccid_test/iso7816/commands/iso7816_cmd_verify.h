/**
 * @file iso7816_cmd_verify.h
 * @brief ISO7816-4 VERIFY Command (INS = 0x20)
 * 
 * Implements the VERIFY command as defined in ISO/IEC 7816-4:2005 Section 7.5.6.
 * 
 * The VERIFY command initiates comparison of verification data (PIN, password)
 * sent from the interface device with reference data stored in the card.
 * 
 * Features:
 * - PIN verification with retry counter
 * - Check remaining retries (empty data)
 * - Global vs specific reference data
 * - Blocked PIN detection
 */

#pragma once

#include "../iso7816_t0_apdu.h"
#include "../iso7816_filesystem.h"

#ifdef __cplusplus
extern "C" {
#endif

/** VERIFY instruction code */
#define ISO7816_INS_VERIFY          0x20
#define ISO7816_INS_VERIFY_BIO      0x21  /**< Biometric verification (not implemented) */

/** Default PIN retry limit */
#define ISO7816_PIN_DEFAULT_RETRIES 3

/**
 * @brief P2 - Reference Data Qualifier (Table 65)
 * 
 * Bit 8: 0 = Global reference data (MF specific)
 *        1 = Specific reference data (DF specific)
 * Bits 7-6: Reserved (must be 00)
 * Bits 5-1: Qualifier (PIN reference number 0-31)
 */

/** Global reference data (MF specific, e.g., card PIN) */
#define ISO7816_VERIFY_P2_GLOBAL        0x00

/** Specific reference data (DF specific, e.g., app PIN) */
#define ISO7816_VERIFY_P2_SPECIFIC      0x80

/** Mask for reference qualifier (bits 5-1) */
#define ISO7816_VERIFY_P2_QUALIFIER_MASK    0x1F

/** Common PIN reference numbers */
typedef enum {
    /** No specific reference (P2=0x00) */
    ISO7816_PIN_REF_GLOBAL_0        = 0x00,
    /** Global PIN reference 1 */
    ISO7816_PIN_REF_GLOBAL_1        = 0x01,
    /** Global PIN reference 2 */
    ISO7816_PIN_REF_GLOBAL_2        = 0x02,
    /** Application-specific PIN */
    ISO7816_PIN_REF_APP_1           = 0x81,
    /** Application-specific PIN 2 */
    ISO7816_PIN_REF_APP_2           = 0x82,
} Iso7816PinReference;

/**
 * @brief Process VERIFY command
 * 
 * @param fs File system context (contains PIN storage)
 * @param command Command APDU
 * @param response Response APDU to populate
 */
void iso7816_cmd_verify(
    Iso7816FileSystem* fs,
    const ISO7816_Command_APDU* command,
    ISO7816_Response_APDU* response);

/**
 * @brief Register a PIN for verification
 * 
 * @param fs File system context
 * @param reference P2 value for this PIN (0x00-0x1F global, 0x80-0x9F specific)
 * @param pin PIN data
 * @param pin_len Length of PIN (1-16 bytes)
 * @param max_retries Maximum verification attempts (0 = unlimited)
 * @return true on success
 */
bool iso7816_register_pin(
    Iso7816FileSystem* fs,
    uint8_t reference,
    const uint8_t* pin,
    uint8_t pin_len,
    uint8_t max_retries);

/**
 * @brief Check if a PIN is verified
 * 
 * @param fs File system context
 * @param reference P2 value for the PIN
 * @return true if PIN has been successfully verified
 */
bool iso7816_is_pin_verified(
    Iso7816FileSystem* fs,
    uint8_t reference);

/**
 * @brief Reset PIN verification status (e.g., on card reset)
 * 
 * @param fs File system context
 */
void iso7816_reset_pin_verification(Iso7816FileSystem* fs);

/**
 * @brief Unblock a PIN and set new retry counter
 * 
 * @param fs File system context
 * @param reference P2 value for the PIN
 * @return true on success
 */
bool iso7816_unblock_pin(
    Iso7816FileSystem* fs,
    uint8_t reference);

/**
 * @brief Get remaining retries for a PIN
 * 
 * @param fs File system context
 * @param reference P2 value for the PIN
 * @return Remaining retries, or -1 if PIN not found
 */
int8_t iso7816_get_pin_retries(
    Iso7816FileSystem* fs,
    uint8_t reference);

#ifdef __cplusplus
}
#endif
