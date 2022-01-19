#pragma once

#include <stdbool.h>
#include "view.h"

typedef struct ViewStack ViewStack;

ViewStack* view_stack_alloc(void);
void view_stack_free(ViewStack* view_stack);
View* view_stack_get_view(ViewStack* view_stack);
void view_stack_add_view(ViewStack* view_stack, View* view);
void view_stack_remove_view(ViewStack* view_stack, View* view);
