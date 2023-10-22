/*
Authors: Tanish Bhongade and RaZe
*/

#include <stdio.h>
#include <furi.h>
#include <gui/gui.h>
#include "furi_hal_random.h"
#include <input/input.h>
#include <gui/elements.h>
#include "scrambler.h"

// 6 moves along with direction
char moves[6] = {'R', 'U', 'F', 'B', 'L', 'D'};
char dir[4] = {'\'', '2'};
const int32_t SLEN = 20;
#define RESULT_SIZE 100

struct GetScramble {
    char mainScramble[25][3];
};
struct GetScramble a;

void scrambleReplace() {
    // Initialize the mainScramble array with all the possible moves
    for(int32_t i = 0; i < SLEN; i++) {
        a.mainScramble[i][0] = moves[furi_hal_random_get() % 6];
        a.mainScramble[i][1] = dir[furi_hal_random_get() % 3];
    }

    /* // Perform the Fisher-Yates shuffle
    for (int32_t i = 6 - 1; i > 0; i--)
    {
        int32_t j = rand() % (i + 1);
        char temp[3];
        strcpy(temp, a.mainScramble[i]);
        strcpy(a.mainScramble[i], a.mainScramble[j]);
        strcpy(a.mainScramble[j], temp);
    } */

    // Select the first 10 elements as the scramble, using only the first two elements of the dir array
    for(int32_t i = 0; i < SLEN; i++) {
        a.mainScramble[i][1] = dir[furi_hal_random_get() % 3];
    }
    for(int32_t i = 1; i < SLEN; i++) {
        while(a.mainScramble[i][0] == a.mainScramble[i - 2][0] ||
              a.mainScramble[i][0] == a.mainScramble[i - 1][0]) {
            a.mainScramble[i][0] = moves[furi_hal_random_get() % 5];
        }
    }
}

char* printData() {
    static char result[RESULT_SIZE];
    int32_t offset = 0;
    for(int32_t loop = 0; loop < SLEN; loop++) {
        offset += snprintf(result + offset, RESULT_SIZE - offset, "%s ", a.mainScramble[loop]);
    }
    return result;
}