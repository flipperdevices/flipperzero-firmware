#include "utilities.h"
#include "calculator.h"

char getKeyAtPosition(short x, short y) {
    // Handle the first two rows
    if(y == 0) {
        return '*'; // MODE
    }

    if(y == 1) {
        return ' '; // MODE display
    }
    // Handle the keys for the remaining rows
    // Assuming the layout has 5 columns for rows 3 to 5, and 4 columns for the last row
    const char keys[4][5] = {
        {'7', '8', '9', 'A', 'B'},  // Row 3
        {'4', '5', '6', 'C', 'D'},  // Row 4
        {'1', '2', '3', 'E', 'F'},  // Row 5
        {'<', '0', '=', 'R', ' '}   // Last row, with only 4 keys (' ' denotes no key)
    };

    // Check bounds to avoid array index out of range
    if(y >= 2 && y <= 5 && x < 5) {
        if (y == 5 && x == 4) {
            // No key at the last position of the last row
            return ' ';
        } else {
            // Return the key from the array
            return keys[y - 2][x];
        }
    }

    return ' '; // Return a space for any undefined keys
}


short calculateStringWidth(const char* str, short length) {
    short width = 0;
    for (short i = 0; i < length; i++) {
        switch (str[i]) {
            case '1':
            case '2':
            case '3':
            case '4':width += 4;break;
            case '5':width += 4;break;
            case '6':width += 4;break;
            case '7':width += 4;break;
            case '8':width += 4;break;
            case '9':width += 4;break;
            case '0':width += 4;break;
            case '<':
            case '=':
            case 'R':
            // case 'H':
            case '*':
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
            default:
                width += 3;
                break;
        }
        width += 1;
    }

    return width;
}