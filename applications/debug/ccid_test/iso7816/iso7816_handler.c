/**
 * @file iso7816_handler.c
 * @brief ISO7816 Command Handler Implementation
 * 
 * Transforms low-level CCID callbacks into structured ISO7816 command/response handling.
 * Manages the file system and routes commands to appropriate handlers.
 */

#include <stdint.h>
#include <stddef.h>
#include <furi.h>
#include <furi_hal.h>

#include "iso7816_handler.h"
#include "iso7816_t0_apdu.h"
#include "iso7816_atr.h"
#include "iso7816_response.h"
#include "iso7816_filesystem.h"
#include "commands/iso7816_cmd_select.h"
#include "commands/iso7816_cmd_get_data.h"
#include "commands/iso7816_cmd_verify.h"

#define TAG "ISO7816_Handler"

/** Handler state */
static Iso7816Handler* iso7816_handler = NULL;
static CcidCallbacks* ccid_callbacks = NULL;
static uint8_t* command_apdu_buffer = NULL;
static uint8_t* response_apdu_buffer = NULL;
static Iso7816FileSystem* filesystem = NULL;

/**
 * @brief Process standard ISO7816-4 interindustry commands (CLA = 0x00)
 */
static bool iso7816_process_interindustry_command(
    const ISO7816_Command_APDU* command,
    ISO7816_Response_APDU* response) {
    
    switch(command->INS) {
        case ISO7816_INS_SELECT:
            iso7816_cmd_select_process(filesystem, command, response);
            return true;
            
        case ISO7816_INS_GET_DATA:
        case ISO7816_INS_GET_DATA_ODD:
            iso7816_cmd_get_data(filesystem, command, response);
            return true;
            
        case ISO7816_INS_VERIFY:
        case ISO7816_INS_VERIFY_BIO:
            iso7816_cmd_verify(filesystem, command, response);
            return true;
            
        // TODO: Add more standard commands here
        // case ISO7816_INS_READ_BINARY:
        // case ISO7816_INS_GET_RESPONSE:
        // etc.
            
        default:
            return false;  // Not handled
    }
}

/**
 * @brief Default command processor
 * 
 * Handles standard ISO7816-4 commands and delegates to registered apps.
 */
static void iso7816_default_process_command(
    const ISO7816_Command_APDU* command,
    ISO7816_Response_APDU* response) {
    
    // Check CLA byte for interindustry commands
    // CLA = 0x0X means interindustry class with no SM, channel 0-3
    uint8_t cla_masked = command->CLA & 0xF0;
    
    if(cla_masked == 0x00) {
        // Interindustry command
        if(iso7816_process_interindustry_command(command, response)) {
            return;  // Command was handled
        }
    }
    
    // Check if there's a selected application that can handle this
    const Iso7816AppDefinition* app = iso7816_fs_get_current_app(filesystem);
    if(app && app->handler) {
        app->handler(app->context, command, response);
        return;
    }
    
    // Unknown instruction
    FURI_LOG_D(TAG, "Unhandled command CLA=%02X INS=%02X", command->CLA, command->INS);
    iso7816_set_response(response, ISO7816_RESPONSE_INSTRUCTION_NOT_SUPPORTED);
}

/**
 * @brief ICC Power On callback - returns ATR
 */
static void iso7816_icc_power_on_callback(uint8_t* atr_data, uint32_t* atr_data_len, void* context) {
    furi_check(context);

    Iso7816Handler* handler = (Iso7816Handler*)context;

    Iso7816Atr iso7816_atr;
    handler->iso7816_answer_to_reset(&iso7816_atr);

    furi_assert(iso7816_atr.T0 == 0x00);

    uint8_t atr_buffer[2] = {iso7816_atr.TS, iso7816_atr.T0};

    *atr_data_len = 2;

    memcpy(atr_data, atr_buffer, sizeof(uint8_t) * (*atr_data_len));
    
    // Reset to MF on power on
    if(filesystem) {
        iso7816_fs_select_mf(filesystem);
    }
    
    FURI_LOG_I(TAG, "ICC Power On - ATR sent, MF selected");
}

/**
 * @brief XFR DataBlock callback - processes APDUs
 */
