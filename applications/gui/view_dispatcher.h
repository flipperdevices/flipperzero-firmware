#pragma once

#include "view.h"
#include "gui.h"

typedef enum {
    ViewDispatcherTypeFullscreen,
    ViewDispatcherTypeWindow
} ViewDispatcherType;

typedef struct ViewDispatcher ViewDispatcher;

ViewDispatcher* view_dispatcher_alloc();

/* Free View Dispatcher
 * @param pointer to View
 */
void view_dispatcher_free(ViewDispatcher* view_dispatcher);

/* Add view to View Dispatcher
 * @param view_dispatcher, View Dispatcher instance
 * @param view_id, View id to register
 * @param view, View instance
 */
void view_dispatcher_add_view(ViewDispatcher* view_dispatcher, uint32_t view_id, View* view);

/* Switch to View
 * @param view_dispatcher, View Dispatcher instance
 * @param view_id, View id to register
 */
void view_dispatcher_switch_to_view(ViewDispatcher* view_dispatcher, uint32_t view_id);

/* Attach View Dispatcher to GUI
 * @param view_dispatcher, View Dispatcher instance
 * @param gui, GUI instance to attach to
 */
void view_dispatcher_attach_to_gui(ViewDispatcher* view_dispatcher, Gui* gui, ViewDispatcherType type);
