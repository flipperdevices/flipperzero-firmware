#include "calculator_functions.h"

double add(double a) {
    return a;
}

double subtract(double a) {
    return -a;
}

double multiply(double a, double b) {
    return a * b;
}

double divide(double a, double b) {
    return a / b;
}

double square(double a) {
    return pow(a, 2);
}

const CalculatorFunction CalculatorFunctionAdd = {
    .type = CalculatorFunctionTypeOneInput,
    .one_param_func = add};

const CalculatorFunction CalculatorFunctionSubtract = {
    .type = CalculatorFunctionTypeOneInput,
    .one_param_func = subtract};

const CalculatorFunction CalculatorFunctionMultiply = {
    .type = CalculatorFunctionTypeTwoInput,
    .two_param_func = multiply};

const CalculatorFunction CalculatorFunctionDivide = {
    .type = CalculatorFunctionTypeTwoInput,
    .two_param_func = divide};

const CalculatorFunction CalculatorFunctionSquare = {
    .type = CalculatorFunctionTypeOneInput,
    .one_param_func = square};
