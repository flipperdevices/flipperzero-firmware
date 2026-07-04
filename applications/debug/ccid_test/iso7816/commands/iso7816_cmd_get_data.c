/**
 * @file iso7816_cmd_get_data.c
 * @brief ISO7816-4 GET DATA Command Implementation
 * 
 * Implements GET DATA (INS = 0xCA) as defined in ISO/IEC 7816-4:2005 Section 7.4.2.
 */

#include "iso7816_cmd_get_data.h"
#include "../iso7816_response.h"
#include <furi.h>
#include <string.h>

#define TAG "ISO7816_GET_DATA"

/**
 * @brief Built-in ATR information data object (tag 5F51)
 * 
 * Contains basic card information for GET DATA 5F51.
 */
static const uint8_t atr_info_data[] = {
    0x5F, 0x51,        // Tag: ATR information
    0x0E,              // Length: 14 bytes
    0x46, 0x6C, 0x69, 0x70, 0x70, 0x65, 0x72,  // "Flipper"
    0x20, 0x5A, 0x65, 0x72, 0x6F,              // " Zero"
    0x00, 0x00                                 // Version placeholder
};

/**
 * @brief Built-in historical bytes data object (tag 5F52)
 * 
 * Historical bytes from ATR, formatted as data object.
 */
static const uint8_t historical_bytes_data[] = {
    0x5F, 0x52,        // Tag: Historical bytes
    0x08,              // Length: 8 bytes
    0x80,              // Category indicator: status info present
    0x73,              // Card service data
    0x00, 0x00, 0x40,  // Card capabilities
    0x00,              // Reserved
    0x90, 0x00         // Status: OK
};

/**
 * @brief Built-in extended length info (tag 5F56)
 * 
 * Indicates support for extended length APDUs.
 */
static const uint8_t extended_length_data[] = {
    0x5F, 0x56,        // Tag: Extended length information
    0x06,              // Length: 6 bytes
    0x08, 0x00,        // Max command data length: 2048
    0x08, 0x00,        // Max response data length: 2048
    0x00, 0x00         // Reserved
};

/**
 * @brief Get built-in data object by tag
 * 
 * @param tag Tag to look up (P1-P2 format)
 * @param data Output pointer to data
 * @param data_len Output data length
 * @return true if found, false otherwise
 */
static bool get_builtin_data_object(
    uint16_t tag,
    const uint8_t** data,
    uint8_t* data_len) {
    
    switch(tag) {
    case ISO7816_TAG_ATR_INFO:  // 0x5F51
        *data = atr_info_data;
        *data_len = sizeof(atr_info_data);
        return true;
        
    case ISO7816_TAG_HISTORICAL_BYTES:  // 0x5F52
        *data = historical_bytes_data;
        *data_len = sizeof(historical_bytes_data);
        return true;
        
    case ISO7816_TAG_EXTENDED_LENGTH_INFO:  // 0x5F56
        *data = extended_length_data;
        *data_len = sizeof(extended_length_data);
        return true;
        
    default:
        return false;
    }
}

/**
 * @brief Get registered data object by tag
 * 
 * @param fs File system context
 * @param tag Tag to look up
 * @param data Output pointer to data
 * @param data_len Output data length
 * @return true if found, false otherwise
 */
static bool get_registered_data_object(
    Iso7816FileSystem* fs,
    uint16_t tag,
    const uint8_t** data,
    uint8_t* data_len) {
    
    if(!fs) return false;
    
    for(uint8_t i = 0; i < ISO7816_MAX_DATA_OBJECTS; i++) {
        if(fs->data_objects[i].active && fs->data_objects[i].tag == tag) {
            *data = fs->data_objects[i].data;
            *data_len = fs->data_objects[i].data_len;
            return true;
        }
    }
    
    return false;
}

/**
 * @brief Get data from currently selected EF (for P1-P2 = 0000)
 * 
 * When P1-P2 is 0000, GET DATA dumps the content of the current file
 * or returns card-originated byte strings.
 * 
 * @param fs File system context
 * @param response Response APDU
 * @param max_len Maximum bytes to return (from Le)
 * @return true if data was retrieved
 */
