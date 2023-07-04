#include "eth_view_process.h"
#include "eth_worker.h"
#include "eth_worker_i.h"
#include "finik_eth_icons.h"

#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/canvas.h>
#include <string.h>

#include "u8g2.h"

#define TAG "EthView"

EthViewProcess* ethernet_view_process_malloc(EthWorkerProcess type) {
    EthViewProcess* evp = malloc(sizeof(EthViewProcess));
    evp->type = type;
    evp->autofill = 1;
    evp->carriage = 0;
    evp->position = 0;
    evp->x = 27;
    evp->y = 6;

    if(type == EthWorkerProcessInit) {
        evp->y += 22;
        evp->draw_struct = malloc(sizeof(EthViewDrawInit));
        memset(evp->draw_struct, 0, sizeof(EthViewDrawInit));
    }
    return evp;
}

void ethernet_view_process_free(EthViewProcess* evp) {
    if(evp->type == EthWorkerProcessInit) {
        free(evp->draw_struct);
    }
    free(evp);
}

static void draw_hex_digit(Canvas* canvas, uint8_t x, uint8_t y, uint8_t digit) {
    char digit_str[] = "0";
    if(digit < 0xA) {
        digit_str[0] += digit;
    } else if(digit < 0x10) {
        digit_str[0] = 'A';
        digit_str[0] += digit - 0xA;
    } else {
        return;
    }

    canvas_draw_str(canvas, x, y, digit_str);
}

void ethernet_view_process_draw(EthViewProcess* process, Canvas* canvas) {
    furi_assert(canvas);
    furi_assert(process);
    canvas_set_font(canvas, FontSecondary);

    const uint8_t x = process->x;
    const uint8_t y = process->y;
    const uint8_t str_height = 11;
    const uint8_t str_count = (64 - y) / str_height;
    uint8_t carriage = process->carriage;
    uint8_t position = process->position;

    if(process->autofill) {
        position = (carriage + SCREEN_STRINGS_COUNT - str_count) % SCREEN_STRINGS_COUNT;
        process->position = position;
    }

    for(uint8_t i = 0; i < str_count; ++i) {
        uint8_t y1 = y + (i + 1) * str_height;
        canvas_draw_str(canvas, x, y1, process->fifo[(position + i) % SCREEN_STRINGS_COUNT]);
    }

    if(process->type == EthWorkerProcessInit) {
        uint8_t editing = process->editing;
        canvas_draw_icon(canvas, 27, 10, &I_init_100x19px);
        for(uint8_t i = 0; i < 6; ++i) {
            uint8_t x1 = 29 + i * 17;
            uint8_t x2 = x1 + 6;
            uint8_t mac = ((EthViewDrawInit*)process->draw_struct)->mac[i];
            uint8_t octet = ((EthViewDrawInit*)process->draw_struct)->current_octet;
            draw_hex_digit(canvas, x1, 25, (mac & 0x0F));
            draw_hex_digit(canvas, x2, 25, (mac & 0xF0) >> 4);
            if(editing && (octet / 2 == i)) {
                uint8_t x = octet & 1 ? x2 : x1;
                canvas_draw_line(canvas, x, 26, x + 4, 26);
                canvas_draw_line(canvas, x, 27, x + 4, 27);
            }
        }
    }
}

static void mac_change_hex_digit(uint8_t* mac, uint8_t octet, int8_t diff) {
    uint8_t digit = (octet & 1) ? (mac[octet / 2] >> 4) : (mac[octet / 2]);
    digit = (digit + diff) & 0xF;
    mac[octet / 2] = (mac[octet / 2] & ((octet & 1) ? 0x0F : 0xF0)) |
                     (digit << ((octet & 1) ? 4 : 0));
}

void ethernet_view_process_keyevent(EthViewProcess* process, InputKey key) {
    furi_assert(process);
    if(process->type == EthWorkerProcessInit) {
        uint8_t octet = ((EthViewDrawInit*)process->draw_struct)->current_octet;
        uint8_t* mac = ((EthViewDrawInit*)process->draw_struct)->mac;
        if(key == InputKeyLeft) {
            if(octet > 0) octet -= 1;
        } else if(key == InputKeyRight) {
            if(octet < 12) octet += 1;
        } else if(key == InputKeyUp) {
            mac_change_hex_digit(mac, octet, 1);
        } else if(key == InputKeyDown) {
            mac_change_hex_digit(mac, octet, -1);
        } else if(key == InputKeyOk) {
            process->editing = 0;
        }
        ((EthViewDrawInit*)process->draw_struct)->current_octet = octet;
    }
}

void ethernet_view_process_move(EthViewProcess* process, int8_t shift) {
    furi_assert(process);
    uint8_t position = process->position;
    if(shift <= -SCREEN_STRINGS_COUNT) {
        position = 0;
    } else if(shift >= SCREEN_STRINGS_COUNT) {
        position = process->carriage - 1;
    } else {
        position = (position + (SCREEN_STRINGS_COUNT + shift)) % SCREEN_STRINGS_COUNT;
    }
    process->position = position;
    process->autofill = !shift;
}

void ethernet_view_process_autofill(EthViewProcess* process, uint8_t state) {
    furi_assert(process);
    process->autofill = state;
}

static uint16_t get_string_with_width(const char* str, uint16_t width) {
    u8g2_t canvas_memory;
    Canvas* canvas = (Canvas*)&canvas_memory; // grazniy hack
    canvas_set_font(canvas, FontSecondary);

    uint8_t end = 0;
    char copy[SCREEN_SYMBOLS_WIDTH + 1] = {0};

    for(;;) {
        if(str[end] == '\0') {
            break;
        }
        if(end == SCREEN_SYMBOLS_WIDTH) {
            break;
        }
        copy[end] = str[end];
        if(canvas_string_width(canvas, copy) > width) {
            end -= 1;
            break;
        }
        end += 1;
    }

    return end;
}

void ethernet_view_process_print(EthViewProcess* process, const char* str) {
    furi_assert(process);

    uint16_t max_width = 126 - process->x;
    uint16_t ptr = 0;
    uint16_t len = strlen(str);

    while(ptr < len) {
        uint16_t start = ptr;
        ptr += get_string_with_width(str + ptr, max_width);
        uint8_t carriage = process->carriage;
        uint8_t carriage1 = (carriage + 1) % SCREEN_STRINGS_COUNT;
        uint8_t carriage2 = (carriage + 2) % SCREEN_STRINGS_COUNT;
        FURI_LOG_I(TAG, "print %d %d %d %d %d", max_width, len, start, carriage, carriage1);
        memset(process->fifo[carriage], 0, SCREEN_SYMBOLS_WIDTH);
        memset(process->fifo[carriage1], 0, SCREEN_SYMBOLS_WIDTH);
        memset(process->fifo[carriage2], 0, SCREEN_SYMBOLS_WIDTH);
        memcpy(process->fifo[carriage], str + start, ptr - start);
        process->carriage = carriage1;
    }
}
