#ifndef INPUT_HANDLING_H
#define INPUT_HANDLING_H

#include "calculator_state.h"
#include <input/input.h>
#include <gui/gui.h>
#include <furi_hal.h>

void calculator_input_callback(InputEvent* input_event, void* ctx);
void handle_short_press(Calculator* calculator_state, ViewPort* view_port, InputEvent* event);
void handle_long_press(Calculator* calculator_state, ViewPort* view_port, InputEvent* event);
void handle_key_press(Calculator* calculator_state, char key);
void handle_event(Calculator* calculator_state, ViewPort* view_port, InputEvent* event);

#endif // INPUT_HANDLING_H
