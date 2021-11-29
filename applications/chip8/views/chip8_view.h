#pragma once

#include <gui/view.h>
#include "../chip8.h"

typedef struct Chip8View Chip8View;
typedef void (*Chip8ViewCallback)(InputType type, void* context);

Chip8View* chip8_alloc();

void chip8_free(Chip8View* chip8);

View* chip8_get_view(Chip8View* chip8);

void chip8_set_ok_callback(Chip8View* chip8, Chip8ViewCallback callback, void* context);

void chip8_set_file_name(Chip8View* chip8, char* name);

void chip8_set_state(Chip8View* chip8, Chip8State* st);
