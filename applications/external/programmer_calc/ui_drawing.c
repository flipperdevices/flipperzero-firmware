#include "ui_drawing.h"
#include <string.h>
#include "utilities.h"
#include "calculator.h"

void generate_calculator_layout(Canvas* canvas) {
    canvas_draw_box(canvas, 0, 126, 64, 2);
    canvas_draw_box(canvas, 0, 113, 64, 1);
    canvas_draw_box(canvas, 0, 100, 64, 1);
    canvas_draw_box(canvas, 0, 87, 64, 1);
    canvas_draw_box(canvas, 0, 73, 64, 2);
    canvas_draw_box(canvas, 0, 61, 64, 1);
    canvas_draw_box(canvas, 0, 47, 2, 80);

    canvas_draw_box(canvas, 13, 73, 1, 55);
    canvas_draw_box(canvas, 25, 73, 1, 55);
    canvas_draw_box(canvas, 62, 47, 2, 81);
    canvas_draw_box(canvas, 37, 73, 2, 55);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 5, 111, "1");
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 17, 111, "2");
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 29, 111, "3");
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 4, 98, "4");
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 17, 98, "5");
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 29, 98, "6");
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 17, 124, "0");
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 5, 85, "7");
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 17, 85, "8");
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 29, 85, "9");
    canvas_draw_box(canvas, 50, 73, 1, 41);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 42, 85, "A");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 54, 85, "B");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 42, 98, "C");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 54, 98, "D");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 42, 111, "E");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 54, 111, "F");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 29, 124, "=");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 42, 124, "CLR");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 5, 124, "<");

    canvas_set_font(canvas, FontSecondary);
    // canvas_draw_str(canvas, 15, 71, "Dec -> Bin");
    canvas_draw_box(canvas, 0, 47, 64, 2);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 19, 59, "MODE");
    canvas_draw_frame(canvas, 0, 0, 64, 46);
    canvas_set_font(canvas, FontSecondary);
    // canvas_draw_str(canvas, 11, 11, "Calculator");
    // canvas_draw_frame(canvas, 0, 1, 62, 13);
    // canvas_draw_frame(canvas, 1, 14, 62, 31);

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 5, 38, "");

    // for (int i = 0; i < 64; i++){
    //     if (i%2 == 0){
    //         canvas_draw_dot(canvas, i, 46);
    //     }
    // }
    canvas_draw_line(canvas, 1, 44, 62, 44);
    canvas_draw_line(canvas, 5, 46, 58, 46);
    // canvas_draw_dot(canvas, 63, 46);
    // canvas_draw_dot(canvas, 0, 46);
    canvas_draw_frame(canvas, 2, 2, 60, 24);
    canvas_draw_frame(canvas, 2, 27, 60, 16);
}

void draw_highlighted_cell(Canvas* canvas, short x, short y, short width, short height) {
    canvas_set_color(canvas, ColorBlack); // Set the color for highlighting
    canvas_draw_box(canvas, x, y, width, height);
}

void displayResultInTwoLines(Canvas* canvas, char* result, int x, int y) {
    if(strlen(result) > 11) {
        char line1[12]; // Buffer for the first line (12 chars + null terminator)
        strncpy(line1, result, 11);
        line1[11] = '\0'; // Terminate the first line correctly

        const char* line2 = result + 11; // Pointer to the second part of the string

        canvas_draw_str(canvas, x, y, line1); // First line
        canvas_draw_str(canvas, x, y + 11, line2); // Second line
    } else {
        canvas_draw_str(canvas, x, y, result); // Single line if 12 characters or less
    }
}

