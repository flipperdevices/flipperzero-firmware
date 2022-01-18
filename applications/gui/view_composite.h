#pragma once

#include <stdbool.h>
#include "view.h"

typedef struct ViewComposite ViewComposite;

ViewComposite* view_composite_alloc(void);
void view_composite_free(ViewComposite* view_composite);
void view_composite_top_enable(ViewComposite* view_composite, bool enable);
void view_composite_tie_views(ViewComposite* view_composite, View* view_bottom, View* view_top);
View* view_composite_get_view(ViewComposite* view_composite);
