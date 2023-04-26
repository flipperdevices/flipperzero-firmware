#ifndef RESISTOR_LOGIC_HEADERS
#define RESISTOR_LOGIC_HEADERS

typedef enum { Resistor4Band = 4, Resistor5Band = 5 } ResistorType;

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
} ResistorBand;

extern const char blank_descriptor[];

void update_resistor_descriptor(ResistorType bands, int resistor_bands[], char descriptor[]);

#endif