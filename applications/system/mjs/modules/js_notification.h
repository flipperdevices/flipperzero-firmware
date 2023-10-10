#pragma once
#include "js_thread_i.h"

void* js_notification_create(struct mjs* mjs, mjs_val_t* object);

void js_notification_destroy(void* inst);