void calculator_draw_callback(Canvas* canvas, void* ctx) {
    furi_assert(ctx);
    const Calculator* calculator_state = ctx;
    furi_mutex_acquire(calculator_state->mutex, FuriWaitForever);

    // Clear the canvas for fresh drawing
    canvas_clear(canvas);

    // Draw the static UI layout
    generate_calculator_layout(canvas);

    char resultLabel[2 * MAX_TEXT_LENGTH]; // Buffer to hold the result label

    // Check which mode is selected and prepare the label accordingly
    if(calculator_state->mode == ModeHexToDec) {
        snprintf(resultLabel, sizeof(resultLabel), "Dec: %s", calculator_state->decResult);
    } else if(calculator_state->mode == ModeDecToBin) {
        snprintf(resultLabel, sizeof(resultLabel), "Bin: %s", calculator_state->binaryResult);
    } else if(calculator_state->mode == ModeDecToChar) {
        snprintf(resultLabel, sizeof(resultLabel), "Charachter %s", calculator_state->charResult);
    } else if(calculator_state->mode == ModeHexToBin) {
        snprintf(resultLabel, sizeof(resultLabel), "Bin: %s", calculator_state->binaryResult);
    } else if(calculator_state->mode == ModeDecToHex) {
        snprintf(resultLabel, sizeof(resultLabel), "Hex: %s", calculator_state->hexResult);
    } else if(calculator_state->mode == ModeBinToDec) {
        snprintf(resultLabel, sizeof(resultLabel), "Dec: %s", calculator_state->decResult);
    } else if(calculator_state->mode == ModeBinToHex) {
        snprintf(resultLabel, sizeof(resultLabel), "Hex: %s", calculator_state->hexResult);
    } else {
        // If no mode is selected, you can display a default message or leave it empty
        strncpy(resultLabel, " Click MODE> P Calc v0.7", sizeof(resultLabel));
    }

    // Display the result, splitting into two lines if necessary
    displayResultInTwoLines(canvas, resultLabel, 5, 12);

    // Draw new input with ">" label or mode selection prompt
    char inputLabel[MAX_TEXT_LENGTH + 3]; // Adjusted size for "> "

    snprintf(inputLabel, sizeof(inputLabel), "$> %s", calculator_state->text);
    canvas_draw_str(canvas, 5, 39, inputLabel);
    // Define the cell dimensions for each row and column
    const short cellDimensions[6][5][2] = {
        // {Width, Height} for each cell
        {{12, 14}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}, // Row 1 (One column)
        {{9, 13}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}, // Row 2 (One column)
        {{12, 13}, {12, 13}, {12, 13}, {12, 13}, {12, 13}}, // Row 3
        {{12, 13}, {12, 13}, {12, 13}, {12, 13}, {12, 13}}, // Row 4
        {{12, 13}, {12, 13}, {12, 13}, {12, 13}, {12, 13}}, // Row 5
        {{12, 13}, {12, 13}, {12, 13}, {24, 13}, {0, 0}}, // Row 6, with different width for column 4
    };

    // Display mode indicator next to the mode if selected
    // short modeIndicatorX = 2; // X position for mode indicator
    // if(calculator_state->mode == ModeHexToDec) {
    //     canvas_draw_str(canvas, modeIndicatorX, 59, "  *"); // Draw star next to Hex -> Dec
    // } else if(calculator_state->mode == ModeDecToBin) {
    //     canvas_draw_str(canvas, modeIndicatorX, 71, "  *"); // Draw star next to Dec -> Bin
    // }
    const char* modeStr = "";
    switch(calculator_state->mode) {
    case ModeDecToBin:
        modeStr = " 1: Dec >> Bin";
        break;
    case ModeDecToHex:
        modeStr = "2: Dec >> Hex";
        break;
    case ModeDecToChar:
        modeStr = "3: Dec >> Char";
        break;
    case ModeHexToBin:
        modeStr = "4: Hex >> Bin";
        break;
    case ModeHexToDec:
        modeStr = "5: Hex >> Dec";
        break;
    case ModeBinToDec:
        modeStr = "6: Bin >> Dec";
        break;
    case ModeBinToHex:
        modeStr = "7: Bin >> Hex";
        break;
    default:
        modeStr = "      waiting ...";
        break;
    }
    canvas_draw_str(canvas, 4, 71, modeStr);
    short cursorX = 2;
    short cursorY = 47; // Starting Y position

    for(int i = 0; i < calculator_state->position.y; i++) {
        cursorY += cellDimensions[i][0][1]; // Add the height of each previous row
    }

    for(int i = 0; i < calculator_state->position.x; i++) {
        cursorX += cellDimensions[calculator_state->position.y][i]
                                 [0]; // Add the width of each previous column
    }

    short cellWidth =
        cellDimensions[calculator_state->position.y][calculator_state->position.x][0];
    short cellHeight =
        cellDimensions[calculator_state->position.y][calculator_state->position.x][1];

    // Draw the highlighted cell
    draw_highlighted_cell(canvas, cursorX, cursorY, cellWidth, cellHeight);

    // Determine the content to display in the cell
    char cellContent[2]; // Buffer to hold the cell content
    snprintf(
        cellContent,
        sizeof(cellContent),
        "%c",
        getKeyAtPosition(calculator_state->position.x, calculator_state->position.y));

    // Calculate center position for the text
    short textX = cursorX + (cellWidth - calculateStringWidth(cellContent, 2)) / 1;
    short textY = cursorY + (cellHeight - 2) / 1;

    // Set the color for the text and draw it
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_str(canvas, textX, textY, cellContent);

    furi_mutex_release(calculator_state->mutex);
}
