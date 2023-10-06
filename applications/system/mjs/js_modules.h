#pragma once
#include "js_thread_i.h"

typedef struct JsModules JsModules;

JsModules* js_modules_create(struct mjs* mjs);

void js_modules_destroy(JsModules* modules);

mjs_val_t js_module_require(JsModules* modules, const char* name, size_t name_len);
