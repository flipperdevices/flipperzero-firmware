#pragma once

#include <gui/view.h>
#include "../helpers/meal_pager_custom_event.h"

typedef struct Meal_PagerStartscreen Meal_PagerStartscreen;

typedef void (*Meal_PagerStartscreenCallback)(Meal_PagerCustomEvent event, void* context);

void meal_pager_startscreen_set_callback(
    Meal_PagerStartscreen* meal_pager_startscreen,
    Meal_PagerStartscreenCallback callback,
    void* context);

View* meal_pager_startscreen_get_view(Meal_PagerStartscreen* meal_pager_static);

Meal_PagerStartscreen* meal_pager_startscreen_alloc();

void meal_pager_startscreen_free(Meal_PagerStartscreen* meal_pager_static);