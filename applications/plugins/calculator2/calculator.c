#include <string.h>
#include "calculator.h"

void calculator_reset(Calculator* clc) {
    furi_string_reset(clc->framed_number);
    furi_message_queue_reset(clc->operation_queue);
}

void calculator_add_calculator_calculation(Calculator* clc, CalculatorCalculation* calculation) {
    if(calculation->function->type == CalculatorFunctionTypeTwoInput) {
        CalculatorCalculation previous_calculation;
        // Hopefully message_queue_get removes element from queue when getted
        // If they just presed divide as first thing without any number before
        if(furi_message_queue_get(clc->operation_queue, &previous_calculation, FuriWaitForever) ==
           FuriStatusError) {
            calculator_reset(clc);
            return;
        }

        // calculation->value = previous_calculation.value * calculation->value;
        calculation->value =
            calculation->function->two_param_func(previous_calculation.value, calculation->value);

    } else if(calculation->function->type == CalculatorFunctionTypeOneInput) {
        calculation->value = calculation->function->one_param_func(calculation->value);
    }

    // calculation->function->type =
    // CalculatorFunctionTypePassive; // May be issue here, shouldn't change
    furi_message_queue_put(clc->operation_queue, calculation, FuriWaitForever);
}

CalculatorCalculation* calculator_calculation_alloc(const CalculatorFunction* func, double value) {
    CalculatorCalculation* calc_calc = malloc(sizeof(CalculatorCalculation));
    calc_calc->function = func;
    calc_calc->value = value;
    return calc_calc;
}

Calculator* calculator_alloc() {
    Calculator* clc = malloc(sizeof(Calculator));
    clc->operation_queue = furi_message_queue_alloc(LIMIT_OF_CHAIN, sizeof(CalculatorCalculation));
    // clc->framed_number = "";
    clc->framed_number = furi_string_alloc();
    return clc;
}

void calculator_free(Calculator* clc) {
    furi_message_queue_free(clc->operation_queue);
    furi_string_free(clc->framed_number);
    free(clc);
}

double calculator_full_solve(Calculator* clc) {
    double left_over;

    const char* c_str = furi_string_get_cstr(clc->framed_number);
    if(furi_string_search_char(clc->framed_number, '.') == FURI_STRING_FAILURE) {
        int bruh;
        sscanf(c_str, "%d", &bruh);
        left_over = bruh;
    } else {
        sscanf(c_str, "%lf", &left_over);
    }

    double sum = 0;
    CalculatorCalculation calculation;

    /*while(furi_message_queue_get(clc->operation_queue, &calculation, FuriWaitForever) ==
          FuriStatusOk) {
        sum += calculation.value;
    }*/

    for(size_t i = 0; i < furi_message_queue_get_count(clc->operation_queue); i++) {
        furi_message_queue_get(clc->operation_queue, &calculation, FuriWaitForever);
        sum += calculation.value;
    }

    sum += left_over;

    // So that u can use the result in later calcs
    // This should be the only thing in the queue

    calculator_reset(clc);

    calculator_add_calculator_calculation(
        clc, calculator_calculation_alloc(&CalculatorFunctionAdd, sum));

    return sum;
}