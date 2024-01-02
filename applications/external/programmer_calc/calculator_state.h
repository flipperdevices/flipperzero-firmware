#ifndef CALCULATOR_STATE_H
#define CALCULATOR_STATE_H

#include <furi.h>
#include <stdbool.h>

#define MAX_TEXT_LENGTH_INPUT 19
#define MAX_TEXT_LENGTH_RESULT 37

typedef enum {
    ModeNone,
    ModeDecToBin,
    ModeDecToHex,
    ModeDecToChar,
    ModeHexToBin,
    ModeHexToDec,
    ModeBinToDec,
    ModeBinToHex
} CalculatorMode;

typedef struct {
    short x;
    short y;
} selectedPosition;

typedef struct {
    FuriMutex* mutex;
    selectedPosition position;
    char text[MAX_TEXT_LENGTH_INPUT];
    char originalInput[MAX_TEXT_LENGTH_INPUT];
    char decToBinResult[MAX_TEXT_LENGTH_RESULT];
    char decToHexResult[MAX_TEXT_LENGTH_RESULT];
    char decToCharResult[MAX_TEXT_LENGTH_RESULT];
    char hexToBinResult[MAX_TEXT_LENGTH_RESULT];
    char hexToDecResult[MAX_TEXT_LENGTH_RESULT];
    char binToDecResult[MAX_TEXT_LENGTH_RESULT];
    char binToHexResult[MAX_TEXT_LENGTH_RESULT];
    short textLength;
    char log[MAX_TEXT_LENGTH_RESULT];
    bool newInputStarted;
    CalculatorMode mode;
} Calculator;

void toggle_mode(Calculator* calculator_state);

#endif // CALCULATOR_STATE_H
