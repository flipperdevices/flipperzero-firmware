/**
 * @file iso7816_cmd_select.c
 * @brief ISO7816-4 SELECT Command Implementation
 * 
 * Implements SELECT command (INS = 0xA4) as per ISO/IEC 7816-4:2005.
 */

#include "iso7816_cmd_select.h"
#include "../iso7816_response.h"
#include <furi.h>
#include <string.h>

#define TAG "ISO7816_SELECT"

/** Maximum FCI buffer size */
#define FCI_BUFFER_SIZE 128

/**
 * @brief Select by file identifier (P1 = 0x00)
 * 
 * If no data: Select MF
 * If 2 bytes: Select file by FID
 */
static void select_by_fid(
    Iso7816FileSystem* fs,
    const ISO7816_Command_APDU* command,
    ISO7816_Response_APDU* response,
    uint8_t p2) {
    
    Iso7816File* file = NULL;
    
    if(command->Lc == 0) {
        // No data = select MF
        file = fs->mf;
        FURI_LOG_D(TAG, "SELECT MF (no data)");
    } else if(command->Lc == 2) {
        // 2-byte file identifier
        uint16_t fid = ((uint16_t)command->Data[0] << 8) | command->Data[1];
        FURI_LOG_D(TAG, "SELECT by FID: %04X", fid);
        
        if(fid == ISO7816_FID_MF) {
            file = fs->mf;
        } else {
            // Search in current DF
            file = iso7816_fs_find_by_fid(fs, fid);
        }
    } else {
        // Wrong length
        iso7816_set_response(response, ISO7816_RESPONSE_WRONG_LENGTH);
        return;
    }
    
    if(!file) {
        iso7816_set_response(response, ISO7816_RESPONSE_FILE_NOT_FOUND);
        return;
    }
    
    // Check if file is deactivated
    if(file->lcs == ISO7816_LCS_DEACTIVATED) {
        // Select succeeds but with warning
        iso7816_fs_select(fs, file);
        iso7816_set_response(response, ISO7816_RESPONSE_FILE_DEACTIVATED);
        return;
    }
    
    if(file->lcs == ISO7816_LCS_TERMINATED) {
        iso7816_fs_select(fs, file);
        iso7816_set_response(response, ISO7816_RESPONSE_FILE_TERMINATED);
        return;
    }
    
    if(!iso7816_fs_select(fs, file)) {
        iso7816_set_response(response, ISO7816_RESPONSE_CONDITIONS_NOT_SATISFIED);
        return;
    }
    
    // Build response based on P2
    uint8_t fci_type = p2 & ISO7816_SELECT_P2_FCI_MASK;
    
    if(fci_type == ISO7816_SELECT_P2_NONE && !command->LePresent) {
        // No response data requested
        response->DataLen = 0;
        iso7816_set_response(response, ISO7816_RESPONSE_OK);
        return;
    }
    
    // Build FCI or FCP
    if(fci_type == ISO7816_SELECT_P2_FCP) {
        response->DataLen = iso7816_fs_build_fcp(file, response->Data, FCI_BUFFER_SIZE);
    } else {
        // Default to FCI
        response->DataLen = iso7816_fs_build_fci(file, response->Data, FCI_BUFFER_SIZE);
    }
    
    // Check Le if present
    if(command->LePresent && command->Le < response->DataLen && command->Le != 0) {
        // Le too small - return available length
        response->DataLen = 0;
        iso7816_set_response_len(response, 0x6C, response->DataLen);
        return;
    }
    
    iso7816_set_response(response, ISO7816_RESPONSE_OK);
}

/**
 * @brief Select child DF (P1 = 0x01)
 */
