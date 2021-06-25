#pragma once

#include "app_scene.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VIEW_NAV_SEARCH_PREV_SCENE_FLAG (0x80000000UL)
#define VIEW_NAV_IS_SEARCH_PREV_SCENE_EVENT(event) ((event)&VIEW_NAV_SEARCH_PREV_SCENE_FLAG)
#define VIEW_NAV_SEARCH_PREV_SCENE(scene_id) (VIEW_NAV_SEARCH_PREV_SCENE_FLAG | (scene_id))
#define VIEW_NAV_SEARCH_GET_SCENE(event) ((event) & ~VIEW_NAV_SEARCH_PREV_SCENE_FLAG)

typedef enum {
    ViewNavigatorEventNext,
    ViewNavigatorEventPrevious,
    ViewNavigatorEventPreviousSearch = VIEW_NAV_SEARCH_PREV_SCENE_FLAG,
} ViewNavigatorEvent;

typedef struct ViewNavigator ViewNavigator;

ViewNavigator* view_navigator_alloc(void* context);
void view_navigator_free(ViewNavigator* view_navigator);

bool view_navigator_handle_custom_event(ViewNavigator* view_navigator, uint32_t event);
bool view_navigator_handle_navigation_event(ViewNavigator* view_navigator, uint32_t event);

void view_navigator_add_next_scene(ViewNavigator* view_navigator, AppScene* scene);
void view_navigator_start(ViewNavigator* view_navigator);

#ifdef __cplusplus
}
#endif
