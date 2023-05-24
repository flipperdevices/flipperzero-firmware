#pragma once

#include <math.h>

typedef double(CalculatorCalculateFunctionOneInput)(double a);
typedef double(CalculatorCalculateFunctionTwoInput)(double a, double b);

typedef enum {
    CalculatorFunctionTypeOneInput, // Real functions like sqrt, sinx, squared, etc
    CalculatorFunctionTypeTwoInput, // Operations multiplying and dividing
    CalculatorFunctionTypePassive, // Add and subtract
} CalculatorFunctionType;

typedef struct {
    CalculatorFunctionType type;
    CalculatorCalculateFunctionOneInput* one_param_func;
    CalculatorCalculateFunctionTwoInput* two_param_func;
} CalculatorFunction;

double add(double a);

double subtract(double a);

double multiply(double a, double b);

double divide(double a, double b);

double square(double a);

extern const CalculatorFunction CalculatorFunctionAdd;

extern const CalculatorFunction CalculatorFunctionSubtract;

extern const CalculatorFunction CalculatorFunctionMultiply;

extern const CalculatorFunction CalculatorFunctionDivide;

extern const CalculatorFunction CalculatorFunctionSquare;
