#include <furi_hal.h>
#include <lib/toolbox/strint.h>
#include "ducky_script.h"
#include "ducky_script_i.h"

typedef int32_t (*DuckyCmdCallback)(BadUsbScript* bad_usb, const char* line, int32_t param);

typedef struct {
    char* name;
    DuckyCmdCallback callback;
    int32_t param;
} DuckyCmd;

static int32_t ducky_fnc_delay(BadUsbScript* bad_usb, const char* line, int32_t param) {
    UNUSED(param);

    line = &line[ducky_get_command_len(line) + 1];
    uint32_t delay_val = 0;
    bool state = ducky_get_number(line, &delay_val);
    if((state) && (delay_val > 0)) {
        return (int32_t)delay_val;
    }

    return ducky_error(bad_usb, "Invalid number %s", line);
}

static int32_t ducky_fnc_defdelay(BadUsbScript* bad_usb, const char* line, int32_t param) {
    UNUSED(param);

    line = &line[ducky_get_command_len(line) + 1];
    bool state = ducky_get_number(line, &bad_usb->defdelay);
    if(!state) {
        return ducky_error(bad_usb, "Invalid number %s", line);
    }
    return 0;
}

static int32_t ducky_fnc_strdelay(BadUsbScript* bad_usb, const char* line, int32_t param) {
    UNUSED(param);

    line = &line[ducky_get_command_len(line) + 1];
    bool state = ducky_get_number(line, &bad_usb->stringdelay);
    if(!state) {
        return ducky_error(bad_usb, "Invalid number %s", line);
    }
    return 0;
}

static int32_t ducky_fnc_defstrdelay(BadUsbScript* bad_usb, const char* line, int32_t param) {
    UNUSED(param);

    line = &line[ducky_get_command_len(line) + 1];
    bool state = ducky_get_number(line, &bad_usb->defstringdelay);
    if(!state) {
        return ducky_error(bad_usb, "Invalid number %s", line);
    }
    return 0;
}

static int32_t ducky_fnc_define(BadUsbScript* bad_usb, const char* line, int32_t param) {
    line = &line[ducky_get_command_len(line) + 1];
    return ducky_define(bad_usb, line, param);
}

// Function to check if a character is valid in a variable name
int is_valid_var_char(char c, int is_first) {
    if (is_first) {
        return isalpha(c) || c == '_'; // First character must be a letter or underscore
    }
    return isalnum(c) || c == '_'; // Subsequent characters can be letters, digits, or underscores
}

static int32_t ducky_fnc_string(BadUsbScript* bad_usb, const char* line, int32_t param) {
    line = &line[ducky_get_command_len(line) + 1];

    char* line_ptr = (char*)line;
    bool first = true;

    while (*line_ptr != '\0') {
        // Skip leading spaces safely
        while (*line_ptr != '\0' && *line_ptr == ' ') line_ptr++;
        if (*line_ptr == '\0') break;

        // Find the next space
        char* end = strchr(line_ptr, ' ');
        size_t token_length = (end) ? (size_t)(end - line_ptr) : strlen(line_ptr);

        // Allocate token buffer dynamically
        char* token = (char*)malloc(token_length + 1);
        if (!token) break; // Handle allocation failure
        strncpy(token, line_ptr, token_length);
        token[token_length] = '\0';

        // Determine the appropriate map
        map_str_t* temp_map = &(bad_usb->constants);
        if (token[0] == '#')
            temp_map = &(bad_usb->constants_sharp);
        else if (token[0] == '$')
            temp_map = &(bad_usb->variables);

        // Handle mapping
        if (first) {
            if (ducky_map_get(*temp_map, token) == NULL)
                furi_string_set_str(bad_usb->string_print, token);
            else
                furi_string_set_str(bad_usb->string_print, ducky_map_get(*temp_map, token));
            first = false;
        } else {
            if (ducky_map_get(*temp_map, token) == NULL)
                furi_string_cat_str(bad_usb->string_print, token);
            else
                furi_string_cat_str(bad_usb->string_print, ducky_map_get(*temp_map, token));
        }

        // Free allocated memory
        free(token);

        // Add spaces if necessary
        if (end) {
            size_t space_count = 0;
            while (*end == ' ') {
                space_count++;
                end++;
            }
            for (size_t i = 0; i < space_count; ++i)
                furi_string_cat_str(bad_usb->string_print, " ");
            line_ptr = end;
        } else {
            break; // Prevents undefined behavior if end is NULL
        }
    }

    if(param == 1) {
        furi_string_cat(bad_usb->string_print, "\n");
    }

    if(bad_usb->stringdelay == 0 &&
       bad_usb->defstringdelay == 0) { // stringdelay not set - run command immediately
        bool state = ducky_string(bad_usb, furi_string_get_cstr(bad_usb->string_print));
        if(!state) {
            return ducky_error(bad_usb, "Invalid string %s", line);
        }
    } else { // stringdelay is set - run command in thread to keep handling external events
        return SCRIPT_STATE_STRING_START;
    }

    return 0;
}

