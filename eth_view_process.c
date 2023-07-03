#include "eth_view_process.h"
#include "eth_worker.h"
#include "eth_worker_i.h"

#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/canvas.h>
#include <string.h>

#include "u8g2.h"

#define TAG "EthView"

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
    Canvas* canvas = &canvas_memory; // grazniy hack
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
        memset(process->fifo[carriage], 0, SCREEN_SYMBOLS_WIDTH);
        memset(process->fifo[carriage1], 0, SCREEN_SYMBOLS_WIDTH);
        memset(process->fifo[carriage2], 0, SCREEN_SYMBOLS_WIDTH);
        memcpy(process->fifo[carriage], str + start, ptr - start);
        process->carriage = carriage1;
    }
}