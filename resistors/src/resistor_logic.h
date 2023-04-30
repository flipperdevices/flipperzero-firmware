#ifndef RESISTOR_LOGIC_HEADERS
#define RESISTOR_LOGIC_HEADERS

typedef enum { R3 = 3, R4 = 4, R5 = 5, R6 = 6 } ResistorType;

typedef enum {
    BandBlack = 0,
    BandBrown = 1,
    BandRed = 2,
    BandOrange = 3,
    BandYellow = 4,
    BandGreen = 5,
    BandBlue = 6,
    BandPurple = 7,
    BandGray = 8,
    BandWhite = 9,
    BandGold = 10,
    BandSilver = 11
} BandColour;

extern const char blank_descriptor_R4[];
extern const char blank_descriptor_R5[];

void update_resistor_descriptor(ResistorType bands, BandColour resistor_bands[], char descriptor[]);
void update_calculation(ResistorType rtype, BandColour bands[], char string[]);

BandColour
    alter_resistor_band(ResistorType rtype, int band, BandColour current_colour, int direction);

#endif