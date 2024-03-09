#include "input_handling.h"
#include "calculator_state.h"
#include "calculator.h"
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void calculator_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}


void handle_short_press(Calculator* calculator_state, ViewPort* view_port, InputEvent* event) {
    switch(event->key) {
    case InputKeyUp:
        if(calculator_state->position.y > 0) {
            if(calculator_state->position.y == 1) {
                calculator_state->position.x = 0;
            }
            calculator_state->position.y--;
            // if(calculator_state->position.y == 1) { // If cursor moves to row 2, skip it
            //     calculator_state->position.y--;
            // }
        }   
        break;
    case InputKeyDown:
        if(calculator_state->position.y < 5 - 1) {
            if(calculator_state->position.y == 5 - 2 && 
                (calculator_state->position.x == 3 || calculator_state->position.x == 4)) {
                calculator_state->position.y = 5 - 1;
                calculator_state->position.x = 3;
            } else {
                calculator_state->position.y++;
            }
            // if(calculator_state->position.y == 1) { // If cursor moves to row 2, skip it
            //     calculator_state->position.y++;
            // }
        }
        break;
    case InputKeyLeft:
        if(calculator_state->position.y > 0 && calculator_state->position.x > 0) {
            calculator_state->position.x--;
        }
        break;
    case InputKeyRight:
        if(calculator_state->position.y < 1) {
            // Cursor stays in the same column
        } else if(calculator_state->position.y == 5 - 1) {
            if(calculator_state->position.x < 3) {
                calculator_state->position.x++;
            }
        } else {
            if(calculator_state->position.x < 5 - 1) {
                calculator_state->position.x++;
            }
        }
        break;
    case InputKeyOk:
        if(calculator_state->position.y == 0) {
            toggle_mode(calculator_state);
        } else {
            char key = getKeyAtPosition(calculator_state->position.x, calculator_state->position.y);
            handle_key_press(calculator_state, key);
        }
        break;

    default:
        break;
    }

    view_port_update(view_port);
}

void handle_long_press(Calculator* calculator_state, ViewPort* view_port, InputEvent* event) {
    switch(event->key) {
        case InputKeyOk:
            if(calculator_state->position.x == 0 && calculator_state->position.y == 4) {
                if(calculator_state->textLength < MAX_TEXT_LENGTH_INPUT) {
                    calculator_state->text[calculator_state->textLength++] = ')';
                    calculator_state->text[calculator_state->textLength] = '\0';
                }
            }
            break;
        default:
            break;
    }

    view_port_update(view_port);
}

void handle_key_press(Calculator* calculator_state, char key) {
    switch(key) {
    case '=':
        // Logic for '=' key
        strncpy(calculator_state->originalInput, calculator_state->text, MAX_TEXT_LENGTH_INPUT);
        calculate(calculator_state);
        // calculator_state->text[0] = '\0';
        calculator_state->textLength = 0;
        break;
    case 'R':
        // Logic for 'R' key, typically 'Clear'
        calculator_state->text[0] = '\0';
        calculator_state->textLength = 0;
        calculator_state->decToBinResult[0] = '\0';
        calculator_state->decToHexResult[0] = '\0';
        calculator_state->decToCharResult[0] = '\0';
        calculator_state->hexToBinResult[0] = '\0';
        calculator_state->hexToDecResult[0] = '\0';
        calculator_state->binToDecResult[0] = '\0';
        calculator_state->binToHexResult[0] = '\0';
        calculator_state->newInputStarted = false;
        break;
    case '<':
        // Logic for '<' key, typically 'Backspace'
        if(calculator_state->textLength > 0) {
            calculator_state->text[--calculator_state->textLength] = '\0';
        }
        calculator_state->newInputStarted = false;
        break;
    default:
        // Default logic for number and operator keys
        if(calculator_state->newInputStarted) {
            // Reset the text for a fresh input if new input has started
            calculator_state->text[0] = '\0';
            calculator_state->textLength = 0;
            calculator_state->newInputStarted = false;
        }
        // Add the new character to the text, respecting the maximum text length
        if(calculator_state->textLength < MAX_TEXT_LENGTH_INPUT - 1) {
            calculator_state->text[calculator_state->textLength++] = key;
            calculator_state->text[calculator_state->textLength] = '\0';
        }
        break;
    }
}

void handle_event(Calculator* calculator_state, ViewPort* view_port, InputEvent* event) {
    if(event->type == InputTypeShort) {
        handle_short_press(calculator_state, view_port, event);
    } else if(event->type == InputTypeLong) {
        handle_long_press(calculator_state, view_port, event);
    }
    view_port_update(view_port);
}
