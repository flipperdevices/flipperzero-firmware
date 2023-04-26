#include "resistor_logic.h"
#include <string.h>

// Xx_Xx_Xx_Xx_-_Xx\0 = 17 characters
const char blank_descriptor[17] = "                ";

void update_resistor_descriptor(ResistorType bands, int resistor_bands[], char descriptor[]) {
    strcpy(descriptor, blank_descriptor);
    for(int i = 0; i < bands; i++) {
        int c = i * 3;
        if(i == bands - 1) {
            descriptor[c] = '-';
            c += 2;
        }
        switch(resistor_bands[i]) {
        case BandBlack:
            descriptor[c] = 'B';
            descriptor[c + 1] = 'k';
            break;
        case BandBrown:
            descriptor[c] = 'B';
            descriptor[c + 1] = 'r';
            break;
        case BandRed:
            descriptor[c] = 'R';
            descriptor[c + 1] = 'e';
            break;
        case BandOrange:
            descriptor[c] = 'O';
            descriptor[c + 1] = 'r';
            break;
        case BandYellow:
            descriptor[c] = 'Y';
            descriptor[c + 1] = 'e';
            break;
        case BandGreen:
            descriptor[c] = 'G';
            descriptor[c + 1] = 'r';
            break;
        case BandBlue:
            descriptor[c] = 'B';
            descriptor[c + 1] = 'u';
            break;
        case BandPurple:
            descriptor[c] = 'P';
            descriptor[c + 1] = 'u';
            break;
        case BandGray:
            descriptor[c] = 'G';
            descriptor[c + 1] = 'y';
            break;
        case BandWhite:
            descriptor[c] = 'W';
            descriptor[c + 1] = 'h';
            break;
        case BandGold:
            descriptor[c] = 'G';
            descriptor[c + 1] = 'o';
            break;
        case BandSilver:
            descriptor[c] = 'S';
            descriptor[c + 1] = 'i';
            break;
        }
    } // i
}
