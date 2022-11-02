#pragma once

#include <gui/gui.h>
#include <stdbool.h>

#include "constants.h"


typedef struct {
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

/**
 * Set a pixel in the pwnagotchi screen buffer
 * 
 * @param pwn Pwnagotchi structure
 * @param i Row
 * @param j Column
 * @param status ? on : off
 * @return If set was successful
 */
bool pwnagotchi_screen_set(Pwnagotchi* pwn, uint8_t i, uint8_t j, bool status);

/**
 * Clears the screen buffer of the pwnagotchi
 * 
 * @param pwn Pwn to clear
 */
void pwnagotchi_screen_clear(Pwnagotchi* pwn);

/**
 * Flushes the buffer to the screen, used as a callback
 * 
 * @param canvas Passed in by OS, allows drawing
 * @param context Operating context, pwnagotchi itself
 */
void pwnagotchi_screen_flush(Canvas* canvas, void* context);
