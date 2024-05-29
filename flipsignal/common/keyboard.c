#include "keyboard_i.h"

/**
 * @brief Allocates a new flipboard keyboard.
 * @details Allocates a new flipboard keyboard. The keyboard is
 * used to send keycodes to the host attached via the USB port.
 * @return The new flipboard keyboard.
 */
FlipboardKeyboard* flipboard_keyboard_alloc() {
    FlipboardKeyboard* keyboard = malloc(sizeof(FlipboardKeyboard));
    keyboard->interface = FlipboardKeyboardInterfaceUnknown;
    keyboard->use_ble = false;
    keyboard->hid = bad_usb_hid_get_interface(
        keyboard->use_ble ? BadUsbHidInterfaceBle : BadUsbHidInterfaceUsb);
    return keyboard;
}

/**
 * @brief Frees a flipboard keyboard.
 * @details Frees a flipboard keyboard. The keyboard will
 * automatically be detached from the USB port.
 * @param keyboard The keyboard to free.
 */
void flipboard_keyboard_free(FlipboardKeyboard* keyboard) {
    flipboard_keyboard_detatch(keyboard);
    free(keyboard);
}

/**
 * @brief Attaches the keyboard to the USB port.
 * @details Attaches the keyboard to the USB port. This will
 * replace the current USB configuration. You should call
 * flipboard_keyboard_detatch to detach the keyboard.
 * @param keyboard The keyboard to attach.
 */
void flipboard_keyboard_attach(FlipboardKeyboard* keyboard) {
    keyboard->interface = FlipboardKeyboardInterfaceUnknown;
    keyboard->use_ble = false;
    keyboard->hid = bad_usb_hid_get_interface(BadUsbHidInterfaceUsb);
    keyboard->instance = keyboard->hid->init(NULL);
    uint8_t retries = 10;
    do {
        furi_delay_ms(100);
    } while(!keyboard->hid->is_connected(keyboard->instance) && retries-- > 0);

    if(keyboard->hid->is_connected(keyboard->instance)) {
        FURI_LOG_I("Keyboard", "Keyboard attached");
        keyboard->interface = FlipboardKeyboardInterfaceUsb;
    } else {
        keyboard->hid->deinit(keyboard->instance);
        FURI_LOG_W("Keyboard", "Keyboard not attached, using BLE instead.");
        keyboard->use_ble = true;
        keyboard->hid = bad_usb_hid_get_interface(BadUsbHidInterfaceBle);
        keyboard->instance = keyboard->hid->init(NULL);
        keyboard->interface = FlipboardKeyboardInterfaceBle;
    }
}

/**
 * @brief Detaches the keyboard from the USB port.
 * @details Detaches the keyboard from the USB port. This will
 * restore the previous USB configuration.
 * @param keyboard The keyboard to detach.
 */
void flipboard_keyboard_detatch(FlipboardKeyboard* keyboard) {
    if(keyboard && keyboard->instance) {
        keyboard->hid->deinit(keyboard->instance);
        keyboard->instance = NULL;
    }
}

/**
 * @brief Sends a character to the host.
 * @details Sends a character to the host. This will send the
 * character as a key press and release.
 * @param keyboard The keyboard to send the character with.
 * @param ch The character to send.
 */
void flipboard_keyboard_send_char(FlipboardKeyboard* keyboard, char ch) {
    if(keyboard->hid->is_connected(keyboard->instance)) {
        uint16_t keycode = HID_ASCII_TO_KEY(ch);
        keyboard->hid->kb_press(keyboard->instance, keycode);
        furi_delay_ms(PRESS_DELAY_MS);
        keyboard->hid->kb_release(keyboard->instance, keycode);
        furi_delay_ms(RELEASE_DELAY_MS);
    }
}

/**
 * @brief Sends a string to the host.
 * @details Sends a string to the host. This will send the
 * string as a series of key presses and releases.
 * @param keyboard The keyboard to send the string with.
 * @param message The string to send.
 */
void flipboard_keyboard_send_text(FlipboardKeyboard* keyboard, const char* message) {
    if(message != NULL) {
        for(size_t i = 0; i < strlen(message); i++) {
            // For now, we don't send the Paragraph symbol, but instead do an enter.
            if(message[i] == 0xb6) {
                // NOTE: New line will map to Enter by HID_ASCII_TO_KEY.
                char new_line = '\n';
                flipboard_keyboard_send_char(keyboard, new_line);
            } else {
                flipboard_keyboard_send_char(keyboard, message[i]);
            }
        }
    }
}

/**
 * @brief Sends a key code to the host.
 * @details Sends a key code to the host. This will send the
 * key code as a key press.  It does not release the key.
 * @param keyboard The keyboard to send the key code with.
 * @param code The key code to send.
 */
void flipboard_keyboard_send_keycode(FlipboardKeyboard* keyboard, uint16_t code) {
    FURI_LOG_D("Keyboard", "Sending keycode: %d", code);
    if(keyboard->hid->is_connected(keyboard->instance)) {
        furi_delay_ms(PRESS_DELAY_MS);
        keyboard->hid->kb_press(keyboard->instance, code);
    }
}

/**
 * @brief Releases a key code sent to the host.
 * @details Releases a key code sent to the host. This will send
 * the key code as a key release.  It does not press the key.
 * @param keyboard The keyboard to send the key codes with.
 * @param codes The key codes to release.
 */
void flipboard_keyboard_release_keycode(FlipboardKeyboard* keyboard, uint16_t code) {
    FURI_LOG_D("Keyboard", "Release keycode: %d", code);
    if(keyboard->hid->is_connected(keyboard->instance)) {
        furi_delay_ms(PRESS_DELAY_MS);
        keyboard->hid->kb_release(keyboard->instance, code);
    }
}

/**
 * @brief Releases all key codes sent to the host.
 * @details Releases all key codes sent to the host. This will 
 * release all the keys.
 * @param keyboard The keyboard to release key codes with.
 */
void flipboard_keyboard_release_all(FlipboardKeyboard* keyboard) {
    FURI_LOG_D("Keyboard", "Release all keys");
    if(keyboard->hid->is_connected(keyboard->instance)) {
        furi_delay_ms(RELEASE_DELAY_MS);
        keyboard->hid->release_all(keyboard->instance);
    }
}

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
    size_t nb_codes) {
    for(size_t i = 0; i < nb_codes; i++) {
        flipboard_keyboard_send_keycode(keyboard, codes[i]);
    }

    flipboard_keyboard_release_all(keyboard);
}

/**
 * @brief Gets the interface the keyboard is using.
 * @param keyboard The keyboard to send the key codes with.
 * @return The interface the keyboard is using.
 */
FlipboardKeyboardInterface flipboard_keyboard_get_inteface(FlipboardKeyboard* keyboard) {
    return keyboard->interface;
}