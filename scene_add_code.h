/*
Scene to manually enter codes via the keyboard.

Can't enter @ or ^ special characters though...

TODO: at some point, limit characters to 
A-F, 0-9, @, ^, -

since these are the only current valid digirom characters
*/
#ifndef SCENE_ADD_CODE_HEADERS
#define SCENE_ADD_CODE_HEADERS

#include "flipper.h"

typedef enum {
    SaveCodeInputRead,
} SaveCodeEvent;

void fcom_add_code_scene_on_enter(void* context);
bool fcom_add_code_scene_on_event(void* context, SceneManagerEvent event);
void fcom_add_code_scene_on_exit(void* context);

#endif