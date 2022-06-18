#pragma once
#include "hid_usage_desktop.h"
#include "hid_usage_button.h"
#include "hid_usage_keyboard.h"
#include "hid_usage_consumer.h"
#include "hid_usage_led.h"

#define HID_KEYBOARD_NONE 0x00

/** ASCII to keycode conversion table */
static const uint16_t hid_asciimap[] = {
    HID_KEYBOARD_NONE, // NUL
    HID_KEYBOARD_NONE, // SOH
    HID_KEYBOARD_NONE, // STX
    HID_KEYBOARD_NONE, // ETX
    HID_KEYBOARD_NONE, // EOT
    HID_KEYBOARD_NONE, // ENQ
    HID_KEYBOARD_NONE, // ACK
    HID_KEYBOARD_NONE, // BEL
    HID_KEYPAD_BACKSPACE, // BS   Backspace
    HID_KEYPAD_TAB, // TAB  Tab
    HID_KEYPAD_ENTER, // LF   Enter
    HID_KEYBOARD_NONE, // VT
    HID_KEYBOARD_NONE, // FF
    HID_KEYBOARD_NONE, // CR
    HID_KEYBOARD_NONE, // SO
    HID_KEYBOARD_NONE, // SI
    HID_KEYBOARD_NONE, // DEL
    HID_KEYBOARD_NONE, // DC1
    HID_KEYBOARD_NONE, // DC2
    HID_KEYBOARD_NONE, // DC3
    HID_KEYBOARD_NONE, // DC4
    HID_KEYBOARD_NONE, // NAK
    HID_KEYBOARD_NONE, // SYN
    HID_KEYBOARD_NONE, // ETB
    HID_KEYBOARD_NONE, // CAN
    HID_KEYBOARD_NONE, // EM
    HID_KEYBOARD_NONE, // SUB
    HID_KEYBOARD_NONE, // ESC
    HID_KEYBOARD_NONE, // FS
    HID_KEYBOARD_NONE, // GS
    HID_KEYBOARD_NONE, // RS
    HID_KEYBOARD_NONE, // US
    HID_KEYBOARD_SPACEBAR, // ' ' Space
    HID_KEYBOARD_1 | HID_KEYBOARD_L_SHIFT, // !
    HID_KEYBOARD_APOSTROPHE | HID_KEYBOARD_L_SHIFT, // "
    HID_KEYBOARD_3 | HID_KEYBOARD_L_SHIFT, // #
    HID_KEYBOARD_4 | HID_KEYBOARD_L_SHIFT, // $
    HID_KEYBOARD_5 | HID_KEYBOARD_L_SHIFT, // %
    HID_KEYBOARD_7 | HID_KEYBOARD_L_SHIFT, // &
    HID_KEYBOARD_APOSTROPHE, // '
    HID_KEYBOARD_9 | HID_KEYBOARD_L_SHIFT, // (
    HID_KEYBOARD_0 | HID_KEYBOARD_L_SHIFT, // )
    HID_KEYBOARD_8 | HID_KEYBOARD_L_SHIFT, // *
    HID_KEYBOARD_EQUAL_SIGN | HID_KEYBOARD_L_SHIFT, // +
    HID_KEYPAD_COMMA, // ,
    HID_KEYBOARD_MINUS, // -
    HID_KEYBOARD_DOT, // .
    HID_KEYBOARD_SLASH, // /
    HID_KEYBOARD_0, // 0
    HID_KEYBOARD_1, // 1
    HID_KEYBOARD_2, // 2
    HID_KEYBOARD_3, // 3
    HID_KEYBOARD_4, // 4
    HID_KEYBOARD_5, // 5
    HID_KEYBOARD_6, // 6
    HID_KEYBOARD_7, // 7
    HID_KEYBOARD_8, // 8
    HID_KEYBOARD_9, // 9
    HID_KEYBOARD_SEMICOLON | HID_KEYBOARD_L_SHIFT, // :
    HID_KEYBOARD_SEMICOLON, // ;
    HID_KEYPAD_COMMA | HID_KEYBOARD_L_SHIFT, // <
    HID_KEYBOARD_EQUAL_SIGN, // =
    HID_KEYBOARD_DOT | HID_KEYBOARD_L_SHIFT, // >
    HID_KEYBOARD_SLASH | HID_KEYBOARD_L_SHIFT, // ?
    HID_KEYBOARD_2 | HID_KEYBOARD_L_SHIFT, // @
    HID_KEYBOARD_A | HID_KEYBOARD_L_SHIFT, // A
    HID_KEYBOARD_B | HID_KEYBOARD_L_SHIFT, // B
    HID_KEYBOARD_C | HID_KEYBOARD_L_SHIFT, // C
    HID_KEYBOARD_D | HID_KEYBOARD_L_SHIFT, // D
    HID_KEYBOARD_E | HID_KEYBOARD_L_SHIFT, // E
    HID_KEYBOARD_F | HID_KEYBOARD_L_SHIFT, // F
    HID_KEYBOARD_G | HID_KEYBOARD_L_SHIFT, // G
    HID_KEYBOARD_H | HID_KEYBOARD_L_SHIFT, // H
    HID_KEYBOARD_I | HID_KEYBOARD_L_SHIFT, // I
    HID_KEYBOARD_J | HID_KEYBOARD_L_SHIFT, // J
    HID_KEYBOARD_K | HID_KEYBOARD_L_SHIFT, // K
    HID_KEYBOARD_L | HID_KEYBOARD_L_SHIFT, // L
    HID_KEYBOARD_M | HID_KEYBOARD_L_SHIFT, // M
    HID_KEYBOARD_N | HID_KEYBOARD_L_SHIFT, // N
    HID_KEYBOARD_O | HID_KEYBOARD_L_SHIFT, // O
    HID_KEYBOARD_P | HID_KEYBOARD_L_SHIFT, // P
    HID_KEYBOARD_Q | HID_KEYBOARD_L_SHIFT, // Q
    HID_KEYBOARD_R | HID_KEYBOARD_L_SHIFT, // R
    HID_KEYBOARD_S | HID_KEYBOARD_L_SHIFT, // S
    HID_KEYBOARD_T | HID_KEYBOARD_L_SHIFT, // T
    HID_KEYBOARD_U | HID_KEYBOARD_L_SHIFT, // U
    HID_KEYBOARD_V | HID_KEYBOARD_L_SHIFT, // V
    HID_KEYBOARD_W | HID_KEYBOARD_L_SHIFT, // W
    HID_KEYBOARD_X | HID_KEYBOARD_L_SHIFT, // X
    HID_KEYBOARD_Y | HID_KEYBOARD_L_SHIFT, // Y
    HID_KEYBOARD_Z | HID_KEYBOARD_L_SHIFT, // Z
    HID_KEYBOARD_OPEN_BRACKET, // [
    HID_KEYBOARD_BACKSLASH, // bslash
    HID_KEYBOARD_CLOSE_BRACKET, // ]
    HID_KEYBOARD_6 | HID_KEYBOARD_L_SHIFT, // ^
    HID_KEYBOARD_MINUS | HID_KEYBOARD_L_SHIFT, // _
    HID_KEYBOARD_GRAVE_ACCENT, // `
    HID_KEYBOARD_A, // a
    HID_KEYBOARD_B, // b
    HID_KEYBOARD_C, // c
    HID_KEYBOARD_D, // d
    HID_KEYBOARD_E, // e
    HID_KEYBOARD_F, // f
    HID_KEYBOARD_G, // g
    HID_KEYBOARD_H, // h
    HID_KEYBOARD_I, // i
    HID_KEYBOARD_J, // j
    HID_KEYBOARD_K, // k
    HID_KEYBOARD_L, // l
    HID_KEYBOARD_M, // m
    HID_KEYBOARD_N, // n
    HID_KEYBOARD_O, // o
    HID_KEYBOARD_P, // p
    HID_KEYBOARD_Q, // q
    HID_KEYBOARD_R, // r
    HID_KEYBOARD_S, // s
    HID_KEYBOARD_T, // t
    HID_KEYBOARD_U, // u
    HID_KEYBOARD_V, // v
    HID_KEYBOARD_W, // w
    HID_KEYBOARD_X, // x
    HID_KEYBOARD_Y, // y
    HID_KEYBOARD_Z, // z
    HID_KEYBOARD_OPEN_BRACKET | HID_KEYBOARD_L_SHIFT, // {
    HID_KEYBOARD_BACKSLASH | HID_KEYBOARD_L_SHIFT, // |
    HID_KEYBOARD_CLOSE_BRACKET | HID_KEYBOARD_L_SHIFT, // }
    HID_KEYBOARD_GRAVE_ACCENT | HID_KEYBOARD_L_SHIFT, // ~
    HID_KEYBOARD_NONE, // DEL
};