static void iso7816_xfr_datablock_callback(
    const uint8_t* pc_to_reader_datablock,
    uint32_t pc_to_reader_datablock_len,
    uint8_t* reader_to_pc_datablock,
    uint32_t* reader_to_pc_datablock_len,
    void* context) {
    furi_check(context);

    Iso7816Handler* handler = (Iso7816Handler*)context;

    ISO7816_Response_APDU* response_apdu = (ISO7816_Response_APDU*)response_apdu_buffer;
    ISO7816_Command_APDU* command_apdu = (ISO7816_Command_APDU*)command_apdu_buffer;
    
    // Clear response
    response_apdu->DataLen = 0;
    response_apdu->SW1 = 0;
    response_apdu->SW2 = 0;

    uint8_t result = iso7816_read_command_apdu(
        command_apdu, pc_to_reader_datablock, pc_to_reader_datablock_len, CCID_EXTENDED_APDU_SIZE);

    if(result == ISO7816_READ_COMMAND_APDU_OK) {
        // Try application handler first
        bool handled = false;
        if(handler->iso7816_process_command) {
            handled = handler->iso7816_process_command(command_apdu, response_apdu);
        }
        
        // If not handled by application, use default handler
        if(!handled) {
            iso7816_default_process_command(command_apdu, response_apdu);
        }

        furi_assert(response_apdu->DataLen <= CCID_EXTENDED_APDU_SIZE);
    } else if(result == ISO7816_READ_COMMAND_APDU_ERROR_WRONG_LE) {
        iso7816_set_response(response_apdu, ISO7816_RESPONSE_WRONG_LE);
    } else if(result == ISO7816_READ_COMMAND_APDU_ERROR_WRONG_LENGTH) {
        iso7816_set_response(response_apdu, ISO7816_RESPONSE_WRONG_LENGTH);
    }

    iso7816_write_response_apdu(response_apdu, reader_to_pc_datablock, reader_to_pc_datablock_len);
}

Iso7816Handler* iso7816_handler_alloc(void) {
    iso7816_handler = malloc(sizeof(Iso7816Handler));
    memset(iso7816_handler, 0, sizeof(Iso7816Handler));

    command_apdu_buffer = malloc(sizeof(ISO7816_Command_APDU) + CCID_EXTENDED_APDU_SIZE);
    response_apdu_buffer = malloc(sizeof(ISO7816_Response_APDU) + CCID_EXTENDED_APDU_SIZE);

    ccid_callbacks = malloc(sizeof(CcidCallbacks));
    ccid_callbacks->icc_power_on_callback = iso7816_icc_power_on_callback;
    ccid_callbacks->xfr_datablock_callback = iso7816_xfr_datablock_callback;
    
    // Initialize file system
    filesystem = iso7816_filesystem_alloc();
    
    // Register default test PINs (PIN "1234" with 3 retries)
    const uint8_t default_pin[] = {0x31, 0x32, 0x33, 0x34};  // "1234"
    iso7816_register_pin(filesystem, 0x01, default_pin, 4, 3);  // Global PIN ref 1
    iso7816_register_pin(filesystem, 0x02, default_pin, 4, 3);  // Global PIN ref 2
    iso7816_register_pin(filesystem, 0x03, default_pin, 4, 3);  // Global PIN ref 3
    
    FURI_LOG_I(TAG, "ISO7816 handler allocated with filesystem and default PINs");

    return iso7816_handler;
}

void iso7816_handler_set_usb_ccid_callbacks(void) {
    furi_hal_usb_ccid_set_callbacks(ccid_callbacks, iso7816_handler);
}

void iso7816_handler_reset_usb_ccid_callbacks(void) {
    furi_hal_usb_ccid_set_callbacks(NULL, NULL);
}

void iso7816_handler_free(Iso7816Handler* handler) {
    // Free file system
    if(filesystem) {
        iso7816_filesystem_free(filesystem);
        filesystem = NULL;
    }
    
    free(ccid_callbacks);
    ccid_callbacks = NULL;

    free(command_apdu_buffer);
    command_apdu_buffer = NULL;
    
    free(response_apdu_buffer);
    response_apdu_buffer = NULL;

    free(handler);
    iso7816_handler = NULL;
    
    FURI_LOG_I(TAG, "ISO7816 handler freed");
}

Iso7816FileSystem* iso7816_handler_get_filesystem(void) {
    return filesystem;
}

Iso7816File* iso7816_handler_register_app(uint16_t fid, const Iso7816AppDefinition* app) {
    if(!filesystem) {
        FURI_LOG_E(TAG, "Cannot register app: filesystem not initialized");
        return NULL;
    }
    
    return iso7816_fs_register_app(filesystem, NULL, fid, app);
}
