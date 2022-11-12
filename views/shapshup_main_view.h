#pragma once

#include "../shapshup_custom_event.h"
#include <gui/view.h>
#include "assets_icons.h"
#include <input/input.h>
#include <gui/elements.h>
#include <gui/icon.h>

typedef void (*ShapShupMainViewCallback)(shapshupCustomEvent event, void* context);
typedef struct ShapShupMainView ShapShupMainView;

void shapshup_main_view_set_callback(
    ShapShupMainView* instance,
    ShapShupMainViewCallback callback,
    void* context);

ShapShupMainView* shapshup_main_view_alloc();
void shapshup_main_view_free(ShapShupMainView* instance);
View* shapshup_main_view_get_view(ShapShupMainView* instance);
void shapshup_main_view_set_index(ShapShupMainView* instance, uint8_t idx);
void shapshup_main_view_add_item(ShapShupMainView* instance, const char* name, uint8_t type);
uint8_t shapshup_main_view_get_index(ShapShupMainView* instance);
void shapshup_attack_view_enter(void* context);
void shapshup_attack_view_exit(void* context);
bool shapshup_attack_view_input(InputEvent* event, void* context);
void shapshup_attack_view_draw(Canvas* canvas, void* context);