#pragma once

#include <gui/view.h>

typedef struct MjsConsoleView MjsConsoleView;

MjsConsoleView* console_view_alloc(void);

void console_view_free(MjsConsoleView* console_view);

View* console_view_get_view(MjsConsoleView* console_view);

void console_view_print(MjsConsoleView* console_view, const char* text);
