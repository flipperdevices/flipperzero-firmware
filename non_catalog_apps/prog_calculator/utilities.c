#include "utilities.h"
#include "calculator.h"

char getKeyAtPosition(short x, short y) {
    const char keys[6][5] = {
        {'M', ' ', ' ', ' ', ' '}, // Row 1 (MODE key)
        {'7', '8', '9', 'A', 'B'}, // Row 2
        {'4', '5', '6', 'C', 'D'}, // Row 3
        {'1', '2', '3', 'E', 'F'}, // Row 4
        {'<', '0', '=', 'R', ' '}  // Row 5
    };

    return (y < 6 && x < 5) ? keys[y][x] : ' ';
}

short calculateStringWidth(const char* str, short length) {
    short width = 0;
    for(short i = 0; i < length; i++) {
        switch (str[i]) {
        case 'M':
            width += 5;
            break;
        case '0':
            width += 4;
            break;
        case '1':
            width += 3;
            break;
        case '2':
        case '3':
        case '4':
            width += 4;
            break;
        case '5':
            width += 4;
            break;
        case '6':
            width += 4;
            break;
        case '7':
            width += 4;
            break;
        case '8':
            width += 4;
            break;
        case '9':
            width += 4;
            break;
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case '<':
            width += 3;
            break;
        case '=':
            width += 4;
            break;
        case 'R':
            width += 9;
            break;
        default:
            width += 3;
            break;
        }
        width += 1;
    }
    return width;
}