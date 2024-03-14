#include "ui_drawing.h"
#include <string.h>
#include "utilities.h"
#include "calculator.h"

void drawElement(Canvas* canvas, const char* str, int x, int y, int width, int height, Font font) {
    if(str) {
        canvas_set_font(canvas, font);
        canvas_draw_str(canvas, x, y, str);
    } else {
        canvas_draw_box(canvas, x, y, width, height);
    }
}

void generate_calculator_layout(Canvas* canvas) {
    // display
    canvas_draw_frame(canvas, 0, 0, 64, 62); // display frame
    canvas_draw_frame(canvas, 2, 2, 60, 38); // output  frame
    canvas_draw_frame(canvas, 2, 41, 60, 19); // input   frame

    // Horizonal and Vertical lines
    // drawElement(canvas, NULL, 0, 48, 64, 2, 0);  // H line 1
    drawElement(canvas, NULL, 0, 61, 64, 2, 0); // H line 2
    drawElement(canvas, NULL, 0, 73, 64, 2, 0); // H line 3
    drawElement(canvas, NULL, 0, 87, 64, 1, 0); // H line 4
    drawElement(canvas, NULL, 0, 100, 64, 1, 0); // H line 5
    drawElement(canvas, NULL, 0, 113, 64, 1, 0); // H line 6
    drawElement(canvas, NULL, 0, 126, 64, 2, 0); // H line 7

    drawElement(canvas, NULL, 0, 61, 2, 65, 0); // V line 1
    drawElement(canvas, NULL, 13, 61, 1, 65, 0); // V line 2
    drawElement(canvas, NULL, 25, 73, 1, 55, 0); // V line 3
    drawElement(canvas, NULL, 37, 73, 2, 55, 0); // V line 4
    drawElement(canvas, NULL, 50, 73, 1, 41, 0); // V line 5
    drawElement(canvas, NULL, 62, 61, 2, 65, 0); // V line 6

    // MODE key and display
    // row 1
    drawElement(canvas, "M", 4, 72, 0, 0, FontSecondary);

    // Keys
    // row 2
    drawElement(canvas, "7", 5, 85, 0, 0, FontSecondary);
    drawElement(canvas, "8", 17, 85, 0, 0, FontSecondary);
    drawElement(canvas, "9", 29, 85, 0, 0, FontSecondary);
    drawElement(canvas, "A", 42, 85, 0, 0, FontSecondary);
    drawElement(canvas, "B", 54, 85, 0, 0, FontSecondary);
    // row 3
    drawElement(canvas, "4", 5, 98, 0, 0, FontSecondary);
    drawElement(canvas, "5", 17, 98, 0, 0, FontSecondary);
    drawElement(canvas, "6", 29, 98, 0, 0, FontSecondary);
    drawElement(canvas, "C", 42, 98, 0, 0, FontSecondary);
    drawElement(canvas, "D", 54, 98, 0, 0, FontSecondary);
    // row 4
    drawElement(canvas, "1", 6, 111, 0, 0, FontSecondary);
    drawElement(canvas, "2", 17, 111, 0, 0, FontSecondary);
    drawElement(canvas, "3", 29, 111, 0, 0, FontSecondary);
    drawElement(canvas, "E", 42, 111, 0, 0, FontSecondary);
    drawElement(canvas, "F", 54, 111, 0, 0, FontSecondary);
    // row 5
    drawElement(canvas, "<", 6, 124, 0, 0, FontSecondary);
    drawElement(canvas, "0", 17, 124, 0, 0, FontSecondary);
    drawElement(canvas, "=", 29, 124, 0, 0, FontSecondary);
    drawElement(canvas, "CLR", 42, 124, 0, 0, FontSecondary);
}

// highlighting when cursor moves
void draw_highlighted_cell(Canvas* canvas, short x, short y, short width, short height) {
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_box(canvas, x, y, width, height);
}

void displayInput(Canvas* canvas, const char* input, int x, int startY) {
    const int maxWidth = 56; // Maximum width for each line
    const int lineSpacing = 8; // Vertical spacing between lines
    const int symbolWidth = 3; // ">" symbol takes 3px
    int currentWidth = symbolWidth; // Start with ">" symbol
    int initialX = x; // Save the initial x position

    // Buffer to store a line of input, including extra space for ">" and null terminator
    char lineBuffer[MAX_TEXT_LENGTH_INPUT + 2];
    int bufferIndex = 0;

    // Add ">" at the beginning of the first line
    lineBuffer[bufferIndex++] = '>';
    lineBuffer[bufferIndex] = '\0';

    int y = startY; // Current y position, starting from startY

    for(int i = 0; input[i] != '\0'; ++i) {
        // Calculate width of the current character ('1' takes 3 px, others take 6 px)
        int charWidth = (input[i] == '1') ? 3 : 6;

        if(currentWidth + charWidth > maxWidth) {
            // If adding this character exceeds the maxWidth, draw the current line
            canvas_draw_str(canvas, x, y, lineBuffer);

            // Reset for the next line
            y += lineSpacing;
            currentWidth = symbolWidth; // Reset current width to include ">" width for alignment
            bufferIndex = 0; // Reset buffer index for the new line
            x = initialX + 4;

            // lineBuffer[bufferIndex++] = '>'; // Add ">" at the beginning of the new line
            // lineBuffer[bufferIndex] = '\0'; // Ensure the buffer is null-terminated
        }

        // Add current character to the buffer and update the current width
        lineBuffer[bufferIndex++] = input[i];
        lineBuffer[bufferIndex] = '\0'; // Ensure the buffer is null-terminated
        currentWidth += charWidth;
    }

    canvas_draw_str(canvas, x, y, lineBuffer);
}

