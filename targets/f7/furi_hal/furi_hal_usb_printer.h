#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <furi_hal_usb.h>

#ifdef __cplusplus
extern "C" {
#endif

/** USB Printer Class Interface */
extern FuriHalUsbInterface usb_printer;

/** Printer job status */
typedef enum {
    PrinterStatusIdle = 0,
    PrinterStatusBusy,
    PrinterStatusError,
} PrinterStatus;

/** Printer error codes */
typedef enum {
    PrinterErrorNone = 0,
    PrinterErrorPaperEmpty,
    PrinterErrorOverheat,
    PrinterErrorGeneral,
} PrinterError;

/** Printer callbacks structure */
typedef struct {
    void (*data_rx_callback)(const uint8_t* data, size_t len, void* context);
    void (*status_callback)(bool connected, void* context);
} PrinterCallbacks;

/**
 * @brief Receive data from the host
 * @param buffer Pointer to the buffer to store received data
 * @param max_len Maximum length of the buffer
 * @return Number of bytes received, 0 if no data available
 */
int32_t furi_hal_usb_printer_receive(uint8_t* buffer, uint16_t max_len);

/**
 * @brief Set printer port status
 * @param paper_empty true if paper is empty
 * @param selected true if printer is selected
 * @param error true if printer is in error state
 */
void furi_hal_usb_printer_set_status(bool paper_empty, bool selected, bool error);

/**
 * @brief Check if printer interface is connected
 * @return true if connected
 */
bool furi_hal_usb_printer_is_connected(void);

/**
 * @brief Get current printer status
 * @return Current printer status
 */
PrinterStatus furi_hal_usb_printer_get_status(void);

/**
 * @brief Get last printer error
 * @return Last printer error code
 */
PrinterError furi_hal_usb_printer_get_error(void);

/**
 * @brief Set structured callbacks for printer events
 * @param callbacks Pointer to PrinterCallbacks structure
 * @param context Context pointer passed to callbacks
 */
void furi_hal_usb_printer_set_callbacks(PrinterCallbacks* callbacks, void* context);

/**
 * @brief Set callback for receiving data from host (legacy function)
 * @param callback Function pointer to callback
 * @param context Context pointer passed to callback
 */
void furi_hal_usb_printer_set_rx_callback(void (*callback)(uint8_t* data, size_t len, void* context), void* context);

#ifdef __cplusplus
}
#endif
