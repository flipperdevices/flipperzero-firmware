#ifndef __VIEW_DISPATCHER_H__
#define __VIEW_DISPATCHER_H__

/* Types of view modules used by the application */
enum ViewTypes {
    loading_type, // First view while loading the saved state
    button_type, // Main screen
    variable_item_list_type, // Settings
    dialog_type, // Reset screen
    text_box_type // About screen
};

/* When view_dispatcer receives events, they are sent directly
 * to the scene_manager. */
bool view_dispatcher_custom_event_callback(void*, uint32_t);
bool view_dispatcher_navigation_event_callback(void*);

#endif
