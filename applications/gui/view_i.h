#pragma once

#include "view.h"
#include "view_dispatcher.h"

struct View {
    ViewDispatcher *dispatcher;
};

view_draw(View* view, Canvas* canvas);

view_input(View* view, Canvas* canvas);
