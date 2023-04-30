#include "flipper.h"
#include "resistors_app.h"
#include "resistor_logic.h"
#include <math.h>

const int CHARS_NUMERIC = 3;
const int CHARS_MULTIPLIER = 7;
const int CHARS_TOLERANCE = 7;

const int INDEX_NUMERIC = 0;
const int INDEX_MULTIPLIER = 4;
const int INDEX_TOLERANCE = 0;

const int CALCULATION_LEN = 12;
const char BLANK_CALCULATION[] = "           ";
//                               "nnn x 10^nn";

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

bool is_multiplier_colour(BandColour colour) {
    UNUSED(colour);
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
        if(is_multiplier_band(rtype, band) && is_multiplier_colour(colour)) accepted = true;
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

char* decode_resistance_multiplier(BandColour colour) {
    static char unit[] = "x 10^  ";
    if(colour > 9) {
        unit[5] = '-';
        unit[6] = (char)(48 + colour - 9);
    } else {
        unit[5] = (char)(48 + colour);
        unit[6] = '\0';
    }
    return unit;
}

void update_resistance_multiplier(BandColour colour, char string[], int index) {
    char* unit = decode_resistance_multiplier(colour);
    char* target = string + index;
    strncpy(target, unit, CHARS_MULTIPLIER);
}

char* decode_resistance_tolerance(BandColour colour) {
    switch(colour) {
    case BandBrown:;
        return "1%";
    case BandRed:;
        return "2%";
    case BandGreen:;
        return "0.5%";
    case BandBlue:;
        return "0.25%";
    case BandPurple:;
        return "0.1%";
    case BandGray:;
        return "0.05%";
    case BandGold:;
        return "5%";
    case BandSilver:;
        return "10%";
    default:;
        return "--";
    }
}

void update_resistance_calculation(ResistorType rtype, BandColour bands[], char* string) {
    strcpy(string, BLANK_CALCULATION);
    update_resistance_number(rtype, bands, string, INDEX_NUMERIC);
    update_resistance_multiplier(bands[rtype - 2], string, INDEX_MULTIPLIER);
}

char* get_colour_short_description(BandColour colour) {
    switch(colour) {
    case BandBlack:
        return "Bk";
    case BandBrown:
        return "Br";
    case BandRed:
        return "Re";
    case BandOrange:
        return "Or";
    case BandYellow:
        return "Ye";
    case BandGreen:
        return "Gr";
    case BandBlue:
        return "Bl";
    case BandPurple:
        return "Pu";
    case BandGray:
        return "Gy";
    case BandWhite:
        return "Wh";
    case BandGold:
        return "Go";
    case BandSilver:
        return "Si";
    default:
        return "--";
    }
}
