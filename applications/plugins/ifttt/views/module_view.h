#pragma once

#include <gui/view.h>

typedef struct ModuleView ModuleView;

ModuleView* module_view_alloc();

void module_view_free(ModuleView* module_view);

View* module_view_get_view(ModuleView* module_view);