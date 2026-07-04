/**
 * @file iso7816_cmd_get_data.h
 * @brief ISO7816-4 GET DATA Command (INS = 0xCA/0xCB)
 * 
 * Implements the GET DATA command as defined in ISO/IEC 7816-4:2005 Section 7.4.2.
 * 
 * GET DATA is used to retrieve:
 * - Data objects from the current context
 * - Contents of EFs with TLV structure
 * - Card-originated byte strings
 * 
 * Supported modes:
 * - INS=0xCA: P1-P2 specify tag directly
 * - INS=0xCB: Command data contains tag list (not yet implemented)
 */

#pragma once

#include "../iso7816_t0_apdu.h"
#include "../iso7816_filesystem.h"

#ifdef __cplusplus
extern "C" {
#endif

/** GET DATA instruction codes */
#define ISO7816_INS_GET_DATA        0xCA
#define ISO7816_INS_GET_DATA_ODD    0xCB  /**< With tag list in command data */

/**
 * @brief P1-P2 encoding for GET DATA (Table 62)
 * 
 * For INS = 0xCA (even):
 * - '0000': Dumping a file, or card-originated byte strings
 * - '0040' to '00FF': BER-TLV tag (one byte) in P2
 * - '0100' to '01FF': Proprietary
 * - '0200' to '02FF': SIMPLE-TLV tag in P2
 * - '4000' to 'FFFF': BER-TLV tag (two bytes) in P1-P2
 */

/** Common single-byte BER-TLV tags (in P2, P1=0x00) */
typedef enum {
    /** Application label (tag 50) */
    ISO7816_TAG_APPLICATION_LABEL       = 0x50,
    /** Command to perform (tag 52) */
    ISO7816_TAG_COMMAND_TO_PERFORM      = 0x52,
    /** Application identifier (tag 4F) */
    ISO7816_TAG_AID                     = 0x4F,
    /** DF name (tag 84) */
    ISO7816_TAG_DF_NAME                 = 0x84,
} Iso7816GetDataTagSingleByte;

/** Common two-byte BER-TLV tags (P1-P2 encodes full tag) */
typedef enum {
    /** Answer-to-Reset information (tag 5F51) */
    ISO7816_TAG_ATR_INFO                = 0x5F51,
    /** Historical bytes (tag 5F52) */
    ISO7816_TAG_HISTORICAL_BYTES        = 0x5F52,
    /** Application template (tag 5F50) - URL */
    ISO7816_TAG_URL                     = 0x5F50,
    /** Card capabilities (tag 5F56) - Extended length */
    ISO7816_TAG_EXTENDED_LENGTH_INFO    = 0x5F56,
} Iso7816GetDataTagTwoBytes;

/** Special P1-P2 values */
#define ISO7816_GET_DATA_P1P2_DUMP          0x0000  /**< Dump file or card-originated */
#define ISO7816_GET_DATA_P1P2_ALL_BER_TLV   0x00FF  /**< All common BER-TLV objects */
#define ISO7816_GET_DATA_P1P2_ALL_SIMPLE    0x02FF  /**< All common SIMPLE-TLV objects */

/** BER-TLV tag ranges */
#define ISO7816_GET_DATA_BER_TLV_1BYTE_MIN  0x0040  /**< Single-byte BER-TLV min */
#define ISO7816_GET_DATA_BER_TLV_1BYTE_MAX  0x00FF  /**< Single-byte BER-TLV max */
#define ISO7816_GET_DATA_PROPRIETARY_MIN   0x0100  /**< Proprietary min */
#define ISO7816_GET_DATA_PROPRIETARY_MAX   0x01FF  /**< Proprietary max */
#define ISO7816_GET_DATA_SIMPLE_TLV_MIN    0x0200  /**< SIMPLE-TLV min */
#define ISO7816_GET_DATA_SIMPLE_TLV_MAX    0x02FF  /**< SIMPLE-TLV max */
#define ISO7816_GET_DATA_BER_TLV_2BYTE_MIN 0x4000  /**< Two-byte BER-TLV min */

/**
 * @brief Process GET DATA command (INS = 0xCA)
 * 
 * Retrieves data objects based on P1-P2 tag specification.
 * 
 * @param fs File system context
 * @param command Command APDU containing P1-P2 tag
 * @param response Response APDU to populate
 */
void iso7816_cmd_get_data(
    Iso7816FileSystem* fs,
    const ISO7816_Command_APDU* command,
    ISO7816_Response_APDU* response);

/**
 * @brief Register a data object for GET DATA retrieval
 * 
 * Allows applications to register custom data objects that can be
 * retrieved via GET DATA.
 * 
 * @param fs File system context
 * @param tag Two-byte tag (P1-P2 format)
 * @param data Pointer to data (copied)
 * @param data_len Length of data
 * @return true on success, false if table is full
 */
bool iso7816_register_data_object(
    Iso7816FileSystem* fs,
    uint16_t tag,
    const uint8_t* data,
    uint8_t data_len);

/**
 * @brief Clear all registered data objects
 * 
 * @param fs File system context
 */
void iso7816_clear_data_objects(Iso7816FileSystem* fs);

#ifdef __cplusplus
}
#endif