static bool get_current_ef_data(
    Iso7816FileSystem* fs,
    ISO7816_Response_APDU* response,
    uint16_t max_len) {
    
    if(!fs || !fs->current_ef) {
        return false;
    }
    
    Iso7816File* ef = fs->current_ef;
    
    // Only transparent EFs can be dumped this way
    if(ef->type != ISO7816_FILE_TYPE_EF_TRANSPARENT) {
        return false;
    }
    
    if(!ef->data || ef->data_len == 0) {
        return false;
    }
    
    // Calculate how much to return
    uint16_t len = ef->data_len;
    if(max_len > 0 && len > max_len) {
        len = max_len;
    }
    if(len > 256) {
        len = 256;  // Max for short APDU response
    }
    
    memcpy(response->Data, ef->data, len);
    response->DataLen = len;
    
    return true;
}

/**
 * @brief Handle single-byte BER-TLV tag (P1=0x00, P2=0x40-0xFF)
 */
static bool handle_single_byte_ber_tlv(
    Iso7816FileSystem* fs,
    uint8_t tag,
    ISO7816_Response_APDU* response) {
    
    // For single-byte tags, we look for data objects in the current DF
    // Currently, we support a few basic tags
    
    if(!fs || !fs->current_df) return false;
    
    Iso7816File* df = fs->current_df;
    
    switch(tag) {
    case ISO7816_TAG_AID:  // 0x4F - Application Identifier
        if(df->aid_len > 0) {
            response->Data[0] = 0x4F;
            response->Data[1] = df->aid_len;
            memcpy(&response->Data[2], df->aid, df->aid_len);
            response->DataLen = 2 + df->aid_len;
            return true;
        }
        break;
        
    case ISO7816_TAG_DF_NAME:  // 0x84 - DF Name
        if(df->aid_len > 0) {
            response->Data[0] = 0x84;
            response->Data[1] = df->aid_len;
            memcpy(&response->Data[2], df->aid, df->aid_len);
            response->DataLen = 2 + df->aid_len;
            return true;
        }
        break;
        
    case ISO7816_TAG_APPLICATION_LABEL:  // 0x50 - Application Label
        if(df->app && df->app->name) {
            uint8_t name_len = strlen(df->app->name);
            if(name_len > 0 && name_len < 16) {
                response->Data[0] = 0x50;
                response->Data[1] = name_len;
                memcpy(&response->Data[2], df->app->name, name_len);
                response->DataLen = 2 + name_len;
                return true;
            }
        }
        break;
    }
    
    // Check registered data objects with single-byte encoding
    const uint8_t* data;
    uint8_t data_len;
    uint16_t full_tag = 0x0000 | tag;  // P1=0x00, P2=tag
    if(get_registered_data_object(fs, full_tag, &data, &data_len)) {
        memcpy(response->Data, data, data_len);
        response->DataLen = data_len;
        return true;
    }
    
    return false;
}

