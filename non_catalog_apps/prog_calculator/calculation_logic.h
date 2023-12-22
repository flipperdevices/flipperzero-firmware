#ifndef CALCULATION_LOGIC_H
#define CALCULATION_LOGIC_H

#include "calculator_state.h"

bool decToBin(const char* decString, char* binaryResult, size_t resultSize);
bool decToHex(const char* decString, char* hexResult, size_t resultSize);
bool decToChar(const char* decString, char* outChar);
bool hexToBin(const char* hexString, char* binaryResult, size_t resultSize);
bool hexToDec(const char* hexString, int* outNum);
bool binToDec(const char* binaryString, int* decResult);
bool binToHex(const char* binaryString, char* hexResult, size_t resultSize);
void calculate(Calculator* calculator_state);

#endif // CALCULATION_LOGIC_H
