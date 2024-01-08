#ifndef CALLBACKS_H
#define CALLBACKS_H

#include "../app/app.h"

void menu_callback(void* context, uint32_t index);
void chapter_callback(void* context, uint32_t index);
bool back_event_callback(void* context);

#endif // CALLBACKS_H
