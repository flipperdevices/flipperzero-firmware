/**
 * @file flasher.h
 * @brief High-level functions for flashing the VGM firmware.
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Enumeration of possible flasher event types.
 */
typedef enum {
    FlasherEventTypeProgress, /**< Operation progress has been reported. */
    FlasherEventTypeSuccess, /**< Operation has finished successfully. */
    FlasherEventTypeError, /**< Operation has finished with an error. */
} FlasherEventType;

/**
 * @brief Enumeration of possible flasher errors.
 */
typedef enum {
    FlasherErrorBadFile, /**< File error: wrong format, I/O problem, etc.*/
    FlasherErrorDisconnect, /**< Connection error: Module disconnected, frozen, etc. */
    FlasherErrorUnknown, /**< An error that does not fit to any of the above categories. */
} FlasherError;

/**
 * @brief Flasher event structure.
 *
 * Events of FlasherEventTypeSuccess type do not carry additional data.
 */
typedef struct {
    FlasherEventType type; /**< Type of the event that has occurred. */
    union {
        uint8_t progress; /**< Progress value (0-100). */
        FlasherError error; /**< Error value. */
    };
} FlasherEvent;

/**
 * @brief Flasher event callback type.
 *
 * @param[in] event Description of the event that has occurred.
 * @param[in,out] context Pointer to a user-specified object.
 */
typedef void (*FlasherCallback)(FlasherEvent event, void* context);

/**
 * @brief Initialise the flasher.
 *
 * Calling this function will initialise the GPIO, set up the debug
 * connection, halt the module's CPU, etc.
 *
 * @returns true on success, false on failure.
 */
bool flasher_init(void);

/**
 * @brief Disable the flasher.
 *
 * Calling this function will disable all activated hardware and
 * reset the module.
 */
void flasher_deinit(void);

/**
 * @brief Set callback for flasher events.
 *
 * The callback MUST be set before calling flasher_start().
 *
 * @param[in] callback pointer to the function used to receive events.
 * @param[in] context pointer to a user-specified object (will be passed to the callback function).
 */
void flasher_set_callback(FlasherCallback callback, void* context);

/**
 * @brief Start the flashing process.
 *
 * The only way to get the return value is via the event callback.
 *
 * @param[in] file_path pointer to a zero-terminated string containing the full firmware file path.
 */
void flasher_start(const char* file_path);
