/**
 * @file iso7816_cmd_verify.c
 * @brief ISO7816-4 VERIFY Command Implementation
 * 
 * Implements VERIFY (INS = 0x20) as defined in ISO/IEC 7816-4:2005 Section 7.5.6.
 */

#include "iso7816_cmd_verify.h"
#include "../iso7816_response.h"
#include <furi.h>
#include <string.h>

#define TAG "ISO7816_VERIFY"

/**
 * @brief Find PIN data by reference
 * 
 * @param fs File system context
 * @param reference P2 value
 * @return Pointer to PIN data or NULL if not found
 */
static Iso7816PinData* find_pin_by_reference(Iso7816FileSystem* fs, uint8_t reference) {
    if(!fs) return NULL;
    
    for(uint8_t i = 0; i < ISO7816_MAX_PIN_REFS; i++) {
        if(fs->pins[i].active && fs->pins[i].reference == reference) {
            return &fs->pins[i];
        }
    }
    
    return NULL;
}

/**
 * @brief Validate P1 parameter
 * 
 * Per ISO7816-4, P1 must be 0x00
 */
static bool validate_p1(uint8_t p1) {
    return p1 == 0x00;
}

/**
 * @brief Validate P2 parameter
 * 
 * Per ISO7816-4 Table 65:
 * - Bit 8: 0=global, 1=specific
 * - Bits 7-6: must be 00
 * - Bits 5-1: qualifier (0-31)
 */
static bool validate_p2(uint8_t p2) {
    // Bits 7-6 must be 00
    return (p2 & 0x60) == 0x00;
}

/**
 * @brief Handle check retries (VERIFY with no data)
 * 
 * When VERIFY is called with no data (Lc=0), it returns the remaining
 * retry count in SW2 as 63CX where X is the count.
 */
static void handle_check_retries(
    Iso7816FileSystem* fs,
    uint8_t reference,
    ISO7816_Response_APDU* response) {
    
    Iso7816PinData* pin = find_pin_by_reference(fs, reference);
    
    if(!pin) {
        // PIN reference not found
        iso7816_set_response(response, ISO7816_RESPONSE_REF_DATA_NOT_FOUND);
        return;
    }
    
    if(pin->status == ISO7816_PIN_STATUS_BLOCKED) {
        // PIN is blocked
        iso7816_set_response(response, ISO7816_RESPONSE_AUTH_BLOCKED);
        return;
    }
    
    if(pin->max_retries == 0) {
        // Unlimited retries - return success (PIN exists and is active)
        iso7816_set_response(response, ISO7816_RESPONSE_OK);
        return;
    }
    
    // Return remaining retries as 63 CX
    response->SW1 = 0x63;
    response->SW2 = 0xC0 | (pin->retries_remaining & 0x0F);
    response->DataLen = 0;
    
    FURI_LOG_D(TAG, "PIN %02X has %d retries remaining", reference, pin->retries_remaining);
}

/**
 * @brief Compare PIN data with stored reference
 * 
 * Handles padding (0xFF bytes at end)
 */
static bool compare_pin(
    const Iso7816PinData* pin,
    const uint8_t* data,
    uint16_t data_len) {
    
    if(!pin || !data) return false;
    
    // Strip trailing 0xFF padding from input
    while(data_len > 0 && data[data_len - 1] == 0xFF) {
        data_len--;
    }
    
    // Compare lengths
    if(data_len != pin->pin_len) {
        return false;
    }
    
    // Compare data using constant-time comparison to prevent timing attacks
    uint8_t result = 0;
    for(uint8_t i = 0; i < pin->pin_len; i++) {
        result |= pin->pin[i] ^ data[i];
    }
    
    return result == 0;
}

