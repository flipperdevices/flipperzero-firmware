#include "common.h"
#include <furi/furi.h>
#include <furi_hal.h>
#include "../../services/convert/convert.h"

static const uint8_t hid_number_keys[10] = {
    HID_KEYBOARD_0,
    HID_KEYBOARD_1,
    HID_KEYBOARD_2,
    HID_KEYBOARD_3,
    HID_KEYBOARD_4,
    HID_KEYBOARD_5,
    HID_KEYBOARD_6,
    HID_KEYBOARD_7,
    HID_KEYBOARD_8,
    HID_KEYBOARD_9};

void totp_type_code_worker_execute_automation(
    TOTP_AUTOMATION_PRESS_KEY key_press_fn,
    const char* string,
    uint8_t string_length,
    TokenAutomationFeature features) {
    furi_delay_ms(500);
    uint8_t i = 0;
    while(i < string_length && string[i] != 0) {
        uint8_t digit = CONVERT_CHAR_TO_DIGIT(string[i]);
        if(digit > 9) break;
        uint8_t hid_kb_key = hid_number_keys[digit];
        (*key_press_fn)(hid_kb_key);
        i++;
    }

    if(features & TOKEN_AUTOMATION_FEATURE_ENTER_AT_THE_END) {
        furi_delay_ms(30);
        (*key_press_fn)(HID_KEYBOARD_RETURN);
    }

    if(features & TOKEN_AUTOMATION_FEATURE_TAB_AT_THE_END) {
        furi_delay_ms(30);
        (*key_press_fn)(HID_KEYBOARD_TAB);
    }
}