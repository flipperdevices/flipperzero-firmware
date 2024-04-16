#pragma once

#include <gui/view.h>
#include "../helpers/ducky_script.h"

typedef struct BadKb BadKb;
typedef void (*BadKbButtonCallback)(InputKey key, void* context);

BadKb* bad_kb_view_alloc(void);

void bad_kb_view_free(BadKb* bad_kb);

View* bad_kb_view_get_view(BadKb* bad_kb);

void bad_kb_view_set_button_callback(BadKb* bad_kb, BadKbButtonCallback callback, void* context);

void bad_kb_view_set_file_name(BadKb* bad_kb, const char* name);

void bad_kb_view_set_layout(BadKb* bad_kb, const char* layout);

void bad_kb_view_set_state(BadKb* bad_kb, BadKbState* st);

bool bad_kb_view_is_idle_state(BadKb* bad_kb);
