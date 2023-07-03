#include "eth_view_process.h"
#include "eth_worker.h"
#include "eth_worker_i.h"
#include <gui/gui.h>
#include <gui/canvas.h>
#include <string.h>
#include "u8g2.h"

void ethernet_view_process_draw(EthViewProcess* process, Canvas* canvas) {
    furi_assert(canvas);
    furi_assert(process);
    canvas_set_font(canvas, FontSecondary);

    const uint8_t x = process->x;
    const uint8_t y = process->y;
    const uint8_t str_height = 11;
    const uint8_t str_count = (64 - y) / str_height;

    int8_t position = process->position;
    uint8_t carriage = process->carriage;

    if(process->autofill) {
        if(carriage > str_count) {
            position = carriage - str_count;
        } else {
            position = 0;
        }
    }

    for(uint8_t i = 0; i < str_count; ++i) {
        canvas_draw_str(
            canvas,
            x,
            y + (i + 1) * str_height,
            process->fifo[(position + i) % SCREEN_STRINGS_COUNT]);
    }
}

void ethernet_view_process_move(EthViewProcess* process, int8_t shift) {
    furi_assert(process);
    if(shift <= -SCREEN_STRINGS_COUNT) {
        process->position = 0;
    } else if(shift >= SCREEN_STRINGS_COUNT) {
        process->position = process->carriage - 1;
    } else {
        process->position =
            (process->position + (SCREEN_STRINGS_COUNT + shift)) % SCREEN_STRINGS_COUNT;
    }

    process->autofill = !shift;
}

void ethernet_view_process_autofill(EthViewProcess* process, uint8_t state) {
    furi_assert(process);
    process->autofill = state;
}

static uint16_t get_string_with_width(const char* str, uint16_t width) {
    u8g2_t canvas_memory[3];
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
        memset(process->fifo[process->carriage % SCREEN_STRINGS_COUNT], 0, SCREEN_SYMBOLS_WIDTH);
        memcpy(process->fifo[process->carriage % SCREEN_STRINGS_COUNT], str + start, ptr - start);
        process->carriage += 1;
        if(process->carriage > SCREEN_STRINGS_COUNT * 2) {
            process->carriage -= SCREEN_STRINGS_COUNT;
        }
    }
}