#pragma once

#include <furi.h>
#include <gui/gui.h>
#include "calculator_functions.h"
#include "calculator_display.h"
#include "calculator.h"

/*typedef struct {
    // CalculatorFunctionType type;
    // CalculatorCalculateFunctionOneInput* function_one;
    // CalculatorCalculateFunctionTwoInput* function_two;
    const CalculatorFunction* function;
    double value;
} CalculatorCalculation;

typedef struct {
    char* framed_number;
    FuriMessageQueue* operation_queue; // To be added all together Type CalculatorCalculation
} Calculator;*/

// struct CalculatorDisplayButton;
// typedef struct CalculatorApp CalculatorApp;

// struct CalculatorDisplayButton;
typedef struct CalculatorApp {
    ViewPort* vp;
    Gui* gui;
    FuriMessageQueue* msq;
    // TODO: Mutex
    Calculator* calculator;

    FuriMutex* mutex;

    // CalculatorDisplayElementGridLocation selected_grid_element;
    struct CalculatorDisplayButton const* selected_button;
} CalculatorApp;