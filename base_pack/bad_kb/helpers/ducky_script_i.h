#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <furi.h>
#include <furi_hal.h>
#include "ducky_script.h"

#define SCRIPT_STATE_ERROR (-1)
#define SCRIPT_STATE_END (-2)
#define SCRIPT_STATE_NEXT_LINE (-3)
#define SCRIPT_STATE_CMD_UNKNOWN (-4)
#define SCRIPT_STATE_STRING_START (-5)
#define SCRIPT_STATE_WAIT_FOR_BTN (-6)

#define FILE_BUFFER_LEN 16

struct BadKbScript {
    FuriThread* thread;
    BadKbState st;

    FuriString* file_path;
    FuriString* keyboard_layout;
    uint8_t file_buf[FILE_BUFFER_LEN + 1];
    uint8_t buf_start;
    uint8_t buf_len;
    bool file_end;

    uint32_t defdelay;
    uint32_t stringdelay;
    uint32_t defstringdelay;
    uint16_t layout[128];

    FuriString* line;
    FuriString* line_prev;
    uint32_t repeat_cnt;
    uint8_t key_hold_nb;

    FuriString* string_print;
    size_t string_print_pos;

    Bt* bt;
    BadKbApp* app;
};

uint16_t ducky_get_keycode(BadKbScript* bad_kb, const char* param, bool accept_chars);

uint32_t ducky_get_command_len(const char* line);

bool ducky_is_line_end(const char chr);

uint16_t ducky_get_keycode_by_name(const char* param);

uint16_t ducky_get_media_keycode_by_name(const char* param);

bool ducky_get_number(const char* param, uint32_t* val);

void ducky_numlock_on(BadKbScript* bad_kb);

bool ducky_numpad_press(BadKbScript* bad_kb, const char num);

bool ducky_altchar(BadKbScript* bad_kb, const char* charcode);

bool ducky_altstring(BadKbScript* bad_kb, const char* param);

bool ducky_string(BadKbScript* bad_kb, const char* param);

int32_t ducky_execute_cmd(BadKbScript* bad_kb, const char* line);

int32_t ducky_error(BadKbScript* bad_kb, const char* text, ...);

#ifdef __cplusplus
}
#endif
