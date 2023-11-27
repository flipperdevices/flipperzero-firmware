#pragma once

/**
 * @file     config_keystroke.h
 * @brief    The configuration of the keystrokes.
 * @details  This file contains the configuration of the keystrokes.  You can
 * add new keystrokes here, and they will be available in the config.  There
 * is a limit of 256 keystrokes.  Be sure to update the keystroke_names array
 * with the name of the keystroke.  The keystroke_values array contains the
 * HID values of the keystrokes.  The index of the keystroke in this array is
 * the same as the index of the keystroke in the keystroke_names array.  The
 * keystroke_count_names array contains the number of times to repeat the
 * keystroke.  This array should start at value 0, and increment by 1s.
*/

#include <furi_hal.h>

/**
 * @brief    The HID values of the keystrokes.
 * @details  The index of the keystroke in this array is the same as
 *  the index of the keystroke in the keystroke_names array.
*/
uint16_t keystroke_values[] = {
    HID_KEYBOARD_NONE, // Not mapped
    1, // Delay

    // hid_keymap
    HID_KEYBOARD_L_CTRL, // HID_KEYBOARD_L_CTRL
    HID_KEYBOARD_R_CTRL, // HID_KEYBOARD_R_CTRL
    HID_KEYBOARD_L_SHIFT, // HID_KEYBOARD_L_SHIFT
    HID_KEYBOARD_R_SHIFT, // HID_KEYBOARD_R_SHIFT
    HID_KEYBOARD_L_ALT, // HID_KEYBOARD_L_ALT
    HID_KEYBOARD_R_ALT, // HID_KEYBOARD_R_ALT
    HID_KEYBOARD_L_GUI, // HID_KEYBOARD_L_GUI
    HID_KEYBOARD_R_GUI, // HID_KEYBOARD_R_GUI

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

    HID_KEYBOARD_F1, // HID_KEYBOARD_F1
    HID_KEYBOARD_F2, // HID_KEYBOARD_F2
    HID_KEYBOARD_F3, // HID_KEYBOARD_F3
    HID_KEYBOARD_F4, // HID_KEYBOARD_F4
    HID_KEYBOARD_F5, // HID_KEYBOARD_F5
    HID_KEYBOARD_F6, // HID_KEYBOARD_F6
    HID_KEYBOARD_F7, // HID_KEYBOARD_F7
    HID_KEYBOARD_F8, // HID_KEYBOARD_F8
    HID_KEYBOARD_F9, // HID_KEYBOARD_F9
    HID_KEYBOARD_F10, // HID_KEYBOARD_F10
    HID_KEYBOARD_F11, // HID_KEYBOARD_F11
    HID_KEYBOARD_F12, // HID_KEYBOARD_F12

    HID_KEYBOARD_1, // 1
    HID_KEYBOARD_2, // 2
    HID_KEYBOARD_3, // 3
    HID_KEYBOARD_4, // 4
    HID_KEYBOARD_5, // 5
    HID_KEYBOARD_6, // 6
    HID_KEYBOARD_7, // 7
    HID_KEYBOARD_8, // 8
    HID_KEYBOARD_9, // 9
    HID_KEYBOARD_0, // 0

    HID_KEYBOARD_DELETE_FORWARD, // DEL
    HID_KEYBOARD_DELETE, // BS Backspace
    HID_KEYBOARD_TAB, // TAB Tab
    HID_KEYBOARD_RETURN, // LF Enter
    HID_KEYBOARD_SPACEBAR, // ' ' Space
    HID_KEYBOARD_ESCAPE, // HID_KEYBOARD_ESCAPE
    HID_KEYBOARD_PRINT_SCREEN, // HID_KEYBOARD_PRINT_SCREEN
    HID_KEYBOARD_CAPS_LOCK, // HID_KEYBOARD_CAPS_LOCK
    HID_KEYPAD_NUMLOCK, // HID_KEYPAD_NUMLOCK
    HID_KEYBOARD_SCROLL_LOCK, // HID_KEYBOARD_SCROLL_LOCK
    HID_KEYBOARD_PAUSE, // HID_KEYBOARD_PAUSE
    HID_KEYBOARD_INSERT, // HID_KEYBOARD_INSERT
    HID_KEYBOARD_HOME, // HID_KEYBOARD_HOME
    HID_KEYBOARD_PAGE_UP, // HID_KEYBOARD_PAGE_UP
    HID_KEYBOARD_DELETE_FORWARD, // HID_KEYBOARD_DELETE_FORWARD
    HID_KEYBOARD_END, // HID_KEYBOARD_END
    HID_KEYBOARD_PAGE_DOWN, // HID_KEYBOARD_PAGE_DOWN
    HID_KEYBOARD_RIGHT_ARROW, // HID_KEYBOARD_RIGHT_ARROW
    HID_KEYBOARD_LEFT_ARROW, // HID_KEYBOARD_LEFT_ARROW
    HID_KEYBOARD_DOWN_ARROW, // HID_KEYBOARD_DOWN_ARROW
    HID_KEYBOARD_UP_ARROW, // HID_KEYBOARD_UP_ARROW

    HID_KEYBOARD_OPEN_BRACKET, // [
    HID_KEYBOARD_CLOSE_BRACKET, // ]
    HID_KEYBOARD_OPEN_BRACKET | KEY_MOD_LEFT_SHIFT, // {
    HID_KEYBOARD_CLOSE_BRACKET | KEY_MOD_LEFT_SHIFT, // }
    HID_KEYBOARD_BACKSLASH, // / bslash
    HID_KEYBOARD_BACKSLASH | KEY_MOD_LEFT_SHIFT, // |
    HID_KEYBOARD_SEMICOLON, // ;
    HID_KEYBOARD_SEMICOLON | KEY_MOD_LEFT_SHIFT, // :
    HID_KEYBOARD_APOSTROPHE, // '
    HID_KEYBOARD_APOSTROPHE | KEY_MOD_LEFT_SHIFT, // "
    HID_KEYBOARD_COMMA, // ,
    HID_KEYBOARD_COMMA | KEY_MOD_LEFT_SHIFT, // <
    HID_KEYBOARD_DOT | KEY_MOD_LEFT_SHIFT, // >
    HID_KEYBOARD_DOT, // .
    HID_KEYBOARD_SLASH, // /
    HID_KEYBOARD_SLASH | KEY_MOD_LEFT_SHIFT, // ?

    HID_KEYBOARD_GRAVE_ACCENT, // `
    HID_KEYBOARD_GRAVE_ACCENT | KEY_MOD_LEFT_SHIFT, // ~
    HID_KEYBOARD_1 | KEY_MOD_LEFT_SHIFT, // !
    HID_KEYBOARD_2 | KEY_MOD_LEFT_SHIFT, // @
    HID_KEYBOARD_3 | KEY_MOD_LEFT_SHIFT, // #
    HID_KEYBOARD_4 | KEY_MOD_LEFT_SHIFT, // $
    HID_KEYBOARD_5 | KEY_MOD_LEFT_SHIFT, // %
    HID_KEYBOARD_6 | KEY_MOD_LEFT_SHIFT, // ^
    HID_KEYBOARD_7 | KEY_MOD_LEFT_SHIFT, // &
    HID_KEYBOARD_8 | KEY_MOD_LEFT_SHIFT, // *
    HID_KEYBOARD_9 | KEY_MOD_LEFT_SHIFT, // (
    HID_KEYBOARD_0 | KEY_MOD_LEFT_SHIFT, // )
    HID_KEYBOARD_MINUS, // -
    HID_KEYBOARD_MINUS | KEY_MOD_LEFT_SHIFT, // _
    HID_KEYBOARD_EQUAL_SIGN, // =
    HID_KEYBOARD_EQUAL_SIGN | KEY_MOD_LEFT_SHIFT, // +

    HID_KEYBOARD_CUT, // HID_KEYBOARD_CUT
    HID_KEYBOARD_COPY, // HID_KEYBOARD_COPY
    HID_KEYBOARD_PASTE, // HID_KEYBOARD_PASTE
    HID_KEYBOARD_UNDO, // HID_KEYBOARD_UNDO
    HID_KEYBOARD_FIND, // HID_KEYBOARD_FIND
    HID_KEYBOARD_MUTE, // HID_KEYBOARD_MUTE
    HID_KEYBOARD_VOLUME_UP, // HID_KEYBOARD_VOLUME_UP
    HID_KEYBOARD_VOLUME_DOWN, // HID_KEYBOARD_VOLUME_DOWN
    HID_KEYBOARD_HELP, // HID_KEYBOARD_HELP
    HID_KEYBOARD_MENU, // HID_KEYBOARD_MENU
    HID_KEYBOARD_SELECT, // HID_KEYBOARD_SELECT
    HID_KEYBOARD_STOP, // HID_KEYBOARD_STOP
    HID_KEYBOARD_AGAIN, // HID_KEYBOARD_AGAIN
    HID_KEYBOARD_NONUS_BACKSLASH, // HID_KEYBOARD_NONUS_BACKSLASH
    HID_KEYBOARD_APPLICATION, // HID_KEYBOARD_APPLICATION
    HID_KEYBOARD_POWER, // HID_KEYBOARD_POWER
    HID_KEYBOARD_EXECUTE, // HID_KEYBOARD_EXECUTE
    HID_KEYBOARD_LOCK_CAPS_LOCK, // HID_KEYBOARD_LOCK_CAPS_LOCK
    HID_KEYBOARD_LOCK_NUM_LOCK, // HID_KEYBOARD_LOCK_NUM_LOCK
    HID_KEYBOARD_LOCK_SCROLL_LOCK, // HID_KEYBOARD_LOCK_SCROLL_LOCK
    HID_KEYBOARD_ALTERNATE_ERASE, // HID_KEYBOARD_ALTERNATE_ERASE
    HID_KEYBOARD_SYSREQ, // HID_KEYBOARD_SYSREQ
    HID_KEYBOARD_CANCEL, // HID_KEYBOARD_CANCEL
    HID_KEYBOARD_CLEAR, // HID_KEYBOARD_CLEAR
    HID_KEYBOARD_PRIOR, // HID_KEYBOARD_PRIOR
    HID_KEYBOARD_RETURN_1, // HID_KEYBOARD_RETURN_1
    HID_KEYBOARD_SEPARATOR, // HID_KEYBOARD_SEPARATOR
    HID_KEYBOARD_OUT, // HID_KEYBOARD_OUT
    HID_KEYBOARD_OPER, // HID_KEYBOARD_OPER
    HID_KEYBOARD_CLEAR_AGAIN, // HID_KEYBOARD_CLEAR_AGAIN
    HID_KEYBOARD_CRSEL_PROPS, // HID_KEYBOARD_CRSEL_PROPS
    HID_KEYBOARD_EXSEL, // HID_KEYBOARD_EXSEL

    HID_KEYPAD_SLASH, // HID_KEYPAD_SLASH
    HID_KEYPAD_ASTERISK, // HID_KEYPAD_ASTERISK
    HID_KEYPAD_MINUS, // HID_KEYPAD_MINUS
    HID_KEYPAD_PLUS, // HID_KEYPAD_PLUS
    HID_KEYPAD_ENTER, // HID_KEYPAD_ENTER
    HID_KEYPAD_1, // HID_KEYPAD_1
    HID_KEYPAD_2, // HID_KEYPAD_2
    HID_KEYPAD_3, // HID_KEYPAD_3
    HID_KEYPAD_4, // HID_KEYPAD_4
    HID_KEYPAD_5, // HID_KEYPAD_5
    HID_KEYPAD_6, // HID_KEYPAD_6
    HID_KEYPAD_7, // HID_KEYPAD_7
    HID_KEYPAD_8, // HID_KEYPAD_8
    HID_KEYPAD_9, // HID_KEYPAD_9
    HID_KEYPAD_0, // HID_KEYPAD_0
    HID_KEYPAD_DOT, // HID_KEYPAD_DOT
    HID_KEYPAD_EQUAL, // HID_KEYPAD_EQUAL
    HID_KEYPAD_00, // HID_KEYPAD_00
    HID_KEYPAD_000, // HID_KEYPAD_000
    HID_KEYPAD_OPEN_PARENTHESIS, // HID_KEYPAD_OPEN_PARENTHESIS
    HID_KEYPAD_CLOSE_PARENTHESIS, // HID_KEYPAD_CLOSE_PARENTHESIS
    HID_KEYPAD_OPEN_BRACE, // HID_KEYPAD_OPEN_BRACE
    HID_KEYPAD_CLOSE_BRACE, // HID_KEYPAD_CLOSE_BRACE
    HID_KEYPAD_COMMA, // HID_KEYPAD_COMMA
    HID_KEYPAD_EQUAL_SIGN, // HID_KEYPAD_EQUAL_SIGN
    HID_KEYPAD_TAB, // HID_KEYPAD_TAB
    HID_KEYPAD_BACKSPACE, // HID_KEYPAD_BACKSPACE
    HID_KEYPAD_A, // HID_KEYPAD_A
    HID_KEYPAD_B, // HID_KEYPAD_B
    HID_KEYPAD_C, // HID_KEYPAD_C
    HID_KEYPAD_D, // HID_KEYPAD_D
    HID_KEYPAD_E, // HID_KEYPAD_E
    HID_KEYPAD_F, // HID_KEYPAD_F
    HID_KEYPAD_XOR, // HID_KEYPAD_XOR
    HID_KEYPAD_CARET, // HID_KEYPAD_CARET
    HID_KEYPAD_PERCENT, // HID_KEYPAD_PERCENT
    HID_KEYPAD_LESS_THEN, // HID_KEYPAD_LESS_THEN
    HID_KEYPAD_GREATER_THEN, // HID_KEYPAD_GREATER_THEN
    HID_KEYPAD_AMPERSAND, // HID_KEYPAD_AMPERSAND
    HID_KEYPAD_DOUBLE_AMPERSAND, // HID_KEYPAD_DOUBLE_AMPERSAND
    HID_KEYPAD_PIPE, // HID_KEYPAD_PIPE
    HID_KEYPAD_DOUBLE_PIPE, // HID_KEYPAD_DOUBLE_PIPE
    HID_KEYPAD_COLON, // HID_KEYPAD_COLON
    HID_KEYPAD_HASH, // HID_KEYPAD_HASH
    HID_KEYPAD_SPACE, // HID_KEYPAD_SPACE
    HID_KEYPAD_AT, // HID_KEYPAD_AT
    HID_KEYPAD_BANG, // HID_KEYPAD_BANG
    HID_KEYPAD_MEM_STORE, // HID_KEYPAD_MEM_STORE
    HID_KEYPAD_MEM_RECALL, // HID_KEYPAD_MEM_RECALL
    HID_KEYPAD_MEM_CLEAR, // HID_KEYPAD_MEM_CLEAR
    HID_KEYPAD_MEM_ADD, // HID_KEYPAD_MEM_ADD
    HID_KEYPAD_MEM_SUBTRACT, // HID_KEYPAD_MEM_SUBTRACT
    HID_KEYPAD_MEM_MULTIPLY, // HID_KEYPAD_MEM_MULTIPLY
    HID_KEYPAD_MEM_DIVIDE, // HID_KEYPAD_MEM_DIVIDE
    HID_KEYPAD_PLUS_MINUS, // HID_KEYPAD_PLUS_MINUS
    HID_KEYPAD_CLEAR, // HID_KEYPAD_CLEAR
    HID_KEYPAD_CLEAR_ENTRY, // HID_KEYPAD_CLEAR_ENTRY
    HID_KEYPAD_BINARY, // HID_KEYPAD_BINARY
    HID_KEYPAD_OCTAL, // HID_KEYPAD_OCTAL
    HID_KEYPAD_DECIMAL, // HID_KEYPAD_DECIMAL
    HID_KEYPAD_HEXADECIMAL, // HID_KEYPAD_HEXADECIMAL

    HID_KEYBOARD_A | KEY_MOD_LEFT_SHIFT, // A
    HID_KEYBOARD_B | KEY_MOD_LEFT_SHIFT, // B
    HID_KEYBOARD_C | KEY_MOD_LEFT_SHIFT, // C
    HID_KEYBOARD_D | KEY_MOD_LEFT_SHIFT, // D
    HID_KEYBOARD_E | KEY_MOD_LEFT_SHIFT, // E
    HID_KEYBOARD_F | KEY_MOD_LEFT_SHIFT, // F
    HID_KEYBOARD_G | KEY_MOD_LEFT_SHIFT, // G
    HID_KEYBOARD_H | KEY_MOD_LEFT_SHIFT, // H
    HID_KEYBOARD_I | KEY_MOD_LEFT_SHIFT, // I
    HID_KEYBOARD_J | KEY_MOD_LEFT_SHIFT, // J
    HID_KEYBOARD_K | KEY_MOD_LEFT_SHIFT, // K
    HID_KEYBOARD_L | KEY_MOD_LEFT_SHIFT, // L
    HID_KEYBOARD_M | KEY_MOD_LEFT_SHIFT, // M
    HID_KEYBOARD_N | KEY_MOD_LEFT_SHIFT, // N
    HID_KEYBOARD_O | KEY_MOD_LEFT_SHIFT, // O
    HID_KEYBOARD_P | KEY_MOD_LEFT_SHIFT, // P
    HID_KEYBOARD_Q | KEY_MOD_LEFT_SHIFT, // Q
    HID_KEYBOARD_R | KEY_MOD_LEFT_SHIFT, // R
    HID_KEYBOARD_S | KEY_MOD_LEFT_SHIFT, // S
    HID_KEYBOARD_T | KEY_MOD_LEFT_SHIFT, // T
    HID_KEYBOARD_U | KEY_MOD_LEFT_SHIFT, // U
    HID_KEYBOARD_V | KEY_MOD_LEFT_SHIFT, // V
    HID_KEYBOARD_W | KEY_MOD_LEFT_SHIFT, // W
    HID_KEYBOARD_X | KEY_MOD_LEFT_SHIFT, // X
    HID_KEYBOARD_Y | KEY_MOD_LEFT_SHIFT, // Y
    HID_KEYBOARD_Z | KEY_MOD_LEFT_SHIFT, // Z

    HID_KEYBOARD_F13, // HID_KEYBOARD_F13
    HID_KEYBOARD_F14, // HID_KEYBOARD_F14
    HID_KEYBOARD_F15, // HID_KEYBOARD_F15
    HID_KEYBOARD_F16, // HID_KEYBOARD_F16
    HID_KEYBOARD_F17, // HID_KEYBOARD_F17
    HID_KEYBOARD_F18, // HID_KEYBOARD_F18
    HID_KEYBOARD_F19, // HID_KEYBOARD_F19
    HID_KEYBOARD_F20, // HID_KEYBOARD_F20
    HID_KEYBOARD_F21, // HID_KEYBOARD_F21
    HID_KEYBOARD_F22, // HID_KEYBOARD_F22
    HID_KEYBOARD_F23, // HID_KEYBOARD_F23
    HID_KEYBOARD_F24, // HID_KEYBOARD_F24

    HID_KEYBOARD_INTERNATIONAL_1, // HID_KEYBOARD_INTERNATIONAL_1
    HID_KEYBOARD_INTERNATIONAL_2, // HID_KEYBOARD_INTERNATIONAL_2
    HID_KEYBOARD_INTERNATIONAL_3, // HID_KEYBOARD_INTERNATIONAL_3
    HID_KEYBOARD_INTERNATIONAL_4, // HID_KEYBOARD_INTERNATIONAL_4
    HID_KEYBOARD_INTERNATIONAL_5, // HID_KEYBOARD_INTERNATIONAL_5
    HID_KEYBOARD_INTERNATIONAL_6, // HID_KEYBOARD_INTERNATIONAL_6
    HID_KEYBOARD_INTERNATIONAL_7, // HID_KEYBOARD_INTERNATIONAL_7
    HID_KEYBOARD_INTERNATIONAL_8, // HID_KEYBOARD_INTERNATIONAL_8
    HID_KEYBOARD_INTERNATIONAL_9, // HID_KEYBOARD_INTERNATIONAL_9
    HID_KEYBOARD_LANG_1, // HID_KEYBOARD_LANG_1
    HID_KEYBOARD_LANG_2, // HID_KEYBOARD_LANG_2
    HID_KEYBOARD_LANG_3, // HID_KEYBOARD_LANG_3
    HID_KEYBOARD_LANG_4, // HID_KEYBOARD_LANG_4
    // HID_KEYBOARD_LANG_5, // HID_KEYBOARD_LANG_5
    // HID_KEYBOARD_LANG_6, // HID_KEYBOARD_LANG_6
    // HID_KEYBOARD_LANG_7, // HID_KEYBOARD_LANG_7
    // HID_KEYBOARD_LANG_8, // HID_KEYBOARD_LANG_8
    // HID_KEYBOARD_LANG_9, // HID_KEYBOARD_LANG_9
};