void iso7816_cmd_verify(
    Iso7816FileSystem* fs,
    const ISO7816_Command_APDU* command,
    ISO7816_Response_APDU* response) {
    
    furi_assert(fs);
    furi_assert(command);
    furi_assert(response);
    
    // Initialize response
    response->DataLen = 0;
    
    uint8_t p1 = command->P1;
    uint8_t p2 = command->P2;
    
    FURI_LOG_D(TAG, "VERIFY: P1=%02X P2=%02X Lc=%d", p1, p2, command->Lc);
    
    // Validate P1
    if(!validate_p1(p1)) {
        FURI_LOG_W(TAG, "Invalid P1=%02X (must be 00)", p1);
        iso7816_set_response(response, ISO7816_RESPONSE_WRONG_PARAMETERS_P1_P2);
        return;
    }
    
    // Validate P2
    if(!validate_p2(p2)) {
        FURI_LOG_W(TAG, "Invalid P2=%02X (bits 6-7 must be 00)", p2);
        iso7816_set_response(response, ISO7816_RESPONSE_WRONG_PARAMETERS_P1_P2);
        return;
    }
    
    // INS=0x21 (biometric) is not supported
    if(command->INS == ISO7816_INS_VERIFY_BIO) {
        FURI_LOG_W(TAG, "Biometric VERIFY (INS=21) not supported");
        iso7816_set_response(response, ISO7816_RESPONSE_FUNCTION_NOT_SUPPORTED);
        return;
    }
    
    // Check if this is a "check retries" request (no data)
    if(command->Lc == 0) {
        handle_check_retries(fs, p2, response);
        return;
    }
    
    // Find the PIN reference
    Iso7816PinData* pin = find_pin_by_reference(fs, p2);
    
    if(!pin) {
        FURI_LOG_W(TAG, "PIN reference %02X not found", p2);
        iso7816_set_response(response, ISO7816_RESPONSE_REF_DATA_NOT_FOUND);
        return;
    }
    
    // Check if PIN is blocked
    if(pin->status == ISO7816_PIN_STATUS_BLOCKED) {
        FURI_LOG_W(TAG, "PIN %02X is blocked", p2);
        iso7816_set_response(response, ISO7816_RESPONSE_AUTH_BLOCKED);
        return;
    }
    
    // Check if PIN is not initialized
    if(pin->status == ISO7816_PIN_STATUS_NOT_INIT) {
        FURI_LOG_W(TAG, "PIN %02X is not initialized", p2);
        iso7816_set_response(response, ISO7816_RESPONSE_REF_DATA_NOT_USABLE);
        return;
    }
    
    // Verify the PIN
    if(compare_pin(pin, command->Data, command->Lc)) {
        // Verification successful
        pin->verified = true;
        pin->retries_remaining = pin->max_retries;  // Reset retry counter
        FURI_LOG_I(TAG, "PIN %02X verified successfully", p2);
        iso7816_set_response(response, ISO7816_RESPONSE_OK);
        return;
    }
    
    // Verification failed
    pin->verified = false;
    
    if(pin->max_retries > 0) {
        // Decrement retry counter
        if(pin->retries_remaining > 0) {
            pin->retries_remaining--;
        }
        
        if(pin->retries_remaining == 0) {
            // PIN is now blocked
            pin->status = ISO7816_PIN_STATUS_BLOCKED;
            FURI_LOG_W(TAG, "PIN %02X is now BLOCKED", p2);
            iso7816_set_response(response, ISO7816_RESPONSE_AUTH_BLOCKED);
            return;
        }
        
        // Return remaining retries
        FURI_LOG_W(TAG, "PIN %02X verification failed, %d retries remaining", 
            p2, pin->retries_remaining);
        response->SW1 = 0x63;
        response->SW2 = 0xC0 | (pin->retries_remaining & 0x0F);
        response->DataLen = 0;
    } else {
        // Unlimited retries - just return generic failure
        FURI_LOG_W(TAG, "PIN %02X verification failed (unlimited retries)", p2);
        iso7816_set_response(response, ISO7816_RESPONSE_COUNTER_WARNING);
    }
}

bool iso7816_register_pin(
    Iso7816FileSystem* fs,
    uint8_t reference,
    const uint8_t* pin,
    uint8_t pin_len,
    uint8_t max_retries) {
    
    if(!fs || !pin || pin_len == 0 || pin_len > ISO7816_PIN_MAX_LENGTH) {
        return false;
    }
    
    // Validate reference (bits 6-7 must be 00)
    if((reference & 0x60) != 0x00) {
        FURI_LOG_E(TAG, "Invalid PIN reference %02X", reference);
        return false;
    }
    
    // Check if reference already exists - update it
    Iso7816PinData* existing = find_pin_by_reference(fs, reference);
    if(existing) {
        memcpy(existing->pin, pin, pin_len);
        existing->pin_len = pin_len;
        existing->max_retries = max_retries;
        existing->retries_remaining = max_retries;
        existing->status = ISO7816_PIN_STATUS_ACTIVE;
        existing->verified = false;
        FURI_LOG_D(TAG, "Updated PIN reference %02X", reference);
        return true;
    }
    
    // Find empty slot
    for(uint8_t i = 0; i < ISO7816_MAX_PIN_REFS; i++) {
        if(!fs->pins[i].active) {
            fs->pins[i].reference = reference;
            memcpy(fs->pins[i].pin, pin, pin_len);
            fs->pins[i].pin_len = pin_len;
            fs->pins[i].max_retries = max_retries;
            fs->pins[i].retries_remaining = max_retries;
            fs->pins[i].status = ISO7816_PIN_STATUS_ACTIVE;
            fs->pins[i].verified = false;
            fs->pins[i].active = true;
            fs->pin_count++;
            FURI_LOG_D(TAG, "Registered PIN reference %02X (%d retries)", 
                reference, max_retries);
            return true;
        }
    }
    
    FURI_LOG_E(TAG, "PIN table full, cannot register %02X", reference);
    return false;
}

bool iso7816_is_pin_verified(Iso7816FileSystem* fs, uint8_t reference) {
    Iso7816PinData* pin = find_pin_by_reference(fs, reference);
    return pin && pin->verified;
}

void iso7816_reset_pin_verification(Iso7816FileSystem* fs) {
    if(!fs) return;
    
    for(uint8_t i = 0; i < ISO7816_MAX_PIN_REFS; i++) {
        if(fs->pins[i].active) {
            fs->pins[i].verified = false;
        }
    }
    
    FURI_LOG_D(TAG, "Reset all PIN verification states");
}

bool iso7816_unblock_pin(Iso7816FileSystem* fs, uint8_t reference) {
    Iso7816PinData* pin = find_pin_by_reference(fs, reference);
    
    if(!pin) return false;
    
    pin->status = ISO7816_PIN_STATUS_ACTIVE;
    pin->retries_remaining = pin->max_retries;
    pin->verified = false;
    
    FURI_LOG_D(TAG, "Unblocked PIN reference %02X", reference);
    return true;
}

int8_t iso7816_get_pin_retries(Iso7816FileSystem* fs, uint8_t reference) {
    Iso7816PinData* pin = find_pin_by_reference(fs, reference);
    
    if(!pin) return -1;
    
    if(pin->max_retries == 0) return -1;  // Unlimited
    
    return pin->retries_remaining;
}