typedef struct {
    uint32_t vid;
    uint32_t pid;
    char manuf[32];
    char product[32];
} FuriHalUsbHidConfig;

typedef void (*HidStateCallback)(bool state, void* context);

/** ASCII to keycode conversion macro */
#define HID_ASCII_TO_KEY(x) (((uint8_t)x < 128) ? (hid_asciimap[(uint8_t)x]) : HID_KEYBOARD_NONE)

/** HID keyboard modifiers */
enum HidKeyboardModifiers {
    HID_KB_MOD_L_CTRL = (1 << 0),
    HID_KB_MOD_L_SHIFT = (1 << 1),
    HID_KB_MOD_L_ALT = (1 << 2),
    HID_KB_MOD_L_GUI = (1 << 3),
    HID_KB_MOD_R_CTRL = (1 << 4),
    HID_KB_MOD_R_SHIFT = (1 << 5),
    HID_KB_MOD_R_ALT = (1 << 6),
    HID_KB_MOD_R_GUI = (1 << 7),
};

/** HID keyboard leds */
enum HidKeyboardLeds {
    HID_KB_LED_NUM = (1 << 0),
    HID_KB_LED_CAPS = (1 << 1),
    HID_KB_LED_SCROLL = (1 << 2),
};

/** HID mouse buttons */
enum HidMouseButtons {
    HID_MOUSE_BTN_LEFT = (1 << 0),
    HID_MOUSE_BTN_RIGHT = (1 << 1),
    HID_MOUSE_BTN_WHEEL = (1 << 2),
};