static void select_child_df(
    Iso7816FileSystem* fs,
    const ISO7816_Command_APDU* command,
    ISO7816_Response_APDU* response,
    uint8_t p2) {
    
    if(command->Lc != 2) {
        iso7816_set_response(response, ISO7816_RESPONSE_WRONG_LENGTH);
        return;
    }
    
    uint16_t fid = ((uint16_t)command->Data[0] << 8) | command->Data[1];
    FURI_LOG_D(TAG, "SELECT child DF: %04X", fid);
    
    // Search only direct children of current DF
    Iso7816File* file = iso7816_fs_find_by_fid_in_df(fs->current_df, fid);
    
    if(!file || !iso7816_file_is_df(file)) {
        iso7816_set_response(response, ISO7816_RESPONSE_FILE_NOT_FOUND);
        return;
    }
    
    if(!iso7816_fs_select(fs, file)) {
        iso7816_set_response(response, ISO7816_RESPONSE_CONDITIONS_NOT_SATISFIED);
        return;
    }
    
    // Build response
    uint8_t fci_type = p2 & ISO7816_SELECT_P2_FCI_MASK;
    if(fci_type == ISO7816_SELECT_P2_NONE && !command->LePresent) {
        response->DataLen = 0;
    } else if(fci_type == ISO7816_SELECT_P2_FCP) {
        response->DataLen = iso7816_fs_build_fcp(file, response->Data, FCI_BUFFER_SIZE);
    } else {
        response->DataLen = iso7816_fs_build_fci(file, response->Data, FCI_BUFFER_SIZE);
    }
    
    iso7816_set_response(response, ISO7816_RESPONSE_OK);
}

/**
 * @brief Select EF under current DF (P1 = 0x02)
 */
static void select_ef_under_df(
    Iso7816FileSystem* fs,
    const ISO7816_Command_APDU* command,
    ISO7816_Response_APDU* response,
    uint8_t p2) {
    
    if(command->Lc != 2) {
        iso7816_set_response(response, ISO7816_RESPONSE_WRONG_LENGTH);
        return;
    }
    
    uint16_t fid = ((uint16_t)command->Data[0] << 8) | command->Data[1];
    FURI_LOG_D(TAG, "SELECT EF under DF: %04X", fid);
    
    Iso7816File* file = iso7816_fs_find_by_fid_in_df(fs->current_df, fid);
    
    if(!file || !iso7816_file_is_ef(file)) {
        iso7816_set_response(response, ISO7816_RESPONSE_FILE_NOT_FOUND);
        return;
    }
    
    if(!iso7816_fs_select(fs, file)) {
        iso7816_set_response(response, ISO7816_RESPONSE_CONDITIONS_NOT_SATISFIED);
        return;
    }
    
    // Build response
    uint8_t fci_type = p2 & ISO7816_SELECT_P2_FCI_MASK;
    if(fci_type == ISO7816_SELECT_P2_NONE && !command->LePresent) {
        response->DataLen = 0;
    } else if(fci_type == ISO7816_SELECT_P2_FCP) {
        response->DataLen = iso7816_fs_build_fcp(file, response->Data, FCI_BUFFER_SIZE);
    } else {
        response->DataLen = iso7816_fs_build_fci(file, response->Data, FCI_BUFFER_SIZE);
    }
    
    iso7816_set_response(response, ISO7816_RESPONSE_OK);
}

/**
 * @brief Select parent DF (P1 = 0x03)
 */
static void select_parent_df(
    Iso7816FileSystem* fs,
    const ISO7816_Command_APDU* command,
    ISO7816_Response_APDU* response,
    uint8_t p2) {
    
    // Should have no data
    if(command->Lc != 0) {
        iso7816_set_response(response, ISO7816_RESPONSE_WRONG_LENGTH);
        return;
    }
    
    FURI_LOG_D(TAG, "SELECT parent DF");
    
    if(!iso7816_fs_select_parent(fs)) {
        // Already at MF - select MF anyway (success)
        iso7816_fs_select_mf(fs);
    }
    
    Iso7816File* file = fs->current_df;
    
    // Build response
    uint8_t fci_type = p2 & ISO7816_SELECT_P2_FCI_MASK;
    if(fci_type == ISO7816_SELECT_P2_NONE && !command->LePresent) {
        response->DataLen = 0;
    } else if(fci_type == ISO7816_SELECT_P2_FCP) {
        response->DataLen = iso7816_fs_build_fcp(file, response->Data, FCI_BUFFER_SIZE);
    } else {
        response->DataLen = iso7816_fs_build_fci(file, response->Data, FCI_BUFFER_SIZE);
    }
    
    iso7816_set_response(response, ISO7816_RESPONSE_OK);
}

