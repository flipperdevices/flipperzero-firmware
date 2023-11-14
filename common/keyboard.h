#pragma once

/**
 * @file keyboard.h
 * @brief A keyboard that can be used to send key codes to the host using the 
 * USB cable connected to the Flipper Zero.
*/

#include <furi.h>
#include <furi_hal.h>

typedef struct FlipboardKeyboard FlipboardKeyboard;

/**
 * @brief Allocates a new flipboard keyboard.
 * @details Allocates a new flipboard keyboard. The keyboard is
 * used to send keycodes to the host attached via the USB port.
 * @return The new flipboard keyboard.
 */
FlipboardKeyboard* flipboard_keyboard_alloc();

/**
 * @brief Frees a flipboard keyboard.
 * @details Frees a flipboard keyboard. The keyboard will
 * automatically be detached from the USB port.
 * @param keyboard The keyboard to free.
 */
void flipboard_keyboard_free(FlipboardKeyboard* keyboard);

/**
 * @brief Attaches the keyboard to the USB port.
 * @details Attaches the keyboard to the USB port. This will
 * replace the current USB configuration. You should call
 * flipboard_keyboard_detatch to detach the keyboard.
 * @param keyboard The keyboard to attach.
 */
void flipboard_keyboard_attach(FlipboardKeyboard* keyboard);

/**
 * @brief Detaches the keyboard from the USB port.
 * @details Detaches the keyboard from the USB port. This will
 * restore the previous USB configuration.
 * @param keyboard The keyboard to detach.
 */
void flipboard_keyboard_detatch(FlipboardKeyboard* keyboard);

/**
 * @brief Sends a character to the host.
 * @details Sends a character to the host. This will send the
 * character as a key press and release.
 * @param keyboard The keyboard to send the character with.
 * @param ch The character to send.
 */
void flipboard_keyboard_send_char(FlipboardKeyboard* keyboard, char ch);

/**
 * @brief Sends a string to the host.
 * @details Sends a string to the host. This will send the
 * string as a series of key presses and releases.
 * @param keyboard The keyboard to send the string with.
 * @param message The string to send.
 */
void flipboard_keyboard_send_text(FlipboardKeyboard* keyboard, const char* message);

/**
 * @brief Sends a key code to the host.
 * @details Sends a key code to the host. This will send the
 * key code as a key press.  It does not release the key.
 * @param keyboard The keyboard to send the key code with.
 * @param code The key code to send.
 */
void flipboard_keyboard_send_keycode(FlipboardKeyboard* keyboard, uint16_t code);

/**
 * @brief Releases a key code sent to the host.
 * @details Releases a key code sent to the host. This will send
 * the key code as a key release.  It does not press the key.
 * @param keyboard The keyboard to send the key codes with.
 * @param codes The key codes to release.
 */
void flipboard_keyboard_release_keycode(FlipboardKeyboard* keyboard, uint16_t code);

/**
 * @brief Releases all key codes sent to the host.
 * @details Releases all key codes sent to the host. This will 
 * release all the keys.
 * @param keyboard The keyboard to release key codes with.
 */
void flipboard_keyboard_release_all(FlipboardKeyboard* keyboard);

/**
 * @brief Sends a series of key codes to the host.
 * @details Sends a series of key codes to the host. This will
 * send the key codes as a series of key presses.  It releases
 * all keys once it has sent all the codes.
 * @param keyboard The keyboard to send the key codes with.
 * @param codes The key codes to send.
 * @param nb_codes The number of key codes to send.
 */
void flipboard_keyboard_send_keycodes(
    FlipboardKeyboard* keyboard,
    uint16_t* codes,
    size_t nb_codes);
