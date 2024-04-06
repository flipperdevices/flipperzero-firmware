#include "bit_ops.h"

// https://stackoverflow.com/questions/746171/efficient-algorithm-for-bit-reversal-from-msb-lsb-to-lsb-msb-in-c/746382#746382
int bit_reversal(uint32_t input) {
    int s = sizeof(input) * 2;
    int i, x, y, p;
    int rtn = 0;

    for(i = 0; i < (s / 2); i++) {
        // extract bit on the left, from MSB
        p = s - i - 1;
        x = input & (1 << p);
        x = x >> p;

        // extract bit on the right, from LSB
        y = input & (1 << i);
        y = y >> i;

        rtn = rtn | (x << i);
        rtn = rtn | (y << p);
    }
    return rtn;
}