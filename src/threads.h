#ifndef __threads_h__
#define __threads_h__

#include <stdint.h>
#include <gui/gui.h> // Canvas
#include <input/input.h> // InputEvent

#include "flipper_structs.h"

/* Main thread used to receive user inputs and to request
 * the processing of background tasks */
void main_thread(struct ApplicationContext *);

/* Thread dedicated to the processing of the game events */
int32_t secondary_thread(void *);

/* Thread called every time there's the need to refresh the GUI */
void main_draw_callback(Canvas *, void *);

/* Thread called every time the user press a button.
 * It only passes the message to the main thread */
void main_input_callback(InputEvent *, void *);

#endif
