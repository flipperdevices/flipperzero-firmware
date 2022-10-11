#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <stdlib.h>

#include "pwnagotchi.h"

typedef struct {
    Pwnagotchi* pwnagotchi;

    // To be expandable in the future for input buffers etc.

} PwnZero;

/**
 * @brief Allocates and constructs the PwnZero structure. Initializes gui
 * 
 * @return PwnZero* Newly constructed object
 */
PwnZero* pwn_zero_alloc();

/**
 * @brief Deallocates and closes the pwnagotchi connection
 * 
 * @param pwn PwnZero to destroy
 */
void pwn_zero_free(PwnZero* pwn);

/**
 * @brief Set a given pixel on or off
 * 
 * @param pwn PwnZero object to operate on
 * @param i Column to set
 * @param j Row to set
 * @param status ? on : off
 * @return Whether the operation was successful
 */
bool pwn_zero_screen_set(PwnZero* pwn, uint8_t i, uint8_t j, bool status);

/**
 * @brief Clears the screen buffer
 * 
 * @param pwn PwnZero to operate on
 */
void pwn_zero_screen_clear(const PwnZero* pwn);

/**
 * @brief Flushes the screen buffer and writes the pixels to the screen (does not wipe the buffer)
 * 
 * @param canvas Canvas object to draw on
 * @param context PwnZero to operate on
 */
void pwn_zero_screen_flush(Canvas* canvas, void* context);

/**
 * @brief Callback function to add the inputs to the queue
 * 
 * @param inputEvent Event that needs adding to queue
 * @param eventQueue Queue to add inputEvent
 */
void input_callback(InputEvent* inputEvent, FuriMessageQueue* eventQueue);
