/*
Not currently functional yet...

But after listen menu, sends the selected listen code
and then listens for codes from both devices as they
communicate. Then allows the user to save either code
to the flipper.

This should ideally allow saving both codes...
*/
#ifndef SCENE_READ_CODE_HEADERS
#define SCENE_READ_CODE_HEADERS

#include "flipper.h"

void fcom_read_code_scene_on_enter(void* context);
bool fcom_read_code_scene_on_event(void* context, SceneManagerEvent event);
void fcom_read_code_scene_on_exit(void* context);

#endif