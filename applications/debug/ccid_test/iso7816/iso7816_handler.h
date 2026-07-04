/**
 * @file iso7816_handler.h
 * @brief ISO7816 Command Handler
 * 
 * Provides the interface between CCID USB layer and ISO7816 command processing.
 * Manages the file system and routes commands to appropriate handlers.
 */

#pragma once

#include <stdint.h>
#include "iso7816_atr.h"
#include "iso7816_t0_apdu.h"
#include "iso7816_filesystem.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ISO7816 Handler callbacks
 * 
 * Applications can provide custom handlers for ATR and command processing.
 * If iso7816_process_command returns without setting a response, the
 * default handler will process standard ISO7816-4 commands.
 */
typedef struct {
    /** Callback to provide Answer-to-Reset data */
    void (*iso7816_answer_to_reset)(Iso7816Atr* atr);
    
    /** 
     * Callback to process application-specific commands.
     * Return true if command was handled, false to use default processing.
     */
    bool (*iso7816_process_command)(
        const ISO7816_Command_APDU* command,
        ISO7816_Response_APDU* response);
} Iso7816Handler;

/**
 * @brief Allocate and initialize the ISO7816 handler
 * 
 * Creates the handler and initializes the file system with MF.
 * 
 * @return Pointer to the handler
 */
Iso7816Handler* iso7816_handler_alloc(void);

/**
 * @brief Free the ISO7816 handler
 * 
 * @param handler Handler to free
 */
void iso7816_handler_free(Iso7816Handler* handler);

/**
 * @brief Set USB CCID callbacks to use this handler
 */
void iso7816_handler_set_usb_ccid_callbacks(void);

/**
 * @brief Reset USB CCID callbacks (disconnect handler)
 */
void iso7816_handler_reset_usb_ccid_callbacks(void);

/**
 * @brief Get the file system from the handler
 * 
 * Allows applications to register files and applications.
 * 
 * @return Pointer to the file system
 */
Iso7816FileSystem* iso7816_handler_get_filesystem(void);

/**
 * @brief Register an application with the handler
 * 
 * Convenience function to register an app with the file system.
 * 
 * @param fid File identifier for the application DF
 * @param app Application definition
 * @return Application DF or NULL on error
 */
Iso7816File* iso7816_handler_register_app(uint16_t fid, const Iso7816AppDefinition* app);

#ifdef __cplusplus
}
#endif