void displayResult(Canvas* canvas, const char* result, int x, int y) {
    const int maxWidth = 56; // Maximum width for each line
    const int lineSpacing = 9; // Vertical spacing between lines
    char lines[4][MAX_TEXT_LENGTH_RESULT + 1] = {
        {0}}; // Ensure enough space for the result + NULL terminator
    int lineIndex = 0;
    int lineWidth = 0;

    for(int i = 0; result[i] != '\0' && lineIndex < 4; ++i) { // max 4 lines
        int charWidth = (result[i] == '1') ? 3 : 6; // '1' has width 3, others have width 6

        // Check if adding another character exceeds maxWidth
        if(lineWidth + charWidth > maxWidth) {
            if(lineIndex < 3) { // Move to the next line if not yet at the 4th line
                lineWidth = 0; // Reset line width for the new line
                lineIndex++; // Move to the next line
            } else {
                break;
            }
        }

        // Add current character to the current line
        int len = strlen(lines[lineIndex]);
        lines[lineIndex][len] = result[i];
        lines[lineIndex][len + 1] = '\0';

        // Update current line width
        lineWidth += charWidth;
    }

    // Display the lines
    for(int i = 0; i <= lineIndex && i < 4; i++) {
        canvas_draw_str(canvas, x, y + (i * lineSpacing), lines[i]);
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

    char resultLabel[2 * MAX_TEXT_LENGTH_RESULT]; // Buffer to hold the result label

    switch(calculator_state->mode) {
    case ModeDecToBin:
        snprintf(resultLabel, sizeof(resultLabel), "%s", calculator_state->decToBinResult);
        break;
    case ModeDecToHex:
        snprintf(resultLabel, sizeof(resultLabel), "%s", calculator_state->decToHexResult);
        break;
    case ModeDecToChar:
        snprintf(resultLabel, sizeof(resultLabel), "%s", calculator_state->decToCharResult);
        break;
    case ModeHexToBin:
        snprintf(resultLabel, sizeof(resultLabel), "%s", calculator_state->hexToBinResult);
        break;
    case ModeHexToDec:
        snprintf(resultLabel, sizeof(resultLabel), "%s", calculator_state->hexToDecResult);
        break;
    case ModeBinToDec:
        snprintf(resultLabel, sizeof(resultLabel), "%s", calculator_state->binToDecResult);
        break;
    case ModeBinToHex:
        snprintf(resultLabel, sizeof(resultLabel), "%s", calculator_state->binToHexResult);
        break;
    default:
        strncpy(resultLabel, "         -> [M]ODE -------- v0.9.2", sizeof(resultLabel));
        break;
    }

    // Display the result, splitting into two lines if necessary
    displayResult(canvas, resultLabel, 4, 11);

    // Draw new input with ">" label or mode selection prompt
    char inputLabel[MAX_TEXT_LENGTH_INPUT + 2]; // Adjusted size for "> "
    snprintf(inputLabel, sizeof(inputLabel), "%s", calculator_state->text);
    displayInput(canvas, inputLabel, 4, 50);

    // Define the cell dimensions for each row and column
    const short cellDimensions[5][5][2] = {
        // {Width, Height} for each cell
        {{12, 13}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}, // Row 1 (One column)
        {{12, 13}, {12, 13}, {12, 13}, {12, 13}, {12, 13}}, // Row 2
        {{12, 13}, {12, 13}, {12, 13}, {12, 13}, {12, 13}}, // Row 3
        {{12, 13}, {12, 13}, {12, 13}, {12, 13}, {12, 13}}, // Row 4
        {{12, 13}, {12, 13}, {12, 13}, {24, 13}, {0, 0}}, // Row 5, with different width for column 4
    };

    const char* modeStr = "";
    switch(calculator_state->mode) {
    case ModeDecToBin:
        modeStr = "dec >> bin";
        break;
    case ModeDecToHex:
        modeStr = "dec >> hex";
        break;
    case ModeDecToChar:
        modeStr = "dec >> char";
        break;
    case ModeHexToBin:
        modeStr = "hex >> bin";
        break;
    case ModeHexToDec:
        modeStr = "hex >> dec";
        break;
    case ModeBinToDec:
        modeStr = "bin >> dec";
        break;
    case ModeBinToHex:
        modeStr = "bin >> hex";
        break;
    default:
        modeStr = " waiting ...";
        break;
    }

    canvas_draw_str(canvas, 16, 71, modeStr);
    short cursorX = 2;
    short cursorY = 61; // Starting Y position

    for(int i = 0; i < calculator_state->position.y; i++) {
        cursorY += cellDimensions[i][0][1]; // Add the height of each previous row
    }

    for(int i = 0; i < calculator_state->position.x; i++) {
        cursorX += cellDimensions[calculator_state->position.y][i][0];
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