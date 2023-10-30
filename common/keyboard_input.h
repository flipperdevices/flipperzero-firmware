#pragma once

#include <gui/view.h>
#include "key_setting_model.h"

typedef struct KeyboardInput KeyboardInput;

typedef void (*KeyboardInputCallback)(uint16_t key_code, void* context);

KeyboardInput* keyboard_input_alloc();
void keyboard_input_free(KeyboardInput* keyboard_input);
View* keyboard_input_get_view(KeyboardInput* keyboard_input);
void keyboard_input_set_key(KeyboardInput* keyboard_input, uint16_t key_code);
void keyboard_input_set_callback(
    KeyboardInput* keyboard_input,
    KeyboardInputCallback callback,
    void* context);