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
    uint8_t* messageQueue;
    /// Size of the message queue in bytes
    unsigned int queueSize;
    /// Location of memory index that is currently being read
    uint8_t* readIdx;
    /// Location of memory index that should be used for writing
    uint8_t* writeIdx;

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
