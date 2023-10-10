#pragma once
#include "js_thread_i.h"

void* js_dialog_create(struct mjs* mjs, mjs_val_t* object);

void js_dialog_destroy(void* inst);