/**
 * @brief Select by DF name / AID (P1 = 0x04)
 * 
 * This is the most common SELECT mode for application selection.
 */
static void select_by_df_name(
    Iso7816FileSystem* fs,
    const ISO7816_Command_APDU* command,
    ISO7816_Response_APDU* response,
    uint8_t p2) {
    
    // AID must be 5-16 bytes
    if(command->Lc < 5 || command->Lc > 16) {
        FURI_LOG_W(TAG, "Invalid AID length: %d", command->Lc);
        iso7816_set_response(response, ISO7816_RESPONSE_WRONG_LENGTH);
        return;
    }
    
    FURI_LOG_D(TAG, "SELECT by AID, length %d", command->Lc);
    
    // Check occurrence
    uint8_t occurrence = p2 & ISO7816_SELECT_P2_OCCURRENCE_MASK;
    
    // For now, only support first occurrence
    // TODO: Implement next/previous/last occurrence
    if(occurrence != ISO7816_SELECT_P2_FIRST) {
        FURI_LOG_W(TAG, "Occurrence %d not yet supported", occurrence);
    }
    
    // Search for matching AID
    // Support partial matching (AID may be truncated)
    Iso7816File* file = iso7816_fs_find_by_aid(fs, command->Data, command->Lc, true);
    
    if(!file) {
        FURI_LOG_D(TAG, "Application not found");
        iso7816_set_response(response, ISO7816_RESPONSE_FILE_NOT_FOUND);
        return;
    }
    
    FURI_LOG_I(TAG, "Found application DF %04X", file->fid);
    
    // Check life cycle
    if(file->lcs == ISO7816_LCS_DEACTIVATED) {
        iso7816_fs_select(fs, file);
        iso7816_set_response(response, ISO7816_RESPONSE_FILE_DEACTIVATED);
        return;
    }
    
    if(file->lcs == ISO7816_LCS_TERMINATED) {
        iso7816_fs_select(fs, file);
        iso7816_set_response(response, ISO7816_RESPONSE_FILE_TERMINATED);
        return;
    }
    
    if(!iso7816_fs_select(fs, file)) {
        iso7816_set_response(response, ISO7816_RESPONSE_CONDITIONS_NOT_SATISFIED);
        return;
    }
    
    // Build response
    uint8_t fci_type = p2 & ISO7816_SELECT_P2_FCI_MASK;
    if(fci_type == ISO7816_SELECT_P2_NONE && !command->LePresent) {
        response->DataLen = 0;
    } else if(fci_type == ISO7816_SELECT_P2_FCP) {
        response->DataLen = iso7816_fs_build_fcp(file, response->Data, FCI_BUFFER_SIZE);
    } else {
        response->DataLen = iso7816_fs_build_fci(file, response->Data, FCI_BUFFER_SIZE);
    }
    
    iso7816_set_response(response, ISO7816_RESPONSE_OK);
}

/**
 * @brief Select by path from MF (P1 = 0x08) or current DF (P1 = 0x09)
 */
