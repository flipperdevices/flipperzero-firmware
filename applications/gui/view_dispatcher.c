#include "view_dispatcher_i.h"

ViewDispatcher* view_dispatcher_alloc() {
    ViewDispatcher* view_dispatcher = furi_alloc(sizeof(ViewDispatcher));
    ViewDict_init(view_dispatcher->views);
    return view_dispatcher;
}

void view_dispatcher_free(ViewDispatcher* view_dispatcher) {
    free(view_dispatcher);
}

void view_dispatcher_add_view(ViewDispatcher* view_dispatcher, uint32_t view_id, View* view) {
    furi_assert(view_dispatcher);
    furi_assert(view);
    // Check if view id is not used and resgister view
    furi_check(ViewDict_get(view_dispatcher->views, view_id) == NULL);
    ViewDict_set_at(view_dispatcher->views, view_id, view);
    view_set_dispatcher(view, view_dispatcher);
}

void view_dispatcher_switch_to_view(ViewDispatcher* view_dispatcher, uint32_t view_id) {
    furi_assert(view_dispatcher);
    View** view_pp = ViewDict_get(view_dispatcher->views, view_id);
    furi_check(view_pp != NULL);
    view_dispatcher->current_view = *view_pp;
    widget_update(view_dispatcher->widget);
}

void view_dispatcher_attach_to_widget(ViewDispatcher* view_dispatcher, Widget* widget) {
    furi_assert(view_dispatcher);
    furi_assert(widget);
    view_dispatcher->widget = widget;
    widget_draw_callback_set(widget, view_dispatcher_draw_callback, view_dispatcher);
    widget_input_callback_set(widget, view_dispatcher_input_callback, view_dispatcher);
}

void view_dispatcher_draw_callback(Canvas* canvas, void* context) {
    ViewDispatcher* view_dispatcher = context;
    if (view_dispatcher->current_view) {
        view_draw(view_dispatcher->current_view, canvas);
    }
}

void view_dispatcher_input_callback(InputEvent* event, void* context) {
    ViewDispatcher* view_dispatcher = context;
    if (view_dispatcher->current_view) {
        view_input(view_dispatcher->current_view, event);
    }
}
