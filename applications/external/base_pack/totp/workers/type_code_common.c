#include "type_code_common.h"
#include <furi_hal_usb_hid.h>
#include <furi/core/kernel.h>
#include <furi/core/log.h>
#include "../../services/convert/convert.h"
#include "../../services/kb_layouts/kb_layout_provider.h"

static uint32_t get_keystroke_delay(TokenAutomationFeature features) {
    if(features & TokenAutomationFeatureTypeSlower) {
        return 100;
    }

    return 30;
}

static uint32_t get_keypress_delay(TokenAutomationFeature features) {
    if(features & TokenAutomationFeatureTypeSlower) {
        return 60;
    }

    return 30;
}

static void totp_type_code_worker_press_key(
    uint16_t key,
    TOTP_AUTOMATION_KEY_HANDLER key_press_fn,
    TOTP_AUTOMATION_KEY_HANDLER key_release_fn,
    TokenAutomationFeature features) {
    (*key_press_fn)(key);
    furi_delay_ms(get_keypress_delay(features));
    (*key_release_fn)(key);
}

void totp_type_code_worker_execute_automation(
    TOTP_AUTOMATION_KEY_HANDLER key_press_fn,
    TOTP_AUTOMATION_KEY_HANDLER key_release_fn,
    const char* code_buffer,
    uint8_t code_buffer_size,
    TokenAutomationFeature features,
    AutomationKeyboardLayout keyboard_layout,
    uint16_t initial_delay) {
    uint16_t keyboard_layout_dict[TOTP_KB_LAYOUT_DATA_LENGTH];
    if(!totp_kb_layout_provider_get_layout_data(keyboard_layout, &keyboard_layout_dict[0])) {
        return;
    }

    furi_delay_ms(initial_delay);

    uint32_t keystroke_delay = get_keystroke_delay(features);

    char cb_char;
    uint8_t i = 0;
    while(i < code_buffer_size && (cb_char = code_buffer[i]) != 0) {
        uint8_t char_index = CONVERT_CHAR_TO_DIGIT(cb_char);
        if(char_index > 9) {
            char_index = cb_char - 'A' + 10;
        }

        if(char_index >= TOTP_KB_LAYOUT_DATA_LENGTH) break;

        uint16_t hid_kb_key = keyboard_layout_dict[char_index];
        totp_type_code_worker_press_key(hid_kb_key, key_press_fn, key_release_fn, features);
        furi_delay_ms(keystroke_delay);
        i++;
    }

    if(features & TokenAutomationFeatureEnterAtTheEnd) {
        furi_delay_ms(keystroke_delay);
        totp_type_code_worker_press_key(
            HID_KEYBOARD_RETURN, key_press_fn, key_release_fn, features);
    }

    if(features & TokenAutomationFeatureTabAtTheEnd) {
        furi_delay_ms(keystroke_delay);
        totp_type_code_worker_press_key(HID_KEYBOARD_TAB, key_press_fn, key_release_fn, features);
    }
}
