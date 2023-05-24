#ifndef arha_morse
#define arha_morse

#include <stdint.h>

/* data format for each 16 bits
   byte1: dits and dahs, with 0 being a dit/short, left aligned
   byte2: length
*/

uint16_t data[] = {
    /*   lllllllldddddddd */
    0b0000001001000000,
    0b0000010010000000,
    0b0000001100000000, // S
    0b0000001111100000, // O

}

#endif
