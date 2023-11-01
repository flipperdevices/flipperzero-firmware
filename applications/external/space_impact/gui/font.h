#include <furi.h>
#include <stdint.h>
#include <gui/canvas.h>
#include "renderer.h"

#define FONT_HEIGHT 5
#define FONT_WIDTH 3

static uint8_t font[10][15] = {
    {1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1}, // 0
    {0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1}, // 1
    {1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1}, // 2
    {1, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1}, // 3
    {1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1}, // 4
    {1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1}, // 5
    {1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1}, // 6
    {1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1}, // 7
    {1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1}, // 8
    {1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1}, // 9
};

static uint8_t draw_digit(uint8_t digit, uint8_t x, uint8_t y) {
    uint8_t x_shift = 0;
    uint8_t index = 0;

    for(int n = 0; n < FONT_HEIGHT; n++) {
        for(int i = 0; i < FONT_WIDTH; i++) {
            if(font[digit][index] == 1) {
                draw_point(x + i, y + n);
            }
            index++;
        }
    }
    x_shift = 3;
    return x_shift;
}

static void parse_number(uint16_t number, uint8_t* digits, uint8_t size) {
    uint8_t count = 0;
    while(number > 0) //do till num greater than  0
    {
        int mod = number % 10; //split last digit from number
        digits[size - 1 - count] = mod;
        number = number / 10; //divide num by 10. num /= 10 also a valid one
        count++;
    }

    for(int i = 0; i < size - count; i++) {
        digits[i] = 0;
    }
}

static void draw_number(uint8_t x, uint8_t y, int number, uint8_t size) {
    uint8_t digits[size];

    parse_number(number, digits, size);

    y += 0;
    for(int i = 0; i < size; ++i) {
        x += draw_digit(digits[i], x, y);
        x++;
    }
}