static int32_t ducky_fnc_repeat(BadUsbScript* bad_usb, const char* line, int32_t param) {
    UNUSED(param);

    line = &line[ducky_get_command_len(line) + 1];
    bool state = ducky_get_number(line, &bad_usb->repeat_cnt);
    if((!state) || (bad_usb->repeat_cnt == 0)) {
        return ducky_error(bad_usb, "Invalid number %s", line);
    }
    return 0;
}

static int32_t ducky_fnc_sysrq(BadUsbScript* bad_usb, const char* line, int32_t param) {
    UNUSED(param);

    line = &line[ducky_get_command_len(line) + 1];
    uint16_t key = ducky_get_keycode(bad_usb, line, false);
    bad_usb->hid->kb_press(bad_usb->hid_inst, KEY_MOD_LEFT_ALT | HID_KEYBOARD_PRINT_SCREEN);
    bad_usb->hid->kb_press(bad_usb->hid_inst, key);
    bad_usb->hid->release_all(bad_usb->hid_inst);
    return 0;
}

static int32_t ducky_fnc_altchar(BadUsbScript* bad_usb, const char* line, int32_t param) {
    UNUSED(param);

    line = &line[ducky_get_command_len(line) + 1];
    ducky_numlock_on(bad_usb);
    bool state = ducky_altchar(bad_usb, line);
    if(!state) {
        return ducky_error(bad_usb, "Invalid altchar %s", line);
    }
    return 0;
}

static int32_t ducky_fnc_altstring(BadUsbScript* bad_usb, const char* line, int32_t param) {
    UNUSED(param);

    line = &line[ducky_get_command_len(line) + 1];
    ducky_numlock_on(bad_usb);
    bool state = ducky_altstring(bad_usb, line);
    if(!state) {
        return ducky_error(bad_usb, "Invalid altstring %s", line);
    }
    return 0;
}

static int32_t ducky_fnc_hold(BadUsbScript* bad_usb, const char* line, int32_t param) {
    UNUSED(param);
    line = &line[ducky_get_command_len(line) + 1];

    if(bad_usb->key_hold_nb > (HID_KB_MAX_KEYS - 1)) {
        return ducky_error(bad_usb, "Too many keys are held");
    }

    // Handle Mouse Keys here
    uint16_t key = ducky_get_mouse_keycode_by_name(line);
    if(key != HID_MOUSE_NONE) {
        bad_usb->key_hold_nb++;
        bad_usb->hid->mouse_press(bad_usb->hid_inst, key);
        return 0;
    }

    // Handle Keyboard keys here
    key = ducky_get_keycode(bad_usb, line, true);
    if(key != HID_KEYBOARD_NONE) {
        bad_usb->key_hold_nb++;
        bad_usb->hid->kb_press(bad_usb->hid_inst, key);
        return 0;
    }

    // keyboard and mouse were none
    return ducky_error(bad_usb, "Unknown keycode for %s", line);
}

static int32_t ducky_fnc_release(BadUsbScript* bad_usb, const char* line, int32_t param) {
    UNUSED(param);
    line = &line[ducky_get_command_len(line) + 1];

    if(bad_usb->key_hold_nb == 0) {
        return ducky_error(bad_usb, "No keys are held");
    }

    // Handle Mouse Keys here
    uint16_t key = ducky_get_mouse_keycode_by_name(line);
    if(key != HID_MOUSE_NONE) {
        bad_usb->key_hold_nb--;
        bad_usb->hid->mouse_release(bad_usb->hid_inst, key);
        return 0;
    }

    //Handle Keyboard Keys here
    key = ducky_get_keycode(bad_usb, line, true);
    if(key != HID_KEYBOARD_NONE) {
        bad_usb->key_hold_nb--;
        bad_usb->hid->kb_release(bad_usb->hid_inst, key);
        return 0;
    }

    // keyboard and mouse were none
    return ducky_error(bad_usb, "No keycode defined for %s", line);
}

static int32_t ducky_fnc_media(BadUsbScript* bad_usb, const char* line, int32_t param) {
    UNUSED(param);

    line = &line[ducky_get_command_len(line) + 1];
    uint16_t key = ducky_get_media_keycode_by_name(line);
    if(key == HID_CONSUMER_UNASSIGNED) {
        return ducky_error(bad_usb, "No keycode defined for %s", line);
    }
    bad_usb->hid->consumer_press(bad_usb->hid_inst, key);
    bad_usb->hid->consumer_release(bad_usb->hid_inst, key);
    return 0;
}

