#pragma once
#include "js_thread_i.h"

void* js_badusb_create(struct mjs* mjs, mjs_val_t* object);

void js_badusb_destroy(void* inst);