void iso7816_cmd_get_data(
    Iso7816FileSystem* fs,
    const ISO7816_Command_APDU* command,
    ISO7816_Response_APDU* response) {
    
    furi_assert(fs);
    furi_assert(command);
    furi_assert(response);
    
    // Initialize response
    response->DataLen = 0;
    
    uint16_t tag = ((uint16_t)command->P1 << 8) | command->P2;
    uint16_t le = command->Le;
    
    FURI_LOG_D(TAG, "GET DATA: P1=%02X P2=%02X (tag=%04X) Le=%d",
        command->P1, command->P2, tag, le);
    
    // Only INS=0xCA (even) is supported for now
    // INS=0xCB (odd) with tag list in data is not implemented
    if(command->INS == ISO7816_INS_GET_DATA_ODD) {
        FURI_LOG_W(TAG, "GET DATA with tag list (INS=CB) not implemented");
        iso7816_set_response(response, ISO7816_RESPONSE_FUNCTION_NOT_SUPPORTED);
        return;
    }
    
    bool found = false;
    const uint8_t* data = NULL;
    uint8_t data_len = 0;
    
    // Handle based on P1-P2 range
    if(tag == ISO7816_GET_DATA_P1P2_DUMP) {
        // P1-P2 = 0000: Dump current EF or card-originated strings
        if(get_current_ef_data(fs, response, le)) {
            iso7816_set_response(response, ISO7816_RESPONSE_OK);
            return;
        }
        // No current EF selected
        iso7816_set_response(response, ISO7816_RESPONSE_NO_CURRENT_EF);
        return;
        
    } else if(tag >= ISO7816_GET_DATA_BER_TLV_1BYTE_MIN && 
              tag <= ISO7816_GET_DATA_BER_TLV_1BYTE_MAX) {
        // Single-byte BER-TLV tag in P2 (P1 = 0x00)
        if(handle_single_byte_ber_tlv(fs, command->P2, response)) {
            iso7816_set_response(response, ISO7816_RESPONSE_OK);
            return;
        }
        
    } else if(tag >= ISO7816_GET_DATA_PROPRIETARY_MIN && 
              tag <= ISO7816_GET_DATA_PROPRIETARY_MAX) {
        // Proprietary range: check registered data objects
        if(get_registered_data_object(fs, tag, &data, &data_len)) {
            found = true;
        }
        
    } else if(tag >= ISO7816_GET_DATA_SIMPLE_TLV_MIN && 
              tag <= ISO7816_GET_DATA_SIMPLE_TLV_MAX) {
        // SIMPLE-TLV tag in P2
        // Check registered data objects
        if(get_registered_data_object(fs, tag, &data, &data_len)) {
            found = true;
        }
        
    } else if(tag >= ISO7816_GET_DATA_BER_TLV_2BYTE_MIN) {
        // Two-byte BER-TLV tag
        // First check built-in data objects
        if(get_builtin_data_object(tag, &data, &data_len)) {
            found = true;
        } else if(get_registered_data_object(fs, tag, &data, &data_len)) {
            found = true;
        }
    }
    
    if(found && data && data_len > 0) {
        // Check if requested length is sufficient
        if(le > 0 && data_len > le) {
            // Wrong Le - indicate correct length
            iso7816_set_response_len(response, 0x6C, data_len);
            return;
        }
        
        // Copy data to response (data_len is uint8_t, max 255)
        memcpy(response->Data, data, data_len);
        response->DataLen = data_len;
        iso7816_set_response(response, ISO7816_RESPONSE_OK);
        return;
    }
    
    // Data object not found
    FURI_LOG_D(TAG, "Data object %04X not found", tag);
    iso7816_set_response(response, ISO7816_RESPONSE_REF_DATA_NOT_FOUND);
}

bool iso7816_register_data_object(
    Iso7816FileSystem* fs,
    uint16_t tag,
    const uint8_t* data,
    uint8_t data_len) {
    
    if(!fs || !data || data_len == 0) return false;
    if(data_len > ISO7816_DATA_OBJECT_MAX_SIZE) return false;
    
    // Check if tag already exists - update it
    for(uint8_t i = 0; i < ISO7816_MAX_DATA_OBJECTS; i++) {
        if(fs->data_objects[i].active && fs->data_objects[i].tag == tag) {
            memcpy(fs->data_objects[i].data, data, data_len);
            fs->data_objects[i].data_len = data_len;
            FURI_LOG_D(TAG, "Updated data object %04X (%d bytes)", tag, data_len);
            return true;
        }
    }
    
    // Find empty slot
    for(uint8_t i = 0; i < ISO7816_MAX_DATA_OBJECTS; i++) {
        if(!fs->data_objects[i].active) {
            fs->data_objects[i].tag = tag;
            memcpy(fs->data_objects[i].data, data, data_len);
            fs->data_objects[i].data_len = data_len;
            fs->data_objects[i].active = true;
            fs->data_object_count++;
            FURI_LOG_D(TAG, "Registered data object %04X (%d bytes)", tag, data_len);
            return true;
        }
    }
    
    FURI_LOG_E(TAG, "Data object table full, cannot register %04X", tag);
    return false;
}

void iso7816_clear_data_objects(Iso7816FileSystem* fs) {
    if(!fs) return;
    
    for(uint8_t i = 0; i < ISO7816_MAX_DATA_OBJECTS; i++) {
        fs->data_objects[i].active = false;
    }
    fs->data_object_count = 0;
    FURI_LOG_D(TAG, "Cleared all data objects");
}
