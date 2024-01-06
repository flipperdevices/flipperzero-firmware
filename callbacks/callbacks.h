#ifndef CALLBACKS_H
#define CALLBACKS_H

#include "../app/app.h"

void book_menu_callback(void* context, uint32_t index);
void book_chapter_callback(void* context, uint32_t index);
bool book_back_event_callback(void* context);

#endif // CALLBACKS_H
