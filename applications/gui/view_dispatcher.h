#pragma once

#include "view.h"
#include "widget.h"

typedef struct ViewDispatcher ViewDispatcher;

ViewDispatcher* view_dispatcher_alloc();

void view_dispatcher_free(ViewDispatcher* view_dispatcher);

void view_dispatcher_add_view(ViewDispatcher* view_dispatcher, uint32_t view_id, View* view);

void view_dispatcher_set_default_view(ViewDispatcher* view_dispatcher, uint32_t view_id);

void view_dispatcher_switch_to_view(ViewDispatcher* view_dispatcher, uint32_t view_id);

void view_dispatcher_attach_to_widget(ViewDispatcher* view_dispatcher, Widget* widget);
