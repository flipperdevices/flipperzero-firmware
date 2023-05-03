#ifndef TAG_UI_HEADERS
#define TAG_UI_HEADERS

#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_resources.h>
#include <gui/gui.h>

#include <tag_game_icons.h>

#include "tag_app.h"

void tag_ui_init(TagAppState* state);
void tag_ui_destroy(TagAppState* state);

#endif