/**
 * @brief    The HID values of the keystrokes.
 * @details  The index of the keystroke in this array is the same as
 *  the index of the keystroke in the keystroke_names array.
*/
char* keystroke_names[] = {
    "None",
    "Delay",

    "L-CTRL",
    "R-CTRL",
    "L-SHIFT",
    "R-SHIFT",
    "L-ALT",
    "R-ALT",
    "L-WIN",
    "R-WIN",

    "a",
    "b",
    "c",
    "d",
    "e",
    "f",
    "g",
    "h",
    "i",
    "j",
    "k",
    "l",
    "m",
    "n",
    "o",
    "p",
    "q",
    "r",
    "s",
    "t",
    "u",
    "v",
    "w",
    "x",
    "y",
    "z",

    "F1",
    "F2",
    "F3",
    "F4",
    "F5",
    "F6",
    "F7",
    "F8",
    "F9",
    "F10",
    "F11",
    "F12",

    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "0",

    "DELETE",
    "BS", // "BACKSPACE",
    "TAB",
    "ENTER", //RETURN/ENTER",
    "SPACE",
    "ESC",
    "PrtScr", //"PRINT SCREEN",
    "CapsLock",
    "NumLock",
    "ScrLock", //"SCROLL LOCK",
    "PAUSE",
    "INSERT",
    "HOME",
    "PgUp", //"PAGE UP",
    "Del", // "DELETE FWD",
    "END",
    "PgDn", // "PAGE_DOWN",
    "Right", //Right Arrow",
    "Left", //"Left Arrow",
    "Down", //"Down Arrow",
    "Up", // "Up Arrow",

    "[",
    "]",
    "{",
    "}",
    "\\",
    "|",
    ";",
    ":",
    "'",
    "\"",
    ",",
    "<",
    ">",
    ".",
    "/",
    "?",

    "`",
    "~",
    "!",
    "@",
    "#",
    "$",
    "%",
    "^",
    "&",
    "*",
    "(",
    ")",
    "-",
    "_",
    "=",
    "+",

    "CUT",
    "COPY",
    "PASTE",
    "UNDO",
    "FIND",
    "MUTE",
    "VolUp",
    "VolDown",
    "HELP",
    "MENU",
    "SELECT",
    "STOP",
    "AGAIN",
    "NonUs\\",
    "Applic", //"APPLICATION",
    "POWER",
    "Execute",
    "CapsLock", //"LOCK CAPS",
    "NumLock", // "LOCK NUM",
    "ScrlLock", //"LOCK SCROLL",
    "AltErase",
    "SYSREQ",
    "CANCEL",
    "CLEAR",
    "PRIOR",
    "Return1",
    "SEPARATOR",
    "OUT",
    "OPER",
    "CLEAR_AGAIN",
    "CRSEL_PROPS",
    "EXSEL",

    "Pad /",
    "Pad *",
    "Pad -",
    "Pad +",
    "Pad ENTER",
    "Pad 1",
    "Pad 2",
    "Pad 3",
    "Pad 4",
    "Pad 5",
    "Pad 6",
    "Pad 7",
    "Pad 8",
    "Pad 9",
    "Pad 0",
    "Pad .",
    "Pad Eql",
    "Pad 00",
    "Pad 000",
    "Pad (",
    "Pad )",
    "Pad {",
    "Pad }",
    "Pad ,",
    "Pad =",
    "Pad TAB",
    "Pad BS",
    "Pad A",
    "Pad B",
    "Pad C",
    "Pad D",
    "Pad E",
    "Pad F",
    "Pad XOR",
    "Pad ^",
    "Pad %",
    "Pad <",
    "Pad >",
    "Pad &",
    "Pad &&",
    "Pad |",
    "Pad ||",
    "Pad :",
    "Pad #",
    "Pad SPACE",
    "Pad @",
    "Pad !",
    "MemStore", // "Pad MEM STORE",
    "MemRecall", //"Pad MEM RECALL",
    "MemClear", // "Pad MEM CLEAR",
    "Mem +",
    "Mem -",
    "Mem *",
    "Mem /",
    "Pad +-",
    "Pad CLR",
    "Pad CLR ENTRY",
    "Pad BIN",
    "Pad OCT",
    "Pad DEC",
    "Pad HEX",

    "A-Upper",
    "B-Upper",
    "C-Upper",
    "D-Upper",
    "E-Upper",
    "F-Upper",
    "G-Upper",
    "H-Upper",
    "I-Upper",
    "J-Upper",
    "K-Upper",
    "L-Upper",
    "M-Upper",
    "N-Upper",
    "O-Upper",
    "P-Upper",
    "Q-Upper",
    "R-Upper",
    "S-Upper",
    "T-Upper",
    "U-Upper",
    "V-Upper",
    "W-Upper",
    "X-Upper",
    "Y-Upper",
    "Z-Upper",

    "F13",
    "F14",
    "F15",
    "F16",
    "F17",
    "F18",
    "F19",
    "F20",
    "F21",
    "F22",
    "F23",
    "F24",

    "INTL 1",
    "INTL 2",
    "INTL 3",
    "INTL 4",
    "INTL 5",
    "INTL 6",
    "INTL 7",
    "INTL 8",
    "INTL 9",
    "LANG 1",
    "LANG 2",
    "LANG 3",
    "LANG 4",
    // "LANG 5",
    // "LANG 6",
    // "LANG 7",
    // "LANG 8",
    // "LANG 9",
};

