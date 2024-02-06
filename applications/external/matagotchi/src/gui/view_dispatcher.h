#ifndef __VIEW_DISPATCHER_H__
#define __VIEW_DISPATCHER_H__

/* When view_dispatcer receives events, they are sent directly
 * to the scene_manager. */
bool view_dispatcher_custom_event_callback(void*, uint32_t);
bool view_dispatcher_navigation_event_callback(void*);

#endif
