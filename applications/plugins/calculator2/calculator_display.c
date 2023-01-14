#include <string.h>
#include <strings.h>
#include "calculator_display.h"
#include "calculator_app.h"

/*void default_number_click_callback(Calculator* clc, const void* context) {
    CalculatorDisplayButton const* button = context;

    strncat(clc->framed_number, button->text_text, strlen(button->text_text));
}

void empty_callback(Calculator* clc, const void* context) {
    UNUSED(context);
    UNUSED(clc);
}*/

void default_number_click_callback(CalculatorApp* clc_app) {
    furi_string_cat_str(clc_app->calculator->framed_number, clc_app->selected_button->text_text);
}

void button_clear_click_callback(CalculatorApp* clc_app) {
    calculator_reset(clc_app->calculator);
}

void button_add_function_click_callback(CalculatorApp* clc_app) {
    // furi_mutex_acquire(clc_app->mutex, FuriWaitForever);

    /*double typed_value;
    sscanf(furi_string_get_cstr(clc_app->calculator->framed_number), "%f", &typed_value);*/

    // Theres got to be a better way to do this
    double typed_value;

    const char* c_str = furi_string_get_cstr(clc_app->calculator->framed_number);
    if(furi_string_search_char(clc_app->calculator->framed_number, '.') == FURI_STRING_FAILURE) {
        int bruh;
        sscanf(c_str, "%d", &bruh);
        typed_value = bruh;
    } else {
        sscanf(c_str, "%lf", &typed_value);
    }

    CalculatorCalculation* calculation =
        calculator_calculation_alloc(&CalculatorFunctionAdd, typed_value);
    calculator_add_calculator_calculation(clc_app->calculator, calculation);

    furi_string_reset(
        clc_app->calculator
            ->framed_number); // Maybe make a framed_number_reset funciton in calculator.c

    // furi_mutex_release(clc_app->mutex);
}

void button_calculate_click_callback(CalculatorApp* clc_app) {
    double full_calculate = calculator_full_solve(clc_app->calculator);
    // furi_string_reset(clc_app->calculator->framed_number); // Redundent; calculator_full_solve resets the Calculator struct
    furi_string_printf(clc_app->calculator->framed_number, "%lf", full_calculate);
}

void empty_callback(CalculatorApp* clc_app) {
    UNUSED(clc_app);
}

CalculatorDisplayButton const* const CalculatorDisplayButtonNumberZero =
    &(CalculatorDisplayButton){
        1,
        49,
        18,
        10,
        8,
        58,
        FontSecondary,
        "0",
        4,
        0,
        default_number_click_callback,
    };

CalculatorDisplayButton const* const CalculatorDisplayButtonNumberOne = &(CalculatorDisplayButton){
    1,
    37,
    8,
    10,
    5,
    46,
    FontSecondary,
    "1",
    3,
    0,
    default_number_click_callback,
};

CalculatorDisplayButton const* const CalculatorDisplayButtonNumberTwo = &(CalculatorDisplayButton){
    11,
    37,
    8,
    10,
    13,
    46,
    FontSecondary,
    "2",
    3,
    1,
    default_number_click_callback,
};

CalculatorDisplayButton const* const CalculatorDisplayButtonNumberThree =
    &(CalculatorDisplayButton){
        21,
        37,
        8,
        10,
        23,
        46,
        FontSecondary,
        "3",
        3,
        2,
        default_number_click_callback,
    };

CalculatorDisplayButton const* const CalculatorDisplayButtonNumberFour =
    &(CalculatorDisplayButton){
        1,
        25,
        8,
        10,
        3,
        34,
        FontSecondary,
        "4",
        2,
        0,
        default_number_click_callback,
    };

CalculatorDisplayButton const* const CalculatorDisplayButtonNumberFive =
    &(CalculatorDisplayButton){
        11,
        25,
        8,
        10,
        13,
        34,
        FontSecondary,
        "5",
        2,
        1,
        default_number_click_callback,
    };

CalculatorDisplayButton const* const CalculatorDisplayButtonNumberSix = &(CalculatorDisplayButton){
    21,
    25,
    8,
    10,
    23,
    34,
    FontSecondary,
    "6",
    2,
    2,
    default_number_click_callback,
};

CalculatorDisplayButton const* const CalculatorDisplayButtonNumberSeven =
    &(CalculatorDisplayButton){
        1,
        13,
        8,
        10,
        3,
        22,
        FontSecondary,
        "7",
        1,
        0,
        default_number_click_callback,
    };