/** Get USB HID connection state
 *
 * @return      true / false
 */
bool furi_hal_hid_is_connected();

/** Get USB HID keyboard leds state
 *
 * @return      leds state
 */
uint8_t furi_hal_hid_get_led_state();

/** Set USB HID connect/disconnect callback
 *
 * @param      cb  callback
 * @param      ctx  callback context
 */
void furi_hal_hid_set_state_callback(HidStateCallback cb, void* ctx);

/** Set the following key to pressed state and send HID report
 *
 * @param      button  key code
 */
bool furi_hal_hid_kb_press(uint16_t button);

/** Set the following key to released state and send HID report
 *
 * @param      button  key code
 */
bool furi_hal_hid_kb_release(uint16_t button);

/** Clear all pressed keys and send HID report
 *
 */
bool furi_hal_hid_kb_release_all();

/** Set mouse movement and send HID report
 *
 * @param      dx  x coordinate delta
 * @param      dy  y coordinate delta
 */
bool furi_hal_hid_mouse_move(int8_t dx, int8_t dy);

/** Set mouse button to pressed state and send HID report
 *
 * @param      button  key code
 */
bool furi_hal_hid_mouse_press(uint8_t button);

/** Set mouse button to released state and send HID report
 *
 * @param      button  key code
 */
bool furi_hal_hid_mouse_release(uint8_t button);

/** Set mouse wheel position and send HID report
 *
 * @param      delta  number of scroll steps
 */
bool furi_hal_hid_mouse_scroll(int8_t delta);

/** Set the following consumer key to pressed state and send HID report
 *
 * @param      button  key code
 */
bool furi_hal_hid_consumer_key_press(uint16_t button);

/** Set the following consumer key to released state and send HID report
 *
 * @param      button  key code
 */
bool furi_hal_hid_consumer_key_release(uint16_t button);