static void select_by_path(
    Iso7816FileSystem* fs,
    const ISO7816_Command_APDU* command,
    ISO7816_Response_APDU* response,
    uint8_t p1,
    uint8_t p2) {
    
    // Path must be multiple of 2 bytes (sequence of FIDs)
    if(command->Lc == 0 || (command->Lc % 2) != 0) {
        iso7816_set_response(response, ISO7816_RESPONSE_WRONG_LENGTH);
        return;
    }
    
    FURI_LOG_D(TAG, "SELECT by path, length %d, from %s", 
               command->Lc, (p1 == 0x08) ? "MF" : "current DF");
    
    // Start from MF or current DF
    Iso7816File* current;
    if(p1 == ISO7816_SELECT_P1_PATH_FROM_MF) {
        current = fs->mf;
    } else {
        current = fs->current_df;
    }
    
    // Walk the path
    uint16_t path_len = command->Lc / 2;
    for(uint16_t i = 0; i < path_len; i++) {
        uint16_t fid = ((uint16_t)command->Data[i*2] << 8) | command->Data[i*2 + 1];
        
        Iso7816File* next = iso7816_fs_find_by_fid_in_df(current, fid);
        if(!next) {
            FURI_LOG_D(TAG, "Path element %04X not found", fid);
            iso7816_set_response(response, ISO7816_RESPONSE_FILE_NOT_FOUND);
            return;
        }
        
        // For intermediate elements, must be a DF
        if(i < path_len - 1 && !iso7816_file_is_df(next)) {
            FURI_LOG_D(TAG, "Intermediate path element %04X is not a DF", fid);
            iso7816_set_response(response, ISO7816_RESPONSE_INCOMPATIBLE_FILE);
            return;
        }
        
        current = next;
    }
    
    // Select the final file
    if(!iso7816_fs_select(fs, current)) {
        iso7816_set_response(response, ISO7816_RESPONSE_CONDITIONS_NOT_SATISFIED);
        return;
    }
    
    // Build response
    uint8_t fci_type = p2 & ISO7816_SELECT_P2_FCI_MASK;
    if(fci_type == ISO7816_SELECT_P2_NONE && !command->LePresent) {
        response->DataLen = 0;
    } else if(fci_type == ISO7816_SELECT_P2_FCP) {
        response->DataLen = iso7816_fs_build_fcp(current, response->Data, FCI_BUFFER_SIZE);
    } else {
        response->DataLen = iso7816_fs_build_fci(current, response->Data, FCI_BUFFER_SIZE);
    }
    
    iso7816_set_response(response, ISO7816_RESPONSE_OK);
}

void iso7816_cmd_select_process(
    Iso7816FileSystem* fs,
    const ISO7816_Command_APDU* command,
    ISO7816_Response_APDU* response) {
    
    furi_assert(fs);
    furi_assert(command);
    furi_assert(response);
    furi_assert(command->INS == ISO7816_INS_SELECT);
    
    // Initialize response
    response->DataLen = 0;
    
    uint8_t p1 = command->P1;
    uint8_t p2 = command->P2;
    
    FURI_LOG_D(TAG, "SELECT P1=%02X P2=%02X Lc=%d Le=%d", 
               p1, p2, command->Lc, command->Le);
    
    switch(p1) {
        case ISO7816_SELECT_P1_BY_FID:
            select_by_fid(fs, command, response, p2);
            break;
            
        case ISO7816_SELECT_P1_CHILD_DF:
            select_child_df(fs, command, response, p2);
            break;
            
        case ISO7816_SELECT_P1_EF_UNDER_DF:
            select_ef_under_df(fs, command, response, p2);
            break;
            
        case ISO7816_SELECT_P1_PARENT_DF:
            select_parent_df(fs, command, response, p2);
            break;
            
        case ISO7816_SELECT_P1_BY_DF_NAME:
            select_by_df_name(fs, command, response, p2);
            break;
            
        case ISO7816_SELECT_P1_PATH_FROM_MF:
        case ISO7816_SELECT_P1_PATH_FROM_CURRENT:
            select_by_path(fs, command, response, p1, p2);
            break;
            
        default:
            FURI_LOG_W(TAG, "Unsupported P1 value: %02X", p1);
            iso7816_set_response(response, ISO7816_RESPONSE_WRONG_PARAMETERS_P1_P2);
            break;
    }
}
