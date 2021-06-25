#include "view_navigator_i.h"

ViewNavigator* view_navigator_alloc(void* context) {
    furi_assert(context);

    ViewNavigator* view_navigator = furi_alloc(sizeof(ViewNavigator));
    view_navigator->context = context;
    view_navigator->navigator_started = false;

    return view_navigator;
}

void view_navigator_free(ViewNavigator* view_navigator) {
    furi_assert(view_navigator);

    free(view_navigator);
}

bool view_navigator_handle_custom_event(ViewNavigator* view_navigator, uint32_t event) {
    furi_assert(view_navigator);

    return view_navigator->scene[view_navigator->current_scene_i]->on_event(
        view_navigator->context, event);
}

bool view_navigator_handle_navigation_event(ViewNavigator* view_navigator, uint32_t event) {
    furi_assert(view_navigator);

    if(event == ViewNavigatorEventNext) {
        return view_navigator_next_scene(view_navigator);
    } else if(event == ViewNavigatorEventPrevious) {
        return view_navigator_previous_scene(view_navigator);
    } else if(VIEW_NAV_IS_SEARCH_PREV_SCENE_EVENT(event)) {
        return view_navigator_search_previous_scene(
            view_navigator, VIEW_NAV_SEARCH_GET_SCENE(event));
    }
    return false;
}

void view_navigator_add_next_scene(ViewNavigator* view_navigator, AppScene* scene) {
    furi_assert(view_navigator);
    furi_assert(scene);

    if(!view_navigator->navigator_started) {
        view_navigator->scene[0] = scene;
        view_navigator->navigator_started = true;
    } else {
        view_navigator->scene[view_navigator->current_scene_i + 1] = scene;
    }
}

void view_navigator_start(ViewNavigator* view_navigator) {
    furi_assert(view_navigator);

    view_navigator->scene[view_navigator->current_scene_i]->on_enter(view_navigator->context);
}

bool view_navigator_next_scene(ViewNavigator* view_navigator) {
    if(view_navigator->scene[view_navigator->current_scene_i + 1]) {
        view_navigator->scene[view_navigator->current_scene_i]->on_exit(view_navigator->context);
        view_navigator->current_scene_i++;
        view_navigator->scene[view_navigator->current_scene_i]->on_enter(view_navigator->context);
        return true;
    }
    return false;
}

bool view_navigator_previous_scene(ViewNavigator* view_navigator) {
    view_navigator->scene[view_navigator->current_scene_i]->on_exit(view_navigator->context);
    // TODO free after reworking with M-LIB container
    view_navigator->scene[view_navigator->current_scene_i] = NULL;
    if(view_navigator->current_scene_i > 0) {
        view_navigator->current_scene_i--;
        view_navigator->scene[view_navigator->current_scene_i]->on_enter(view_navigator->context);
        return true;
    }
    return false;
}

bool view_navigator_search_previous_scene(ViewNavigator* view_navigator, uint32_t scene_id) {
    int32_t i = view_navigator->current_scene_i;
    bool scene_found = false;
    while(i-- >= 0) {
        if(view_navigator->scene[i]->id == scene_id) {
            scene_found = true;
            break;
        } else {
            // TODO free after reworking with M-LIB container
            view_navigator->scene[i] = NULL;
        }
    };
    // TODO remove this awesome code!!!
    if((i < 0) && scene_found) {
        i = 0;
    }
    if(scene_found) {
        view_navigator->scene[view_navigator->current_scene_i]->on_exit(view_navigator->context);
        view_navigator->current_scene_i = i;
        view_navigator->scene[view_navigator->current_scene_i]->on_enter(view_navigator->context);
        return true;
    }
    return false;
}