CalculatorDisplayButton const* const CalculatorDisplayButtonNumberEight =
    &(CalculatorDisplayButton){
        11,
        13,
        8,
        10,
        13,
        22,
        FontSecondary,
        "8",
        1,
        1,
        default_number_click_callback,
    };

CalculatorDisplayButton const* const CalculatorDisplayButtonNumberNine =
    &(CalculatorDisplayButton){
        21,
        13,
        8,
        10,
        23,
        22,
        FontSecondary,
        "9",
        1,
        2,
        default_number_click_callback,
    };

CalculatorDisplayButton const* const CalculatorDisplayButtonClear = &(CalculatorDisplayButton){
    1,
    1,
    8,
    10,
    3,
    10,
    FontSecondary,
    "C",
    0,
    0,
    button_clear_click_callback,
};

CalculatorDisplayButton const* const CalculatorDisplayButtonNegative = &(CalculatorDisplayButton){
    11,
    1,
    8,
    10,
    13,
    10,
    FontSecondary,
    "-",
    0,
    1,
    empty_callback,
};

CalculatorDisplayButton const* const CalculatorDisplayButtonFunctionRemainder =
    &(CalculatorDisplayButton){
        21,
        1,
        8,
        10,
        23,
        10,
        FontSecondary,
        "%",
        0,
        2,
        empty_callback,
    };

CalculatorDisplayButton const* const CalculatorDisplayButtonFunctionDivide =
    &(CalculatorDisplayButton){
        31,
        1,
        8,
        10,
        32,
        10,
        FontSecondary,
        "/", // change to asset
        0,
        3,
        empty_callback,
    };

CalculatorDisplayButton const* const CalculatorDisplayButtonFunctionMultiply =
    &(CalculatorDisplayButton){
        31,
        13,
        8,
        10,
        33,
        23,
        FontSecondary,
        "*",
        1,
        3,
        empty_callback,
    };

CalculatorDisplayButton const* const CalculatorDisplayButtonFunctionMinus =
    &(CalculatorDisplayButton){
        31,
        25,
        8,
        10,
        33,
        34,
        FontSecondary,
        "-",
        2,
        3,
        empty_callback,
    };

CalculatorDisplayButton const* const CalculatorDisplayButtonCalculate = &(CalculatorDisplayButton){
    31,
    49,
    8,
    10,
    33,
    58,
    FontSecondary,
    "=",
    4,
    3,
    button_calculate_click_callback,
};

CalculatorDisplayButton const* const CalculatorDisplayButtonDecimal = &(CalculatorDisplayButton){
    21,
    49,
    8,
    10,
    25,
    58,
    FontSecondary,
    ".",
    4,
    2,
    empty_callback,
};

CalculatorDisplayButton const* const CalculatorDisplayButtonFunctionAdd =
    &(CalculatorDisplayButton){
        31,
        37,
        8,
        10,
        33,
        46,
        FontSecondary,
        "+",
        3,
        3,
        button_add_function_click_callback,
    };

CalculatorDisplayButton const* calculator_display_button_grid[][4] = {
    {
        CalculatorDisplayButtonClear,
        CalculatorDisplayButtonNegative,
        CalculatorDisplayButtonFunctionRemainder,
        CalculatorDisplayButtonFunctionDivide,
    },
    {
        CalculatorDisplayButtonNumberSeven,
        CalculatorDisplayButtonNumberEight,
        CalculatorDisplayButtonNumberNine,
        CalculatorDisplayButtonFunctionMultiply,
    },
    {
        CalculatorDisplayButtonNumberFour,
        CalculatorDisplayButtonNumberFive,
        CalculatorDisplayButtonNumberSix,
        CalculatorDisplayButtonFunctionMinus,
    },
    {
        CalculatorDisplayButtonNumberOne,
        CalculatorDisplayButtonNumberTwo,
        CalculatorDisplayButtonNumberThree,
        CalculatorDisplayButtonFunctionAdd,
    },
    {
        CalculatorDisplayButtonNumberZero,
        CalculatorDisplayButtonNumberZero,
        CalculatorDisplayButtonDecimal,
        CalculatorDisplayButtonCalculate,
    },
};

const size_t NUMBER_OF_ROWS_BUTTONS =
    sizeof(calculator_display_button_grid) / sizeof(calculator_display_button_grid[0]);

const size_t NUMBER_OF_COLUMNS_BUTTONS =
    sizeof(calculator_display_button_grid[0]) / sizeof(calculator_display_button_grid[0][0]);