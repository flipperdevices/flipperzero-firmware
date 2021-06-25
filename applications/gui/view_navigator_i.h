#pragma once

#include "view_navigator.h"
#include <furi.h>

struct ViewNavigator {
    bool navigator_started;
    // TODO rework with M-LIB container
    uint32_t current_scene_i;
    AppScene* scene[10];
    void* context;
};

bool view_navigator_next_scene(ViewNavigator* view_navigator);
bool view_navigator_previous_scene(ViewNavigator* view_navigator);
bool view_navigator_search_previous_scene(ViewNavigator* view_navigator, uint32_t scene_id);
