#ifndef __threads_h__
#define __threads_h__

#include <stdint.h>

#include "flipper_structs.h"

/* Main thread used to monitor the GUI.
 * It blocks while the GUI is active. */
void main_thread(struct ApplicationContext*);

/* Thread dedicated to the processing of game events */
int32_t secondary_thread(void*);

#endif
