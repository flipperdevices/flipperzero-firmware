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
