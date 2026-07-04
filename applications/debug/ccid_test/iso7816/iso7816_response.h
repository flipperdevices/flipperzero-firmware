/**
 * @file iso7816_response.h
 * @brief ISO7816-4 Status Words (SW1-SW2)
 * 
 * Complete set of status words as defined in ISO/IEC 7816-4:2005.
 * Organized by category: Normal, Warning, Execution Error, Checking Error.
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Normal Processing (Process completed)
 ******************************************************************************/

/** No further qualification - Success */
#define ISO7816_RESPONSE_OK                         0x9000

/** SW2 encodes number of data bytes still available (use GET RESPONSE) */
#define ISO7816_RESPONSE_MORE_DATA_AVAILABLE        0x6100  // Add SW2 = bytes available

/*******************************************************************************
 * Warning Processing (Process completed with warning)
 ******************************************************************************/

/** Part of returned data may be corrupted */
#define ISO7816_RESPONSE_DATA_CORRUPTED             0x6281

/** End of file/record reached before reading Ne bytes */
#define ISO7816_RESPONSE_END_OF_FILE                0x6282

/** Selected file deactivated */
#define ISO7816_RESPONSE_FILE_DEACTIVATED           0x6283

/** File control information not formatted correctly */
#define ISO7816_RESPONSE_FCI_INVALID                0x6284

/** Selected file in termination state */
#define ISO7816_RESPONSE_FILE_TERMINATED            0x6285

/** No information given (warning) */
#define ISO7816_RESPONSE_WARNING_NO_INFO            0x6200

/** File filled up by the last write */
#define ISO7816_RESPONSE_FILE_FILLED                0x6381

/** Counter value warning (63CX where X = counter, e.g., PIN retries) */
#define ISO7816_RESPONSE_COUNTER_WARNING            0x63C0  // Add X = counter value

/*******************************************************************************
 * Execution Errors (Process aborted)
 ******************************************************************************/

/** Execution error - no information given */
#define ISO7816_RESPONSE_EXECUTION_ERROR            0x6400

/** Immediate response required by the card */
#define ISO7816_RESPONSE_IMMEDIATE_RESPONSE         0x6401

/** Memory failure */
#define ISO7816_RESPONSE_MEMORY_FAILURE             0x6581

/** Execution error - no information given (65XX) */
#define ISO7816_RESPONSE_EXEC_NO_INFO               0x6500

/*******************************************************************************
 * Checking Errors - Wrong Length (67XX)
 ******************************************************************************/

/** Wrong length; no further indication */
#define ISO7816_RESPONSE_WRONG_LENGTH               0x6700

/*******************************************************************************
 * Checking Errors - Functions in CLA not supported (68XX)
 ******************************************************************************/

/** No information given */
#define ISO7816_RESPONSE_CLA_FUNC_NO_INFO           0x6800

/** Logical channel not supported */
#define ISO7816_RESPONSE_LOGICAL_CHANNEL_NOT_SUPPORTED  0x6881

/** Secure messaging not supported */
#define ISO7816_RESPONSE_SM_NOT_SUPPORTED           0x6882

/** Last command of the chain expected */
#define ISO7816_RESPONSE_CHAIN_LAST_EXPECTED        0x6883

/** Command chaining not supported */
#define ISO7816_RESPONSE_CHAINING_NOT_SUPPORTED     0x6884

/*******************************************************************************
 * Checking Errors - Command not allowed (69XX)
 ******************************************************************************/

/** Command not allowed - no information given */
#define ISO7816_RESPONSE_NOT_ALLOWED_NO_INFO        0x6900

/** Command incompatible with file structure */
#define ISO7816_RESPONSE_INCOMPATIBLE_FILE          0x6981

/** Security status not satisfied */
#define ISO7816_RESPONSE_SECURITY_NOT_SATISFIED     0x6982

/** Authentication method blocked */
#define ISO7816_RESPONSE_AUTH_BLOCKED               0x6983

/** Reference data not usable */
#define ISO7816_RESPONSE_REF_DATA_NOT_USABLE        0x6984

/** Conditions of use not satisfied */
#define ISO7816_RESPONSE_CONDITIONS_NOT_SATISFIED   0x6985

/** Command not allowed (no current EF) */
#define ISO7816_RESPONSE_NO_CURRENT_EF              0x6986

/** Expected secure messaging data objects missing */
#define ISO7816_RESPONSE_SM_DATA_MISSING            0x6987

