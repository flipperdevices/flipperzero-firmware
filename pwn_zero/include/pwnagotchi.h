#pragma once

#include "constants.h"

#include <stdbool.h>

#include <furi_hal_uart.h>

typedef struct {
    /// UART settings
    FuriHalUartId   channel;
    uint32_t        baudRate;

    /// Screen representation
    bool screen[FLIPPER_SCREEN_HEIGHT][FLIPPER_SCREEN_WIDTH];

    /// Message queue
    uint8_t messageQueue[PWNAGOTCHI_PROTOCOL_QUEUE_SIZE][PWNAGOTCHI_PROTOCOL_BYTE_LEN];
    /// Index of currently saving queue number
    size_t queueIdx;
    /// The current position in the given protocol message
    size_t byteIdx;

} Pwnagotchi;

/**
 * @brief Allocates and constructs a pwnagotchi struct
 * 
 * @return Pwnagotchi* Constructed pwnagotchi pointer
 */
Pwnagotchi* pwnagotchi_alloc();

/**
 * @brief Destruct and free pwnagotchi
 * 
 * @param pwn Pwnagotchi to destruct
 */
void pwnagotchi_free(Pwnagotchi* pwn);
