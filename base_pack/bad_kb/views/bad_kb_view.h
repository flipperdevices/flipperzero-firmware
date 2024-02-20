#pragma once

#include <gui/view.h>
#include "../helpers/ducky_script.h"

typedef struct BadKb BadKb;
typedef void (*BadKbButtonCallback)(InputKey key, void* context);

BadKb* bad_kb_alloc();

void bad_kb_free(BadKb* bad_kb);

View* bad_kb_get_view(BadKb* bad_kb);

void bad_kb_set_button_callback(BadKb* bad_kb, BadKbButtonCallback callback, void* context);

void bad_kb_set_file_name(BadKb* bad_kb, const char* name);

void bad_kb_set_layout(BadKb* bad_kb, const char* layout);

void bad_kb_set_state(BadKb* bad_kb, BadKbState* st);

bool bad_kb_is_idle_state(BadKb* bad_kb);
