#pragma once

#include <gui/view.h>
#include "lightmeter_icons.h"

typedef struct MainView MainView;

MainView* main_view_alloc();

void main_view_free(MainView* main_view);

View* main_view_get_view(MainView* main_view);
