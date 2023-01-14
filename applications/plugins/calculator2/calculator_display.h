#pragma once

// #include "calculator.h"
#include <furi.h>
#include <gui/gui.h>
#include "calculator.h"
#include "calculator_app.h"

struct CalculatorDisplayButton;
struct CalculatorApp;
// typedef void(CalculatorDisplayButtonOnClickCallback)(CalculatorApp* clc);
/*typedef void(CalculatorDisplayButtonOnClickCallback)(
    struct CalculatorApp* clc,
    const struct CalculatorDisplayButton* b);*/

// Maybe the definition of this function at the top should all take void* p cuz they dont all need clc_app
typedef void(CalculatorDisplayButtonOnClickCallback)(struct CalculatorApp* clc_app);

typedef struct CalculatorDisplayButton {
    uint8_t frame_x;
    uint8_t frame_y;
    uint8_t frame_w;
    uint8_t frame_h;

    uint8_t text_x;
    uint8_t text_y;
    Font text_font;
    const char* text_text;

    int8_t row;
    int8_t column;

    CalculatorDisplayButtonOnClickCallback* onclick_callback;
    // CalculatorFunction* function;
} CalculatorDisplayButton;

/*void default_number_click_callback(Calculator* clc, const void* context);

void empty_callback(Calculator* clc, const void* context);*/
// change back to calculator
/*void default_number_click_callback(Calculator* clc, const CalculatorDisplayButton* b);

void empty_callback(Calculator* clc, const CalculatorDisplayButton* b);*/

void default_number_click_callback(struct CalculatorApp* clc_app);

void button_clear_click_callback(struct CalculatorApp* clc_app);

void button_add_function_click_callback(struct CalculatorApp* clc_app);

void button_calculate_click_callback(struct CalculatorApp* clc_app);

void empty_callback(struct CalculatorApp* clc_app);

extern CalculatorDisplayButton const* const CalculatorDisplayButtonNumberZero;

extern CalculatorDisplayButton const* const CalculatorDisplayButtonNumberOne;

extern CalculatorDisplayButton const* const CalculatorDisplayButtonNumberTwo;

extern CalculatorDisplayButton const* const CalculatorDisplayButtonNumberThree;
extern CalculatorDisplayButton const* const CalculatorDisplayButtonNumberFour;

extern CalculatorDisplayButton const* const CalculatorDisplayButtonNumberFive;

extern CalculatorDisplayButton const* const CalculatorDisplayButtonNumberSix;
extern CalculatorDisplayButton const* const CalculatorDisplayButtonNumberSeven;

extern CalculatorDisplayButton const* const CalculatorDisplayButtonNumberEight;

extern CalculatorDisplayButton const* const CalculatorDisplayButtonNumberNine;

extern CalculatorDisplayButton const* const CalculatorDisplayButtonClear;

extern CalculatorDisplayButton const* const CalculatorDisplayButtonNegative;

extern CalculatorDisplayButton const* const CalculatorDisplayButtonFunctionRemainder;

extern CalculatorDisplayButton const* const CalculatorDisplayButtonFunctionDivide;

extern CalculatorDisplayButton const* const CalculatorDisplayButtonFunctionMultiply;
extern CalculatorDisplayButton const* const CalculatorDisplayButtonFunctionMinus;

extern CalculatorDisplayButton const* const CalculatorDisplayButtonFunctionAdd;

extern CalculatorDisplayButton const* const CalculatorDisplayButtonCalculate;

extern CalculatorDisplayButton const* const CalculatorDisplayButtonDecimal;

extern CalculatorDisplayButton const* calculator_display_button_grid[][4];

extern const size_t NUMBER_OF_ROWS_BUTTONS;
extern const size_t NUMBER_OF_COLUMNS_BUTTONS;

// const size_t NUMBER_OF_ROWS_BUTTONS =
// sizeof(calculator_display_button_grid) / sizeof(calculator_display_button_grid[0]);

// const size_t NUMBER_OF_COLUMNS_BUTTONS =
// sizeof(calculator_display_button_grid[0]) / sizeof(calculator_display_button_grid[0][0]);
