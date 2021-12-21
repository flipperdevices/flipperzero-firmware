/**
 * @file view_dispatcher.h
 * GUI: ViewDispatcher API
 */

#pragma once

#include "view.h"
#include "gui.h"
#include "scene_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

/** ViewDispatcher view_port placement */
typedef enum {
    ViewDispatcherTypeDesktop, /**< Desktop layer: fullscreen with status bar on top of it. For internal usage. */
    ViewDispatcherTypeWindow, /**< Window layer: with status bar  */
    ViewDispatcherTypeFullscreen /**< Fullscreen layer: without status bar */
} ViewDispatcherType;

typedef struct ViewDispatcher ViewDispatcher;

/** Allocate ViewDispatcher instance
 *
 * @return     pointer to ViewDispatcher instance
 */
ViewDispatcher* view_dispatcher_alloc();

/** Free ViewDispatcher instance
 *
 * @param      view_dispatcher  pointer to ViewDispatcher
 */
void view_dispatcher_free(ViewDispatcher* view_dispatcher);

/** Allocate Scene Manager
 *
 * @param       view_dispatcher     ViewDispatcher instance
 * @param       app_scene_handlers  Scene Manager handlers
 * @param       context             Scene Manager context
 */
void view_dispatcher_allocate_scene_manager(
    ViewDispatcher* view_dispatcher,
    const SceneManagerHandlers* app_scene_handlers,
    void* context);

void view_dispatcher_set_start_scene(ViewDispatcher* view_dispatcher, uint32_t start_scene_id);

// REMOVE!
SceneManager* view_dispatcher_get_scene_manager(ViewDispatcher* view_dispatcher);

void view_dispatcher_next_scene(ViewDispatcher* view_dispatcher, uint32_t scene_id);

void view_dispatcher_previous_scene(ViewDispatcher* view_dispatcher);

void view_dispatcher_search_and_switch_to_previous_scene(
    ViewDispatcher* view_dispatcher,
    uint32_t scene_id);

void view_dispatcher_search_and_switch_to_another_scene(
    ViewDispatcher* view_dispatcher,
    uint32_t scene_id);

/** Enable queue support
 *
 * If queue enabled all input and custom events will be dispatched throw
 * internal queue
 *
 * @param      view_dispatcher  ViewDispatcher instance
 */
void view_dispatcher_enable_queue(ViewDispatcher* view_dispatcher);

/** Send custom event
 *
 * @param      view_dispatcher  ViewDispatcher instance
 * @param[in]  event            The event
 */
void view_dispatcher_send_custom_event(ViewDispatcher* view_dispatcher, uint32_t event);

/** Set View Dispatcher event handler
 *
 * Called on View Dispatcher Event
 *
 * @param      view_dispatcher  ViewDispatcher instance
 * @param      callback         ViewDispatcherEventCallback instance
 */
// void view_dispatcher_set_event_callback(ViewDispatcher* view_dispatcher, ViewDispatcherEventCallback callback, void* context);

/** Set tick event period
 *
 * @param      view_dispatcher  ViewDispatcher instance
 * @param      tick_period      callback call period
 */
void view_dispatcher_set_tick_event_period(ViewDispatcher* view_dispatcher, uint32_t tick_period);

/** Run ViewDispatcher
 *
 * Use only after queue enabled
 *
 * @param      view_dispatcher  ViewDispatcher instance
 */
void view_dispatcher_run(ViewDispatcher* view_dispatcher);

/** Stop ViewDispatcher
 *
 * Use only after queue enabled
 *
 * @param      view_dispatcher  ViewDispatcher instance
 */
void view_dispatcher_stop(ViewDispatcher* view_dispatcher);

/** Add view to ViewDispatcher
 *
 * @param      view_dispatcher  ViewDispatcher instance
 * @param      view_id          View id to register
 * @param      view             View instance
 */
void view_dispatcher_add_view(ViewDispatcher* view_dispatcher, uint32_t view_id, View* view);

/** Remove view from ViewDispatcher
 *
 * @param      view_dispatcher  ViewDispatcher instance
 * @param      view_id          View id to remove
 */
void view_dispatcher_remove_view(ViewDispatcher* view_dispatcher, uint32_t view_id);

/** Switch to View
 *
 * @param      view_dispatcher  ViewDispatcher instance
 * @param      view_id          View id to register
 * @warning    switching may be delayed till input events complementarity
 *             reached
 */
void view_dispatcher_switch_to_view(ViewDispatcher* view_dispatcher, uint32_t view_id);

/** Send ViewPort of this ViewDispatcher instance to front
 *
 * @param      view_dispatcher  ViewDispatcher instance
 */
void view_dispatcher_send_to_front(ViewDispatcher* view_dispatcher);

/** Send ViewPort of this ViewDispatcher instance to back
 *
 * @param      view_dispatcher  ViewDispatcher instance
 */
void view_dispatcher_send_to_back(ViewDispatcher* view_dispatcher);

/** Attach ViewDispatcher to GUI
 *
 * @param      view_dispatcher  ViewDispatcher instance
 * @param      gui              GUI instance to attach to
 * @param[in]  type             The type
 */
void view_dispatcher_attach_to_gui(
    ViewDispatcher* view_dispatcher,
    Gui* gui,
    ViewDispatcherType type);

#ifdef __cplusplus
}
#endif
