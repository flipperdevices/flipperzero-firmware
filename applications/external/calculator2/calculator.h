#pragma once

#include <furi.h>
#include "calculator_functions.h"

#define LIMIT_OF_CHAIN 10

typedef struct {
    // CalculatorFunctionType type;
    // CalculatorCalculateFunctionOneInput* function_one;
    // CalculatorCalculateFunctionTwoInput* function_two;
    const CalculatorFunction* function;
    double value;
} CalculatorCalculation;

typedef struct {
    // char framed_number[];
    FuriString* framed_number;
    FuriMessageQueue* operation_queue; // To be added all together Type CalculatorCalculation
} Calculator;

Calculator* calculator_alloc();

CalculatorCalculation* calculator_calculation_alloc(const CalculatorFunction* func, double value);

void calculator_free(Calculator* clc);

void calculator_reset(Calculator* clc);

void calculator_add_calculator_calculation(Calculator* clc, CalculatorCalculation* calculation);

double calculator_get_framed_number_as_double(Calculator* clc);

void calculator_full_solve(Calculator* clc);
