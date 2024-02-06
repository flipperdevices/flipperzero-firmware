#include <gui/scene_manager.h>
#include <furi.h>

#include "view_dispatcher.h"
#include "../constants.h"
#include "../flipper_structs.h"

bool view_dispatcher_custom_event_callback(void* ctx, uint32_t custom_event) {
    struct ApplicationContext* context = (struct ApplicationContext*)ctx;
    FURI_LOG_T(LOG_TAG, "view_dispatcher_custom_event_callback");
    return scene_manager_handle_custom_event(context->scene_manager, custom_event);
}

bool view_dispatcher_navigation_event_callback(void* ctx) {
    struct ApplicationContext* context = (struct ApplicationContext*)ctx;
    FURI_LOG_T(LOG_TAG, "view_dispatcher_navigation_event_callback");
    return scene_manager_handle_back_event(context->scene_manager);
}
