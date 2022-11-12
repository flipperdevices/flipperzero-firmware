#pragma once

#include "../shapshup_custom_event.h"
#include <gui/view.h>
#include "assets_icons.h"
#include <input/input.h>
#include <gui/elements.h>
#include <gui/icon.h>

typedef void (*shapshupMainViewCallback)(shapshupCustomEvent event, void* context);
typedef struct shapshupMainView shapshupMainView;

void shapshup_main_view_set_callback(
    shapshupMainView* instance,
    shapshupMainViewCallback callback,
    void* context);

shapshupMainView* shapshup_main_view_alloc();
void shapshup_main_view_free(shapshupMainView* instance);
View* shapshup_main_view_get_view(shapshupMainView* instance);
void shapshup_main_view_set_index(shapshupMainView* instance, uint8_t idx);
void shapshup_main_view_add_item(shapshupMainView* instance, const char* name, uint8_t type);
uint8_t shapshup_main_view_get_index(shapshupMainView* instance);
void shapshup_attack_view_enter(void* context);
void shapshup_attack_view_exit(void* context);
bool shapshup_attack_view_input(InputEvent* event, void* context);
void shapshup_attack_view_draw(Canvas* canvas, void* context);