/** Incorrect secure messaging data objects */
#define ISO7816_RESPONSE_SM_DATA_INCORRECT          0x6988

/*******************************************************************************
 * Checking Errors - Wrong parameters (6AXX)
 ******************************************************************************/

/** Wrong parameters - no information given */
#define ISO7816_RESPONSE_WRONG_PARAMS_NO_INFO       0x6A00

/** Function not supported */
#define ISO7816_RESPONSE_FUNCTION_NOT_SUPPORTED     0x6A81

/** File or application not found */
#define ISO7816_RESPONSE_FILE_NOT_FOUND             0x6A82

/** Record not found */
#define ISO7816_RESPONSE_RECORD_NOT_FOUND           0x6A83

/** Not enough memory space in the file */
#define ISO7816_RESPONSE_NOT_ENOUGH_MEMORY          0x6A84

/** Nc inconsistent with TLV structure */
#define ISO7816_RESPONSE_NC_TLV_INCONSISTENT        0x6A85

/** Incorrect parameters P1-P2 */
#define ISO7816_RESPONSE_WRONG_PARAMETERS_P1_P2     0x6A86

/** Nc inconsistent with parameters P1-P2 */
#define ISO7816_RESPONSE_NC_P1P2_INCONSISTENT       0x6A87

/** Referenced data or reference data not found */
#define ISO7816_RESPONSE_REF_DATA_NOT_FOUND         0x6A88

/** File already exists */
#define ISO7816_RESPONSE_FILE_EXISTS                0x6A89

/** DF name already exists */
#define ISO7816_RESPONSE_DF_NAME_EXISTS             0x6A8A

/*******************************************************************************
 * Checking Errors - Wrong parameters P1-P2 (6BXX)
 ******************************************************************************/

/** Wrong parameters P1-P2 (offset outside EF) */
#define ISO7816_RESPONSE_OFFSET_OUTSIDE_EF          0x6B00

/*******************************************************************************
 * Checking Errors - Wrong Le field (6CXX)
 ******************************************************************************/

/** Wrong Le field; SW2 encodes exact number of available data bytes */
#define ISO7816_RESPONSE_WRONG_LE                   0x6C00  // Add SW2 = available bytes

/*******************************************************************************
 * Checking Errors - Instruction code (6DXX)
 ******************************************************************************/

/** Instruction code not supported or invalid */
#define ISO7816_RESPONSE_INSTRUCTION_NOT_SUPPORTED  0x6D00

/*******************************************************************************
 * Checking Errors - Class not supported (6EXX)
 ******************************************************************************/

/** Class not supported */
#define ISO7816_RESPONSE_CLASS_NOT_SUPPORTED        0x6E00

/*******************************************************************************
 * Checking Errors - No precise diagnosis (6FXX)
 ******************************************************************************/

/** No precise diagnosis */
#define ISO7816_RESPONSE_INTERNAL_EXCEPTION         0x6F00

/*******************************************************************************
 * Helper macros for status words with variable SW2
 ******************************************************************************/

/**
 * @brief Create "more data available" response (61 XX)
 * @param bytes Number of bytes still available
 */
#define ISO7816_SW_MORE_DATA(bytes)     (0x6100 | ((bytes) & 0xFF))

/**
 * @brief Create "wrong Le" response (6C XX)
 * @param exact_len Exact number of available bytes
 */
#define ISO7816_SW_WRONG_LE(exact_len)  (0x6C00 | ((exact_len) & 0xFF))

/**
 * @brief Create PIN retry counter warning (63 CX)
 * @param retries Number of retries remaining (0-15)
 */
#define ISO7816_SW_PIN_RETRIES(retries) (0x63C0 | ((retries) & 0x0F))

/*******************************************************************************
 * Functions
 ******************************************************************************/

/**
 * @brief Set response status word
 * 
 * @param responseAPDU Response APDU structure
 * @param responseCode 16-bit status word (SW1 << 8 | SW2)
 */
void iso7816_set_response(ISO7816_Response_APDU* responseAPDU, uint16_t responseCode);

/**
 * @brief Set response with data length info (for 61XX and 6CXX)
 * 
 * @param responseAPDU Response APDU structure  
 * @param sw1 Status byte 1 (0x61 or 0x6C)
 * @param len Length or count value for SW2
 */
void iso7816_set_response_len(ISO7816_Response_APDU* responseAPDU, uint8_t sw1, uint8_t len);

#ifdef __cplusplus
}
#endif
