#ifndef CALCULATOR_STATE_H
#define CALCULATOR_STATE_H

#include <furi.h>
#include <stdbool.h>

#define MAX_TEXT_LENGTH 30

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
    char text[MAX_TEXT_LENGTH];
    char originalInput[MAX_TEXT_LENGTH];
    char binaryResult[MAX_TEXT_LENGTH];
    char hexResult[MAX_TEXT_LENGTH];
    char decResult[MAX_TEXT_LENGTH];
    char charResult[MAX_TEXT_LENGTH];
    short textLength;
    char log[MAX_TEXT_LENGTH];
    bool newInputStarted;
    CalculatorMode mode;
} Calculator;

void toggle_mode(Calculator* calculator_state);

#endif // CALCULATOR_STATE_H
