#ifndef TAG_APP_LOOP_HEADERS
#define TAG_APP_LOOP_HEADERS

#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_resources.h>
#include <gui/gui.h>
#include "tag_app.h"

void tag_app_game_loop_run(TagAppState* state, uint32_t duration_s);

#endif
