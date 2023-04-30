#include "flipper.h"
#include "resistors_app.h"
#include "resistor_logic.h"
#include <math.h>

// Xx_Xx_Xx_Xx_-_Xx\0 = 17 characters
const char blank_descriptor_R4[14] = "             ";
const char blank_descriptor_R5[17] = "                ";

const int RESISTOR_NUMERIC_CHARS = 3;
const int RESISTOR_MULTIPLIER_UNIT_CHARS = 7;
const int RESISTOR_TOLERANCE_CHARS = 7;

const int RESISTOR_NUMERIC_POSITION = 0;
const int RESISTOR_MULTIPLIER_UNIT_POSITION = 4;
const int RESISTOR_TOLERANCE_POSITION = 14;

//                                 "nnn x 10^nn @ nnnn%";
const char blank_calculation[24] = "                   ";

bool is_numeric_band(ResistorType rtype, int index) {
    if(rtype < 5) {
        return index <= 2;
    } else {
        return index <= 3;
    }
}

bool is_multiplier_band(ResistorType rtype, int index) {
    switch(rtype) {
    case R3:
        return index == 2;
    case R4:
        return index == 2;
    case R5:
        return index == 3;
    case R6:
        return index == 3;
    default:
        return false;
    }
}

bool is_tolerance_band(ResistorType rtype, int index) {
    switch(rtype) {
    case R3:
        return false;
    case R4:
        return index == 3;
    case R5:
        return index == 4;
    case R6:
        return index == 4;
    default:
        return false;
    }
}

bool is_temp_coefficient_band(ResistorType rtype, int index) {
    return rtype == R6 && index == 5;
}

bool is_numeric_colour(BandColour colour) {
    return colour <= 9;
}

bool is_multiplier_colour() {
    return true;
}

bool is_tolerance_colour(BandColour colour) {
    return colour == BandBrown || colour == BandRed || colour == BandGreen || colour == BandBlue ||
           colour == BandPurple || colour == BandGray || colour == BandGold ||
           colour == BandSilver;
}

BandColour
    alter_resistor_band(ResistorType rtype, int band, BandColour current_colour, int direction) {
    int colour = current_colour;
    bool accepted = false;
    while(!accepted) {
        colour += direction;
        if(colour > 11) colour = 0;
        if(colour < 0) colour = 12;
        if(is_numeric_band(rtype, band) && is_numeric_colour(colour)) accepted = true;
        if(is_tolerance_band(rtype, band) && is_tolerance_colour(colour)) accepted = true;
        if(is_multiplier_band(rtype, band) && is_multiplier_colour()) accepted = true;
    }
    return colour;
}

void update_resistance_number(ResistorType rtype, BandColour colours[], char string[], int index) {
    int bands = rtype - 2;
    int value = 0;
    for(int b = 0; b < bands; b++) {
        int pwr = bands - b - 1;
        int delta = ((int)pow(10.0, pwr)) * colours[b];
        value += delta;
    }

    int length = snprintf(NULL, 0, "%d", value);
    char* str = malloc(length + 1);
    snprintf(str, length + 1, "%d", value);

    char* target = string + index;
    strncpy(target, str, length);
    free(str);
}

void update_resistance_multiplier_unit(BandColour colour, char string[], int index) {
    char unit[] = "x 10^  ";
    if(colour > 9) {
        unit[5] = '-';
        unit[6] = (char)(48 + colour - 9);
    } else {
        unit[5] = (char)(48 + colour);
        unit[6] = ' ';
    }
    char* target = string + index;
    strncpy(target, unit, RESISTOR_MULTIPLIER_UNIT_CHARS);
}

void update_resistance_tolerance(BandColour colour, char string[], int index) {
    char* target = string + index;
    switch(colour) {
    case BandBrown:;
        char tolerance_brown[8] = "@ 1%   ";
        strncpy(target, tolerance_brown, RESISTOR_TOLERANCE_CHARS);
        break;
    case BandRed:;
        char tolerance_red[8] = "@ 2%   ";
        strncpy(target, tolerance_red, RESISTOR_TOLERANCE_CHARS);
        break;
    case BandGreen:;
        char tolerance_green[8] = "@ 0.5% ";
        strncpy(target, tolerance_green, RESISTOR_TOLERANCE_CHARS);
        break;
    case BandBlue:;
        char tolerance_blue[8] = "@ 0.25%";
        strncpy(target, tolerance_blue, RESISTOR_TOLERANCE_CHARS);
        break;
    case BandPurple:;
        char tolerance_purple[8] = "@ 0.1% ";
        strncpy(target, tolerance_purple, RESISTOR_TOLERANCE_CHARS);
        break;
    case BandGray:;
        char tolerance_gray[8] = "@ 0.05%";
        strncpy(target, tolerance_gray, RESISTOR_TOLERANCE_CHARS);
        break;
    case BandGold:;
        char tolerance_gold[8] = "@ 5%   ";
        strncpy(target, tolerance_gold, RESISTOR_TOLERANCE_CHARS);
        break;
    case BandSilver:;
        char tolerance_silver[8] = "@ 10%  ";
        strncpy(target, tolerance_silver, RESISTOR_TOLERANCE_CHARS);
        break;
    default:;
        char tolerance_nk[8] = "@ ????%";
        strncpy(target, tolerance_nk, RESISTOR_TOLERANCE_CHARS);
        break;
    }
}

void update_calculation(ResistorType rtype, BandColour bands[], char string[]) {
    strcpy(string, blank_calculation);
    update_resistance_number(rtype, bands, string, RESISTOR_NUMERIC_POSITION);
    update_resistance_multiplier_unit(bands[rtype - 2], string, RESISTOR_MULTIPLIER_UNIT_POSITION);
    update_resistance_tolerance(bands[rtype - 1], string, RESISTOR_TOLERANCE_POSITION);
}

void update_resistor_descriptor(ResistorType rtype, BandColour resistor_bands[], char descriptor[]) {
    if(rtype == R4) strcpy(descriptor, blank_descriptor_R4);
    if(rtype == R5) strcpy(descriptor, blank_descriptor_R5);

    for(int i = 0; i < rtype; i++) {
        int c = i * 3;
        bool last_band = i == rtype - 1;
        if(last_band) {
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
            descriptor[c + 1] = 'l';
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