static int32_t ducky_fnc_globe(BadUsbScript* bad_usb, const char* line, int32_t param) {
    UNUSED(param);

    line = &line[ducky_get_command_len(line) + 1];
    uint16_t key = ducky_get_keycode(bad_usb, line, false);
    if(key == HID_KEYBOARD_NONE) {
        return ducky_error(bad_usb, "No keycode defined for %s", line);
    }

    bad_usb->hid->consumer_press(bad_usb->hid_inst, HID_CONSUMER_FN_GLOBE);
    bad_usb->hid->kb_press(bad_usb->hid_inst, key);
    bad_usb->hid->kb_release(bad_usb->hid_inst, key);
    bad_usb->hid->consumer_release(bad_usb->hid_inst, HID_CONSUMER_FN_GLOBE);
    return 0;
}

static int32_t ducky_fnc_waitforbutton(BadUsbScript* bad_usb, const char* line, int32_t param) {
    UNUSED(param);
    UNUSED(bad_usb);
    UNUSED(line);

    return SCRIPT_STATE_WAIT_FOR_BTN;
}

static int32_t ducky_fnc_mouse_scroll(BadUsbScript* bad_usb, const char* line, int32_t param) {
    UNUSED(param);

    line = &line[strcspn(line, " ") + 1];
    int32_t mouse_scroll_dist = 0;

    if(strint_to_int32(line, NULL, &mouse_scroll_dist, 10) != StrintParseNoError) {
        return ducky_error(bad_usb, "Invalid Number %s", line);
    }

    bad_usb->hid->mouse_scroll(bad_usb->hid_inst, mouse_scroll_dist);

    return 0;
}

static int32_t ducky_fnc_mouse_move(BadUsbScript* bad_usb, const char* line, int32_t param) {
    UNUSED(param);

    line = &line[strcspn(line, " ") + 1];
    int32_t mouse_move_x = 0;
    int32_t mouse_move_y = 0;

    if(strint_to_int32(line, NULL, &mouse_move_x, 10) != StrintParseNoError) {
        return ducky_error(bad_usb, "Invalid Number %s", line);
    }

    line = &line[strcspn(line, " ") + 1];

    if(strint_to_int32(line, NULL, &mouse_move_y, 10) != StrintParseNoError) {
        return ducky_error(bad_usb, "Invalid Number %s", line);
    }

    bad_usb->hid->mouse_move(bad_usb->hid_inst, mouse_move_x, mouse_move_y);

    return 0;
}

static const DuckyCmd ducky_commands[] = {
    {"REM", NULL, -1},
    {"ID", NULL, -1},
    {"DELAY", ducky_fnc_delay, -1},
    {"DEFINE", ducky_fnc_define, 1},
    {"VAR", ducky_fnc_define, 0},
    {"STRING", ducky_fnc_string, 0},
    {"STRINGLN", ducky_fnc_string, 1},
    {"DEFAULT_DELAY", ducky_fnc_defdelay, -1},
    {"DEFAULTDELAY", ducky_fnc_defdelay, -1},
    {"STRINGDELAY", ducky_fnc_strdelay, -1},
    {"STRING_DELAY", ducky_fnc_strdelay, -1},
    {"DEFAULT_STRING_DELAY", ducky_fnc_defstrdelay, -1},
    {"DEFAULTSTRINGDELAY", ducky_fnc_defstrdelay, -1},
    {"REPEAT", ducky_fnc_repeat, -1},
    {"SYSRQ", ducky_fnc_sysrq, -1},
    {"ALTCHAR", ducky_fnc_altchar, -1},
    {"ALTSTRING", ducky_fnc_altstring, -1},
    {"ALTCODE", ducky_fnc_altstring, -1},
    {"HOLD", ducky_fnc_hold, -1},
    {"RELEASE", ducky_fnc_release, -1},
    {"WAIT_FOR_BUTTON_PRESS", ducky_fnc_waitforbutton, -1},
    {"MEDIA", ducky_fnc_media, -1},
    {"GLOBE", ducky_fnc_globe, -1},
    {"MOUSEMOVE", ducky_fnc_mouse_move, -1},
    {"MOUSE_MOVE", ducky_fnc_mouse_move, -1},
    {"MOUSESCROLL", ducky_fnc_mouse_scroll, -1},
    {"MOUSE_SCROLL", ducky_fnc_mouse_scroll, -1},
};

#define TAG "BadUsb"

#define WORKER_TAG TAG "Worker"

int32_t ducky_execute_cmd(BadUsbScript* bad_usb, const char* line) {
    size_t cmd_word_len = strcspn(line, " ");
    for(size_t i = 0; i < COUNT_OF(ducky_commands); i++) {
        size_t cmd_compare_len = strlen(ducky_commands[i].name);

        if(cmd_compare_len != cmd_word_len) {
            continue;
        }

        if(strncmp(line, ducky_commands[i].name, cmd_compare_len) == 0) {
            if(ducky_commands[i].callback == NULL) {
                return 0;
            } else {
                return (ducky_commands[i].callback)(bad_usb, line, ducky_commands[i].param);
            }
        }
    }

    return SCRIPT_STATE_CMD_UNKNOWN;
}
