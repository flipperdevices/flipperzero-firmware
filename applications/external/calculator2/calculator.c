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

double calculator_get_framed_number_as_double(Calculator* clc) {
    double sum;
    int left;
    int right;
    if(furi_string_search_char(clc->framed_number, '.') != FURI_STRING_FAILURE) {
        if(furi_string_start_with_str(clc->framed_number, ".")) {
            sscanf(furi_string_get_cstr(clc->framed_number), ".%d", &right);
            size_t digits = furi_string_size(clc->framed_number) - 1;
            // sum = (double)((double)right / pow10(((int)log10(right) + 1)));
            sum = (double)((double)right / pow10(digits));
            // 0.001 = 1/1000
            // digits = 3 | pow10(3) = 1000 |
        } else {
            sscanf(furi_string_get_cstr(clc->framed_number), "%d.%d", &left, &right);
            char buffer[8];
            sscanf(furi_string_get_cstr(clc->framed_number), "%*d.%s", buffer);

            size_t digits = strlen(buffer);

            // sum = (double)left + (double)((double)right / pow10(((int)log10(right) + 1)));
            sum = (double)left + (double)((double)right / pow10(digits));
        }
    } else {
        sscanf(furi_string_get_cstr(clc->framed_number), "%d", &left);
        sum = left;
    }

    return sum;
}

void calculator_full_solve(Calculator* clc) {
    double sum = calculator_get_framed_number_as_double(clc);

    // sscanf(furi_string_get_cstr(clc->framed_number), "%d.%d", &left, &right);
    // sum = (double)left + (double)((double)right / pow10(((int)log10(right) + 1)));

    // double sum = 0;
    CalculatorCalculation calculation;

    /*while(furi_message_queue_get(clc->operation_queue, &calculation, FuriWaitForever) ==
          FuriStatusOk) {
        sum += calculation.value;
    }*/

    for(size_t i = 0; i < furi_message_queue_get_count(clc->operation_queue); i++) {
        furi_message_queue_get(clc->operation_queue, &calculation, FuriWaitForever);
        sum += calculation.value;
    }

    // sum += left_over;

    // So that u can use the result in later calcs
    // This should be the only thing in the queue

    calculator_reset(clc);

    // bruh
    calculator_add_calculator_calculation(
        clc, calculator_calculation_alloc(&CalculatorFunctionAdd, sum));

    // return sum;
}