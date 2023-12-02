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

uint16_t ducky_get_keycode(BadKbScript* bad_kb, const char* param, bool accept_chars);

uint32_t ducky_get_command_len(const char* line);

bool ducky_is_line_end(const char chr);

uint16_t ducky_get_keycode_by_name(const char* param);

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
