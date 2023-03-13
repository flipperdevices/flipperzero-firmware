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

#define FILE_BUFFER_LEN 16

struct BadUsbScript {
    FuriHalUsbHidConfig hid_cfg;
    BadUsbState st;
    FuriString* file_path;
    uint32_t defdelay;
    uint16_t layout[128];
    uint32_t stringdelay;
    FuriThread* thread;
    uint8_t file_buf[FILE_BUFFER_LEN + 1];
    uint8_t buf_start;
    uint8_t buf_len;
    bool file_end;
    FuriString* line;

    FuriString* line_prev;
    uint32_t repeat_cnt;
    uint8_t key_hold_nb;
};

uint16_t ducky_get_keycode(BadUsbScript* bad_usb, const char* param, bool accept_chars);

uint32_t ducky_get_command_len(const char* line);

bool ducky_is_line_end(const char chr);

uint16_t ducky_get_keycode_by_name(const char* param);

bool ducky_get_number(const char* param, uint32_t* val);

void ducky_numlock_on(void);

bool ducky_numpad_press(const char num);

bool ducky_altchar(const char* charcode);

bool ducky_altstring(const char* param);

bool ducky_string(BadUsbScript* bad_usb, const char* param);

int32_t ducky_execute_cmd(BadUsbScript* bad_usb, const char* line);

int32_t ducky_error(BadUsbScript* bad_usb, const char* text, ...);

#ifdef __cplusplus
}
#endif