/**
 * @brief    The number of times to repeat the keystroke.
 * @details  This array should start at value 0, and increment by 1s. 
*/
char* keystroke_count_names[] = {
    "0",   "1",   "2",   "3",   "4",   "5",   "6",   "7",   "8",   "9",   "10",  "11",  "12",
    "13",  "14",  "15",  "16",  "17",  "18",  "19",  "20",  "21",  "22",  "23",  "24",  "25",
    "26",  "27",  "28",  "29",  "30",  "31",  "32",  "33",  "34",  "35",  "36",  "37",  "38",
    "39",  "40",  "41",  "42",  "43",  "44",  "45",  "46",  "47",  "48",  "49",  "50",  "51",
    "52",  "53",  "54",  "55",  "56",  "57",  "58",  "59",  "60",  "61",  "62",  "63",  "64",
    "65",  "66",  "67",  "68",  "69",  "70",  "71",  "72",  "73",  "74",  "75",  "76",  "77",
    "78",  "79",  "80",  "81",  "82",  "83",  "84",  "85",  "86",  "87",  "88",  "89",  "90",
    "91",  "92",  "93",  "94",  "95",  "96",  "97",  "98",  "99",  "100", "101", "102", "103",
    "104", "105", "106", "107", "108", "109", "110", "111", "112", "113", "114", "115", "116",
    "117", "118", "119", "120", "121", "122", "123", "124", "125", "126", "127", "128", "129",
    "130", "131", "132", "133", "134", "135", "136", "137", "138", "139", "140", "141", "142",
    "143", "144", "145", "146", "147", "148", "149", "150", "151", "152", "153", "154", "155",
    "156", "157", "158", "159", "160", "161", "162", "163", "164", "165", "166", "167", "168",
    "169", "170", "171", "172", "173", "174", "175", "176", "177", "178", "179", "180", "181",
    "182", "183", "184", "185", "186", "187", "188", "189", "190", "191", "192", "193", "194",
    "195", "196", "197", "198", "199", "200", "201", "202", "203", "204", "205", "206", "207",
    "208", "209", "210", "211", "212", "213", "214", "215", "216", "217", "218", "219", "220",
    "221", "222", "223", "224", "225", "226", "227", "228", "229", "230", "231", "232", "233",
    "234", "235", "236", "237", "238", "239", "240", "241", "242", "243", "244", "245", "246",
    "247", "248", "249", "250"};
