#ifndef __main_callbacks_h__
#define __main_callbacks_h__

#include <stdint.h>
#include <gui/gui.h> // Canvas
#include <input/input.h> // InputEvent

void main_draw_callback(Canvas *, void *);
void main_input_callback(InputEvent *, void *);
int32_t main_thread_callback(void *);

#endif
