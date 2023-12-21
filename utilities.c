#include "utilities.h"
#include "calculator.h"

char getKeyAtPosition(short x, short y) {
    const char keys[6][5] = {
        {'*', ' ', ' ', ' ', ' '}, // Row 1 (MODE key)
        {' ', ' ', ' ', ' ', ' '}, // Row 2 (display row)
        {'7', '8', '9', 'A', 'B'}, // Row 3
        {'4', '5', '6', 'C', 'D'}, // Row 4
        {'1', '2', '3', 'E', 'F'}, // Row 5
        {'<', '0', '=', 'R', ' '}  // Row 6
    };

    return (y < 6 && x < 5) ? keys[y][x] : ' ';
}

short calculateStringWidth(const char* str, short length) {
    short width = 0;
    for (short i = 0; i < length; i++) {
        width += (str[i] == '*' || str[i] == '<' || str[i] == '=' || str[i] == 'R') ? 3 : 4;
        width += 1; // space between characters
    }
    return width;
}
